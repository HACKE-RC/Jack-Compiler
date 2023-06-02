#include "JackTokenizer.hpp"

#include <utility>

JackTokenizer::JackTokenizer(std::string fName, std::string outfName) {
    CODE code;

    std::vector<std::string> fNames = {};

    if (std::filesystem::is_directory(fName.c_str())){

        for (const auto& entry: std::filesystem::directory_iterator(fName.c_str())){
            if (std::filesystem::is_regular_file(entry)){
                auto filename = entry.path().filename().string();
                auto full_filename = fName.append("\\" + filename);
                if (filename.ends_with(".jack") && std::find(fNames.begin(), fNames.end(), full_filename) == fNames.end()){
                    fNames.push_back(full_filename);
                }
            }
        }
        if (std::filesystem::is_directory(outfName.substr(0, outfName.find_last_of("\\")))){
            this->m_outfName = outfName;
        }
        else{
            this->m_outfName = fName + "\\" +  outfName;
        }
    }
    else{
        fNames.push_back(fName);
        this->m_outfName = outfName.append(".asm");
    }
    for (const auto& file: fNames){
        std::ifstream fStream(file);
        std::string fData;

        if (!fStream.good()){
            std::cerr << "ERR: " << "File not found!" << std::endl;
            std::exit(ERROR_FILE_NOT_FOUND);
        }

        while (getline(fStream, fData)) {
            m_code.push_back(fData);
        }
    }

}

void JackTokenizer::to_lower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
}

void JackTokenizer::printCode() {
    for (const auto& token: this->m_code){
        std::cout << token << std::endl;
    }
}

void JackTokenizer::cleanCode() {
    CODE tokens;
    std::string tok;

    for (const auto& token: this->m_code){
        if (token.starts_with("/") || token.empty()) {continue;}
        auto slash_idx = token.find("/");

        if (slash_idx != std::string::npos){
            tok = token.substr(0, slash_idx);
        }
        else{
            tok = token;
        }
        tokens.push_back(tok);
    }
    this->m_code = tokens;
}

void JackTokenizer::tokenizeCode() {
    CODE tokens;
    CODE temp_tokens;

    for (auto &code: m_code) {

//      C++ magic to remove trailing spaces
        auto tokenNoSpace = std::find_if_not(code.rbegin(), code.rend(), ::isspace).base();
        std::string token(code.begin(), tokenNoSpace);
        auto tokenNoSpace2 = std::find_if_not(token.begin(), token.end(), ::isspace).base();
        token = tokenNoSpace2;

        if (isNotEmpty(token)){
            temp_tokens = splitString(token, ' ');
            for (auto &item: temp_tokens){
                if (isValid(validKeywords, item) || isValid(validSymbols, item)){
                    std::cout << item << std::endl;
                    tokens.push_back(token);
                }
                else{
//                    lookup the grammar rules
                }
            }
        }
    }
}

bool JackTokenizer::isValid(const CODE &vec, std::string &str) {
    if (std::count(vec.begin(), vec.end(), str)){
        return true;
    }
    return false;
}

bool JackTokenizer::isNotEmpty(std::string& str) {
     std::string temp = str;
     temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end());

     if (!temp.empty()){
         return true;
     }

    return false;
}

CODE JackTokenizer::splitString(std::string str, char delim) {
    CODE vec;
    std::string temp;

    auto idx = str.find(delim);
    while (idx != std::string::npos) {
        temp = str.substr(0, idx);
        temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end());
        if (isNotEmpty(temp)){
         vec.push_back(temp);
        }

        str = str.substr(idx + 1);
        if (idx >= str.length()){
           break;
        }
    }
    return vec;
}