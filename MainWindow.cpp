#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QTextStream>

// Incluimos tu motor C++
#include "LexicalAnalyzer.h"
#include "ErrorManager.h"
#include "ReportGenerator.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    configurarInterfaz();
}

MainWindow::~MainWindow() {}

void MainWindow::configurarInterfaz() {
    this->setWindowTitle("MedLexer - Analizador Léxico Hospitalario");
    this->resize(1000, 700);

    // Widget central y Layout principal
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 1. Panel superior: Botones
    QHBoxLayout *panelBotones = new QHBoxLayout();
    btnCargar = new QPushButton("Cargar Archivo .med", this);
    btnAnalizar = new QPushButton("Analizar Código", this);
    btnReportes = new QPushButton("Abrir Reportes HTML", this);
    btnLimpiar = new QPushButton("Limpiar Todo", this);

    panelBotones->addWidget(btnCargar);
    panelBotones->addWidget(btnAnalizar);
    panelBotones->addWidget(btnReportes);
    panelBotones->addWidget(btnLimpiar);

    // 2. Área de texto para el código 
    editorCodigo = new QTextEdit(this);
    editorCodigo->setPlaceholderText("Ingresa el código MedLang aquí o carga un archivo...");

    // 3. Tablas de resultados 
    QHBoxLayout *panelTablas = new QHBoxLayout();
    
    tablaTokens = new QTableWidget(0, 5, this);
    tablaTokens->setHorizontalHeaderLabels({"No.", "Lexema", "Tipo", "Línea", "Columna"});
    
    tablaErrores = new QTableWidget(0, 6, this);
    tablaErrores->setHorizontalHeaderLabels({"No.", "Lexema", "Error", "Descripción", "Línea", "Columna"});

    panelTablas->addWidget(tablaTokens);
    panelTablas->addWidget(tablaErrores);

    // Ensamblar todo
    mainLayout->addLayout(panelBotones);
    mainLayout->addWidget(editorCodigo, 2); // Le damos más espacio al código
    mainLayout->addLayout(panelTablas, 1);

    this->setCentralWidget(centralWidget);

    // Conectar botones con sus funciones (Slots)
    connect(btnCargar, &QPushButton::clicked, this, &MainWindow::cargarArchivo);
    connect(btnAnalizar, &QPushButton::clicked, this, &MainWindow::analizarCodigo);
    connect(btnReportes, &QPushButton::clicked, this, &MainWindow::abrirReportes);
    connect(btnLimpiar, &QPushButton::clicked, this, &MainWindow::limpiarTodo);
}

void MainWindow::cargarArchivo() {
    QString rutaArchivo = QFileDialog::getOpenFileName(this, "Abrir Archivo MedLang", "", "Archivos Med (*.med);;Todos los archivos (*)");
    if (!rutaArchivo.isEmpty()) {
        QFile archivo(rutaArchivo);
        if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream entrada(&archivo);
            editorCodigo->setPlainText(entrada.readAll());
            archivo.close();
        }
    }
}

void MainWindow::analizarCodigo() {
    // 1. Limpiar tablas
    tablaTokens->setRowCount(0);
    tablaErrores->setRowCount(0);

    // 2. Extraer el texto de la GUI (¡Usando UTF-8 para evitar choques de compilador!)
    std::string codigoFuente = editorCodigo->toPlainText().toUtf8().constData();
    if (codigoFuente.empty()) {
        QMessageBox::warning(this, "Advertencia", "No hay código para analizar.");
        return;
    }

    // 3. ¡Ejecutar tu motor léxico!
    ErrorManager gestorErrores;
    LexicalAnalyzer analizador(codigoFuente, gestorErrores);
    std::vector<Token> listaTokensValidos;

    Token t = analizador.nextToken();
    int filaToken = 0;

    while (t.tipo != TokenType::FIN_ARCHIVO) {
        if (t.tipo != TokenType::ERROR_LEXICO) {
            listaTokensValidos.push_back(t);
            
            // Insertar en la GUI (Convirtiendo de vuelta usando fromUtf8)
            tablaTokens->insertRow(filaToken);
            tablaTokens->setItem(filaToken, 0, new QTableWidgetItem(QString::number(filaToken + 1)));
            tablaTokens->setItem(filaToken, 1, new QTableWidgetItem(QString::fromUtf8(t.lexema.c_str())));
            tablaTokens->setItem(filaToken, 2, new QTableWidgetItem(QString::number((int)t.tipo)));
            tablaTokens->setItem(filaToken, 3, new QTableWidgetItem(QString::number(t.linea)));
            tablaTokens->setItem(filaToken, 4, new QTableWidgetItem(QString::number(t.columna)));
            filaToken++;
        }
        t = analizador.nextToken();
    }

    // 4. Mostrar errores en la GUI si los hay
    if (gestorErrores.tieneErrores()) {
        int filaError = 0;
        for (const auto& error : gestorErrores.getErrores()) {
            tablaErrores->insertRow(filaError);
            tablaErrores->setItem(filaError, 0, new QTableWidgetItem(QString::number(error.id)));
            tablaErrores->setItem(filaError, 1, new QTableWidgetItem(QString::fromUtf8(error.lexema.c_str())));
            tablaErrores->setItem(filaError, 2, new QTableWidgetItem(QString::fromUtf8(error.tipoError.c_str())));
            tablaErrores->setItem(filaError, 3, new QTableWidgetItem(QString::fromUtf8(error.descripcion.c_str())));
            tablaErrores->setItem(filaError, 4, new QTableWidgetItem(QString::number(error.linea)));
            tablaErrores->setItem(filaError, 5, new QTableWidgetItem(QString::number(error.columna)));
            filaError++;
        }
        QMessageBox::warning(this, "Análisis Completado", "Se encontraron errores léxicos. Revisa la tabla.");
    } else {
        QMessageBox::information(this, "Análisis Completado", "¡Análisis léxico exitoso! 0 errores.");
    }

    // 5. Generar Reportes en el disco
    ReportGenerator generador(listaTokensValidos, gestorErrores.getErrores());
    generador.generarReporteErrores();
    generador.generarReportePacientes();
    generador.generarReporteMedicos();
    generador.generarReporteCitas();
    generador.generarReporteEstadistico();
    generador.generarGraphviz();
}



void MainWindow::abrirReportes() {
    // Abre el índice o directamente un reporte en el navegador web 
    QDesktopServices::openUrl(QUrl::fromLocalFile("Reporte_Estadistico.html"));
}

void MainWindow::limpiarTodo() {
    // 1. Apagamos señales para que el editor no dispare eventos ocultos
    editorCodigo->blockSignals(true);
    editorCodigo->clear();
    editorCodigo->blockSignals(false);
    
    // 2. Método a prueba de balas: Borrar fila por fila desde la primera
    // Así Qt limpia la memoria ordenadamente y no explota
    while (tablaTokens->rowCount() > 0) {
        tablaTokens->removeRow(0);
    }
    
    while (tablaErrores->rowCount() > 0) {
        tablaErrores->removeRow(0);
    }
}