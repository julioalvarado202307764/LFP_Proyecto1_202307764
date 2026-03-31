#pragma once
#include <string>

enum class TokenType {
    HOSPITAL, PACIENTES, MEDICOS, CITAS, DIAGNOSTICOS,
    PACIENTE_ELEM, MEDICO_ELEM, CITA_ELEM, DIAGNOSTICO_ELEM,
    CADENA, NUMERO, FECHA, HORA, ID_CODIGO,
    ESPECIALIDAD, TIPO_SANGRE, DOSIS, SIMBOLO,
    ERROR_LEXICO, FIN_ARCHIVO
};

struct Token {
    std::string lexema;
    TokenType tipo;
    int linea;
    int columna;
};