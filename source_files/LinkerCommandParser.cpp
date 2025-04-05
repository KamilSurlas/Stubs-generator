#include "LinkerCommandParser.h"
#include "fstream"
#include <cstdio>
#include <array>
#include <sstream>
#include <iostream>
#include <memory>
#include <regex>

using namespace std;

string LinkerCommandParser::parse() const
{
    array<char, 128> buffer;
    string parsedMakefile;
#ifdef _WIN32
    const string command = "mingw32-make -nkC " + m_pathToMakefile;
    unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
#else
    const string command = "make -nkC " + m_pathToMakefile;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
#endif
    if (!pipe) {
        return string();
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        parsedMakefile += buffer.data();
    }
    return parsedMakefile;
}

std::string LinkerCommandParser::retrieveFromLinkTxt(const std::string &pathToLinkTxt) const
{
    ifstream file(pathToLinkTxt);
    if (!file.is_open()) {
        throw runtime_error("Failed to open link.txt file: " + pathToLinkTxt);
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return content;
}

string LinkerCommandParser::retrieveLinkerCommand() const
{
    string parsedMakefile = parse();
    if (parsedMakefile.empty()) {
        throw runtime_error("Failed to parse makefile.");
    }    
    string linkerCommand;
    istringstream stream(parsedMakefile);
    string line;
    regex objFileRegex(R"(\S+\.o\b)"); 
    while (getline(stream, line)) {
        if (line.find("-c") == string::npos &&
            line.find("-o") != string::npos &&
            regex_search(line, objFileRegex)) 
        {
            return line; 
        }        
        if (line.find("link.txt") != string::npos) {
            size_t pos = line.find("link.txt");
            size_t start = line.rfind(' ', pos);
            start == string::npos ? start = 0 : start ++; 
            string path = line.substr(start, pos - start + 8); 
#ifdef _WIN32
            linkerCommand = retrieveFromLinkTxt(m_pathToMakefile.back() == '\\' ? m_pathToMakefile + path : m_pathToMakefile + '\\' + path);
#else
            linkerCommand = retrieveFromLinkTxt(m_pathToMakefile.back() == '/' ? m_pathToMakefile + path : m_pathToMakefile + '/' + path);
#endif
        }
    }
    return linkerCommand;
}
LinkerCommandParser::LinkerCommandParser(const string& fileToTest, const string& pathToMakefile)
    : m_fileToTest(fileToTest), m_pathToMakefile(pathToMakefile)
{
}
