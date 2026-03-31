#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "Token.h"
#include "ErrorManager.h"

class ReportGenerator {
private:
    std::vector<Token> listaTokens;
    std::vector<ErrorLexico> listaErrores;

    // Métodos privados de ayuda para crear el cascarón del HTML
    std::string obtenerEncabezadoHTML(const std::string& titulo);
    std::string obtenerPieHTML();

public:
    // El constructor recibe copias o referencias de los datos ya procesados
    ReportGenerator(const std::vector<Token>& tokens, const std::vector<ErrorLexico>& errores);

    // Los 4 reportes principales
    void generarReportePacientes();
    void generarReporteMedicos();
    void generarReporteCitas();
    void generarReporteEstadistico();
    
    // Reporte de errores y Graphviz
    void generarReporteErrores();
    void generarGraphviz();
};