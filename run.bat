@echo off
REM Exit on error
setlocal enabledelayedexpansion

REM Create build folder if it doesn't exist
if not exist build mkdir build
cd build

REM Run CMake configuration
cmake ..

REM Build the project
cmake --build .

REM Run the executable
.\Debug\vector-db.exe