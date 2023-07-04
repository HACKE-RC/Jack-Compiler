#include "CompilationEngine.hpp"
#include <utility>


CompilationEngine::CompilationEngine(std::string fName) {
    JackTokenizer tokenizer(std::move(fName), "test.tst");
    tokenizer.cleanCode();
    m_code = tokenizer.getAllCodeVector();

    m_currentLine = 0;

    temp_tokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
    m_currentLine++;

    if (temp_tokens[0] == "class") {
        compileClass();
    }
}

void CompilationEngine::compileClass() {
    temp_tokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
    m_currentLine++;

    if (JackTokenizer::isValid(validVarDecs, temp_tokens[0])) {
        compileClassVarDec(temp_tokens);
        compileSubroutine();
//        classSymbolTable.display();

    }
}


void CompilationEngine::compileClassVarDec(CODE tokens) {

    if (tokens[0] == "static" || tokens[0] == "field") {
        if (JackTokenizer::isValid(validVarTypes, tokens[1])){
            if (isValidName(tokens[2])){
                classSymbolTable.insert(tokens[2], tokens[1], tokens[0]);
            }
        }

        std::string token = getNthToken(m_currentLine);
        temp_tokens = JackTokenizer::tokenizeCode(token);
        m_currentLine++;

        compileClassVarDec(temp_tokens);
    }
    else{
        return;
    }


}

void CompilationEngine::compileSubroutine() {

    if (JackTokenizer::isValid(validSubroutineDec, temp_tokens[0])) {
        if (JackTokenizer::isValid(validSubroutineTypes, temp_tokens[1])){
            if (isValidName(temp_tokens[2])){
                compileVarDec();
                classSymbolTable.display();
                std::cout << " --- " << std::endl << std::endl;
                subroutineSymbolTable.display();
                std::cout << " --- " << std::endl << std::endl;
                std::cout << classSymbolTable.index("field3") << std::endl;
            }
        }
    }
}

std::string CompilationEngine::getNthToken(int n) {
    if (JackTokenizer::isNotEmpty(m_code[n])){
        return m_code[n];
    }
    else{
        m_currentLine++;
        return getNthToken(n+1);
    }
}

bool CompilationEngine::isValidName(std::string name) {
    if (JackTokenizer::isValid(validVarTypes, name) || JackTokenizer::isValid(validSubroutineDec, name) ||
        JackTokenizer::isValid(validSymbols, name) || JackTokenizer::isValid(validKeywords, name)) {
        return false;
    }
    else{
        if (std::regex_match(name, std::regex("^(_|[a-zA-Z])[a-zA-Z0-9_]*$"))){
            return true;
        }
    }
    return false;
}

void CompilationEngine::compileVarDec() {
    temp_tokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
    m_currentLine++;

    if ((temp_tokens[0] == "var") || temp_tokens[0] == "let"){
        if (isValidName(temp_tokens[1])){
            subroutineSymbolTable.insert(temp_tokens[2], temp_tokens[1], temp_tokens[0]);

            std::string token = getNthToken(m_currentLine);
            temp_tokens = JackTokenizer::tokenizeCode(token);
            m_currentLine++;
            compileVarDec();
        }
        else{
            return;
        }
    }
    else{
        return;
    }

}
