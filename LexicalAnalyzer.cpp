#include "LexicalAnalyzer.h"
#include <cctype>

// CLASE ANALIZADOR LEXICO
LexicalAnalyzer::LexicalAnalyzer(const std::string &fuente, ErrorManager &manager)
    : codigoFuente(fuente), posicion(0), linea(1), columna(1), errorManager(manager) {}

TokenType LexicalAnalyzer::clasificarPalabra(const std::string &palabra)
{
    // Palabras reservadas principales
    if (palabra == "HOSPITAL")
        return TokenType::HOSPITAL;
    if (palabra == "PACIENTES")
        return TokenType::PACIENTES;
    if (palabra == "MEDICOS")
        return TokenType::MEDICOS;
    if (palabra == "CITAS")
        return TokenType::CITAS;
    if (palabra == "DIAGNOSTICOS")
        return TokenType::DIAGNOSTICOS;

    // Elementos y conectores
    if (palabra == "paciente")
        return TokenType::PACIENTE_ELEM;
    if (palabra == "medico")
        return TokenType::MEDICO_ELEM;
    if (palabra == "cita")
        return TokenType::CITA_ELEM;
    if (palabra == "diagnostico")
        return TokenType::DIAGNOSTICO_ELEM;
    if (palabra == "con")
        return TokenType::SIMBOLO; // O un token CONECTOR específico

    // Atributos
    if (palabra == "edad" || palabra == "tipo_sangre" || palabra == "habitacion" ||
        palabra == "especialidad" || palabra == "codigo" || palabra == "fecha" ||
        palabra == "hora" || palabra == "condicion" || palabra == "medicamento" ||
        palabra == "dosis")
    {
        return TokenType::SIMBOLO; // Puedes crear un TokenType::ATRIBUTO si prefieres
    }

    // Enumeraciones - Especialidades
    if (palabra == "CARDIOLOGIA" || palabra == "NEUROLOGIA" || palabra == "PEDIATRIA" ||
        palabra == "CIRUGIA" || palabra == "MEDICINA_GENERAL" || palabra == "ONCOLOGIA")
    {
        return TokenType::ESPECIALIDAD;
    }

    // Enumeraciones - Dosis
    if (palabra == "DIARIA" || palabra == "CADA_8_HORAS" ||
        palabra == "CADA_12_HORAS" || palabra == "SEMANAL")
    {
        return TokenType::DOSIS;
    }

    // Si llega aquí, es una palabra que no pertenece al lenguaje
    return TokenType::ERROR_LEXICO;
}

TokenType LexicalAnalyzer::clasificarCadena(const std::string &lexema)
{
    // Quitamos las comillas para evaluar el contenido limpio
    // Ej: pasamos de '"AB+"' a 'AB+'
    std::string contenido = lexema.substr(1, lexema.length() - 2);

    // 1. Validar Tipo de Sangre Restringido
    if (contenido == "A+" || contenido == "A-" || contenido == "B+" ||
        contenido == "B-" || contenido == "O+" || contenido == "O-" ||
        contenido == "AB+" || contenido == "AB-")
    {
        return TokenType::TIPO_SANGRE;
    }

    // 2. Validar Código ID (Formato: 3 letras + guión + dígitos)
    // Buscamos si tiene un guion en la posición 3 (índice 3 base cero)
    if (contenido.length() >= 5 && contenido[3] == '-')
    {
        bool letrasValidas = std::isalpha(contenido[0]) &&
                             std::isalpha(contenido[1]) &&
                             std::isalpha(contenido[2]);

        bool numerosValidos = true;
        for (size_t i = 4; i < contenido.length(); i++)
        {
            if (!std::isdigit(contenido[i]))
            {
                numerosValidos = false;
                break;
            }
        }

        if (letrasValidas && numerosValidos)
        {
            return TokenType::ID_CODIGO;
        }
    }

    // 3. Si no es ni sangre ni código, es una cadena de texto normal
    return TokenType::CADENA;
}

Token LexicalAnalyzer::nextToken()
{
    int estado = 0;
    std::string lexema = "";

    // Guardamos la posición inicial del token
    int tokenLinea = linea;
    int tokenColumna = columna;

    while (posicion < codigoFuente.length())
    {
        char c = codigoFuente[posicion];

        // Tracking preciso de línea y columna
        if (c == '\n')
        {
            linea++;
            columna = 0; // Se incrementa a 1 al final del ciclo
        }

        switch (estado)
        {
        case 0: // S0: Estado inicial
            if (std::isspace(c))
            {
                // Columna 'w': S0 + w -> S0 (Ignoramos el espacio fuera de tokens)
                posicion++;
                columna++;
                tokenLinea = linea;
                tokenColumna = columna;
                continue;
            }
            else if (c == '"')
            {
                // Columna '"': S0 + " -> S1 (Inicia cadena)
                estado = 1;
                lexema += c;
            }
            else if (std::isalpha(c))
            {
                // Columna 'letra': Lo mandamos a un estado diferente (ej. 14)
                // para acumular la palabra, ya que la tabla original cicla S0 extrañamente
                estado = 14;
                lexema += c;
            }
            else if (std::isdigit(c))
            {
                // Columna 'N': S0 + N -> S2 (Inicia número, fecha u hora)
                estado = 2;
                lexema += c;
            }
            else
            {
                // Columna 'S' u otros: Símbolos sueltos ({, }, [, ], etc.)
                lexema += c;
                posicion++;
                columna++;
                return {lexema, TokenType::SIMBOLO, tokenLinea, tokenColumna};
            }
            break;

        case 1: // S1: Estado de Cadenas (según tu nueva tabla)
            if (c == '"')
            {
                // Columna '"': S1 + " -> S0 (Fin de la cadena)
                lexema += c;
                posicion++;
                columna++;

                // Clasificamos si es TIPO_SANGRE, ID_CODIGO o CADENA normal
                TokenType tipo = clasificarCadena(lexema);
                return {lexema, tipo, tokenLinea, tokenColumna};
            }
            else if (c == '\n')
            {
                // Manejo de error CRÍTICO: salto de línea sin cerrar comillas
                errorManager.agregarError(
                    lexema,
                    "Cadena sin cerrar",
                    "Se encontró el inicio de una cadena pero no su cierre.",
                    tokenLinea,
                    tokenColumna,
                    "CRÍTICO");
                estado = 0; // Reseteamos para recuperar el análisis
                return {"", TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
            }
            else
            {
                // Columnas 'letra', 'N', '-', ':', 'S', 'w':
                // Cualquier cosa dentro de comillas nos mantiene en S1
                lexema += c;
            }
            break;
        case 2: // S2: Viene de un dígito en S0
            if (std::isdigit(c))
            {
                estado = 3; // S3: Segundo dígito (posible fecha/hora o número más grande)
                lexema += c;
            }
            else
            {
                // Si no viene otro dígito (ni un guion o dos puntos), es un Número Entero (ej. edad: 45)
                return {lexema, TokenType::NUMERO, tokenLinea, tokenColumna};
            }
            break;

        case 3: // S3: Ya leímos dos dígitos
            if (std::isdigit(c))
            {
                estado = 4; // S4: Tercer dígito de una fecha AAAA
                lexema += c;
            }
            else if (c == ':')
            {
                estado = 7; // S7: Dos puntos, nos fuimos por el camino de la HORA (HH:)
                lexema += c;
            }
            else
            {
                return {lexema, TokenType::NUMERO, tokenLinea, tokenColumna};
            }
            break;
        // --- RUTA DE LA FECHA (Continuación) ---
        case 4: // S4: Tercer dígito de AAAA
            if (std::isdigit(c))
            {
                estado = 5; // S5: Cuarto dígito (año completo)
                lexema += c;
            }
            else
            {
                // Si se corta aquí, es un número válido (ej. una habitación o edad larga)
                return {lexema, TokenType::NUMERO, tokenLinea, tokenColumna};
            }
            break;

        case 5: // S5: Año completo (AAAA), esperamos obligatoriamente el primer guion '-'
            if (c == '-')
            {
                estado = 6; // S6: Primer guion leído
                lexema += c;
            }
            else
            {
                // Si no viene guion, retorna como NUMERO (el analizador no consumió el carácter actual)
                return {lexema, TokenType::NUMERO, tokenLinea, tokenColumna};
            }
            break;

        case 6: // S6: Esperamos primer dígito del mes
            if (std::isdigit(c))
            {
                estado = 8; // S8: Primer dígito del mes leído
                lexema += c;
            }
            else
            {
                // Error: "AAAA-" no es un token válido
                return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
            }
            break;

        case 8: // S8: Esperamos segundo dígito del mes
            if (std::isdigit(c))
            {
                estado = 10; // S10: Mes completo (MM)
                lexema += c;
            }
            else
            {
                return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
            }
            break;

        case 10: // S10: Esperamos el segundo guion '-'
            if (c == '-')
            {
                estado = 11; // S11: Segundo guion leído
                lexema += c;
            }
            else
            {
                return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
            }
            break;

        case 11: // S11: Esperamos primer dígito del día
            if (std::isdigit(c))
            {
                estado = 12; // S12: Primer dígito del día leído
                lexema += c;
            }
            else
            {
                return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
            }
            break;

        case 12: // S12: Esperamos segundo dígito del día -> ¡FIN DE LA FECHA!
            if (std::isdigit(c))
            {
                lexema += c;
                posicion++;
                columna++; // Consumimos este último dígito manualmente
                return {lexema, TokenType::FECHA, tokenLinea, tokenColumna};
            }
            else
            {
                return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
            }
            break;

        // --- RUTA DE LA HORA ---
        case 7: // S7: Leímos ':', esperamos el primer dígito de los minutos
            if (std::isdigit(c))
            {
                estado = 9; // S9: Primer dígito de minutos leído
                lexema += c;
            }
            else
            {
                return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
            }
            break;

        case 9: // S9: Esperamos segundo dígito de los minutos -> ¡FIN DE LA HORA!
            if (std::isdigit(c))
            {
                lexema += c;
                posicion++;
                columna++; // Consumimos este último dígito manualmente
                return {lexema, TokenType::HORA, tokenLinea, tokenColumna};
            }
            else
            {
                return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
            }
            break;

            // --- RUTA DE CADENAS DE TEXTO (Comillas Dobles) ---
        case 13: // Estado para literales de texto
            if (c == '"')
            {
                lexema += c; // Agregamos la comilla final
                posicion++;
                columna++;

                // ¡Aquí hacemos la clasificación final!
                TokenType tipo = clasificarCadena(lexema);

                return {lexema, tipo, tokenLinea, tokenColumna};
            }
            else if (c == '\n')
            {
                return {lexema, TokenType::ERROR_LEXICO, tokenLinea, tokenColumna};
            }
            else
            {
                lexema += c;
            }
            break;
        case 14: // Nuestro estado personalizado para capturar palabras
            // Aceptamos letras, números y guion bajo (vital para "tipo_sangre" o "CADA_8_HORAS")
            if (std::isalnum(c) || c == '_')
            {
                lexema += c;
            }
            else
            {
                // Si leemos un espacio, salto de línea o símbolo (como '{' o ':'), la palabra terminó.
                // OJO: NO avanzamos 'posicion' ni 'columna' aquí.
                // Ese carácter extra que leímos le pertenece al siguiente token.

                TokenType tipo = clasificarPalabra(lexema);
                return {lexema, tipo, tokenLinea, tokenColumna};
            }
            break;
        }

        posicion++;
        columna++;
    }

    // Si el ciclo termina y no retornó nada, llegamos al final
    if (!lexema.empty())
    {
        // Manejar un posible token final que quedó a medias
    }

    return {"EOF", TokenType::FIN_ARCHIVO, linea, columna};
}
