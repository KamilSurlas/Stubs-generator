#include "CompilationExecutor.h"
#include "SystemCommandExecutor.h"
#include <fstream>
#include <cstdio>
#include <array>
#include <sstream>
#include <iostream>
#include <memory>
#include <regex>

using namespace std;

CompilationExecutor::CompilationExecutor(const std::string &pathToMakefile)
    :CompilationExecutorBase(pathToMakefile)
{
}

std::string CompilationExecutor::compile() const
{
#ifdef _WIN32
    const string command = "mingw32-make -C " + m_pathToMakefile + " 2>&1";
#else
    const string command = "make -C " + m_pathToMakefile + " 2>&1";
#endif
    return SystemCommandExecutor::execute(command);
}