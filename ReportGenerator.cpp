#include "ReportGenerator.h"
#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cstdlib>

ReportGenerator::ReportGenerator(const std::vector<Token>& tokens, const std::vector<ErrorLexico>& errores)
    : listaTokens(tokens), listaErrores(errores) {}

// --- MÉTODOS DE AYUDA PARA HTML (CSS Embebido) ---
std::string ReportGenerator::obtenerEncabezadoHTML(const std::string& titulo) {
    // Un diseño limpio con CSS embebido como pide el proyecto
    return "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n<title>" + titulo + "</title>\n"
           "<style>\n"
           "body { font-family: Arial, sans-serif; background-color: #f4f7f6; margin: 20px; }\n"
           "h1 { color: #2c3e50; text-align: center; }\n"
           "table { width: 100%; border-collapse: collapse; margin-top: 20px; background-color: #fff; }\n"
           "th, td { border: 1px solid #ddd; padding: 12px; text-align: left; }\n"
           "th { background-color: #34495e; color: white; }\n"
           "tr:nth-child(even) { background-color: #f2f2f2; }\n"
           ".error { color: #e74c3c; font-weight: bold; }\n"
           "</style>\n</head>\n<body>\n"
           "<h1>" + titulo + "</h1>\n";
}

std::string ReportGenerator::obtenerPieHTML() {
    return "</body>\n</html>";
}

// --- GENERACIÓN DE REPORTES ---

void ReportGenerator::generarReporteErrores() {
    // Creamos y abrimos el archivo
    std::ofstream archivo("Reporte_Errores.html");
    
    if (!archivo.is_open()) {
        std::cerr << "Error al crear el archivo Reporte_Errores.html\n";
        return;
    }

    archivo << obtenerEncabezadoHTML("Tabla de Errores Léxicos");

    if (listaErrores.empty()) {
        archivo << "<p style='text-align:center; color:green; font-size:18px;'><strong>¡Excelente! No se encontraron errores léxicos.</strong></p>\n";
    } else {
        // Estructura de la tabla de errores según el enunciado
        archivo << "<table>\n";
        archivo << "<tr><th>No.</th><th>Lexema</th><th>Tipo de Error</th><th>Descripción</th><th>Línea</th><th>Columna</th><th>Gravedad</th></tr>\n";

        for (const auto& error : listaErrores) {
            archivo << "<tr>\n"
                    << "<td>" << error.id << "</td>\n"
                    << "<td class='error'>" << error.lexema << "</td>\n"
                    << "<td>" << error.tipoError << "</td>\n"
                    << "<td>" << error.descripcion << "</td>\n"
                    << "<td>" << error.linea << "</td>\n"
                    << "<td>" << error.columna << "</td>\n"
                    << "<td>" << error.gravedad << "</td>\n"
                    << "</tr>\n";
        }
        archivo << "</table>\n";
    }

    archivo << obtenerPieHTML();
    archivo.close();
    std::cout << "Reporte de Errores generado con exito: Reporte_Errores.html\n";
}

// Esqueletos para los demás (los llenaremos en el siguiente paso)
void ReportGenerator::generarReportePacientes() { 
    // Estructura temporal para agrupar los datos del paciente [cite: 88, 89, 90]
    struct InfoPaciente {
        std::string edad = "-";
        std::string sangre = "-";
        std::string condicion = "Sin diagnóstico registrado";
        std::string medicamento = "-";
        std::string dosis = "";
        std::string estado = "SIN DIAG."; // Estado por defecto [cite: 91]
        std::string colorEstado = "orange";
    };

    std::map<std::string, InfoPaciente> mapaPacientes;

    // 1. Recorremos los tokens para extraer la información
    for (size_t i = 0; i < listaTokens.size(); i++) {
        
        // Si encontramos un paciente en el bloque PACIENTES
        if (listaTokens[i].tipo == TokenType::PACIENTE_ELEM) {
            std::string nombre = "";
            // El nombre siempre viene dos posiciones después (paciente -> : -> "Nombre")
            if (i + 2 < listaTokens.size() && listaTokens[i+2].tipo == TokenType::CADENA) {
                nombre = listaTokens[i+2].lexema;
                // Quitamos las comillas para tener el nombre limpio
                if (nombre.size() > 2) nombre = nombre.substr(1, nombre.size() - 2);
            }

            if (!nombre.empty()) {
                InfoPaciente info;
                // Escaneamos hacia adelante hasta encontrar el corchete de cierre ']'
                size_t j = i + 3;
                while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                    if (listaTokens[j].lexema == "edad" && j + 2 < listaTokens.size()) {
                        info.edad = listaTokens[j+2].lexema;
                    } else if (listaTokens[j].lexema == "tipo_sangre" && j + 2 < listaTokens.size()) {
                        std::string s = listaTokens[j+2].lexema;
                        if (s.size() > 2) s = s.substr(1, s.size() - 2);
                        info.sangre = s;
                    }
                    j++;
                }
                mapaPacientes[nombre] = info;
            }
        }
        
        // Si encontramos un diagnóstico, buscamos al paciente en el mapa y lo actualizamos
        else if (listaTokens[i].tipo == TokenType::DIAGNOSTICO_ELEM) {
            std::string nombre = "";
            if (i + 2 < listaTokens.size() && listaTokens[i+2].tipo == TokenType::CADENA) {
                nombre = listaTokens[i+2].lexema;
                if (nombre.size() > 2) nombre = nombre.substr(1, nombre.size() - 2);
            }

            // Si el paciente existe en nuestro mapa, le agregamos su diagnóstico
            if (!nombre.empty() && mapaPacientes.count(nombre)) {
                size_t j = i + 3;
                while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                    if (listaTokens[j].lexema == "condicion" && j + 2 < listaTokens.size()) {
                        std::string c = listaTokens[j+2].lexema;
                        if (c.size() > 2) c = c.substr(1, c.size() - 2);
                        mapaPacientes[nombre].condicion = c;
                    } else if (listaTokens[j].lexema == "medicamento" && j + 2 < listaTokens.size()) {
                        std::string m = listaTokens[j+2].lexema;
                        if (m.size() > 2) m = m.substr(1, m.size() - 2);
                        mapaPacientes[nombre].medicamento = m;
                    } else if (listaTokens[j].lexema == "dosis" && j + 2 < listaTokens.size()) {
                        mapaPacientes[nombre].dosis = listaTokens[j+2].lexema;
                    }
                    j++;
                }
                // Actualizamos el estado visual a ACTIVO (verde) [cite: 91]
                mapaPacientes[nombre].estado = "ACTIVO";
                mapaPacientes[nombre].colorEstado = "green";
            }
        }
    }

    // 2. Generar el archivo HTML
    std::ofstream archivo("Reporte_Pacientes.html");
    if (!archivo.is_open()) return;

    archivo << obtenerEncabezadoHTML("Historial de Pacientes");
    archivo << "<table>\n";
    archivo << "<tr><th>Paciente</th><th>Edad</th><th>Sangre</th><th>Diagnóstico activo</th><th>Medicamento / Dosis</th><th>Estado</th></tr>\n";

    for (const auto& par : mapaPacientes) {
        std::string medDosis = (par.second.medicamento == "-") ? "-" : (par.second.medicamento + " / " + par.second.dosis);
        
        archivo << "<tr>\n"
                << "<td>" << par.first << "</td>\n"
                << "<td>" << par.second.edad << "</td>\n"
                << "<td>" << par.second.sangre << "</td>\n"
                << "<td>" << par.second.condicion << "</td>\n"
                << "<td>" << medDosis << "</td>\n"
                << "<td style='color: white; background-color: " << par.second.colorEstado << "; text-align: center; font-weight: bold;'>" 
                << par.second.estado << "</td>\n"
                << "</tr>\n";
    }

    archivo << "</table>\n";
    archivo << obtenerPieHTML();
    archivo.close();
    std::cout << "Reporte de Pacientes generado con exito: Reporte_Pacientes.html\n";
 }

void ReportGenerator::generarReporteMedicos() {
    // Estructura para agrupar los datos del médico
    struct InfoMedico {
        std::string codigo = "-";
        std::string especialidad = "-";
        int citasProg = 0;
        std::set<std::string> pacientesUnicos; // El 'set' evita duplicados automáticamente
        std::string nivelCarga = "BAJA";
        std::string colorCarga = "blue";
    };

    std::map<std::string, InfoMedico> mapaMedicos;

    // 1. Recorremos los tokens para extraer la información
    for (size_t i = 0; i < listaTokens.size(); i++) {
        
        // A. Extraemos el perfil del médico desde el bloque MEDICOS
        if (listaTokens[i].tipo == TokenType::MEDICO_ELEM) {
            std::string nombreMedico = "";
            if (i + 2 < listaTokens.size() && listaTokens[i+2].tipo == TokenType::CADENA) {
                nombreMedico = listaTokens[i+2].lexema;
                if (nombreMedico.size() > 2) nombreMedico = nombreMedico.substr(1, nombreMedico.size() - 2);
            }

            if (!nombreMedico.empty()) {
                InfoMedico info;
                size_t j = i + 3;
                while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                    if (listaTokens[j].lexema == "especialidad" && j + 2 < listaTokens.size()) {
                        info.especialidad = listaTokens[j+2].lexema;
                    } else if (listaTokens[j].lexema == "codigo" && j + 2 < listaTokens.size()) {
                        std::string cod = listaTokens[j+2].lexema;
                        if (cod.size() > 2) cod = cod.substr(1, cod.size() - 2); // Quitamos comillas al ID
                        info.codigo = cod;
                    }
                    j++;
                }
                mapaMedicos[nombreMedico] = info;
            }
        }
        
        // B. Contamos sus citas y pacientes desde el bloque CITAS
        else if (listaTokens[i].tipo == TokenType::CITA_ELEM) {
            // Estructura esperada: cita : "Paciente" con "Medico" [ ... ]
            if (i + 4 < listaTokens.size() && listaTokens[i+2].tipo == TokenType::CADENA && listaTokens[i+4].tipo == TokenType::CADENA) {
                
                std::string nombrePaciente = listaTokens[i+2].lexema;
                std::string nombreMedico = listaTokens[i+4].lexema;
                
                if (nombrePaciente.size() > 2) nombrePaciente = nombrePaciente.substr(1, nombrePaciente.size() - 2);
                if (nombreMedico.size() > 2) nombreMedico = nombreMedico.substr(1, nombreMedico.size() - 2);

                // Si el médico existe, le sumamos la cita y agregamos el paciente al Set
                if (mapaMedicos.count(nombreMedico)) {
                    mapaMedicos[nombreMedico].citasProg++;
                    mapaMedicos[nombreMedico].pacientesUnicos.insert(nombrePaciente);
                }
            }
        }
    }

    // 2. Calcular los niveles de carga según los umbrales sugeridos en el PDF
    for (auto& par : mapaMedicos) {
        int citas = par.second.citasProg;
        if (citas >= 9) {
            par.second.nivelCarga = "SATURADA";
            par.second.colorCarga = "red"; // SATURADA (rojo) [cite: 105]
        } else if (citas >= 7) {
            par.second.nivelCarga = "ALTA";
            par.second.colorCarga = "orange"; // ALTA (naranja) [cite: 105]
        } else if (citas >= 4) {
            par.second.nivelCarga = "NORMAL";
            par.second.colorCarga = "green"; // NORMAL (verde) [cite: 105]
        } else {
            par.second.nivelCarga = "BAJA";
            par.second.colorCarga = "blue"; // BAJA (azul) [cite: 105]
        }
    }

    // 3. Generar el archivo HTML
    std::ofstream archivo("Reporte_Medicos.html");
    if (!archivo.is_open()) return;

    archivo << obtenerEncabezadoHTML("Carga de Médicos por Especialidad");
    archivo << "<table>\n";
    archivo << "<tr><th>Médico</th><th>Código</th><th>Especialidad</th><th>Citas Prog.</th><th>Pacientes</th><th>Nivel de Carga</th></tr>\n";

    for (const auto& par : mapaMedicos) {
        archivo << "<tr>\n"
                << "<td>" << par.first << "</td>\n"
                << "<td>" << par.second.codigo << "</td>\n"
                << "<td>" << par.second.especialidad << "</td>\n"
                << "<td>" << par.second.citasProg << "</td>\n"
                << "<td>" << par.second.pacientesUnicos.size() << "</td>\n"
                << "<td style='color: white; background-color: " << par.second.colorCarga << "; text-align: center; font-weight: bold;'>" 
                << par.second.nivelCarga << "</td>\n"
                << "</tr>\n";
    }

    archivo << "</table>\n";
    archivo << obtenerPieHTML();
    archivo.close();
    std::cout << "Reporte de Medicos generado con exito: Reporte_Medicos.html\n";
}

void ReportGenerator::generarReporteCitas() {
    // Estructura para almacenar cada cita individual
    struct InfoCita {
        std::string fecha;
        std::string hora;
        std::string paciente;
        std::string medico;
        std::string especialidad;
        std::string estado = "CONFIRMADA";
        std::string colorFondo = "#d5f5e3"; // Verde claro por defecto
    };

    std::vector<InfoCita> listaCitas;
    std::map<std::string, std::string> mapaEspecialidades; // Para cruzar Médico -> Especialidad

    // 1. Primer barrido: Extraemos las especialidades de los médicos
    for (size_t i = 0; i < listaTokens.size(); i++) {
        if (listaTokens[i].tipo == TokenType::MEDICO_ELEM) {
            std::string nombreMedico = "";
            if (i + 2 < listaTokens.size() && listaTokens[i+2].tipo == TokenType::CADENA) {
                nombreMedico = listaTokens[i+2].lexema;
                if (nombreMedico.size() > 2) nombreMedico = nombreMedico.substr(1, nombreMedico.size() - 2);
            }
            if (!nombreMedico.empty()) {
                size_t j = i + 3;
                while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                    if (listaTokens[j].lexema == "especialidad" && j + 2 < listaTokens.size()) {
                        mapaEspecialidades[nombreMedico] = listaTokens[j+2].lexema;
                    }
                    j++;
                }
            }
        }
    }

    // 2. Segundo barrido: Extraemos todas las citas
    for (size_t i = 0; i < listaTokens.size(); i++) {
        if (listaTokens[i].tipo == TokenType::CITA_ELEM) {
            InfoCita nuevaCita;
            
            // Extraer paciente y médico (cita: "Paciente" con "Medico")
            if (i + 4 < listaTokens.size() && listaTokens[i+2].tipo == TokenType::CADENA && listaTokens[i+4].tipo == TokenType::CADENA) {
                nuevaCita.paciente = listaTokens[i+2].lexema;
                nuevaCita.medico = listaTokens[i+4].lexema;
                
                if (nuevaCita.paciente.size() > 2) nuevaCita.paciente = nuevaCita.paciente.substr(1, nuevaCita.paciente.size() - 2);
                if (nuevaCita.medico.size() > 2) nuevaCita.medico = nuevaCita.medico.substr(1, nuevaCita.medico.size() - 2);
                
                // Le asignamos su especialidad cruzando datos con el mapa
                nuevaCita.especialidad = mapaEspecialidades.count(nuevaCita.medico) ? mapaEspecialidades[nuevaCita.medico] : "NO REGISTRADA";
            }

            // Extraer fecha y hora escaneando hasta el ']'
            size_t j = i + 5;
            while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                if (listaTokens[j].lexema == "fecha" && j + 2 < listaTokens.size()) {
                    nuevaCita.fecha = listaTokens[j+2].lexema;
                } else if (listaTokens[j].lexema == "hora" && j + 2 < listaTokens.size()) {
                    nuevaCita.hora = listaTokens[j+2].lexema;
                }
                j++;
            }
            // Solo agregar la cita si realmente extrajo al paciente y al médico
            if (!nuevaCita.paciente.empty() && !nuevaCita.medico.empty()) {
                listaCitas.push_back(nuevaCita);
            }
        }
    }

    // 3. Ordenar cronológicamente (primero por fecha, luego por hora) 
    std::sort(listaCitas.begin(), listaCitas.end(), [](const InfoCita& a, const InfoCita& b) {
        if (a.fecha != b.fecha) return a.fecha < b.fecha;
        return a.hora < b.hora;
    });

    // 4. Detección de Conflictos y asignación de Estado [cite: 113, 114, 119]
    std::string fechaActual = "2026-03-31"; // Fecha base para "Pendientes"

    for (size_t i = 0; i < listaCitas.size(); i++) {
        bool tieneConflicto = false;
        
        // Comparamos esta cita con todas las demás para ver si chocan en Médico, Fecha y Hora [cite: 124]
        for (size_t k = 0; k < listaCitas.size(); k++) {
            if (i != k && 
                listaCitas[i].medico == listaCitas[k].medico &&
                listaCitas[i].fecha == listaCitas[k].fecha && 
                listaCitas[i].hora == listaCitas[k].hora) {
                tieneConflicto = true;
                break;
            }
        }

        if (tieneConflicto) {
            listaCitas[i].estado = "CONFLICTO";
            listaCitas[i].colorFondo = "#f5b7b1"; // Rojo claro para filas con conflicto [cite: 114]
        } else if (listaCitas[i].fecha > fechaActual) {
            listaCitas[i].estado = "PENDIENTE";
            listaCitas[i].colorFondo = "#fdebd0"; // Naranja claro para fechas futuras 
        }
    }

    // 5. Generar el archivo HTML [cite: 82]
    std::ofstream archivo("Reporte_Citas.html");
    if (!archivo.is_open()) return;

    archivo << obtenerEncabezadoHTML("Agenda de Citas");
    archivo << "<table>\n";
    archivo << "<tr><th>Fecha</th><th>Hora</th><th>Paciente</th><th>Médico</th><th>Especialidad</th><th>Estado</th></tr>\n";

    for (const auto& cita : listaCitas) {
        // Aquí aplicamos el color de fondo a toda la fila (tr) tal como pide el ejemplo [cite: 114, 120]
        archivo << "<tr style='background-color: " << cita.colorFondo << ";'>\n"
                << "<td>" << cita.fecha << "</td>\n"
                << "<td>" << cita.hora << "</td>\n"
                << "<td>" << cita.paciente << "</td>\n"
                << "<td>" << cita.medico << "</td>\n"
                << "<td>" << cita.especialidad << "</td>\n"
                << "<td style='font-weight: bold;'>" << cita.estado << "</td>\n"
                << "</tr>\n";
    }

    archivo << "</table>\n";
    archivo << obtenerPieHTML();
    archivo.close();
    std::cout << "Reporte de Citas generado con exito: Reporte_Citas.html\n";
}

void ReportGenerator::generarReporteEstadistico() {
    // --- VARIABLES PARA SECCIÓN A (KPIs) ---
    int totalPacientes = 0, totalMedicos = 0, totalCitas = 0;
    int sumaEdades = 0;
    std::set<std::string> pacientesConDiag;
    std::map<std::string, int> conteoMedicamentos;
    
    // Para contar conflictos reutilizamos una estructura simple
    struct CitaSimple { std::string medico, fecha, hora; };
    std::vector<CitaSimple> listaCitasSimple;
    int citasConConflicto = 0;

    // --- VARIABLES PARA SECCIÓN B (Especialidades) ---
    struct StatsEspecialidad {
        int numMedicos = 0;
        int citasTotales = 0;
        std::set<std::string> pacientesDistintos;
    };
    std::map<std::string, StatsEspecialidad> statsEspecialidades;
    std::map<std::string, std::string> medicoAEspecialidad;

    // 1. BARRIDO DE EXTRACCIÓN DE DATOS
    for (size_t i = 0; i < listaTokens.size(); i++) {
        
        // Pacientes
        if (listaTokens[i].tipo == TokenType::PACIENTE_ELEM) {
            totalPacientes++;
            size_t j = i + 3;
            while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                if (listaTokens[j].lexema == "edad" && j + 2 < listaTokens.size()) {
                    sumaEdades += std::stoi(listaTokens[j+2].lexema);
                }
                j++;
            }
        }
        
        // Médicos
        else if (listaTokens[i].tipo == TokenType::MEDICO_ELEM) {
            totalMedicos++;
            std::string nombreMedico = "";
            if (i + 2 < listaTokens.size() && listaTokens[i+2].tipo == TokenType::CADENA) {
                nombreMedico = listaTokens[i+2].lexema;
                if (nombreMedico.size() > 2) nombreMedico = nombreMedico.substr(1, nombreMedico.size() - 2);
            }
            if (!nombreMedico.empty()) {
                size_t j = i + 3;
                while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                    if (listaTokens[j].lexema == "especialidad" && j + 2 < listaTokens.size()) {
                        std::string esp = listaTokens[j+2].lexema;
                        medicoAEspecialidad[nombreMedico] = esp;
                        statsEspecialidades[esp].numMedicos++; // Sumamos un médico a esta especialidad
                    }
                    j++;
                }
            }
        }
        
        // Citas
        else if (listaTokens[i].tipo == TokenType::CITA_ELEM) {
            totalCitas++;
            std::string paciente = "", medico = "", fecha = "", hora = "";
            if (i + 4 < listaTokens.size()) {
                paciente = listaTokens[i+2].lexema;
                medico = listaTokens[i+4].lexema;
                if (paciente.size() > 2) paciente = paciente.substr(1, paciente.size() - 2);
                if (medico.size() > 2) medico = medico.substr(1, medico.size() - 2);
            }
            
            size_t j = i + 5;
            while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                if (listaTokens[j].lexema == "fecha" && j + 2 < listaTokens.size()) fecha = listaTokens[j+2].lexema;
                else if (listaTokens[j].lexema == "hora" && j + 2 < listaTokens.size()) hora = listaTokens[j+2].lexema;
                j++;
            }
            
            listaCitasSimple.push_back({medico, fecha, hora});
            
            // Sumar a la especialidad
            std::string esp = medicoAEspecialidad[medico];
            if (!esp.empty()) {
                statsEspecialidades[esp].citasTotales++;
                statsEspecialidades[esp].pacientesDistintos.insert(paciente);
            }
        }
        
        // Diagnósticos
        else if (listaTokens[i].tipo == TokenType::DIAGNOSTICO_ELEM) {
            if (i + 2 < listaTokens.size() && listaTokens[i+2].tipo == TokenType::CADENA) {
                std::string paciente = listaTokens[i+2].lexema;
                pacientesConDiag.insert(paciente);
            }
            size_t j = i + 3;
            while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                if (listaTokens[j].lexema == "medicamento" && j + 2 < listaTokens.size()) {
                    std::string med = listaTokens[j+2].lexema;
                    if (med.size() > 2) med = med.substr(1, med.size() - 2);
                    conteoMedicamentos[med]++;
                }
                j++;
            }
        }
    }

    // 2. CÁLCULOS COMPLEJOS
    // A. Conflictos
    for (size_t i = 0; i < listaCitasSimple.size(); i++) {
        for (size_t k = i + 1; k < listaCitasSimple.size(); k++) {
            if (listaCitasSimple[i].medico == listaCitasSimple[k].medico &&
                listaCitasSimple[i].fecha == listaCitasSimple[k].fecha &&
                listaCitasSimple[i].hora == listaCitasSimple[k].hora) {
                citasConConflicto++;
            }
        }
    }

    // B. Promedios y porcentajes
    double promedioEdad = totalPacientes > 0 ? (double)sumaEdades / totalPacientes : 0.0;
    int porcentajeDiag = totalPacientes > 0 ? (pacientesConDiag.size() * 100) / totalPacientes : 0;

    // C. Top Medicamento
    std::string topMedicamento = "Ninguno";
    int maxMed = 0;
    for (const auto& par : conteoMedicamentos) {
        if (par.second > maxMed) { maxMed = par.second; topMedicamento = par.first; }
    }

    // D. Top Especialidad
    std::string topEspecialidad = "Ninguna";
    int maxCitasEsp = 0;
    for (const auto& par : statsEspecialidades) {
        if (par.second.citasTotales > maxCitasEsp) {
            maxCitasEsp = par.second.citasTotales;
            topEspecialidad = par.first;
        }
    }

    // 3. GENERACIÓN DEL HTML
    std::ofstream archivo("Reporte_Estadistico.html");
    if (!archivo.is_open()) return;

    archivo << obtenerEncabezadoHTML("Estadístico General del Hospital");
    
    // SECCIÓN A: KPIs
    archivo << "<h2>Sección A - Indicadores Clave</h2>\n";
    archivo << "<table>\n<tr><th>Indicador</th><th>Valor</th></tr>\n";
    archivo << "<tr><td>Nombre del hospital</td><td>Hospital General San Carlos</td></tr>\n";
    archivo << "<tr><td>Total de pacientes registrados</td><td>" << totalPacientes << "</td></tr>\n";
    archivo << "<tr><td>Total de médicos activos</td><td>" << totalMedicos << "</td></tr>\n";
    archivo << "<tr><td>Total de citas programadas</td><td>" << totalCitas << "</td></tr>\n";
    archivo << "<tr><td>Citas con conflicto de horario</td><td style='color:red; font-weight:bold;'>" << citasConConflicto << "</td></tr>\n";
    archivo << "<tr><td>Pacientes con diagnóstico activo</td><td>" << pacientesConDiag.size() << " de " << totalPacientes << " (" << porcentajeDiag << "%)</td></tr>\n";
    archivo << "<tr><td>Medicamento más prescrito</td><td>" << topMedicamento << " (" << maxMed << " pacientes)</td></tr>\n";
    archivo << "<tr><td>Especialidad con mayor carga</td><td>" << topEspecialidad << " (" << maxCitasEsp << " citas)</td></tr>\n";
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << promedioEdad;
    archivo << "<tr><td>Promedio de edad</td><td>" << ss.str() << " años</td></tr>\n";
    archivo << "</table>\n";

    // SECCIÓN B: Distribución por Especialidad
    archivo << "<h2>Sección B - Distribución de Carga</h2>\n";
    archivo << "<table>\n<tr><th>Especialidad</th><th>Médicos</th><th>Citas</th><th>Pacientes</th><th>Ocupación</th></tr>\n";

    for (const auto& par : statsEspecialidades) {
        int ocupacion = totalCitas > 0 ? (par.second.citasTotales * 100) / totalCitas : 0;
        std::string colorBarra = ocupacion > 80 ? "#e74c3c" : "#3498db"; // Rojo si > 80%, azul si no 
        
        archivo << "<tr>\n"
                << "<td>" << par.first << "</td>\n"
                << "<td>" << par.second.numMedicos << "</td>\n"
                << "<td>" << par.second.citasTotales << "</td>\n"
                << "<td>" << par.second.pacientesDistintos.size() << "</td>\n"
                << "<td>\n"
                << "  <div style='width: 100%; background-color: #e0e0e0; border-radius: 4px;'>\n"
                << "    <div style='width: " << ocupacion << "%; background-color: " << colorBarra << "; color: white; text-align: center; border-radius: 4px; font-size: 12px; padding: 2px 0;'>\n"
                << "      " << ocupacion << "%\n"
                << "    </div>\n"
                << "  </div>\n"
                << "</td>\n"
                << "</tr>\n";
    }

    archivo << "</table>\n";
    archivo << obtenerPieHTML();
    archivo.close();
    std::cout << "Reporte Estadistico generado con exito: Reporte_Estadistico.html\n";
}

void ReportGenerator::generarGraphviz() {
    // Función auxiliar Lambda para limpiar nombres y usarlos como IDs en Graphviz
    auto cleanID = [](std::string str) {
        std::string id = "";
        for (char c : str) {
            if (std::isalnum(c)) id += c;
            else if (c == ' ' || c == '-') id += "_";
        }
        return id;
    };

    // Estructuras temporales para guardar los nodos y aristas
    std::map<std::string, std::string> pacientesNodos; // ID -> Label
    std::map<std::string, std::string> medicosNodos;   // ID -> Label
    
    struct CitaGraph { std::string idPac, idMed, label; };
    std::vector<CitaGraph> aristasCitas;

    struct DiagGraph { std::string idDiag, idPac, labelNodo; };
    std::vector<DiagGraph> nodosDiagnosticos;

    int diagCounter = 1; // Para darle un ID único a cada nodo de diagnóstico

    // BARRIDO DE DATOS
    for (size_t i = 0; i < listaTokens.size(); i++) {
        // Extraer Pacientes
        if (listaTokens[i].tipo == TokenType::PACIENTE_ELEM) {
            if (i + 2 < listaTokens.size() && listaTokens[i+2].tipo == TokenType::CADENA) {
                std::string nombre = listaTokens[i+2].lexema;
                if (nombre.size() > 2) nombre = nombre.substr(1, nombre.size() - 2);
                
                std::string sangre = "-";
                size_t j = i + 3;
                while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                    if (listaTokens[j].lexema == "tipo_sangre" && j + 2 < listaTokens.size()) {
                        sangre = listaTokens[j+2].lexema;
                        if (sangre.size() > 2) sangre = sangre.substr(1, sangre.size() - 2);
                    }
                    j++;
                }
                pacientesNodos[cleanID(nombre)] = nombre + "\\nSangre: " + sangre;
            }
        }
        // Extraer Médicos
        else if (listaTokens[i].tipo == TokenType::MEDICO_ELEM) {
            if (i + 2 < listaTokens.size() && listaTokens[i+2].tipo == TokenType::CADENA) {
                std::string nombre = listaTokens[i+2].lexema;
                if (nombre.size() > 2) nombre = nombre.substr(1, nombre.size() - 2);
                
                std::string especialidad = "-", codigo = "-";
                size_t j = i + 3;
                while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                    if (listaTokens[j].lexema == "especialidad" && j + 2 < listaTokens.size()) especialidad = listaTokens[j+2].lexema;
                    else if (listaTokens[j].lexema == "codigo" && j + 2 < listaTokens.size()) {
                        codigo = listaTokens[j+2].lexema;
                        if (codigo.size() > 2) codigo = codigo.substr(1, codigo.size() - 2);
                    }
                    j++;
                }
                medicosNodos[cleanID(nombre)] = nombre + "\\n" + codigo + " | " + especialidad;
            }
        }
        // Extraer Citas (Aristas)
        else if (listaTokens[i].tipo == TokenType::CITA_ELEM) {
            if (i + 4 < listaTokens.size()) {
                std::string paciente = listaTokens[i+2].lexema;
                std::string medico = listaTokens[i+4].lexema;
                if (paciente.size() > 2) paciente = paciente.substr(1, paciente.size() - 2);
                if (medico.size() > 2) medico = medico.substr(1, medico.size() - 2);
                
                std::string fecha = "-", hora = "-";
                size_t j = i + 5;
                while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                    if (listaTokens[j].lexema == "fecha" && j + 2 < listaTokens.size()) fecha = listaTokens[j+2].lexema;
                    else if (listaTokens[j].lexema == "hora" && j + 2 < listaTokens.size()) hora = listaTokens[j+2].lexema;
                    j++;
                }
                aristasCitas.push_back({cleanID(paciente), cleanID(medico), fecha + "\\n" + hora});
            }
        }
        // Extraer Diagnósticos (Nodos y Aristas)
        else if (listaTokens[i].tipo == TokenType::DIAGNOSTICO_ELEM) {
            if (i + 2 < listaTokens.size()) {
                std::string paciente = listaTokens[i+2].lexema;
                if (paciente.size() > 2) paciente = paciente.substr(1, paciente.size() - 2);
                
                std::string condicion = "-", med = "-", dosis = "-";
                size_t j = i + 3;
                while (j < listaTokens.size() && listaTokens[j].lexema != "]") {
                    if (listaTokens[j].lexema == "condicion" && j + 2 < listaTokens.size()) condicion = listaTokens[j+2].lexema.substr(1, listaTokens[j+2].lexema.size()-2);
                    else if (listaTokens[j].lexema == "medicamento" && j + 2 < listaTokens.size()) med = listaTokens[j+2].lexema.substr(1, listaTokens[j+2].lexema.size()-2);
                    else if (listaTokens[j].lexema == "dosis" && j + 2 < listaTokens.size()) dosis = listaTokens[j+2].lexema;
                    j++;
                }
                
                std::string idDiag = "diag_" + std::to_string(diagCounter++);
                std::string label = condicion + "\\n" + med + " / " + dosis;
                nodosDiagnosticos.push_back({idDiag, cleanID(paciente), label});
            }
        }
    }

    // GENERAR EL ARCHIVO DOT
    std::ofstream archivo("hospital.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph Hospital {\n";
    archivo << "  rankdir=TB; node [shape=box, style=filled, fontname=\"Arial\"];\n\n";

    // Nodos Principales
    archivo << "  H [label=\"Hospital General San Carlos\", fillcolor=\"#1A4731\", fontcolor=white, shape=ellipse];\n";
    archivo << "  P [label=\"PACIENTES\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    archivo << "  M [label=\"MEDICOS\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    archivo << "  C [label=\"CITAS\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    archivo << "  D [label=\"DIAGNOSTICOS\", fillcolor=\"#2E7D52\", fontcolor=white];\n\n";

    // Conexiones de la Raíz a las Secciones
    archivo << "  H -> P; H -> M; H -> C; H -> D;\n\n";

    // Imprimir Nodos de Pacientes
    for (const auto& par : pacientesNodos) {
        archivo << "  " << par.first << " [label=\"" << par.second << "\", fillcolor=\"#D4EDDA\"];\n";
        archivo << "  P -> " << par.first << ";\n";
    }
    archivo << "\n";

    // Imprimir Nodos de Médicos
    for (const auto& par : medicosNodos) {
        archivo << "  " << par.first << " [label=\"" << par.second << "\", fillcolor=\"#D6EAF8\"];\n";
        archivo << "  M -> " << par.first << ";\n";
    }
    archivo << "\n";

    // Imprimir Nodos de Diagnósticos y conectarlos al Paciente
    for (const auto& diag : nodosDiagnosticos) {
        archivo << "  " << diag.idDiag << " [label=\"" << diag.labelNodo << "\", fillcolor=\"#FDEBD0\"];\n";
        archivo << "  D -> " << diag.idDiag << ";\n";
        archivo << "  " << diag.idDiag << " -> " << diag.idPac << " [label=\"diagnóstico activo\", color=\"#C0392B\"];\n";
    }
    archivo << "\n";

    // Imprimir Aristas de Citas (Paciente -> Médico)
    for (const auto& cita : aristasCitas) {
        // El enunciado pide conectar CITAS al paciente y al médico, o directo Paciente -> Médico. 
        // Usaremos Paciente -> Médico con etiqueta de fecha y hora, que es más legible.
        archivo << "  " << cita.idPac << " -> " << cita.idMed << " [label=\"" << cita.label << "\", color=\"#E67E22\", style=dashed];\n";
    }

    archivo << "}\n";
    archivo.close();
    std::cout << "Diagrama Graphviz generado con exito: hospital.dot\n";

    // --- MAGIA AUTOMÁTICA: Generar el PNG ---
    std::cout << "Generando imagen PNG...\n";
    int resultado = std::system("dot -Tpng hospital.dot -o hospital.png");
    
    if (resultado == 0) {
        std::cout << "Imagen PNG generada con exito: hospital.png\n";
    } else {
        std::cout << "Error al generar PNG. Verifica que Graphviz (dot) este en las variables de entorno.\n";
    }
    std::cout << "Diagrama Graphviz generado con exito: hospital.dot\n";
}