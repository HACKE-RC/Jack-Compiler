#include <iostream>
#include <vector>
#include <regex>
#include <numeric>
#include <algorithm>
#include "../JackTokenizer/JackTokenizer.hpp"
#include "../SymbolTable/SymbolTable.hpp"


static CODE validVarTypes = {"int", "char", "boolean"};
static CODE validSubroutineTypes = {"void", "int", "boolean", "char"};
static CODE validStatementInitials = {"var", "let", "do", "if", "else", "while", "return"};
static std::vector<char> validInt = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

class CompilationEngine {
public:
    explicit CompilationEngine(std::string fName);
    CODE vmCode = {};

private:
    CODE tempTokens;

private:
    CODE m_code;
    SymbolTable classSymbolTable;
    SymbolTable subroutineSymbolTable;
    int m_currentLine;

private:
    void compileDo();
    void compileClass();
    void compileVarDec();
    void compileStatement();
    void compileSubroutine();
    void compileParameterList();
    void compileSubroutineBody();
    void compileExpressionList();
    static bool isNumber(char &ch);
    std::string getNthToken(int n);
    void compileTerm(std::string term);
    void compileClassVarDec(CODE tokens);
    static CODE removeBrackets(CODE code);
    static bool isNumber(std::string &str);
    void compileExpression(std::string &expr);
    static bool isValidName(std::string name);
    static std::string clearName(std::string name);
    static CODE getExpressionVector(std::string& expr);
    static long long countParameters(CODE parameterList);
    std::string prioritizeBrackets(std::string& expression);
    static std::string removeBrackets(const std::string& str);
    static std::vector<std::string> splitString(std::string &str, char delim);
    static char isCharacterPresent(const std::string &str, const std::string &str2);
};