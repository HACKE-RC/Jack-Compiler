#include <iostream>
#include <vector>
#include <regex>
#include "../JackTokenizer/JackTokenizer.hpp"
#include "../SymbolTable/SymbolTable.hpp"


static CODE validVarTypes = {"int", "char", "boolean"};
static CODE validSubroutineTypes = {"void", "int", "boolean", "char"};

class CompilationEngine {
public:
    explicit CompilationEngine(std::string fName);

private:
    CODE tempTokens;


private:
    CODE m_code;
    SymbolTable classSymbolTable;
    SymbolTable subroutineSymbolTable;
    int m_currentLine;

private:
    void compileClass();
    void compileClassVarDec(CODE tokens);
    void compileSubroutine();
    void compileVarDec();
    void compileParameterList();
    static long long countParameters(CODE parameterList);
    static bool isValidName(std::string name);
    std::string getNthToken(int n);
};

