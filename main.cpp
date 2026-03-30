#include <iostream>
#include <string>
#include <vector>
#include <cctype>

// 1. Definición de los tipos de token (MedLang)
enum class TokenType {
    // Palabras reservadas de sección y raíz
    HOSPITAL, PACIENTES, MEDICOS, CITAS, DIAGNOSTICOS,
    // Palabras reservadas de elemento
    PACIENTE_ELEM, MEDICO_ELEM, CITA_ELEM, DIAGNOSTICO_ELEM,
    // Literales y valores
    CADENA, NUMERO, FECHA, HORA, ID_CODIGO,
    ESPECIALIDAD, TIPO_SANGRE, DOSIS,
    // Símbolos y delimitadores ({, }, [, ], :, ,, etc.)
    SIMBOLO,
    // Control
    ERROR_LEXICO, FIN_ARCHIVO
};

// 2. Clase Token
struct Token {
    std::string lexema;
    TokenType tipo;
    int linea;
    int columna;
};

// 3. Clase Analizador Léxico
class LexicalAnalyzer {
private:
    std::string codigoFuente;
    size_t posicion;
    int linea;
    int columna;

public:
    // Constructor
    LexicalAnalyzer(const std::string& fuente) 
        : codigoFuente(fuente), posicion(0), linea(1), columna(1) {}

    // El motor principal del AFD
    Token nextToken() {
        int estado = 0;
        std::string lexema = "";
        
        // Guardamos la posición inicial del token
        int tokenLinea = linea;
        int tokenColumna = columna;

        while (posicion < codigoFuente.length()) {
            char c = codigoFuente[posicion];
            
            // Tracking preciso de línea y columna
            if (c == '\n') {
                linea++;
                columna = 0; // Se incrementa a 1 al final del ciclo
            }

            switch (estado) {
                case 0: // S0: Estado inicial
                    if (std::isspace(c)) {
                        // Ignorar espacios, tabulaciones y saltos de línea fuera de tokens
                        posicion++;
                        columna++;
                        tokenLinea = linea;
                        tokenColumna = columna;
                        continue; 
                    } else if (std::isalpha(c)) {
                        estado = 1; // Transición para letras (Palabras reservadas, IDs, etc.)
                        lexema += c;
                    } else if (std::isdigit(c)) {
                        estado = 2; // S2 en tu tabla: Inicia posible Número, Fecha u Hora
                        lexema += c;
                    } else if (c == '"') {
                        estado = 13; // O el estado que definas para el inicio de CADENA
                        lexema += c;
                    } else {
                        // Símbolos sueltos como { } [ ] : ,
                        lexema += c;
                        posicion++;
                        columna++;
                        return {lexema, TokenType::SIMBOLO, tokenLinea, tokenColumna};
                    }
                    break;

                case 2: // S2: Viene de un dígito en S0
                    if (std::isdigit(c)) {
                        estado = 3; // S3: Segundo dígito (posible fecha/hora o número más grande)
                        lexema += c;
                    } else {
                        // Si no viene otro dígito (ni un guion o dos puntos), es un Número Entero (ej. edad: 45)
                        return {lexema, TokenType::NUMERO, tokenLinea, tokenColumna};
                    }
                    break;

                case 3: // S3: Ya leímos dos dígitos
                    if (std::isdigit(c)) {
                        estado = 4; // S4: Tercer dígito de una fecha AAAA
                        lexema += c;
                    } else if (c == ':') {
                        estado = 7; // S7: Dos puntos, nos fuimos por el camino de la HORA (HH:)
                        lexema += c;
                    } else {
                        return {lexema, TokenType::NUMERO, tokenLinea, tokenColumna};
                    }
                    break;
                // --- RUTA DE LA FECHA (Continuación) ---
                case 4: // S4: Tercer dígito de AAAA
                    if (std::isdigit(c)) {
                        estado = 5; // S5: Cuarto dígito (año completo)
                        lexema += c;
                    } else {
                        // Si se corta aquí, es un número válido (ej. una habitación o edad larga)
                        return {lexema, TokenType::NUMERO, tokenLinea, tokenColumna};
                    }
                    break;

                case 5: // S5: Año completo (AAAA), esperamos obligatoriamente el primer guion '-'
                    if (c == '-') {
                        estado = 6; // S6: Primer guion leído
                        lexema += c;
                    } else {
                        // Si no viene guion, retorna como NUMERO (el analizador no consumió el carácter actual)
                        return {lexema, TokenType::NUMERO, tokenLinea, tokenColumna};
                    }
                    break;

                case 6: // S6: Esperamos primer dígito del mes
                    if (std::isdigit(c)) {
                        estado = 8; // S8: Primer dígito del mes leído
                        lexema += c;
                    } else {
                        // Error: "AAAA-" no es un token válido
                        return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna}; 
                    }
                    break;

                case 8: // S8: Esperamos segundo dígito del mes
                    if (std::isdigit(c)) {
                        estado = 10; // S10: Mes completo (MM)
                        lexema += c;
                    } else {
                        return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
                    }
                    break;

                case 10: // S10: Esperamos el segundo guion '-'
                    if (c == '-') {
                        estado = 11; // S11: Segundo guion leído
                        lexema += c;
                    } else {
                        return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
                    }
                    break;

                case 11: // S11: Esperamos primer dígito del día
                    if (std::isdigit(c)) {
                        estado = 12; // S12: Primer dígito del día leído
                        lexema += c;
                    } else {
                        return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
                    }
                    break;

                case 12: // S12: Esperamos segundo dígito del día -> ¡FIN DE LA FECHA!
                    if (std::isdigit(c)) {
                        lexema += c;
                        posicion++; 
                        columna++; // Consumimos este último dígito manualmente
                        return {lexema, TokenType::FECHA, tokenLinea, tokenColumna};
                    } else {
                        return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
                    }
                    break;

                // --- RUTA DE LA HORA ---
                case 7: // S7: Leímos ':', esperamos el primer dígito de los minutos
                    if (std::isdigit(c)) {
                        estado = 9; // S9: Primer dígito de minutos leído
                        lexema += c;
                    } else {
                        return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
                    }
                    break;

                case 9: // S9: Esperamos segundo dígito de los minutos -> ¡FIN DE LA HORA!
                    if (std::isdigit(c)) {
                        lexema += c;
                        posicion++; 
                        columna++; // Consumimos este último dígito manualmente
                        return {lexema, TokenType::HORA, tokenLinea, tokenColumna};
                    } else {
                        return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
                    }
                    break;

                // --- RUTA DE CADENAS DE TEXTO (Comillas Dobles) ---
                case 13: // Estado S1 para literales de texto (inició con '"' en S0)
                    if (c == '"') {
                        lexema += c;
                        posicion++; 
                        columna++; // Consumimos la comilla de cierre
                        return {lexema, TokenType::CADENA, tokenLinea, tokenColumna};
                    } else if (c == '\n') {
                        // Manejo de error crítico requerido por el proyecto: cadena sin cerrar al salto de línea [cite: 160]
                        return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna}; 
                    } else {
                        // Consumimos cualquier otro carácter (letras, espacios, números) dentro de la cadena
                        lexema += c;
                    }
                    break;   
            }
            
            posicion++;
            columna++;
        }

        // Si el ciclo termina y no retornó nada, llegamos al final
        if (!lexema.empty()) {
            // Manejar un posible token final que quedó a medias
        }

        return {"EOF", TokenType::FIN_ARCHIVO, linea, columna};
    }
};