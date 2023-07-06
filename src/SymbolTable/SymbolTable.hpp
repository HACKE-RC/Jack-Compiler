#ifndef JACK_LEX_SYMBOLTABLE_HPP
#define JACK_LEX_SYMBOLTABLE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

class SymbolTable {
public:
    explicit SymbolTable();

public:
    struct KindCount{
        int staticCount = 0;
        int fieldCount = 0;
        int varCount = 0;
        int argCount = 0;
    } kindCount;

private:
    struct SymbolTableEntry{
        std::string name;
        std::string type;
        std::string kind;
        int index;
    };

    typedef std::unordered_map<int, SymbolTableEntry> symbolTable;

private:
    symbolTable m_symbolTable;
    int m_count = 0;
//    int

public:
    void insert(std::string name, std::string type, const std::string& kind);
    std::string kind(const std::string& name);
    std::string type(const std::string& name);
    int count(const std::string& kind) const;
    int index(const char* name);
    void reset();
    void display();
};

#endif