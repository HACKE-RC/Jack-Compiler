#include <iostream>
#include "JackTokenizer/JackTokenizer.hpp"

int main(int argc, char* argv[]){
    if (argc < 2){
        return -1;
    }
    JackTokenizer tokenizer(argv[1], "test.tst");
    tokenizer.cleanCode();
    tokenizer.tokenizeCode();
    CODE vec;
//    JackTokenizer::test();
//    std::string s = "new(int Ax, int Ay, int Asize)";
//    JackTokenizer::parseFuncParams(s, vec);
    return 0;
}
