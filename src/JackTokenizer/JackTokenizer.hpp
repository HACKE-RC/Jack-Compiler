//ifdef for this file
#ifndef JACK_LEX_JACKTOKENIZER_HPP
#define JACK_LEX_JACKTOKENIZER_HPP


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <filesystem>

typedef std::vector<std::string> CODE;

static CODE validVarDecs = {"static", "field", "var", "let"};
static CODE validKeywords = {"if", "while", "class", "constructor", "function", "method", "field", "static", "var", "int",
                      "char", "boolean", "void", "true", "false", "this", "let", "do", "else", "return"};
static CODE validSymbols = {"{", "}", "(", ")", "[", "]", ".",  ",",  ";",  "+", "-", "*", "/", "&", "|", "<", ">", "=", "~"};
static CODE validSubroutineDec = {"function", "method", "constructor"};
static CODE validOperations = {"+", "-", "/", "*", "=", ">", "<", "&", "|"};


class JackTokenizer{
private:
    CODE m_code;
    std::string m_outfName;
    std::string m_fName;


public:
   explicit JackTokenizer(std::string fName, std::string outfName);

public:
    void printCode();
    void cleanCode();
    void tokenizeAllCode();
    CODE getAllCodeVector();

public:
    static int parseFuncParams(std::string& item, CODE &vec);
    static void splitComma(std::string& str, CODE& vec);
    static std::string addBrackets(std::string item, CODE &vec);
    static int addSemicolon(std::string& item, CODE &vec);
    static void addDot(std::string &item, CODE &vec);
    static int addCurlyBrackets(std::string& item, CODE &vec);
    static CODE tokenizeCode(std::string str);
    static bool isValid(CODE const& vec, std::string& str);
    static bool isValid(const std::vector<char> &vec, char ch);
    static bool isNotEmpty(std::string& str);

private:
    static void to_lower(std::string &str);
};

#endif
