#include <string>
#include "StubGenerator.h"
#include "CompilationOutputParser.h"
#include <vector>

#pragma once

class HeaderFileAnalyzer
{
private:
    static std::vector<std::string> split(const std::string& s, const std::string& delimiter);
    static bool ctorFound(const std::string& line, const std::string& type);
public:
    static bool foundFunctionDeclaration(const std::string& headerFile, FunctionInfo& function);
    static bool defaultCtorExists(const std::vector<std::string>& headerFiles, const std::string& type);
};


