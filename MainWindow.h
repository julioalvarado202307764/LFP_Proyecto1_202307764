#pragma once
#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Acciones de los botones
    void cargarArchivo();
    void analizarCodigo();
    void abrirReportes();
    void limpiarTodo();
private:
    // Componentes visuales mínimos 
    QTextEdit *editorCodigo;
    QTableWidget *tablaTokens;
    QTableWidget *tablaErrores;
    QPushButton *btnCargar;
    QPushButton *btnAnalizar;
    QPushButton *btnReportes;
    QPushButton* btnLimpiar;
    
    // Métodos de apoyo visual
    void configurarInterfaz();
    void llenarTablaTokens();
    void llenarTablaErrores();
};