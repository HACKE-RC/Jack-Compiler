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
//    std::string k = CompilationEngine::removeBrackets("do Main.fillMemory(8001, 15, -1);");
//
//    auto s = compiler.prioritizeBrackets(k);
//    compiler.compileExpressionList(s);
//    auto z = compiler.splitString(s, ' ');
//
//    for (auto i : z){
//        i.erase(std::remove(i.begin(), i.end(), ','), i.end());
//        compiler.compileExpression(i);
//        CompilationEngine::getExpressionVector(i);
//    }
//
//    compiler.callSubroutine("do Main.fillMemory();");
//    for (auto o: compiler.vmCode){
//        std::cout << o << std::endl;
//    }
//    compiler.compileDo("do Main.fillMemory(8001, 15, -1);");

    //    compiler.

    return 0;
}
