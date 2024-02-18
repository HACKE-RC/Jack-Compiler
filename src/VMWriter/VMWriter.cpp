#include "VMWriter.hpp"
#include <utility>

VMWriter::VMWriter(std::string fName) {
    m_fName = std::move(fName);
    m_fStream = std::ofstream(m_fName);

    if (!m_fStream.good()) {
        std::cerr << "ERR: File not found" << std::endl;
    }
}

void VMWriter::writePush(const std::string& segment, int index) {
    std::string code = "push " + segment + " " + std::to_string(index) + "\n";
    m_fStream.write(code.c_str(), code.length());
    consume();
}

void VMWriter::writePop(std::string segment, int index) {
    std::string code = "pop " + segment + " " + std::to_string(index) + "\n";
    m_fStream.write(code.c_str(), code.length());
    consume();
}

void VMWriter::writeArithmetic(std::string command) {
    if (command == "+") {
        m_fStream << "add\n";
        consume();
    }
    else if (command == "-") {
        m_fStream << "sub\n";
        consume();
    }
    else if (command == "*") {
        std::string code = "call Math.multiply 2\n";
        m_fStream.write(code.c_str(), code.length());
        consume();
    }
    else if (command == "/") {
        std::string code = "call Math.divide 2\n";
        m_fStream.write(code.c_str(), code.length());
        consume();
    }
    else if (command == "&") {
        m_fStream << "and\n";
        consume();
    }
    else if (command == "|") {
        m_fStream << "or\n";
        consume();
    }
    else if (command == "<") {
        m_fStream << "lt\n";
        consume();
    }
    else if (command == ">") {
        m_fStream << "gt\n";
        consume();
    }
    else if (command == "=") {
        m_fStream << "eq\n";
        consume();
    }
    else if (command == "~") {
        m_fStream << "not\n";
        consume();
    }
}

void VMWriter::writeNeg() {
    m_fStream << "neg\n";
    consume();
}

void VMWriter::writeLabel(std::string name) {
    m_fStream << "label " + name + "\n";
    consume();
}

void VMWriter::writeGoto(std::string name) {
    m_fStream << "goto " + name + "\n";
    consume();
}

void VMWriter::writeIf(std::string label) {
    m_fStream << "if-goto " + label + "\n";
    consume();
}

void VMWriter::writeCall(std::string functionName, int nArgs) {
    m_fStream << "call " + functionName + " " + std::to_string(nArgs) + "\n";
    consume();
}

void VMWriter::writeFunction(std::string functionName, int nVars) {
    m_fStream << "function " + functionName + " " + std::to_string(nVars) + "\n";
    consume();
}

void VMWriter::writeReturn() {
    m_fStream << "return\n";
    consume();
}

void VMWriter::consume() {
    m_lineNo++;
}

std::streamoff VMWriter::getLineNo() {
    return m_fStream.tellp();
}

void VMWriter::writeComment(std::string comment){
    m_fStream << "// " + comment + "\n";
    consume();
}

VMWriter::VMWriter() = default;
