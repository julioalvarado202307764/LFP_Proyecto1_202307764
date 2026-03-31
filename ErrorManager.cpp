#include "ErrorManager.h"

ErrorManager::ErrorManager() : contadorId(1) {}

void ErrorManager::agregarError(const std::string& lexema, const std::string& tipoError, 
                                const std::string& descripcion, int linea, int columna, 
                                const std::string& gravedad) {
    ErrorLexico nuevoError = {contadorId++, lexema, tipoError, descripcion, linea, columna, gravedad};
    listaErrores.push_back(nuevoError);
}

const std::vector<ErrorLexico>& ErrorManager::getErrores() const {
    return listaErrores;
}

bool ErrorManager::tieneErrores() const {
    return !listaErrores.empty();
}

void ErrorManager::limpiar() {
    listaErrores.clear();
    contadorId = 1;
}