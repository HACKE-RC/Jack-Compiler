cmake_minimum_required(VERSION 3.23)
project(Jack_Lex)

set(CMAKE_CXX_STANDARD 20)

add_executable(Jack_Lex
        src/JackTokenizer/JackTokenizer.cpp
        src/JackTokenizer/JackTokenizer.hpp
        src/pugixml/pugixml.cpp
        src/pugixml/pugixml.hpp
        src/main.cpp)
