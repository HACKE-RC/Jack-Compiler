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
    increaseLineNo();
}

void VMWriter::writePop(std::string segment, int index) {
    std::string code = "pop " + segment + " " + std::to_string(index) + "\n";
    m_fStream.write(code.c_str(), code.length());
    increaseLineNo();
}

void VMWriter::writeArithmetic(std::string command) {
    if (command == "+") {
        m_fStream << "add\n";
        increaseLineNo();
    }
    else if (command == "-") {
        m_fStream << "sub\n";
        increaseLineNo();
    }
    else if (command == "*") {
        std::string code = "call Math.multiply 2\n";
        m_fStream.write(code.c_str(), code.length());
        increaseLineNo();
    }
    else if (command == "/") {
        std::string code = "call Math.divide 2\n";
        m_fStream.write(code.c_str(), code.length());
        increaseLineNo();
    }
    else if (command == "&") {
        m_fStream << "and\n";
        increaseLineNo();
    }
    else if (command == "|") {
        m_fStream << "or\n";
        increaseLineNo();
    }
    else if (command == "<") {
        m_fStream << "lt\n";
        increaseLineNo();
    }
    else if (command == ">") {
        m_fStream << "gt\n";
        increaseLineNo();
    }
    else if (command == "=") {
        m_fStream << "eq\n";
        increaseLineNo();
    }
    else if (command == "~") {
        m_fStream << "not\n";
        increaseLineNo();
    }
}

void VMWriter::writeNeg() {
    m_fStream << "neg\n";
    increaseLineNo();
}

void VMWriter::writeLabel(std::string name) {
    m_fStream << "label " + name + "\n";
    increaseLineNo();
}

void VMWriter::writeGoto(std::string name) {
    m_fStream << "goto " + name + "\n";
    increaseLineNo();
}

void VMWriter::writeIf(std::string label) {
    m_fStream << "if-goto " + label + "\n";
    increaseLineNo();
}

void VMWriter::writeCall(std::string functionName, int nArgs) {
    m_fStream << "call " + functionName + " " + std::to_string(nArgs) + "\n";
    increaseLineNo();
}

void VMWriter::writeFunction(std::string functionName, int nVars) {
    m_fStream << "function " + functionName + " " + std::to_string(nVars) + "\n";
    increaseLineNo();
}

void VMWriter::writeReturn() {
    m_fStream << "return\n";
    increaseLineNo();
}

void VMWriter::increaseLineNo() {
    m_lineNo++;
}

std::streamoff VMWriter::getLineNo() {
    return m_fStream.tellp();
}

void VMWriter::writeAtPos(long long pos, std::string s) {
    auto current = getLineNo();
    m_fStream.seekp(pos);
    m_fStream.write(s.c_str(), s.length());
    m_fStream.seekp(current);
}

//VMWriter::~VMWriter() {
//    m_fStream.close();
//}

VMWriter::VMWriter() = default;
