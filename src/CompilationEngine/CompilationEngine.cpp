#include "CompilationEngine.hpp"
#include <utility>
#include <source_location>

VMWriter vmFile;

[[maybe_unused]] CompilationEngine::CompilationEngine(std::string fName) {
    std::string fName2 = fName;

    CODE code;
    std::vector<std::string> fNames = {};
    fName = fName2;
    if (std::filesystem::is_directory(fName.c_str())){

        for (const auto& entry: std::filesystem::directory_iterator(fName.c_str())){
            if (std::filesystem::is_regular_file(entry)){
                auto filename = entry.path().filename().string();
                auto full_filename = fName + "\\" + filename;
                if (filename.ends_with(".jack") && std::find(fNames.begin(), fNames.end(), full_filename) == fNames.end()){
                    fNames.push_back(full_filename);
                }
            }
        }
    }
    else{
        fNames.push_back(fName);
    }

    for (const auto& file: fNames){
        std::ifstream fStream(file);
        std::string fData;

        if (!fStream.good()){
            std::cout << file << std::endl;
            std::cerr << "ERR: " << "File not found!" << std::endl;
            std::exit(-1);
        }
        else{
            fStream.clear();
            fStream.close();
        }

        vmFile = VMWriter(file + ".vm");
        JackTokenizer tokenizer(file, file);
        tokenizer.cleanCode();
        m_code = tokenizer.getAllCodeVector();
        removeTabs(m_code);
        m_currentLine = 0;
        tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));

        while(tempTokens.empty()){
            tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
            m_currentLine++;
        }

        if (tempTokens[0] == "class") {
            classSymbolTable.reset();
            compileClass();
        }
    }

}

void CompilationEngine::compileClass() {
    std::string name;
    int i = 0;
    while ((i < tempTokens[1].length()) && std::isalnum(tempTokens[1].at(i))){
        if (std::isspace(tempTokens[1].at(i))){
            i++;
            continue;
        }
        name.push_back(tempTokens[1].at(i));
        i++;
    }

    if (isValidName(name)){
        m_currentClassName = name;
        validSubroutineTypes.push_back(m_currentClassName);
    }
    else{
        return;
    }

    m_currentLine++;
    tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));

    compileClassVarDec();

    if (tempTokens.back() == "{"){
        insideClass = true;
    }
    compileSubroutine();
    for (auto& v: vmCode){
        std::cout << v << std::endl;
    }
}


void CompilationEngine::compileClassVarDec() {
    std::string currentLine = getNthToken(m_currentLine);
    auto tokens = JackTokenizer::tokenizeCode(currentLine);
    tempTokens = tokens;
    std::string variables;

    if (tokens[0] == "static" || tokens[0] == "field") {
        if (isValidName(clearName(tokens[2]))){
            m_currentLine++;

            if (tokens[0] == "static") {
                variables = currentLine.substr(currentLine.find("static") + 7);
                variables = variables.substr(variables.find(tempTokens[2]));
                variables = clearName(variables);
            }
            else if (tokens[0] == "field") {
                variables = currentLine.substr(currentLine.find("field") + 6);
                variables = variables.substr(variables.find(tempTokens[2]));
                variables = clearName(variables);
                variables.erase(std::remove(variables.begin(), variables.end(), ' '), variables.end());
            }

            CODE variableVector;
            variableVector = splitString(variables, ',');

            int i = 0;
            for (auto &variable: variableVector){
                std::string name = variableVector[i];

                if (isValidName(clearName(name))) {
                    auto kind = tempTokens[0];
                    classSymbolTable.insert(clearName(name), tempTokens[1], kind);
                    i++;
                }
            }
            currentLine = getNthToken(m_currentLine);
            tempTokens = JackTokenizer::tokenizeCode(currentLine);
            compileClassVarDec();
        }

    }
    else{
        return;
    }
}
std::string removeNonAlphanumeric(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (std::isalnum(c) || c == '_') {
            result += c;
        }
    }
    return result;
}

void CompilationEngine::compileSubroutine() {
    subroutineSymbolTable.display();
    subroutineSymbolTable.reset();
    tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));

    int totalVars = 0;

    if (tempTokens.empty()){
        return;
    }

    if (JackTokenizer::isValid(validSubroutineDec, tempTokens[0])) {
        if ((JackTokenizer::isValid(validSubroutineTypes, tempTokens[1])) || (tempTokens[1] == m_currentClassName)){
            std::string name = removeNonAlphanumeric(tempTokens[2]);
            if (isValidName(name)) {
                if (tempTokens.back() == "{") {
                    insideSubroutine = true;
                }

                m_currentSubroutineDecType = tempTokens[0];
                subroutineTypes[m_currentClassName + "." + name] = tempTokens[1];

                compileParameterList();
                m_currentLine++;

                totalVars = compileVarDec();
                vmFile.writeFunction(m_currentClassName + "." + name, totalVars);
                totalVars = 0;

                compileSubroutineBody();
            }
        }

    }
}

std::string CompilationEngine::getNthToken(int n) {
    std::string str;

    if (n >=  m_code.size()){
        return "";
    }

    if (JackTokenizer::isNotEmpty(m_code[n])){
        auto index = m_code[n].find_first_not_of(' ');
        if (index != std::string::npos){
            m_code[n] = m_code[n].substr(index);
            str = m_code[n];
        }
        if (str.empty()){
            m_currentLine++;
            return getNthToken(n+1);
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

int CompilationEngine::compileVarDec() {
/*
 * returns the number of variables declared and updates variable declarations
 */

    int totalVars = 0;
    std::string currentLine = getNthToken(m_currentLine);

    currentLine = currentLine.substr(currentLine.find_first_not_of(' '));
    tempTokens = splitString(currentLine, ' ');

    if ((tempTokens[0] == "var")){
        std::string variables = currentLine.substr(currentLine.find(tempTokens[2]));
        variables = clearName(variables);
        variables.erase(std::remove(variables.begin(), variables.end(), ' '), variables.end());

        CODE variableVector;
        variableVector = splitString(variables, ',');

        int i = 0;
        for (auto &variable: variableVector){
            std::string name = variableVector[i];

            if (isValidName(clearName(name))) {
                auto kind = tempTokens[0];

                if (kind == "var"){
                    kind = "local";
                }

                subroutineSymbolTable.insert(clearName(name), tempTokens[1], kind);
                i++;
            }
        }
        m_currentLine++;
        std::string token = getNthToken(m_currentLine);
        tempTokens = JackTokenizer::tokenizeCode(token);

        compileVarDec();
    }
    else{
        totalVars = subroutineSymbolTable.count("local");
        return totalVars;
    }

    totalVars = subroutineSymbolTable.count("local");
    return totalVars;
}

void CompilationEngine::compileParameterList() {
    auto index = std::find(tempTokens.begin(), tempTokens.end(), "(");
    long long paramCount = 0;

    if (index != tempTokens.end() ){
        auto index2 =  std::find(tempTokens.begin(), tempTokens.end(), ")");
        if ((index2 != tempTokens.end()) && (std::distance(tempTokens.begin(), index) + 1 != std::distance(tempTokens.begin(), index2))){
            auto index3 = std::distance(tempTokens.begin(), index);
            if (JackTokenizer::isValid(validVarTypes, tempTokens.at(index3+1)) || JackTokenizer::isValid(validComplimentaryVarTypes, tempTokens.at(index3+1))){
                if (isValidName(tempTokens.at(index3+2))){
                    auto tempTokens2 = tempTokens;
                    std::vector<std::string> parameters = removeBrackets(tempTokens2);
                    paramCount = countParameters(parameters);
                    long long i = 0;

                    if (m_currentSubroutineDecType == "method"){
                        subroutineSymbolTable.insert("this", m_currentClassName, "argument");
                    }
                    for (i; i < (paramCount * 3); i+=3) {
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
    count += std::count(parameterList.begin(), parameterList.end(), "String");
    count += std::count(parameterList.begin(), parameterList.end(), "Array");
    return count;
}

void CompilationEngine::compileSubroutineBody() {

    if (m_currentSubroutineDecType == "constructor"){
        vmFile.writePush("constant", classSymbolTable.count("field"));
        vmFile.writeCall("Memory.alloc", 1);
        vmFile.writePop("pointer", 0);
    }
    else if (m_currentSubroutineDecType == "method"){
        subroutineSymbolTable.insert("this", m_currentClassName, "argument");
        vmFile.writePush("argument", 0);
        vmFile.writePop("pointer", 0);
    }

    while (!(getNthToken(m_currentLine).starts_with("}"))){
        compileStatement("");
    }

    tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));

    if (tempTokens.back() == "}"){
        insideSubroutine = false;
    }

    m_currentLine++;
    compileSubroutine();
}

void CompilationEngine::compileStatement(std::string line = "") {
    if (!(line.empty())){
        tempTokens = JackTokenizer::tokenizeCode(line);
        vmCode.push_back("//" + line);
    }
    else{
        tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
        line = getNthToken(m_currentLine);
        vmCode.push_back("//" + getNthToken(m_currentLine));
    }
    if (tempTokens.empty()){
        m_currentLine++;
        return;
    }

    if (JackTokenizer::isValid(validStatementInitials, tempTokens[0])) {
        if (tempTokens[0] == "do") {
            compileDo(line);
            m_currentLine++;
        }
        else if (tempTokens[0] == "let") {
            compileLet(line);
        }
        else if (tempTokens[0] == "if"){
            compileIf();
        }
        else if (tempTokens[0] == "return"){
            compileReturn(line);
        }
        else if (tempTokens[0] == "while"){
            compileWhile();
        }
        else if (tempTokens[0].find("else") != std::string::npos){
            m_currentLine++;
        }
    }
    else if (JackTokenizer::isValid(validSubroutineDec, tempTokens[0])){
        compileSubroutine();
    }
}
std::unordered_map<std::string, int> CompilationEngine::getFunctionName(CODE& lineVec) {
    std::unordered_map<std::string, int> functionData = {};
//  this helps ignore "do" in do's case and = in let's case
    std::string methodName = lineVec[1];
    std::string funcName;
    int objAddition = 0;

    methodName = lineVec[1].substr(0, methodName .find('('));
    if (isCharacterPresent(methodName, '.')){
        std::string objName = methodName.substr(0, methodName .find('.'));
        methodName = methodName.substr(methodName.find('.') + 1);

        if (JackTokenizer::isValid(validVarTypes, objName)){
                std::cerr << "Cannot use '.' operator on predefined types." << std::endl;
        }
        else if ((subroutineSymbolTable.index(objName.c_str()) != -1) || (classSymbolTable.index(objName.c_str())) != -1 ) {
            if (subroutineSymbolTable.index(objName.c_str()) != -1) {
                std::string objType = subroutineSymbolTable.type(objName);

                if (!(JackTokenizer::isValid(validVarTypes, objType))) {
                    vmFile.writePush(subroutineSymbolTable.kind(objName),  subroutineSymbolTable.index(objName.c_str()));
                    methodName = objType + "." + methodName;
                    funcName = clearName(methodName);
                    objAddition = 1;
                    functionData.insert({funcName, objAddition});
                }
            }

            else {
                vmFile.writePush(classSymbolTable.kind(objName), classSymbolTable.index(objName.c_str()));
                methodName = classSymbolTable.type(objName) + "." + methodName;
                funcName = clearName(methodName);
                objAddition = 1;
                functionData.insert({funcName, objAddition});
            }
        }
        else if (JackTokenizer::isValid(validComplimentaryVarTypes, objName)) {
                methodName = objName+ "." + methodName;
                funcName = clearName(methodName);
                objAddition = 1;
                functionData.insert({funcName, objAddition});
        }
        else{
            funcName = lineVec[1].substr(0, lineVec[1].find('('));
            funcName = clearName(funcName);
            functionData.insert({funcName, objAddition});
        }
    }

    else{
        funcName = lineVec[1].substr(0, lineVec[1].find('('));
        funcName = clearName(funcName);
        funcName = m_currentClassName  + "." + funcName;
//            pushes this 0
        vmFile.writePush("pointer", 0);
        objAddition = 1;
        functionData.insert({funcName, objAddition});
    }
    return functionData;
}
void CompilationEngine::compileDo(const std::string& line = "") {
    std::string expressions;

    if (line.empty()){
        expressions = getNthToken(m_currentLine);
    }
    else{
        expressions = line;
    }

    CODE lineVec = splitString(expressions, ' ');
    std::string funcName;
    int objAddition = 0;

    if (lineVec[0] == "do"){
        auto funcData = getFunctionName(lineVec);
        funcName = funcData.begin()->first;
        objAddition = funcData.begin()->second;
    }
    else{
        funcName = lineVec[0].substr(0, lineVec[0].find('('));
    }

    if (line.find('(') != std::string::npos && line.find(')') != std::string::npos){
        expressions = removeBrackets(line, false, roundBrackets);
        if (!expressions.empty()){
            auto k = splitString(expressions, ',');
            if (!k.empty()){
                objAddition = k.size();
            }
        }
    }
    else{
        if (line.find('(') == std::string::npos){
            expressions = line.substr(0, line.find(')'));
        }
        else if (line.find(')') == std::string::npos){
            expressions = line.substr(line.find('(') + 1);
        }
    }

    compileExpressionList(expressions);
    callSubroutine(line, funcName, objAddition);
    vmFile.writePop("temp", 0);
}

CODE CompilationEngine::removeBrackets(CODE code) {
    auto start = std::find(code.begin(), code.end(), "(") + 1;
    auto end = std::find(code.begin(), code.end(), ")");

    std::vector<std::string> removed(start, end);
    return removed;
}

std::string CompilationEngine::removeBrackets(const std::string& str, bool inLine, const std::unordered_map<std::string, std::string>& brackets) {
    auto start = str.find_first_of(brackets.begin()->first);
    unsigned long long end;

    if (start == std::string::npos) {
        return str;
    }

    start = start+1;

    if (!(inLine)){
        end = str.find_last_of(brackets.begin()->second);
    }
    else{
        end = str.find_first_of(brackets.begin()->second);
    }

    std::string str2(str.begin() + start, str.begin() + end);
    return str2;
}


void CompilationEngine::compileExpression(std::string& expr) {
    CODE exprVec;

    int n = 1;
    int i = 1;

    if (expr.empty()){
        return;
    }

    expr = clearName(expr);
    expr = prioritizeBrackets(expr);

    if (expr.starts_with('"') && expr.ends_with('"')) {
        auto temp = expr.substr(1, expr.length() - 2);

        vmFile.writePush("constant", temp.length());
        vmFile.writeCall("String.new", 1);

        for (auto x: temp) {
            vmFile.writePush("constant", int(x));
            vmFile.writeCall("String.appendChar", 2);
        }
        return;
    }

    auto isArray = [](std::string &str) -> bool {return (std::count(str.begin(), str.end(), ']') >= 1 && std::count(str.begin(), str.end(), '[') >= 1);};
    if (isArray(expr)){
        auto exprC = expr;
        while (exprC.starts_with('(') && exprC.ends_with(')')){
            prioritizeBrackets(exprC);
            exprC = removeBrackets(exprC, false, roundBrackets);
        }
//        fix split..Expr function
        auto arrayExprk = splitNonArrayExprFromArrayExpr(exprC);
        if (arrayExprk.size() > 1){
            exprVec = arrayExprk;
            goto skipVecDec;
        }

        auto arrayExpr = compileArray(expr);
        if (!arrayExpr.empty()) {
            exprVec = arrayExpr;
            if (exprVec.empty()){
                return;
            }
            n = 0;
            goto compileExpressionOperation;
        }
        else if (arrayExpr.empty()){
            return;
        }
    }

    exprVec = depthSplit(expr, roundBrackets);
    skipVecDec:
    if (exprVec.size() == 1){
        exprVec = getExpressionVector(expr);
        exprVec.erase(std::remove_if(exprVec.begin(), exprVec.end(), [](const std::string& string) {
            return string.empty();
        }), exprVec.end());

        if (exprVec[0].find('.')!=std::string::npos &&  exprVec[0].find('(') != std::string::npos &&  exprVec[0].ends_with(')')){
            compileDo(exprVec[0]);
            return;
        }
    }

    if (exprVec.empty()){
        return;
    }

    if (exprVec[0] == "-"){
        compileTerm(expr);
    }
    else if (exprVec[0] == "~"){
        if (exprVec[1] == "("){
            expr = removeBrackets(expr, false, roundBrackets);
            compileExpression(expr);
        }
        else{
            expr = expr.substr(1);
            compileExpression(expr);
        }
        vmFile.writeArithmetic("~");
    }
    else if (exprVec[0].starts_with("(")){
        expr = expr.substr(1);
        compileExpression(expr);
        return;
    }
    else{
        compileTerm(exprVec[0]);
    }

    compileExpressionOperation:
    if (exprVec.size() > 1){
        while(JackTokenizer::isValid(validOperations, exprVec[n])){
            if (n+1 > exprVec.size()){
                return;
            }

            while(exprVec[n+i] == "("){
                i++;
            }
            while(exprVec[n+i] == ")"){
                i++;
            }
            if (exprVec[n+i] == "null"){
                vmFile.writePush("constant", 0);
            }
            else if (exprVec[n+i] == "-"){
                compileTerm(exprVec[n+i+1]);
                vmFile.writeNeg();
                i++;
            }
            else{
                compileExpression(exprVec[n+i]);
            }

            vmFile.writeArithmetic(exprVec[n]);
            n += 2 + (i-1);
            i = 1;
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

bool CompilationEngine::isCharacterPresent(const std::string &str1, char c) {
    for (auto x: str1){
        if (c == x){
            return true;
        }
    }
    return false;
}

void CompilationEngine::compileExpressionList(const std::string& expressions) {
    std::string exprList = expressions;

    if (exprList.empty()) {
        return;
    }

    exprList = prioritizeBrackets(exprList);
    CODE exprVec;

    for (char ch : exprList) {
        std::string temp = {ch};
        exprVec.push_back(temp);
    }

    std::vector<int> sepIndex;
    int k = 0;

    for (auto it = exprVec.begin(); it != exprVec.end(); ++it) {
        if (*it == ",") {
            if (std::next(it, 2) != exprVec.end() && (*(std::next(it, 2)) == "," || *(std::next(it, 2)) == ")")) {
                continue;
            }
            sepIndex.push_back(k);
        }
        k++;
    }

    sepIndex.push_back(-1);

    int start = 0;
    std::string expression;

    for (size_t j = 0; j < sepIndex.size(); j++) {
        int index = sepIndex[j];

        if (index >= 0) {
            std::vector<std::string>::iterator startIt = exprVec.begin() + start;
            std::vector<std::string>::iterator endIt = exprVec.begin() + index;

            expression.clear();
            for (auto it = startIt; it != endIt; ++it) {
                expression += *it;
            }
        } else {
            if (index != -1) {
                break;
            }

            auto startIt = exprVec.begin() + start;
            expression.clear();

            for (auto it = startIt; it != exprVec.end(); ++it) {
                expression += *it;
            }
        }

        compileExpression(expression);
        start = index + 1;
    }
}


void CompilationEngine::compileTerm(std::string term) {
    term = clearName(term);

    if (term.starts_with('(')){
        term.erase(0, 1);
    }
    while (term.ends_with(')')){
        term.pop_back();
    }
    int i = 1;

    while(term.starts_with('(')){
        term.erase(0, i++);
    }

    if (isNumber(term)){
        vmFile.writePush("constant", std::stoi(term));
    }
    else if ((subroutineSymbolTable.index(clearName(term).c_str()) != -1) || (classSymbolTable.index(clearName(term).c_str()) != -1)){
        if (subroutineSymbolTable.index(clearName(term).c_str()) != -1){
            vmFile.writePush(subroutineSymbolTable.kind(clearName(term)), subroutineSymbolTable.index(clearName(term).c_str()));
        }
        else if (classSymbolTable.index(term.c_str()) != -1){
            vmFile.writePush(classSymbolTable.kind(term),  classSymbolTable.index(term.c_str()));
        }
    }
    else if ((term.starts_with('(')) && (term.ends_with(')'))){
        compileExpression(term);
    }
    else if (term[0] == '-'){
        vmFile.writePush("constant", stoi(term.substr(1)));
        vmFile.writeNeg();
    }
    else if (reservedValues.find(term) != reservedValues.end()){
        if (reservedValues[term] == "-1"){
            vmFile.writePush("constant", 1);
            vmFile.writeNeg();
        }
        else{
            vmFile.writePush("constant", stoi(reservedValues[term]));
        }
    }
    else{
        compileExpression(term);
    }
}

CODE CompilationEngine::getExpressionVector(std::string expr) {
    CODE exprVec;

    std::vector<int> sepIndex;
    int k = 0;

    if (expr.length() == 1){
        exprVec.push_back(expr);
        return exprVec;
    }

    expr.erase(std::remove(expr.begin(), expr.end(), ' '), expr.end());

    for (char expression: expr){
        if ((expression == '+') || (expression == '-') || (expression == '*') || (expression == '/') || (expression == '&') || (expression == '|') || (expression == '<') || (expression == '>') || (expression == '=') || (expression == '~')){
            sepIndex.push_back(k);
        }
        k++;
    }

    sepIndex.push_back(-1);

    if (sepIndex.size() == 2){
        exprVec.push_back(expr.substr(0, sepIndex[0]));
        exprVec.push_back(expr.substr(sepIndex[0],1));
        exprVec.push_back(expr.substr(sepIndex[0]+1));
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
        if (!(expression.empty())){
            exprVec.push_back(clearName(expression));
            if (!(op.empty())){
                exprVec.push_back(clearName(op));
            }
            else{
                break;
            }
        }
        else if (op == "~"){
            exprVec.push_back(op);
        }


        op = "";
        start = index + 1;
    }
    return exprVec;
}

std::string CompilationEngine::clearName(std::string name) {
    size_t nameStart = name.find_first_not_of(' ');
    size_t spaceEnd = name.find_last_not_of(' ');

    if (nameStart != std::string::npos && spaceEnd != std::string::npos) {
        name =  name.substr(nameStart, spaceEnd - nameStart + 1);

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

void CompilationEngine::callSubroutine(const std::string& line, std::string funcName, int objAddition) {
    std::string params;

    auto paramsVec = splitString(params, ',');
    vmFile.writeCall(std::move(funcName), paramsVec.size() + objAddition);
}

void CompilationEngine::compileLet(const std::string& line = "") {
    std::string currentLine ;
    if (!(line.empty())){
        currentLine = line;
    }
    else{
        currentLine = getNthToken(m_currentLine);
    }

    tempTokens = splitString(currentLine, ' ');
    tempTokens.erase(std::remove(tempTokens.begin(), tempTokens.end(), " "), tempTokens.end());
    tempTokens.erase(std::remove(tempTokens.begin(), tempTokens.end(), ""), tempTokens.end());

    std::string varName = tempTokens[1];

    if (std::count(varName.begin(), varName.end(), '[') >= 1 && std::count(varName.begin(), varName.end(), ']') >= 1){
        m_isArrayDec = true;
        compileExpression(varName);
    }


    std::string value;
    value = currentLine.substr(currentLine.find(tempTokens[3]));
    value.erase(0, value.find_first_not_of(' '));
    value.erase(value.find_last_not_of(' ') + 1);
    std::regex callPattern("^[a-zA-Z_][a-zA-Z0-9_]{0,}[.][a-zA-Z_][a-zA-Z0-9]*\\(");

    if (std::regex_search(value, callPattern)){
        if (value.ends_with(");")){
            std::vector<std::string> lineVec(tempTokens.begin() + 2, tempTokens.end());
            compileExpressionList(removeBrackets(value, false, roundBrackets));
            auto funcData = getFunctionName(lineVec);
            std::string funcName = funcData.begin()->first;
            int objAddition = funcData.begin()->second;

            callSubroutine(line, funcName, objAddition);
        }
    }
    else{
        CODE expressions = splitString(currentLine, '=');
        std::string expression = expressions[1];
        expression = clearName(expression);
        if (expression == "null"){
             vmFile.writePush("constant", 0);
             goto skipExpressionParsing;
        }

        if (std::count(expression.begin(), expression.end(), '[') >= 1 && std::count(expression.begin(), expression.end(), ']') >= 1){
            m_isArrayVal = true;
            compileExpression(expression);
        }
        else{
            compileExpressionList(expression);
        }

//        checks if an array is to be manipulated.
        skipExpressionParsing:
        if (m_isArrayDec){
            vmFile.writePop("temp", 1);
            vmFile.writePop("pointer", 1);
            vmFile.writePush("temp", 1);
            vmFile.writePop("that", 0);
            m_isArrayDec = false;
        }


    }

    if (m_currentSubroutineDecType == "constructor"){
        vmFile.writePop("this", classSymbolTable.index(varName.c_str()));
    }
    else{
        if (subroutineSymbolTable.index(varName.c_str()) != -1){
            vmFile.writePop(subroutineSymbolTable.kind(varName), subroutineSymbolTable.index(varName.c_str()));
        }
        else if (classSymbolTable.index(varName.c_str()) != -1){
            vmFile.writePop(classSymbolTable.kind(varName), classSymbolTable.index(varName.c_str()));
        }
    }

    m_currentLine++;
}

std::string& ltrim(std::string& str) {
    auto it = std::find_if(str.begin(), str.end(), [](char ch) {
        return !std::isspace(ch);
    });
    str.erase(str.begin(), it);
    return str;
}

std::string& rtrim(std::string& str) {
    auto it = std::find_if(str.rbegin(), str.rend(), [](char ch) {
        return !std::isspace(ch);
    });
    str.erase(it.base(), str.end());
    return str;
}

std::string& trim(std::string& str) {
    return ltrim(rtrim(str));
}

void CompilationEngine::compileReturn(const std::string& line) {
    std::string currentLine ;

    if (line.empty()){
        currentLine = getNthToken(m_currentLine);
    }
    else{
        currentLine = line;
    }

    tempTokens = splitString(currentLine, ' ');
    m_currentLine++;

//  All constructors HAVE TO return this, even if they don't do anything with it.
    if (m_currentSubroutineDecType == "constructor"){
        vmFile.writePush("pointer", 0);
        vmFile.writeReturn();
    }
    else if ((tempTokens.size() == 1) || tempTokens[1] == ";"){
        vmFile.writePush("constant", 0);
        vmFile.writeReturn();
    }
    else{
        std::string expression;
        expression = getNthToken(m_currentLine-1);
        expression = expression.substr(expression.find_first_of("return") + 6);
        trim(expression);

        if (expression.back() == '}'){
            expression.pop_back();
        }

        expression = clearName(expression);
        compileExpression(expression);
        vmFile.writeReturn();

        if (tempTokens.back() == "}"){
            insideSubroutine = false;
        }
    }
}

std::vector<std::string> CompilationEngine::splitString(std::string& str, char delim) {
    std::vector<std::string> splitVec;
    std::string split;

    std::size_t start = 0;
    std::size_t end = 0;

    while ((end = str.find(delim, start)) != std::string::npos) {
        split = str.substr(start, end - start);
        trim(split);
        if (!split.empty())
            splitVec.push_back(split);
        start = end + 1;
    }

    split = str.substr(start);
    trim(split);
    if (!split.empty())
        splitVec.push_back(split);

    return splitVec;
}

std::string CompilationEngine::generateLabel(const std::string& labelType) {
    if (labelCounts.find(labelType) == labelCounts.end()) {
        labelCounts[labelType] = 0;  // Initialize label count for this type
    }

    int& labelCount = labelCounts[labelType];  // Get a reference to the count
    labelCount++;  // Increment the count

    return labelType + std::to_string(labelCount);
}

void CompilationEngine::compileIf() {
    std::string continueIfLabel = generateLabel(CONTINUE_IF_LABEL_PREFIX);
    std::string elseBlockLabel = generateLabel(ELSE_LABEL_PREFIX);

    std::string currentLine = getNthToken(m_currentLine);
    tempTokens = splitString(currentLine, ' ');

    std::string expression;
    bool insideIf = false;
    bool inlineIf = false;
    bool use, trigger = false;

    if (currentLine.back() == '{'){
        insideIf = true;
        expression = removeBrackets(currentLine, false, roundBrackets);
    }
    else if (clearName(currentLine).back() == '}'){
        expression = removeBrackets(currentLine, false, roundBrackets);
        inlineIf = true;
    }

    compileExpression(expression);
    vmFile.writeArithmetic("~");

    bool hasElse = false;

    if (inlineIf){
        currentLine = currentLine.substr(currentLine.find('{')+1, currentLine.find('}') - currentLine.find('{')-1);
        currentLine = clearName(currentLine);

        if (m_code[m_currentLine+1].find("else") != std::string::npos){
            vmFile.writeIf(elseBlockLabel);
            hasElse = true;
        }
        else{
            vmFile.writeIf(continueIfLabel);
        }

        compileStatement(currentLine);

        if (hasElse){
            insideIf = true;
            vmFile.writeGoto(continueIfLabel);
            goto compileElse;
        }
        else{
            vmFile.writeLabel(continueIfLabel);
        }

        return;
    }

    vmFile.writeIf(elseBlockLabel);

    use = false;
    trigger = false;


    while((std::find(tempTokens.begin(), tempTokens.end(), "else{") == tempTokens.end()) || (std::find(tempTokens.begin(), tempTokens.end(), "else") == tempTokens.end())){
        if (trigger){
            goto compileElse;
        }

        if (!use){
            m_currentLine++;
        }

        if (tempTokens.empty()){
            m_currentLine += 1;
            tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
        }

        if ((std::find(tempTokens.begin(), tempTokens.end(), "else{") != tempTokens.end()) || (std::find(tempTokens.begin(), tempTokens.end(), "else") != tempTokens.end())){
            break;
        }
        else if (JackTokenizer::isValid(validStatementInitials, tempTokens[0])){
            if (tempTokens[0].find("else") != std::string::npos){
                insideIf = true;
                break;
            }
            tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
        }

        if (std::find(tempTokens.begin(), tempTokens.end(), "}") != tempTokens.end()){
            tempTokens  = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
            if (getNthToken(m_currentLine).find("else") != std::string::npos){
                trigger = true;
                continue;
            }
            else if (getNthToken(m_currentLine + 1).find("else") != std::string::npos){
                m_currentLine += 1;
                tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
                continue;
            }
            else{
                vmFile.writeLabel(elseBlockLabel);
                ++m_currentLine;
                return;
            }
        }

        compileStatement();
        use = true;
        tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
    }

//  these help the execution move to the correct spot in the code after it has executed the if statement
    vmFile.writeGoto(continueIfLabel);

    compileElse:
    if (insideIf){
        bool inlineElse = false;
        insideIf = false;

        currentLine = getNthToken(m_currentLine);
        trim(currentLine);

        if (currentLine == "}"){
            m_currentLine++;
        }
        else if (currentLine.back() == '}'){
            inlineElse = true;
        }

        if (getNthToken(m_currentLine).find("else") != std::string::npos){
            if (!inlineElse){
                m_currentLine++;
            }
        }

        vmFile.writeLabel(elseBlockLabel);

        currentLine = getNthToken(m_currentLine);
        tempTokens = splitString(currentLine, ' ');

        if (inlineElse){
            currentLine = currentLine.substr(currentLine.find('{')+1, currentLine.find('}') - currentLine.find('{')-1);
            currentLine = clearName(currentLine);
            compileStatement(currentLine);
            goto continueElse;
        }

        while(std::find(tempTokens.begin(), tempTokens.end(), "}") == tempTokens.end()){
            compileStatement();
        }

        if (JackTokenizer::isValid(validStatementInitials, tempTokens[0]) && tempTokens.back() == "}"){
            compileStatement();
        }


        m_currentLine++;
        continueElse:
        vmFile.writeLabel(continueIfLabel);
    }
}

void CompilationEngine::compileWhile() {
    std::string currentLine = getNthToken(m_currentLine);
    std::string expression = removeBrackets(currentLine, false, roundBrackets);
    int contCount = ++m_whileLabelCount;
    int contCount2 = ++m_continueWhileLabelCount;

    vmFile.writeLabel(WHILE_LABEL_PREFIX + std::to_string(contCount));
    compileExpression(expression);
    vmFile.writeArithmetic("~");
    vmFile.writeIf(CONTINUE_WHILE_LABEL_PREFIX + std::to_string(contCount2));

    m_currentLine++;
    while (std::find(tempTokens.begin(), tempTokens.end(), "}") == tempTokens.end()) {
        compileStatement();
        tempTokens = JackTokenizer::tokenizeCode(getNthToken(m_currentLine));
    }

    vmFile.writeGoto(WHILE_LABEL_PREFIX + std::to_string(contCount));
    m_whileLabelCount++;

    vmFile.writeLabel(CONTINUE_WHILE_LABEL_PREFIX + std::to_string(contCount2));
    m_currentLine++;
}

void CompilationEngine::removeTabs(std::vector<std::string>& string_vector) {
    for (int i = 0; i < string_vector.size(); i++) {
        string_vector[i].erase(std::remove(string_vector[i].begin(), string_vector[i].end(), '\t'), string_vector[i].end());
        if (string_vector[i].empty()) {
            string_vector.erase(string_vector.begin() + i);
            i--;
        }
    }
}

CODE CompilationEngine::depthSplit(std::string expression, const std::unordered_map<std::string, std::string>& brackets){
    int depth = 0;
    int index = 0;
    bool foundFirstCloseBracket = false;

    char openBracket = brackets.begin()->first[0];
    char closeBracket = brackets.begin()->second[0];

    if (expression.empty()){
        return {};
    }

    CODE exprVec;
    std::string choppedStr;
    int start = 0;
    expression.erase(std::remove_if(expression.begin(), expression.end(), ::isspace), expression.end());

    for (char &c: expression){
        if (c == openBracket){
            if (foundFirstCloseBracket){
                auto temp = expression.substr(start, index);
                temp = temp.substr(start, temp.find_last_of(closeBracket));
                auto op = expression.substr(index-1, 1);
                exprVec.push_back(temp);
                exprVec.push_back(op);
                expression = expression.substr(index);
                CODE k = depthSplit(expression, brackets);
                exprVec.insert(exprVec.end(), k.begin(), k.end());
                return exprVec;
            }
            depth++;
        }
        else if ((c == closeBracket) && (!(foundFirstCloseBracket))){
            foundFirstCloseBracket = true;
            depth--;
        }
        else if (c==closeBracket){
            depth--;
        }
        index++;
    }

    if (index!=0) {
        choppedStr = expression.substr(0, index);
        exprVec.push_back(choppedStr);
    }
    else{
        exprVec.push_back(expression);
    }

    return exprVec;
}

CODE CompilationEngine::splitNonArrayExprFromArrayExpr(std::string& expression){
    int index = 0;
    int lastOpIndex = -1;
    CODE split;

    std::string expressionC;
    expressionC.assign(expression);
    std::string str;
    int i = 0;
    expressionC.erase(std::remove_if(expressionC.begin(), expressionC.end(), ::isspace), expressionC.end());
    for (; i < expressionC.length(); i++){
        if (i+1 < expressionC.length()){
            std::string expr(0, expressionC[i]);

            if (!isNumber(expressionC[i])){
                expr = expressionC[i];
            }

            if (JackTokenizer::isValid(validOperations, expr)){
                if (lastOpIndex == -1) {
                    lastOpIndex = i;
                    if (!str.empty()){
                        split.push_back(str);
                        str.clear();
                    }
                    split.push_back(expressionC.substr(i, 1));
                    auto isArray = [](std::string str) -> bool {return (std::count(str.begin(), str.end(), ']') >= 1 && std::count(str.begin(), str.end(), '[') >= 1);};
                    if (!isArray(expressionC.substr(i+1))) {
                        if (expressionC.find('.') != std::string::npos) {
                            if (expressionC.find('(') != std::string::npos){
                                if (expressionC.back() != ')' && expressionC.find(')') == std::string::npos){
                                    expressionC += ')';
                                    auto k = expressionC.substr(i + 1);
                                    split.push_back(k);
                                    break;
                                }
                                else if (expressionC.find(')') != std::string::npos){
                                    if (expressionC.back() != ')'){
                                        auto k = expressionC.substr(i + 1);
                                        k = k.substr(0, k.find_first_of(')') + 1);
                                        split.push_back(k);
                                        i = -1;
                                        lastOpIndex = -1;
                                        expressionC = expressionC.substr(expressionC.find_first_of(k) + k.length() + 2);
                                    }
                                }
                                else {
                                    continue;
                                }
                            }
                        }
                    }
                    continue;
                }
                else{
                    split.push_back(str);
                    split.push_back(expressionC.substr(i, 1));
                    lastOpIndex = i;
                    str.clear();
                    continue;
                }
            }
            else if (std::isalnum(expressionC[i])){
                if (std::isalnum(expressionC[i + 1])){
                    i++;
                    continue;
                }
                else{
                    str += expressionC.substr(i, 1);
                }
                continue;
            }
            else if (expressionC[i] == ';'){
                continue;
            }
            else if (expressionC.ends_with(']') && !expressionC.starts_with('[')){
                if (lastOpIndex!=-1){
                    expressionC.erase(0, 1);
                }
                split.push_back(expressionC);
                return split;
            }
            else if (expressionC[i] == '['){
                auto s = expressionC.substr(lastOpIndex+1, expressionC.find_first_of(']') - lastOpIndex);
                split.push_back(s);
                expressionC = expressionC.substr(expressionC.find_first_of(']') + 1);
                str.clear();
                lastOpIndex = -1;
                i = -1;
                continue;
            }
        }
        else if (std::isalnum(expressionC[i])){
            str += expressionC.substr(i, 1);
        }
        else {
            if (!str.empty()){
                split.push_back(str);
                str.clear();
            }
        }
    }
    if (!str.empty()){
        split.push_back(str);
    }

    if (lastOpIndex != -1){
        expression = expressionC.substr(i);
    }
    return split;
}

CODE CompilationEngine::splitArrayExpr(std::string expression){
//  this helps keep a copy which is unchanged
    auto expressionC = expression;
    expression = expression.substr(expression.find_first_of('['));

    int i = 0, depth = 0;
    CODE split;
    bool firstRun = true;

    for (auto c: expression){
        if (c == '['){
            ++depth;
        }
        else if (c==']'){
            --depth;
        }

        if (depth == 0){
            ++i;
            auto str = expressionC.substr(0, i + (expressionC.length() - expression.length()));
            split.push_back(str);
            expression = expression.substr(i);
            if (expression.empty()){
                return split;
            }
            auto nonArrayExpr = splitNonArrayExprFromArrayExpr(expression);
            split.insert(split.end(), nonArrayExpr.begin(), nonArrayExpr.end());
            return split;
        }
        i++;
    }
    return split;
}

CODE CompilationEngine::compileArray( std::string& line) {
    m_arrayDepth++;
    std::string lineC = line;
    std::string lineC2 = line;
    std::string varName;

    auto arrayExpr = splitArrayExpr(line);
    lineC = arrayExpr[0].substr(1);
    lineC = removeBrackets(lineC, false, squareBrackets);
    varName = arrayExpr[0].substr(0, arrayExpr[0].find_first_of('['));
    CODE arrayExprVec(arrayExpr.begin() + 1, arrayExpr.end());

    compileExpression(varName);
    compileExpression(lineC);
//    m_insideArrayExp = true;

    vmFile.writeArithmetic("+");

    if (m_arrayDepth > 1 || m_isArrayVal) {
        vmFile.writePop("pointer", 1);
        vmFile.writePush("that", 0);
        lineC2.erase(std::remove_if(lineC2.begin(), lineC2.end(), ::isspace), lineC2.end());

        if (lineC2.back() != ']') {
            m_arrayDepth--;
            return arrayExprVec;
        } else if (!arrayExprVec.empty()) {
            m_arrayDepth--;
            return arrayExprVec;
        } else {
            line.clear();
        }
//    }
//    else{
//        line.clear();
//    }

    }
//    m_insideArrayExp = false;
    m_arrayDepth--;
    return arrayExprVec;
}

void CompilationEngine::pushVariable(std::string &variableName) {
    if (classSymbolTable.index(variableName.c_str()) != -1 || subroutineSymbolTable.index(variableName.c_str()) != -1){
        if (classSymbolTable.index(variableName.c_str()) != -1){
            vmFile.writePush(classSymbolTable.kind(variableName), classSymbolTable.index(variableName.c_str()));
        }
        else{
            vmFile.writePush(subroutineSymbolTable.kind(variableName), subroutineSymbolTable.index(variableName.c_str()));
        }
    }
}