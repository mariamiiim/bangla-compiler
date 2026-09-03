#!/bin/bash
# Builds the Bangla compiler on Linux/macOS using g++.
set -e

g++ -std=c++17 -Iinclude -o bangla_compiler src/lexer.cpp src/symboltable.cpp src/parser.cpp src/main.cpp

echo "Build succeeded: ./bangla_compiler"
