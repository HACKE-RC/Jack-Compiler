#include <iostream>
#include <vector>
#include <regex>
#include "../JackTokenizer/JackTokenizer.hpp"
#include "../SymbolTable/SymbolTable.hpp"


static CODE validVarTypes = {"int", "char", "boolean"};
static CODE validSubroutineTypes = {"void", "int", "boolean", "char"};
static CODE validStatementInitials = {"var", "let", "do", "if", "else", "while", "return"};

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
    std::string getNthToken(int n);
    void compileClassVarDec(CODE tokens);
    static CODE removeBrackets(CODE code);
    static bool isNumber(std::string &str);
    void compileExpression(std::string expr);
    static bool isValidName(std::string name);
    static long long countParameters(CODE parameterList);
    static std::string removeBrackets(const std::string& str);
    static char isCharacterPresent(const std::string &str, const std::string &str2);
};

