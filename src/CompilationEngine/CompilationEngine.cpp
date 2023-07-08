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
            if (isValidName(clearName(tokens[2]))){
                classSymbolTable.insert(clearName(tokens[2]), tokens[1], tokens[0]);
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
                compileSubroutineBody();
            }
        }
    }
}

std::string CompilationEngine::getNthToken(int n) {
    if (JackTokenizer::isNotEmpty(m_code[n])){
        auto index = m_code[n].find_first_not_of(" ");
        if (index != std::string::npos){
            m_code[n] = m_code[n].substr(index);
        }
        return m_code[n];
    }
    else{
        m_currentLine++;
        return getNthToken(n+1);
    }
}

bool CompilationEngine::isValidName(std::string name) {
    size_t index = name.find_first_not_of(' ');
    size_t end = name.find_last_not_of(' ');

    if ((index != std::string::npos) && (end != std::string::npos)){
        name = name.substr(index, end - index + 1);

    }

    if (JackTokenizer::isValid(validVarTypes, name) || JackTokenizer::isValid(validSubroutineDec, name) ||
        JackTokenizer::isValid(validSymbols, name) || JackTokenizer::isValid(validKeywords, name)) {
        return false;
    }
    else{
        if (std::regex_match(name, std::regex("^(_|[a-zA-Z])[a-zA-Z0-9_]*;?$"))){
            return true;
        }
    }
    return false;
}

void CompilationEngine::compileVarDec() {
    tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));

    if ((tempTokens[0] == "var") || tempTokens[0] == "let"){
        if (JackTokenizer::isValid(validVarTypes, tempTokens[1])){
            if (isValidName(clearName(tempTokens[2]))) {
                subroutineSymbolTable.insert(clearName(tempTokens[2]), tempTokens[1], tempTokens[0]);

                std::string token = getNthToken(m_currentLine);
                tempTokens = JackTokenizer::tokenizeCode(token);
                m_currentLine++;

                compileVarDec();
            }
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
                        std::vector<std::string> parameters = removeBrackets(tempTokens2);
                        paramCount = countParameters(parameters);

                        for (long long i = 0; i < (paramCount * 3); i+=3) {
                            subroutineSymbolTable.insert(parameters[i+1], parameters[i], "argument");
                        }
                }
            }
        }
    }
//    m_currentLine++;
}

long long CompilationEngine::countParameters(CODE parameterList) {
    auto count = std::count(parameterList.begin(), parameterList.end(), "int");
    count += std::count(parameterList.begin(), parameterList.end(), "boolean");
    count += std::count(parameterList.begin(), parameterList.end(), "bool");
    count += std::count(parameterList.begin(), parameterList.end(), "char");
    return count;
}

void CompilationEngine::compileSubroutineBody() {
    compileVarDec();
    compileStatement();
}

void CompilationEngine::compileStatement() {
    if (JackTokenizer::isValid(validStatementInitials, tempTokens[0])) {
        if (tempTokens[0] == "do") {
            compileDo();
        }
    }
}

void CompilationEngine::compileDo() {
    compileExpressionList();

     for (auto &i : vmCode){
         std::cout << i << std::endl;
     }
     classSymbolTable.display();
     subroutineSymbolTable.display();
}

CODE CompilationEngine::removeBrackets(CODE code) {
    auto start = std::find(code.begin(), code.end(), "(") + 1;
    auto end = std::find(code.begin(), code.end(), ")");

    std::vector<std::string> removed(start, end);
    return removed;
}

std::string CompilationEngine::removeBrackets(const std::string& str) {
    auto start = str.find_first_of('(') + 1;
    if (start == std::string::npos) {
        return "";
    }
    auto end = str.find_last_of(')');

    std::string str2(str.begin() + start, str.begin() + end);
    return str2;
}

void CompilationEngine::compileExpression(std::string& expr) {
    CODE exprVec;
    exprVec = getExpressionVector(expr);

   compileTerm(exprVec[0]);
   if (exprVec.size()  > 1){
      int n = 1;
      while(JackTokenizer::isValid(validOperations, exprVec[n])){
          compileTerm(exprVec[n+1]);

          if (exprVec[n] == "+"){
             vmCode.push_back("add");
          }
          else if (exprVec[n] == "-"){
              vmCode.push_back("sub");
          }
          else if (exprVec[n] == "*"){
              vmCode.push_back("call Math.multiply 2");
          }
          else if (exprVec[n] == "/"){
              vmCode.push_back("call Math.divide 2");
          }
          n++;
      }
   }


}

bool CompilationEngine::isNumber(std::string &str) {
    return !str.empty() && str.find_first_not_of("0123456789") == std::string::npos;
}

bool CompilationEngine::isNumber(char &ch) {
    if (JackTokenizer::isValid(validInt, ch)){
        return true;
    }
    return false;
}

char CompilationEngine::isCharacterPresent(const std::string& str1, const std::string& str2) {
    for (auto c: str2){
        if (str2.find(c)){
            return c;
        }
    }
    return ' ';
}

void CompilationEngine::compileExpressionList() {
    std::string currentLine = getNthToken(m_currentLine);
    std::string exprList = removeBrackets(currentLine);

    CODE exprVec = JackTokenizer::tokenizeCode(exprList);

//    True Separator Search Algorithm
    std::vector<int> sepIndex;
    int k = 0;

    for (auto const &expr: exprVec){
        if (expr == ","){
            if (exprVec[k+2] == "," || exprVec[k+2] == ")"){
            }
            else{
                sepIndex.push_back(k);
            }
        }
        k++;
    }

    sepIndex.push_back(-1);

    int start = 0;
    std::string expression;
    for (int j = 0; j < (sepIndex.size() + 1); j++) {
        auto index = sepIndex[j];

        if (index >= 0) {
            std::vector<std::string>::iterator startIt = exprVec.begin() + start;
            std::vector<std::string>::iterator endIt = exprVec.begin() + index;
            try{
                expression.clear();
                for (auto it = startIt; it != endIt; ++it) {
                    expression+= *it;
                }
            }
            catch (const std::exception& e){
                break;
            }
        } else {
            if (index != -1) {
                break;
            }
            auto startIt = exprVec.begin() + start;
            expression.clear();
            for (auto it = startIt; it != exprVec.end(); ++it) {
                expression+= *it;
            }

        }
        compileExpression(expression);
        start = index + 1;
    }
}

void CompilationEngine::compileTerm(std::string term) {
    term = clearName(term);
    if (isNumber(term)){
        vmCode.push_back("push constant " + term);
    }
    else if ((subroutineSymbolTable.index(clearName(term).c_str()) != -1) || (classSymbolTable.index(clearName(term).c_str()) != -1)){
        if (subroutineSymbolTable.index(clearName(term).c_str()) != -1){
            vmCode.push_back("push " + subroutineSymbolTable.kind(term) + " " + std::to_string(subroutineSymbolTable.index(term.c_str())));
        }
        else if (classSymbolTable.index(term.c_str()) != -1){
            vmCode.push_back("push " + classSymbolTable.kind(term) + " " + std::to_string(classSymbolTable.index(term.c_str())));
        }
    }
    else if ((term.starts_with('(')) && (term.ends_with(')'))){
        compileExpression(term);
    }
}

CODE CompilationEngine::getExpressionVector(std::string &expr) {
    CODE exprVec;

    std::vector<int> sepIndex;
    int k = 0;

    for (char expression: expr){
        if ((expression == '+') || (expression == '-') || (expression == '*') || (expression == '/')){
            sepIndex.push_back(k);
        }
        k++;
    }

    sepIndex.push_back(-1);

    if (sepIndex.size() == 1){
        exprVec.push_back(expr);
        return exprVec;
    }

    int start = 0;

    std::string expression;
    std::string op;

    for (int index : sepIndex){
        if (index != -1){
            auto startIt = expr.begin() + start;
            auto endIt = expr.begin() + index;

            expression = expr.substr(start, index);
            op = expr.substr(index,1);
        }
        else{
            auto startIt = expr.begin() + start;
            expression = expr.substr(start, expr.length());
        }

        exprVec.push_back(expression);
        if (!(op.empty())){
            exprVec.push_back(op);
        }
        else{
            break;
        }

        op = "";
        start = index + 1;
    }
    return exprVec;
}

std::string CompilationEngine::clearName(std::string name) {
    size_t spaceStart = name.find_first_not_of(' ');
    size_t spaceEnd = name.find_last_not_of(' ');
    if (spaceStart != std::string::npos && spaceEnd != std::string::npos) {
        name =  name.substr(spaceStart, spaceEnd - spaceStart + 1);
        if (name.ends_with(";")) {
            name.pop_back();
//            return name;
        }
    }
    return name;
}
