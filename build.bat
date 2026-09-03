@echo off
REM Builds the Bangla compiler on Windows using g++ (MinGW).
REM If you don't have g++, install MinGW-w64 or use "Dev-C++" / "Code::Blocks".

g++ -std=c++17 -Iinclude -o bangla_compiler.exe src\lexer.cpp src\symboltable.cpp src\parser.cpp src\main.cpp

if %errorlevel% neq 0 (
    echo.
    echo Build FAILED.
    pause
    exit /b 1
)

echo.
echo Build succeeded: bangla_compiler.exe
pause
