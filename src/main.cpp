#include <iostream>
#include "JackTokenizer/JackTokenizer.hpp"

int main(int argc, char* argv[]){
    JackTokenizer tokenizer(argv[1], "test.tst");
    tokenizer.cleanCode();
    tokenizer.tokenizeCode();

    return 0;
}
