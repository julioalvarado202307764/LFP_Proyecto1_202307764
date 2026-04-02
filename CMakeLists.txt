cmake_minimum_required(VERSION 3.16)
project(MedLexer VERSION 1.0 LANGUAGES CXX)

# Configuración estricta para C++17 (Requisito del proyecto)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Variables mágicas de Qt para manejar las macros (Q_OBJECT) automáticamente
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Le decimos a CMake que busque la instalación de Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Agregamos TODOS los archivos de tu proyecto al ejecutable
add_executable(MedLexer
    main.cpp
    MainWindow.cpp
    LexicalAnalyzer.cpp
    ErrorManager.cpp
    ReportGenerator.cpp
)

# Vinculamos la interfaz gráfica (Widgets) a tu ejecutable
target_link_libraries(MedLexer PRIVATE Qt6::Widgets)