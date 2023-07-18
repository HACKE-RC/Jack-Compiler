#include <iostream>
#include <vector>
#include <regex>
#include <numeric>
#include <string>
#include <algorithm>
#include <cctype>
#include "../JackTokenizer/JackTokenizer.hpp"
#include "../SymbolTable/SymbolTable.hpp"


static CODE validVarTypes = {"int", "char", "boolean"};
static CODE validSubroutineTypes = {"void", "int", "boolean", "char"};
static CODE validStatementInitials = {"var", "let", "do", "if", "else", "while", "return"};
static std::vector<char> validPositiveInt = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
static std::vector<std::string> validNegativeInt = {"-1", "-2", "-3", "-4", "-5", "6", "7", "8", "9"};

class CompilationEngine {
public:
    explicit CompilationEngine(std::string fName);
    CODE vmCode = {};

private:
    bool insideClass = false;
    bool insideSubroutine = false;
    bool insideWhile = false;

private:
    std::string m_currentClassName;

private:
    std::map<std::string, std::string> reservedValues = {
            {"true", "-1"},
            {"false", "0"}
    };

    std::map<std::string, std::string> subroutineTypes;

    std::string ELSE_LABEL_PREFIX = "LABEL_ELSE_";
    std::string CONTINUE_IF_LABEL_PREFIX = "LABEL_CONTINUE_IF_";
    std::string CONTINUE_WHILE_LABEL_PREFIX = "LABEL_CONTINUE_WHILE_";
    std::string WHILE_LABEL_PREFIX = "LABEL_WHILE_";

private:
    CODE tempTokens;
    CODE m_code;

private:
    SymbolTable classSymbolTable;
    SymbolTable subroutineSymbolTable;

private:
    int m_currentLine;
    int m_ifLabelCount = 0;
    int m_continueIfLabelCount = 0;
    int m_continueWhileLabelCount = 0;
    int m_whileLabelCount = 0;

private:
    void compileDo();
    void compileIf();
    void compileLet();
    void compileClass();
    void compileWhile();
    void compileReturn();
    void compileVarDec();
    void compileStatement();
    void compileSubroutine();
    void compileParameterList();
    void compileSubroutineBody();
    static bool isNumber(char &ch);
    std::string getNthToken(int n);
    void compileTerm(std::string term);
    void compileClassVarDec(CODE tokens);
    void callSubroutine(std::string line);
    static CODE removeBrackets(CODE code);
    static bool isNumber(std::string &str);
    static bool isValidName(std::string name);
    void compileExpression(std::string &expr);
    static std::string clearName(std::string name);
    static CODE getExpressionVector(std::string expr);
    void compileExpressionList(std::string expressions);
    static long long countParameters(CODE parameterList);
    std::string prioritizeBrackets(std::string& expression);
    static std::string removeBrackets(const std::string& str);
    static void removeTabs(std::vector<std::string>& string_vector);
    std::vector<std::string> splitString(std::string &str, char delim);
    static char isCharacterPresent(const std::string &str, const std::string &str2);
};