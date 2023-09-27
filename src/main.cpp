#include <iostream>
#include "JackTokenizer/JackTokenizer.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "CompilationEngine/CompilationEngine.hpp"

int main(int argc, char* argv[]){
    if (argc < 2){
        std::cerr << "Usage: jcomp.exe <file>" << std::endl;
    }

    CompilationEngine compiler(argv[1]);

    return 0;
}
