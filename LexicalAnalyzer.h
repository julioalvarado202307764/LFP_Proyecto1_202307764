#pragma once
#include <string>
#include "Token.h"
#include "ErrorManager.h"

class LexicalAnalyzer {
private:
    std::string codigoFuente;
    size_t posicion;
    int linea;
    int columna;
    ErrorManager& errorManager;

    TokenType clasificarPalabra(const std::string& palabra);
    TokenType clasificarCadena(const std::string& lexema);

public:
    LexicalAnalyzer(const std::string& fuente, ErrorManager& manager);
    Token nextToken();
};