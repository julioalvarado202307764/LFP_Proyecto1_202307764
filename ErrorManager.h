#pragma once
#include <string>
#include <vector>

// Estructura del error
struct ErrorLexico {
    int id;
    std::string lexema;
    std::string tipoError;
    std::string descripcion;
    int linea;
    int columna;
    std::string gravedad;
};

// Declaración de la clase
class ErrorManager {
private:
    std::vector<ErrorLexico> listaErrores;
    int contadorId;

public:
    ErrorManager();
    void agregarError(const std::string& lexema, const std::string& tipoError, 
                      const std::string& descripcion, int linea, int columna, 
                      const std::string& gravedad = "ERROR");
    const std::vector<ErrorLexico>& getErrores() const;
    bool tieneErrores() const;
    void limpiar();
};