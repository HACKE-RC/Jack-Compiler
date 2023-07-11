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
    CompilationEngine compiler("C:\\Users\\MODX\\Documents\\nand2tetris\\projects\\11\\ConvertToBin\\Main.jack");
//    compiler.

    return 0;
}
