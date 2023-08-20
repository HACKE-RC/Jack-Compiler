#ifndef JACK_LEX_VMWRITER_HPP
#define JACK_LEX_VMWRITER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class VMWriter {
public:
    explicit VMWriter(std::string fName);
    explicit VMWriter();

public:
    void writePush(const std::string& segment, int index);
    void writePop(std::string segment, int index);

public:
    std::string m_fName;
    std::ofstream m_fStream;
    int m_lineNo = 1;

    void writeArithmetic(std::string command);
    void writeNeg();
    void writeLabel(std::string label);
    void writeGoto(std::string label);
    void writeIf(std::string label);
    void writeCall(std::string functionName, int nArgs);
    void writeFunction(std::string functionName, int nVars);
    void writeReturn();
    void increaseLineNo();
    std::streamoff getLineNo();
    void writeAtPos(long long pos, std::string s);
};

#endif
