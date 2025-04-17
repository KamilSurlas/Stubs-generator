#include "CompilationExecutor.h"
#include <fstream>
#include <cstdio>
#include <array>
#include <sstream>
#include <iostream>
#include <memory>
#include <regex>

using namespace std;

CompilationExecutor::CompilationExecutor(const std::string &fileToTest, const std::string &pathToMakefile)
    :CompilationExecutorBase(fileToTest, pathToMakefile)
{
}

std::string CompilationExecutor::compile() const
{
    array<char, 128> buffer;
    string compilationOutput;
#ifdef _WIN32
    const string command = "mingw32-make -C " + m_pathToMakefile;
    unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
#else
    const string command = "make -C " + m_pathToMakefile;
    unique_ptr<FILE, int (*)(FILE*)> pipe(popen(command.c_str(), "r"), pclose);
#endif
    if (!pipe) {
        return string();
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        compilationOutput += buffer.data();
    }
    return compilationOutput;
}