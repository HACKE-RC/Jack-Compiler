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

    std::string lexiconType;
    std::string xmlElement;
    std::string str;

    pugi::xml_document xmlDoc;
    node class_node;
    int counter = 0;

//    bool classDec = false;
//    bool classVarDec = false;

    for (auto &code: m_code) {

//      C++ magic to remove trailing spaces
        auto tokenNoSpace = std::find_if_not(code.rbegin(), code.rend(), ::isspace).base();
        std::string token(code.begin(), tokenNoSpace);
        auto tokenNoSpace2 = std::find_if_not(token.begin(), token.end(), ::isspace).base();
        token = tokenNoSpace2;
        node classVarNode;

        if (isNotEmpty(token)){
            temp_tokens = splitString(token, ' ');

            for (auto &item: temp_tokens){
                if (isValid(validKeywords, item) || isValid(validSymbols, item)){
                    if (item == "class"){
                        class_node = xmlDoc.append_child("class");
//                        classDec = true;
                    }
                    node type_node;

                    lexiconType = isValid(validKeywords, item) ? "keyword" : "symbol";
//                    class_node = classVarDec ? classVarNode : class_node;

                   type_node = class_node.append_child(lexiconType.c_str());

                    type_node.append_child(pugi::node_pcdata).set_value(item.c_str());

//                    if (classDec && (counter == 2)){
//                        classVarDec = true;
//                        classVarNode = class_node.append_child("classVarDec");
//                        classDec = false;
//                        counter = 0;
//                    }
//                    counter++;
                }
                else{
                    if (isNotEmpty(item)) {
                        lexiconType = "identifier";
                        node type_node = class_node.append_child(lexiconType.c_str());
                        type_node.append_child(pugi::node_pcdata).set_value(item.c_str());
//                        counter++;
                    }
                }
            }
        }
    }

    xmlDoc.save(std::cout);
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
    std::string temp2;
    size_t idx = 0;

    while (idx != std::string::npos) {
        idx = str.find(delim);
        temp = str.substr(0, idx);
        temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end());
        if (isNotEmpty(temp)) {
            if (temp.find('.') != std::string::npos) {
                temp2 = temp.substr(0, temp.find('.'));
                vec.push_back(temp2);
                vec.push_back(".");
                temp = temp.substr(temp.find('.') + 1);
            }

            if (temp.find('(') != std::string::npos) {
                temp2 = temp.substr(0, temp.find('('));
                vec.push_back(temp2);
                vec.push_back("(");
                temp = temp.substr(temp.find('(') + 1);
            }

            if (temp.find(')') != std::string::npos) {
                vec.push_back(")");
                temp = temp.substr(temp.find(')') + 1);
            }
            if (temp.ends_with(';') && !(temp == ";")) {
                temp2 = temp;
                temp = temp.substr(0, str.find(';'));
            }
            vec.push_back(temp);
            if (temp2.ends_with(';') && !(temp == ";")) {
                vec.push_back(";");
                break;
            }

            if (temp == ";"){
                break;
            }

            str = str.substr(idx + 1);
            if (idx >= str.length()) {
                if (str.ends_with(';')) {
                    temp2 = str;
                    str = str.substr(0, str.find(';'));
                }
                vec.push_back(str);
                if (temp2.ends_with(';')) {
                    vec.push_back(";");
                }
                break;
            }
        }
    }
    return vec;
}