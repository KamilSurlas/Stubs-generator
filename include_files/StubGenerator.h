#include <vector>
#include <string>
#include <fstream>
#include "CompilationOutputParser.h"

#include <unordered_set>
#include <functional>

#pragma once

struct FunctionInfo {
    std::string m_functionName;
    std::string m_namespaces;
    std::string m_argumentsList;
    bool m_isOverloaded;
};

class StubGenerator
{
private:
    std::vector<UndefinedReferenceError> m_errors;
    std::string &trim(std::string& str) const;
    std::string prepareDirectories() const;
    bool prepareStubFileContent(std::ostringstream& stubbedStream, const UndefinedReferenceError& errorData, const std::string& headerFile) const;
    std::vector<FunctionInfo> retrieveFunctionsInfo(const std::vector<UndefinedReferenceErrorMap>& sygnatures) const;
    FunctionInfo retrieveFunctionInfo(const std::string& sygnature, const std::string& ns) const;
    void writeStubFile(std::ostringstream& stubbedStream, const std::string& stubFilePath) const;
public:
    void generateStubs() const;
    StubGenerator(const std::vector<UndefinedReferenceError>& errors);
};
