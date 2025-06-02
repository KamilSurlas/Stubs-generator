#include "HeaderFileAnalyzer.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

vector<string> HeaderFileAnalyzer::split(const string &s, const string &delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;
    if (s.empty() || delimiter.empty()) {
        return res; 
    }

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

bool HeaderFileAnalyzer::ctorFound(const string &line, const string &type)
{
    // Check if the line contains a default constructor for the given type,
    // allowing for specifiers like 'inline', 'explicit', etc.
    string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));

    // Tokenize the line to skip specifiers
    istringstream iss(trimmed);
    string token;
    vector<string> tokens;
    while (iss >> token) {
        tokens.push_back(token);
    }
    // Find the token that matches the type name followed by '('
    for (size_t i = 0; i < tokens.size(); ++i) {
        size_t pos = tokens[i].find(type + "(");
        if (pos == 0) {
            size_t openParen = tokens[i].find('(');
            size_t closeParen = tokens[i].find(')', openParen);
            string params;
            if (openParen != string::npos && closeParen != string::npos && closeParen > openParen) {
                params = tokens[i].substr(openParen + 1, closeParen - openParen - 1);
            } else {
                // If '(' is not in the same token, check the rest of the line
                size_t lineOpen = trimmed.find(type + "(");
                if (lineOpen != string::npos) {
                    size_t open = trimmed.find('(', lineOpen);
                    size_t close = trimmed.find(')', open);
                    if (open != string::npos && close != string::npos && close > open) {
                        params = trimmed.substr(open + 1, close - open - 1);
                    }
                }
            }
            // Remove whitespace from params
            params.erase(remove_if(params.begin(), params.end(), ::isspace), params.end());
            if (params.empty() || params == "void") {
                return true;
            }
        }
    }
    return false;
}

bool HeaderFileAnalyzer::foundFunctionDeclaration(const string &headerFile, FunctionInfo &function)
{
    ifstream file(headerFile);
    if (file.is_open()) {
        vector<string> ns = split(function.m_namespaces, "::");
        vector<string> positions;
#ifdef DEBUG
        cout << "Size of namespaces: " << ns.size() << endl;
        cout << "Size of positions: " << positions.size() << endl;
        cout << "Header file: " << headerFile << endl;
        cout << "Function: " << function.m_functionName << endl;
        cout << "Namespaces: ";
        for (const auto &n : ns) cout << n << " ";
        cout << endl;
#endif
        string line;
        while(getline(file, line)){
            if (line.find(function.m_functionName) != string::npos && positions == ns) {
#ifdef DEBUG
                cout << "Found function declaration: " << line << endl;
#endif
                return true;
            }
            string stemedFile = filesystem::path(headerFile).stem().string();
            if (line.find("namespace") != string::npos 
                || line.find("class") != string::npos 
                || line.find("struct") != string::npos){
                istringstream lineStream(line);
                string keyword, name;
                lineStream >> keyword >> name;
                if (!name.empty()) {
                    size_t end = name.find_first_of("{");
                    if (end != string::npos) {
                        name = name.substr(0, end);
                    }
                    positions.push_back(name);
                }
            }
            if (line.find("};") != string::npos) {
                if (!positions.empty()) {
                    positions.pop_back();
                }
            }
        }
        file.close();
    }
    return false;
}

bool HeaderFileAnalyzer::defaultCtorExists(const vector<string> &headerFiles, const string &type)
{
    for (const auto &headerFile : headerFiles) {
        bool inClass = false;
        bool inPrivateSection = false;
        ifstream file(headerFile);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if ((line.find("enum") != string::npos && line.find(type) != string::npos)
                || (line.find("typedef") != string::npos && line.find(type) != string::npos)
                || (line.find("using") != string::npos && line.find(type) != string::npos)) {
                    return true;
                }
                // Check if the line contains only 'class' or 'struct' followed by type and optionally '{'
                istringstream iss(line);
                string keyword, name, brace;
                iss >> keyword >> name >> brace;
                if ((keyword == "class" || keyword == "struct") && name == type) {
                    if (brace.empty() || brace == "{") {
                        inClass = true;
                    }
                }
                if (inClass && line.find("private:") != string::npos) {
                    inPrivateSection = true;
                } else if (inClass && line.find(':') != string::npos && line.find("private") == string::npos) {
                    inPrivateSection = false;
                }
                if (inClass && !inPrivateSection && ctorFound(line, type) && line.find("= delete") == string::npos) {
                    file.close();
                    return true;
                } 
                else if (inClass && ctorFound(line, type)) {
                    file.close();
                    return false;
                }
                if (inClass && line.find("};") != string::npos) {
                    inClass = false;
                    inPrivateSection = false;
                }
            }
            file.close();
        }
    }

    return false;
}
