#include <iostream>
#include <vector>
#include <regex>
#include <numeric>
#include <string>
#include <algorithm>
#include <cctype>
#include "../JackTokenizer/JackTokenizer.hpp"
#include "../SymbolTable/SymbolTable.hpp"
#include "../VMWriter/VMWriter.hpp"

static CODE validVarTypes = {"int", "char", "boolean"};
static CODE validSubroutineTypes = {"void", "int", "boolean", "char"};
static CODE validStatementInitials = {"var", "let", "do", "if", "else", "while", "return"};
static std::vector<char> validPositiveInt = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
static CODE validBooleanOp = {"&", "|"};

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
    std::string m_outfName;
private:
    CODE tempTokens;
    CODE m_code;

private:
    SymbolTable classSymbolTable;
    SymbolTable subroutineSymbolTable;

private:
    int m_currentLine;
    int elseBlockLabelCount = 0;
    int m_continueIfLabelCount = 0;
    int m_continueWhileLabelCount = 0;
    int m_whileLabelCount = 0;

private:
    std::string m_currentSubroutineDef;
    std::string m_currentSubroutineDecType;
    std::streamoff m_funcNameIndex{};

public:
    void compileIf();
    void compileClass();
    void compileWhile();
    int compileVarDec();
    void compileSubroutine();
    void compileClassVarDec();
    void compileParameterList();
    void compileSubroutineBody();
    static bool isNumber(char &ch);
    std::string getNthToken(int n);
    void compileTerm(std::string term);
    static CODE removeBrackets(CODE code);
    static bool isNumber(std::string &str);
    CODE depthSplit(std::string expression);
    void compileStatement(std::string line);
    void compileDo(const std::string& line);
    void compileLet(const std::string& line);
    static bool isValidName(std::string name);
    void compileExpression(std::string &expr);
    void compileReturn(const std::string& line);
    static std::string clearName(std::string name);
    CODE getParameterStrings(std::string const line);
    static CODE getExpressionVector(std::string expr);
    static long long countParameters(CODE parameterList);
    std::string prioritizeBrackets(std::string& expression);
    bool isCharacterPresent(const std::string& str1, char c);
    void compileExpressionList(const std::string& expressions);
    static void removeTabs(std::vector<std::string>& string_vector);
    std::vector<std::string> splitString(std::string &str, char delim);
    static std::string removeBrackets(const std::string& str, bool inLine);
    void callSubroutine(std::string line, std::string funcName, int objAddition);
};
