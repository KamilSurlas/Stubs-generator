#include <string>
#include <vector>
#include <filesystem>

#pragma once

struct Dependencies {
    std::string m_fileName;
    std::vector<std::string> m_dependencies;
};

struct UndefinedReferenceErrorMap {
    std::string m_namespaceOrClassName;
    std::vector<std::string> m_functionSygnatures;
    UndefinedReferenceErrorMap(const std::string &namespaceOrClassName, const std::vector<std::string> &functionSygnatures)
        : m_namespaceOrClassName(namespaceOrClassName), m_functionSygnatures(functionSygnatures) {}
    UndefinedReferenceErrorMap(const std::string &namespaceOrClassName)
        : m_namespaceOrClassName(namespaceOrClassName) {}
};

struct UndefinedReferenceError {
    Dependencies m_dependencies;
    std::vector<UndefinedReferenceErrorMap> m_functionSygnatures;
    UndefinedReferenceError(const std::string& fileName)
        : m_dependencies({Dependencies{fileName, {}}}) {}
};

class CompilationOutputParser
{
private:
    std::vector<std::string> retrieveCompilationCommands(const std::string& compileCommandsJsonFile, const std::vector<std::string>& files) const;
    void prepareCompilationCommand(std::string& compilationCommand) const;
    void prepareSygnatures(std::vector<UndefinedReferenceError> &errors) const;
    void removeDuplicates(std::vector<UndefinedReferenceError> &errors) const;
    std::vector<std::pair<std::string, std::string>> getCompilationCommands(const std::vector<std::string>& files) const;
    void invokePreprocessor(std::vector<UndefinedReferenceError> &errors) const;
    std::string retrieveFileNameWin(const std::string& compilationLine) const;
    bool stringEndsWith(const std::string& str, const std::string& suffix) const; 
    std::string m_compilationOutput;
    std::string m_pathToMakefile;
    bool m_isFromCmake;
public:
    std::vector<UndefinedReferenceError> parse() const;
    CompilationOutputParser(const std::string& compilationOutput, const std::string& pathToMakefile, bool isFromCmake);
};

