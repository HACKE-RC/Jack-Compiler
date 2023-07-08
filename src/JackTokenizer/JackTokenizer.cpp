#include "JackTokenizer.hpp"

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
            std::exit(-1);
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

void JackTokenizer::tokenizeAllCode() {
    CODE tokens;
    CODE temp_tokens;

    for (auto &code: m_code) {
        auto tokenNoSpace = std::find_if_not(code.rbegin(), code.rend(), ::isspace).base();
        std::string token(code.begin(), tokenNoSpace);
        auto tokenNoSpace2 = std::find_if_not(token.begin(), token.end(), ::isspace).base();
        token = tokenNoSpace2;

        if (isNotEmpty(token)){
            temp_tokens = tokenizeCode(token);
            for (auto &item: temp_tokens){
                std::cout << item << std::endl;
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

bool JackTokenizer::isValid(const std::vector<char> &vec, char ch) {
   std::ranges::any_of(vec, [&](char c) { return c == ch; });
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

CODE JackTokenizer::tokenizeCode(std::string str) {
    CODE vec;
    std::string item;
    std::string temp2;
    size_t idx = 0;

    while (idx != std::string::npos) {
        idx = str.find(32);
        item = str.substr(0, idx);
        item.erase(remove(item.begin(), item.end(), ' '), item.end());

        if ((item.find('(') != std::string::npos)){
            idx = str.find('(');
            auto idx2 = str.find(')');
            if ((idx+1 != idx2)){
                parseFuncParams(str, vec);
                break;
            }

            idx = str.find(32);
        }

        if (item.find(',') != std::string::npos) {
            item = str.substr(0, str.find_last_of(',')+1);
            idx = str.find_last_of(',')+1;
        }

        if (isNotEmpty(item)) {


            if (item.find(',') != std::string::npos) {
                splitComma(item, vec);
            }

            if (item.empty()){
                item = str.substr(idx);
                item = item.substr(item.find_first_not_of(' '));
            }
            item = addBrackets(item, vec);
            if (addSemicolon(item, vec) == -1){ break; }
            addCurlyBrackets(item, vec);
            str = str.substr(idx + 1);
            if (str.find('.') !=  std::string::npos){
                addDot(str, vec);
                if (str.find('(')){
                    continue;
                }
            }
            if (idx >= str.length()) {
                if (str == "{"){
                    addCurlyBrackets(str, vec);
                    break;
                }
                if (!item.empty()){
                    addSemicolon(str, vec);
                }
                break;
            }
        }
        else{
            str = str.substr(idx + 1);
        }
    }

    vec.erase(std::remove(vec.begin(), vec.end(),""), vec.end());

    return vec;
}

void JackTokenizer::splitComma(std::string& str, std::vector<std::string>& vec){
    CODE temp;
    std::string str2 = str;

    size_t idx = 0;
    std::string s2;

    while(idx!=std::string::npos){
        idx = str.find(' ');
        if (idx == std::string::npos){
            temp.push_back(str);
        }
        else{
            s2 = str.substr(0, idx);
            temp.push_back(s2);
            str = str.substr(idx+1);

        }
    }

    for (auto k: temp){

        k = addBrackets(k , vec);
        auto idx2 = k.find(',');
        if (idx2 != std::string::npos){
            s2 = k.substr(0, idx2);
            vec.push_back(s2);
            std::string s3(1, ',');
            vec.push_back(s3);
            if (k.ends_with(',')){
                str = "";
            }
        }
        else{
            auto item = addBrackets(k, vec);
            int status = addSemicolon(item, vec);
            if (status==-1){
                str = "";
            }
            else{
                if ((item.find(',') != std::string::npos) || (item.find(';') != std::string::npos)){
                    idx2 = item.find(',');
                    char to_find = idx2 == std::string::npos ? ';' : ',';
                    str = str2.substr(item.find_last_of(to_find));
                }
                else if((item.find('(') != std::string::npos) || (item.find(')') != std::string::npos)){
                    idx2 =  item.find('(');
                    char to_find = idx2 == std::string::npos ? ')' : '(';
                    str = str2.substr(item.find_last_of(to_find));
                }
                else if ((item.find('{') != std::string::npos) || (item.find('}')) != std::string::npos){
                    idx2 = item.find('{');
                    char to_find = idx2 == std::string::npos ? '}' : '{';
                    str = str2.substr(item.find_last_of(to_find));
                }
            }
        }
    }
}

std::string JackTokenizer::addBrackets(std::string item, CODE &vec) {
    std::string temp2;

    if (item.find('(') != std::string::npos) {
        temp2 = item.substr(0, item.find('('));
        vec.push_back(temp2);
        vec.push_back("(");
        item = item.substr(item.find('(') + 1);
    }

    if (item.find(')') != std::string::npos) {
        if (item.ends_with(')') && !(item == ")")) {
            temp2 = item;
            temp2 = temp2.substr(0, temp2.length() - 1);
            vec.push_back(temp2);
        }
        else if (item.find(')') != std::string::npos){
            temp2 = item;
            temp2 = temp2.substr(0, temp2.find(')'));
            vec.push_back(temp2);
        }
        vec.push_back(")");
        item = item.substr(item.find(')') + 1);
    }
    return item;
}

int JackTokenizer::addSemicolon(std::string& item, CODE &vec) {
    std::string temp2;

    if (item.ends_with(';') && !(item == ";")) {
        temp2 = item;
        item = item.substr(0, item.find(';'));
    }

    if (item != "}"){
        vec.push_back(item);
    }

    if (temp2.ends_with(';') && !(item == ";")) {
        vec.push_back(";");
        auto idx = temp2.find(';');
        item = item.substr(idx);

        return 0;
    }

    if (item == ";"){
        return -1;
    }

    return 0;
}

int JackTokenizer::addCurlyBrackets(std::string &item, CODE &vec) {
    if (item == "{" || item == "}") {
        vec.push_back(item);
        item = "";
    }
    return 0;
}

int JackTokenizer::parseFuncParams(std::string &item, CODE &vec) {
    if (item.find('(') != std::string::npos) {
        splitComma(item, vec);
    }
    return 0;
}

void JackTokenizer::addDot(std::string &item, CODE &vec) {
    std::string temp2;
    if (item.find('.') != std::string::npos) {
        temp2 = item.substr(0, item.find('.'));
        vec.push_back(temp2);
        vec.push_back(".");
        item = item.substr(item.find('.') + 1);
    }
}

CODE JackTokenizer::getAllCodeVector() {
    return m_code;
}
