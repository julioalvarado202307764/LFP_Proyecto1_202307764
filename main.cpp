#include <iostream>
#include <string>
#include "LexicalAnalyzer.h"
#include "ErrorManager.h"

// Función de ayuda para imprimir bonito en la consola
std::string obtenerNombreToken(TokenType tipo) {
    switch (tipo) {
        case TokenType::HOSPITAL: return "HOSPITAL";
        case TokenType::PACIENTES: return "PACIENTES";
        case TokenType::PACIENTE_ELEM: return "PACIENTE_ELEM";
        case TokenType::CADENA: return "CADENA";
        case TokenType::NUMERO: return "NUMERO";
        case TokenType::FECHA: return "FECHA";
        case TokenType::HORA: return "HORA";
        case TokenType::ID_CODIGO: return "ID_CODIGO";
        case TokenType::ESPECIALIDAD: return "ESPECIALIDAD";
        case TokenType::TIPO_SANGRE: return "TIPO_SANGRE";
        case TokenType::DOSIS: return "DOSIS";
        case TokenType::SIMBOLO: return "SIMBOLO";
        case TokenType::ERROR_LEXICO: return "ERROR_LEXICO";
        case TokenType::FIN_ARCHIVO: return "FIN_ARCHIVO";
        case TokenType::CITAS: return "CITAS";
        case TokenType::CITA_ELEM: return "CITA_ELEM";
        default: return "OTRO";
    }
}

int main() {
    // Un mini código fuente de prueba mezclando varios casos [cite: 66, 67, 74]
    std::string codigoPrueba = 
        "HOSPITAL \n"
        "PACIENTES { \n"
        "   paciente: \"Maria Garcia\" [edad: 45, tipo_sangre: \"A+\"] \n"
        "} \n"
        "CITAS { \n"
        "   cita: \"Maria Garcia\" con \"Dr. Roberto\" [fecha: 2025-04-10, hora: 09:00] \n"
        "}";

    std::cout << "--- INICIANDO ANALISIS LEXICO ---" << std::endl;

    ErrorManager gestorErrores;
    LexicalAnalyzer analizador(codigoPrueba, gestorErrores);

    Token t = analizador.nextToken();
    
    // Ciclo principal que exprime el autómata hasta el final
    while (t.tipo != TokenType::FIN_ARCHIVO) {
        std::cout << "Lexema: [" << t.lexema << "] \t"
                  << "Tipo: " << obtenerNombreToken(t.tipo) << " \t"
                  << "Linea: " << t.linea << " \t"
                  << "Col: " << t.columna << std::endl;
                  
        t = analizador.nextToken();
    }

    // Imprimir los errores si hubieron
    if (gestorErrores.tieneErrores()) {
        std::cout << "\n--- ERRORES ENCONTRADOS ---" << std::endl;
        for (const auto& error : gestorErrores.getErrores()) {
            std::cout << "Error en Linea " << error.linea << " Col " << error.columna 
                      << ": " << error.descripcion << " (Lexema: " << error.lexema << ")" << std::endl;
        }
    } else {
        std::cout << "\nAnalisis exitoso, 0 errores." << std::endl;
    }

    return 0;
}