#include <string>

#pragma once

class CompilationExecutorBase
{
protected:
    /// @brief The file to test.
    std::string m_fileToTest;

    /// @brief The path to the Makefile.
    std::string m_pathToMakefile;
public:

    /// @brief Reads the compilation output
    virtual std::string compile() const = 0;
    /// @brief Constructor for the CompilationExecutorBase class.
    /// @param fileToTest The file to test.
    /// @param pathToMakefile The path to the Makefile.
    CompilationExecutorBase(const std::string& fileToTest, const std::string& pathToMakefile)
        : m_fileToTest(fileToTest), m_pathToMakefile(pathToMakefile) {}

    /// @brief Virtual destructor for the CompilationExecutorBase class.
    virtual ~CompilationExecutorBase() = default;
};

