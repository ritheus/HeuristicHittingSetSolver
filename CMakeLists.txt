cmake_minimum_required(VERSION 3.10)
project(HittingSetSolver)

# Quellcodedateien für die ausführbare Datei
add_executable(HittingSetSolver main.cpp greedy.cpp)

# Include-Verzeichnisse
include_directories(
    ${PROJECT_SOURCE_DIR}
)