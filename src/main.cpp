#include <iostream>
#include "JackTokenizer/JackTokenizer.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "CompilationEngine/CompilationEngine.hpp"

int main(int argc, char* argv[]){
    if (argc < 2){
        return -1;
    }


//    JackTokenizer tokenizer(argv[1], "test.tst");
//    tokenizer.cleanCode();
//    tokenizer.tokenizeAllCode();
    CompilationEngine compiler(argv[1]);
//    compiler.depthSplit("(((y + size) < 254) & ((x + size) < 510))");
    auto s = compiler.getParameterStrings(R"(do Output.printString("Score: 0", 6, "alpha", 5);)");

    return 0;
}
