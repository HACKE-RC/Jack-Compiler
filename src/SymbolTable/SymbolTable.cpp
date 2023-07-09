#include "SymbolTable.hpp"

SymbolTable::SymbolTable() {
    symbolTable sTable;
    m_symbolTable = sTable;
}

void SymbolTable::insert(std::string name, std::string type, const std::string& kind) {
    SymbolTableEntry symbolTableEntry = {std::move(name), std::move(type), kind};

    if (kind == "static"){
        kindCount.staticCount++;
        symbolTableEntry.index = kindCount.staticCount;
        m_symbolTable[m_count] = symbolTableEntry;
    }
    else if (kind == "field"){
        kindCount.fieldCount++;
        symbolTableEntry.index = kindCount.fieldCount;
        m_symbolTable[m_count] = symbolTableEntry;
    }
    else if (kind == "var" || kind == "local"){
        kindCount.varCount++;
        symbolTableEntry.index = kindCount.varCount;
        m_symbolTable[m_count] = symbolTableEntry;
    }
    else if (kind == "argument"){
        kindCount.argCount++;
        symbolTableEntry.index = kindCount.argCount;
        m_symbolTable[m_count] = symbolTableEntry;
    }

    m_count++;
}

std::string SymbolTable::type(const std::string& name) {

    for (int i = 0; i < m_count; i++) {
        if (m_symbolTable[i].name == name) {
            return m_symbolTable[i].type;
        }
    }

    return "";
}

std::string SymbolTable::kind(const std::string &name) {
    for (int i = 0; i < m_count; i++) {
        if (m_symbolTable[i].name == name) {
            return m_symbolTable[i].kind;
        }
    }

    return "";
}

int SymbolTable::count(const std::string& kind) const {
    if (kind=="static"){
        return kindCount.staticCount;
    }
    else if (kind=="field"){
        return kindCount.fieldCount;
    }
    else if (kind=="local" || kind=="var"){
        return kindCount.varCount;
    }
    else if (kind=="argument"){
        return kindCount.argCount;
    }

    return 0;
}

int SymbolTable::index(const char* str){
    for (int i = 0; i < m_count; i++) {
        if (m_symbolTable[i].name == str) {
            return m_symbolTable[i].index - 1;
        }
    }

    return -1;
}

void SymbolTable::reset() {
    symbolTable sTable;
    m_symbolTable = sTable;
    m_count = 0;
    kindCount.staticCount = 0;
    kindCount.fieldCount = 0;
    kindCount.argCount = 0;
    kindCount.varCount = 0;
}

void SymbolTable::display() {
    for (int i = 0; i < m_count; i++) {
        std::cout << m_symbolTable[i].name << ": " << m_symbolTable[i].type << ": " << m_symbolTable[i].kind << std::endl;
    }

}
