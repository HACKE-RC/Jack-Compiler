#include "CompilationEngine.hpp"
#include <utility>

CompilationEngine::CompilationEngine(std::string fName) {
    JackTokenizer tokenizer(std::move(fName), "test.tst");
    tokenizer.cleanCode();
    m_code = tokenizer.getAllCodeVector();

    m_currentLine = 0;
    tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));

    while(tempTokens.empty()){
        tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
        m_currentLine++;
    }

    if (tempTokens[0] == "class") {
        compileClass();
    }
}

void CompilationEngine::compileClass() {
    m_currentLine++;
    tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));

    if (JackTokenizer::isValid(validVarDecs, tempTokens[0])) {
        compileClassVarDec(tempTokens);
    }
//    else{
//        tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
//    }

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
                m_currentLine++;
//                compileVarDec();
                compileSubroutineBody();
            }
        }
    }
}

std::string CompilationEngine::getNthToken(int n) {
    std::string str;

    if (JackTokenizer::isNotEmpty(m_code[n])){
        auto index = m_code[n].find_first_not_of(' ');
        if (index != std::string::npos){
            m_code[n] = m_code[n].substr(index);
            str = m_code[n];
            str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
            str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        }
        return str;
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

    if ((tempTokens[0] == "var")){
        if (JackTokenizer::isValid(validVarTypes, tempTokens[1])){
            if (isValidName(clearName(tempTokens[2]))) {
                auto kind = tempTokens[0];

                if (kind == "var"){
                    kind = "local";
                }

                subroutineSymbolTable.insert(clearName(tempTokens[2]), tempTokens[1], kind);

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

    if (index != tempTokens.end() ){
        auto index2 =  std::find(tempTokens.begin(), tempTokens.end(), ")");
        if ((index2 != tempTokens.end()) && (std::distance(tempTokens.begin(), index) + 1 != std::distance(tempTokens.begin(), index2))){
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
    else{
        return;
    }
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

    while(getNthToken(m_currentLine) != "return;"){
        compileStatement();
        std::string s = getNthToken(m_currentLine);
        m_currentLine++;
    }
}

void CompilationEngine::compileStatement() {
    if (JackTokenizer::isValid(validStatementInitials, tempTokens[0])) {
        if (tempTokens[0] == "do") {
            compileDo();
        }
        else if (tempTokens[0] == "let") {
            compileLet();
        }
        else{
            m_currentLine++;
        }
    }
}

void CompilationEngine::compileDo() {
    compileExpressionList();
    callSubroutine(getNthToken(m_currentLine));

     for (auto &i : vmCode){
         std::cout << i << std::endl;
     }

//    m_currentLine++;
//     classSymbolTable.display();
//     subroutineSymbolTable.display();
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
    expr = clearName(expr);
    prioritizeBrackets(expr);
    exprVec = getExpressionVector(expr);

    compileTerm(exprVec[0]);

    if (exprVec.size() > 1){
        int n = 1;
        while(JackTokenizer::isValid(validOperations, exprVec[n])){
            compileExpression(exprVec[n+1]);

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
         n += 2;
      }
   }
}

bool CompilationEngine::isNumber(std::string &str) {
    return !str.empty() && str.find_first_not_of("0123456789") == std::string::npos;
}

bool CompilationEngine::isNumber(char &ch) {
    if (JackTokenizer::isValid(validPositiveInt, ch)){
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

    if (exprList == ""){
        return;
    }

    exprList = prioritizeBrackets(exprList);
    CODE exprVec;

    for (char ch: exprList){
        std::string temp = {ch};
        exprVec.push_back(temp);
    }

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
    else if (term[0] == '-'){
        vmCode.push_back("push constant " + term.substr(1));
        vmCode.push_back("neg");
    }
}

CODE CompilationEngine::getExpressionVector(std::string expr) {
    CODE exprVec;

    std::vector<int> sepIndex;
    int k = 0;

    expr.erase(std::remove(expr.begin(), expr.end(), ' '), expr.end());

    for (char expression: expr){
        if ((expression == '+') || (expression == '-') || (expression == '*') || (expression == '/')){
            sepIndex.push_back(k);
        }
        k++;
    }

    sepIndex.push_back(-1);

    if (sepIndex.size() == 2){
        exprVec.push_back(clearName(expr));
        return exprVec;
    }

    int start = 0;

    std::string expression;
    std::string op;

    for (int index : sepIndex){
        if (index != -1){

            expression = expr.substr(start, index - start);
            op = expr.substr(index,1);
        }
        else{
            expression = expr.substr(start, expr.length());
        }

        exprVec.push_back(clearName(expression));
        if (!(op.empty())){
            exprVec.push_back(clearName(op));
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
        }
    }

    return name;
}

std::string CompilationEngine::prioritizeBrackets(std::string &expression) {
    CODE expressionVec;
    std::string temp;

    expressionVec = splitString(expression, ',');

    if (std::find(expressionVec.begin(), expressionVec.end(), ")") == expressionVec.end()){
        return expression;
    }

    for (auto &expr: expressionVec) {
        if ((expr.find('(') != std::string::npos) && (expr.find(')') != std::string::npos)) {
            expr.erase(std::remove_if(expr.begin(), expr.end(), ::isspace), expr.end());
            auto idx1 = expr.find_first_of('(');
            auto idx2 = expr.find_first_of(')');

            std::string transformed;
            std::string op;
            std::string oldStart;


            transformed = expr.substr(idx1 + 1, (idx2 - idx1) - 1);
            op = expr.substr(idx1 - 1, 1);
            oldStart = expr.substr(0, idx1 - 1);

            if (op == "-") {
                transformed = op + transformed;
            } else {
                transformed.append(op);
            }

            if (oldStart[0] == '-') {
                if (op == "+") {
                    oldStart[0] = '-';
                } else if (op == "-") {
                    oldStart[0] = '+';
                } else {
                    oldStart = "(" + oldStart + ")";
                }
            }

            transformed.append(oldStart);

            while (transformed.find('(') != std::string::npos) {
                transformed = prioritizeBrackets(transformed);
            }

            if (expr != expressionVec.back()) {
                temp.append(transformed + ",");
            } else {
                temp.append(transformed);
            }
        }
    }
    return temp;
}

std::vector<std::string> CompilationEngine::splitString(std::string &str, char delim) {
//    Split string through True Separator Search Algorithm
    std::vector<int> sepIndex;
    std::vector<std::string> splitVec;
    int k = 0;
    int start = 0;

    std::string split;

    for (char ch: str){
        if (ch == delim){
            sepIndex.push_back(k);
        }
        k++;
    }

    sepIndex.push_back(-1);
    std::string splitStr;

    for (int j = 0; j < (sepIndex.size() + 1); j++) {
        auto index = sepIndex[j];

        if (index >= 0) {
            std::string::iterator startIt = str.begin() + start;
            std::string::iterator endIt = str.begin() + index;

            try{
                splitStr.clear();
                splitStr = std::string(startIt, endIt);
                splitVec.push_back(splitStr);
            }
            catch (const std::exception& e){
                break;
            }
            start = index + 1;
        }
        else{
            splitStr = std::string(str.begin() + start, str.end());
            splitStr = clearName(splitStr);
            splitVec.push_back(splitStr);
            break;
        }
    }
    return splitVec;
}

void CompilationEngine::callSubroutine(std::string line) {
    CODE lineVec = splitString(line, ' ');
    std::string funcName;
    funcName = lineVec[1];
    funcName = lineVec[1].substr(0,  funcName.find('('));

    vmCode.push_back("call " + funcName);
}

void CompilationEngine::compileLet() {
    std::string currentLine = getNthToken(m_currentLine);
    tempTokens = splitString(currentLine, ' ');

    std::string varName = tempTokens[1];

    compileExpressionList();
    callSubroutine(tempTokens[3]);
}
