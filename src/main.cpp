#include <iostream>
#include "JackTokenizer/JackTokenizer.hpp"

int main(int argc, char* argv[]){
    if (argc < 2){
        return -1;
    }
    JackTokenizer tokenizer(argv[1], "test.tst");
    tokenizer.cleanCode();
    tokenizer.tokenizeCode();

    return 0;
}
