#include "CompilationEngine.hpp"
#include <utility>


CompilationEngine::CompilationEngine(std::string fName) {
    JackTokenizer tokenizer(std::move(fName), "test.tst");
    tokenizer.cleanCode();
    m_code = tokenizer.getAllCodeVector();

    m_currentLine = 0;

    tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
    m_currentLine++;

    if (tempTokens[0] == "class") {
        compileClass();
    }
}

void CompilationEngine::compileClass() {
    tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
    m_currentLine++;

    if (JackTokenizer::isValid(validVarDecs, tempTokens[0])) {
        compileClassVarDec(tempTokens);
    }

    compileSubroutine();
}


void CompilationEngine::compileClassVarDec(CODE tokens) {

    if (tokens[0] == "static" || tokens[0] == "field") {
        if (JackTokenizer::isValid(validVarTypes, tokens[1])){
            if (isValidName(tokens[2])){
                classSymbolTable.insert(tokens[2], tokens[1], tokens[0]);
            }
        }

        std::string token = getNthToken(m_currentLine);
        tempTokens = JackTokenizer::tokenizeCode(token);
        m_currentLine++;

        compileClassVarDec(tempTokens);
    }
    else{
        return;
    }


}

void CompilationEngine::compileSubroutine() {

    if (JackTokenizer::isValid(validSubroutineDec, tempTokens[0])) {
        if (JackTokenizer::isValid(validSubroutineTypes, tempTokens[1])){
            if (isValidName(tempTokens[2])){
                compileParameterList();
                compileVarDec();

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
    tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
    m_currentLine++;

    if ((tempTokens[0] == "var") || tempTokens[0] == "let"){
        if (isValidName(tempTokens[1])){
            subroutineSymbolTable.insert(tempTokens[2], tempTokens[1], tempTokens[0]);

            std::string token = getNthToken(m_currentLine);
            tempTokens = JackTokenizer::tokenizeCode(token);
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

void CompilationEngine::compileParameterList() {
    auto index = std::find(tempTokens.begin(), tempTokens.end(), "(");
    long long paramCount = 0;

    if (index != tempTokens.end()){
        auto index2 =  std::find(tempTokens.begin(), tempTokens.end(), ")");
        if ((index2 != tempTokens.end()) && (index + 1 != index2)){
            auto index3 = std::distance(tempTokens.begin(), index);
            if (JackTokenizer::isValid(validVarTypes, tempTokens.at(index3+1))){
                if (isValidName(tempTokens.at(index3+2))){
                    auto tempTokens2 = tempTokens;
//                      array slicing
                        auto start = std::find(tempTokens2.begin(), tempTokens2.end(), "(") + 1;
                        auto end = std::find(tempTokens2.begin(), tempTokens2.end(), ")");

                        std::vector<std::string> parameters(start, end);
                        paramCount = countParameters(parameters);

                        for (long long i = 0; i < (paramCount * 3); i+=3) {
                            subroutineSymbolTable.insert(parameters[i+1], parameters[i], "argument");
                        }
                }
            }
        }

    }

}

long long CompilationEngine::countParameters(CODE parameterList) {
    auto count = std::count(parameterList.begin(), parameterList.end(), "int");
    count += std::count(parameterList.begin(), parameterList.end(), "boolean");
    count += std::count(parameterList.begin(), parameterList.end(), "bool");
    count += std::count(parameterList.begin(), parameterList.end(), "char");
    return count;
}
