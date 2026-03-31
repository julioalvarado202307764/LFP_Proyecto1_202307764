#include <iostream>
#include <string>
#include <vector>
#include "LexicalAnalyzer.h"
#include "ErrorManager.h"
#include "ReportGenerator.h"

// Función de ayuda para imprimir bonito en la consola
std::string obtenerNombreToken(TokenType tipo)
{
    switch (tipo)
    {
    case TokenType::HOSPITAL:
        return "HOSPITAL";
    case TokenType::PACIENTES:
        return "PACIENTES";
    case TokenType::MEDICOS:
        return "MEDICOS";
    case TokenType::CITAS:
        return "CITAS";
    case TokenType::DIAGNOSTICOS:
        return "DIAGNOSTICOS";
    case TokenType::PACIENTE_ELEM:
        return "PACIENTE_ELEM";
    case TokenType::MEDICO_ELEM:
        return "MEDICO_ELEM";
    case TokenType::CITA_ELEM:
        return "CITA_ELEM";
    case TokenType::DIAGNOSTICO_ELEM:
        return "DIAGNOSTICO_ELEM";
    case TokenType::CADENA:
        return "CADENA";
    case TokenType::NUMERO:
        return "NUMERO";
    case TokenType::FECHA:
        return "FECHA";
    case TokenType::HORA:
        return "HORA";
    case TokenType::ID_CODIGO:
        return "ID_CODIGO";
    case TokenType::ESPECIALIDAD:
        return "ESPECIALIDAD";
    case TokenType::TIPO_SANGRE:
        return "TIPO_SANGRE";
    case TokenType::DOSIS:
        return "DOSIS";
    case TokenType::SIMBOLO:
        return "SIMBOLO";
    case TokenType::ERROR_LEXICO:
        return "ERROR_LEXICO";
    case TokenType::FIN_ARCHIVO:
        return "FIN_ARCHIVO";
    default:
        return "OTRO";
    }
}

int main()
{
    // Código fuente de prueba con un error intencional para probar el Reporte de Errores
 std::string codigoPrueba = 
        "HOSPITAL \n"
        "PACIENTES { \n"
        "   paciente: \"Maria Garcia\" [edad: 45, tipo_sangre: \"A+\"] \n"
        "   paciente: \"Pedro Castillo\" [edad: 71, tipo_sangre: \"AB-\"] \n"
        "   paciente: \"Sofia Ramirez\" [edad: 29, tipo_sangre: \"B+\"] \n"
        "} \n"
        "MEDICOS { \n"
        "   medico: \"Dr. Roberto Juarez\" [especialidad: CARDIOLOGIA, codigo: \"MED-001\"] \n"
        "   medico: \"Dr. Luis Herrera\" [especialidad: PEDIATRIA, codigo: \"MED-003\"] \n"
        "} \n"
        "CITAS { \n"
        "   cita: \"Maria Garcia\" con \"Dr. Roberto Juarez\" [fecha: 2025-04-10, hora: 09:00] \n"
        "   cita: \"Pedro Castillo\" con \"Dr. Roberto Juarez\" [fecha: 2025-04-10, hora: 09:00] \n" // <-- ¡CONFLICTO (mismo doctor, hora y fecha)!
        "   cita: \"Sofia Ramirez\" con \"Dr. Luis Herrera\" [fecha: 2026-04-11, hora: 08:00] \n"   // <-- ¡PENDIENTE (fecha futura al 31 de marzo de 2026)!
        "} \n"
        "DIAGNOSTICOS { \n"
        "}";
    std::cout << "--- INICIANDO ANALISIS LEXICO ---" << std::endl;

    ErrorManager gestorErrores;
    LexicalAnalyzer analizador(codigoPrueba, gestorErrores);

    std::vector<Token> listaTokensValidos; // Vector para almacenar los tokens limpios

    Token t = analizador.nextToken();

    // Ciclo principal que exprime el autómata hasta el final
    while (t.tipo != TokenType::FIN_ARCHIVO)
    {
        std::cout << "Lexema: [" << t.lexema << "] \t"
                  << "Tipo: " << obtenerNombreToken(t.tipo) << " \t"
                  << "Linea: " << t.linea << " \t"
                  << "Col: " << t.columna << std::endl;

        // Guardamos el token en nuestra lista, ignorando los que son puro error
        if (t.tipo != TokenType::ERROR_LEXICO)
        {
            listaTokensValidos.push_back(t);
        }

        t = analizador.nextToken();
    }

    // Imprimir los errores en consola si los hay
    if (gestorErrores.tieneErrores())
    {
        std::cout << "\n--- ERRORES ENCONTRADOS ---" << std::endl;
        for (const auto &error : gestorErrores.getErrores())
        {
            std::cout << "Error en Linea " << error.linea << " Col " << error.columna
                      << ": " << error.descripcion << " (Lexema: " << error.lexema << ")" << std::endl;
        }
    }
    else
    {
        std::cout << "\nAnalisis exitoso, 0 errores en consola." << std::endl;
    }

    // Generar los reportes HTML
    std::cout << "\n--- GENERANDO REPORTES HTML ---" << std::endl;

    // Instanciamos el generador pasándole los vectores resultantes
    ReportGenerator generador(listaTokensValidos, gestorErrores.getErrores());
    generador.generarReporteErrores();
    generador.generarReportePacientes();
    generador.generarReporteMedicos();
    generador.generarReporteCitas();
    generador.generarReporteEstadistico();
    generador.generarGraphviz();
    return 0;
}