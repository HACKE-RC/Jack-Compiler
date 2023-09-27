#include <iostream>
#include "JackTokenizer/JackTokenizer.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "CompilationEngine/CompilationEngine.hpp"

int main(int argc, char* argv[]){
    if (argc < 2){
        std::cerr << "Usage: " << argv[0] << " <file or folder path>" << std::endl;
        std::cerr << "The output will be stored in <filename>.jack.vm in the same folder." << std::endl;
        std::exit(-1);
    }

    CompilationEngine compiler(argv[1]);

    return 0;
}
