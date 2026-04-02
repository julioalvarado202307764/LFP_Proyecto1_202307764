# MedLexer - Analizador de Lenguaje Médico (MedLang) 

**MedLexer** es una herramienta avanzada de análisis léxico diseñada para procesar el lenguaje **MedLang**, un formato estructurado para la gestión de datos hospitalarios. El sistema permite cargar archivos, validar sintaxis mediante un Autómata Finito Determinista (AFD) y generar reportes estadísticos visuales detallados.

---

##  Requisitos del Sistema

Para compilar y ejecutar este proyecto, es necesario contar con las siguientes herramientas instaladas y configuradas en las variables de entorno (PATH):

* **Compilador:** MinGW-w64 (GCC 13.1.0 o superior recomendado).
* **Framework:** Qt 6.11.0 (Componente MinGW 64-bit).
* **Gestor de Construcción:** CMake (Versión 3.16 o superior).
* **Visualización:** Graphviz (Para la generación de diagramas de red en formato PNG).

---

##  Instrucciones de Compilación (Paso a Paso)

Sigue estos comandos desde una terminal (PowerShell o CMD) situada en la raíz del proyecto para construir el ejecutable:

1. **Limpiar configuraciones previas (Opcional):**
   ```powershell
   Remove-Item -Recurse -Force build
2. **Configurar el proyecto con CMake:**
   Es vital especificar el generador de MinGW para evitar conflictos con otros compiladores:
   ```powershell
   cmake -G "MinGW Makefiles" -B build
3. **Compilar el binario:**
   ```powershell
   cmake --build build
4. **Ejecutar la aplicación:**
   ```powershell
   .\build\MedLexer.exe
 Guía de Uso
Carga de datos: Puedes escribir directamente en el editor principal o pegar el contenido de un archivo .med.

Análisis: Presiona el botón "Analizar Código". El sistema procesará el texto, llenando automáticamente las tablas de Tokens y Errores.

Gestión de Errores Léxicos: Si el sistema detecta símbolos ajenos al lenguaje (como @, $, &), no detendrá la ejecución. Los registrará en la tabla de Errores con su respectiva línea y columna para su fácil corrección.

Visualización de Reportes: Haz clic en "Abrir Reportes" para ver en tu navegador web los documentos HTML generados (listado de pacientes, carga de médicos, agenda de citas y gráficas estadísticas), además del diagrama de red del hospital.

 Estructura del Lenguaje (.med)
El analizador procesa bloques bajo la estructura jerárquica obligatoria de HOSPITAL. A continuación un ejemplo válido:

Plaintext
HOSPITAL {
   PACIENTES {
      paciente: "Ana Lopez" [edad: 34, tipo_sangre: "O+"]
   }
   MEDICOS {
      medico: "Dr. Roberto Juarez" [especialidad: CARDIOLOGIA, codigo: "MED-001"]
   }
   CITAS {
      cita: "Ana Lopez" con "Dr. Roberto Juarez" [fecha: 2026-03-15, hora: 09:00]
   }
   DIAGNOSTICOS {
      diagnostico: "Ana Lopez" [condicion: "Hipertension", medicamento: "Losartan", dosis: DIARIA]
   }
}
 Detalles Técnicos y Arquitectura
Análisis Léxico: Basado en el Método del Árbol y Tabla de Transiciones (FollowPos) para la construcción de un AFD robusto y sin ambigüedades.

Separación de Responsabilidades: Lógica de tokenización (LexicalAnalyzer), gestión de recuperación (ErrorManager), generación de salidas (ReportGenerator) e Interfaz Gráfica (MainWindow) separadas bajo principios de diseño limpio en C++ puro.

Manejo de Memoria: Uso eficiente de contenedores STL (std::vector) y manejo seguro de punteros en la jerarquía de objetos de Qt.

Desarrollado para el curso de Lenguajes Formales y de Programación - 2026.
