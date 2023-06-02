#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include "../pugixml/pugixml.hpp"

typedef std::vector<std::string> CODE;

class JackTokenizer{
private:
    CODE m_code;

    std::string m_outfName;
    std::string m_fName;

private:
    enum statusCodes{
        ERROR_FILE_NOT_FOUND = -1,
    };

    CODE validKeywords = {"if", "while", "class", "constructor", "function", "method", "field", "static", "var", "int",
                          "char", "boolean", "void", "true", "false", "this", "let", "do", "else", "return"};
    CODE validSymbols = {"{", "}", "(", ")", "[", "]", ".",  ",",  ";",  "+", "-", "*", "/", "&", "|", "<", ">", "=", "~"};

public:
   explicit JackTokenizer(std::string fName, std::string outfName);

public:
    void printCode();
    void cleanCode();

public:
    void tokenizeCode();

private:
    static void to_lower(std::string &str);
    static bool isValid(CODE const& vec, std::string& str);
    static bool isNotEmpty(std::string& str);
    static CODE splitString(std::string str, char deli);
};