cmake_minimum_required(VERSION 3.23)
project(Jack_Lex)

set(CMAKE_CXX_STANDARD 20)

add_executable(Jack_Lex
        src/JackTokenizer/JackTokenizer.cpp
        src/JackTokenizer/JackTokenizer.hpp
        src/main.cpp src/JackTokenizer/JackTokenizer.cpp
		src/SymbolTable/SymbolTable.cpp
		src/SymbolTable/SymbolTable.hpp src/CompilationEngine/CompilationEngine.cpp src/CompilationEngine/CompilationEngine.hpp
		src/VMWriter/VMWriter.cpp src/VMWriter/VMWriter.hpp)
