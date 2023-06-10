#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <filesystem>
//#include "../tinyxml2-9.0.0/tinyxml2.h"
#include "../pugixml/pugixml.hpp"


typedef std::vector<std::string> CODE;
typedef pugi::xml_node node;

class JackTokenizer{
private:
    CODE m_code;

    std::string m_outfName;
    std::string m_fName;

private:
    enum statusCodes{
        ERROR_FILE_NOT_FOUND = -1,
    };

    struct codeInfo{
        bool classDec = false;
        bool varDec = false;
        bool classVarDec = false;
        bool classDecBegin;
        bool varDecEnd = false;
        bool subroutineDec = false;
        bool codeSwitch = false;
        bool parameterDec = false;
        bool subroutineBodyBegin = false;
        bool subroutineStatement = false;
        bool subroutineVarDec = false;
        bool varExpression = false;
        bool varTerm = false;
        bool doStatement = false;
    } codeInfo;

    CODE validKeywords = {"if", "while", "class", "constructor", "function", "method", "field", "static", "var", "int",
                          "char", "boolean", "void", "true", "false", "this", "let", "do", "else", "return"};
    CODE validSymbols = {"{", "}", "(", ")", "[", "]", ".",  ",",  ";",  "+", "-", "*", "/", "&", "|", "<", ">", "=", "~"};
    CODE validVarDecs = {"static", "field", "var", "let"};
    CODE validSubroutineDec = {"function", "method", "constructor"};


public:
   explicit JackTokenizer(std::string fName, std::string outfName);

public:
    void printCode();
    void cleanCode();

public:
    void tokenizeCode();
    static int parseFuncParams(std::string& item, CODE &vec);

    static void splitComma(std::string& str, CODE& vec);
    static std::string addBrackets(std::string item, CODE &vec);
    static int addSemicolon(std::string& item, CODE &vec);
    static int addCurlyBrackets(std::string& item, CODE &vec);
//    static void test();

private:
    static void to_lower(std::string &str);
    static bool isValid(CODE const& vec, std::string& str);
    static bool isNotEmpty(std::string& str);
    static CODE getCodeVector(std::string str);
};