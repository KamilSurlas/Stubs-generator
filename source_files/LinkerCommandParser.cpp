#include "LinkerCommandParser.h"
#include "fstream"
#include <cstdio>
#include <array>
#include <sstream>
#include <iostream>
#include <memory>
#include <regex>

using namespace std;

std::string LinkerCommandParser::compile() const
{
    array<char, 128> buffer;
    string compilationOutput;
#ifdef _WIN32
    const string command = "mingw32-make -C " + m_pathToMakefile;
    unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
#else
    const string command = "make -C " + m_pathToMakefile;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
#endif
    if (!pipe) {
        return string();
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        compilationOutput += buffer.data();
    }
    return compilationOutput;
}

string LinkerCommandParser::invoke() const
{
    string compilationOutput;
#ifdef _WIN32
    if (!m_container.empty()) {
        string command = "start /B " + m_container;
        system(command.c_str());
        compilationOutput = compile();
        command = "exit /B "; 
        system(command.c_str());
    } else {
        compilationOutput = compile();
    }
#else 
    if (!m_container.empty()) {
        string command = m_container + " &";
        system(command.c_str());
        compilationOutput = compile();
        command = "exit &";
        system(command.c_str());
    } else {
        compilationOutput = compile();
    }
#endif
    if (compilationOutput.empty()) {
        throw runtime_error("Compilation failed or no output generated.");
    }
    cout << "Compilation output:\n" << compilationOutput << endl;
    return compilationOutput;
}
LinkerCommandParser::LinkerCommandParser(const std::string &fileToTest, const std::string &pathToMakefile, const std::string &container)
    :m_fileToTest(fileToTest), m_pathToMakefile(pathToMakefile), m_container(container)
{
}
