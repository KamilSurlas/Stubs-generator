#include <string>

#pragma once

/// @brief Base class for executors.
class CompilationExecutorBase
{
protected:
    /// @brief The path to the Makefile.
    std::string m_pathToMakefile;
public:
    /// @brief Pure virtual function for invoking compilation.
    virtual std::string compile() const = 0;
    /// @brief Constructor for the CompilationExecutorBase class.
    /// @param pathToMakefile The path to the Makefile.
    CompilationExecutorBase(const std::string& pathToMakefile)
       :m_pathToMakefile(pathToMakefile) {}

    /// @brief Virtual destructor for the CompilationExecutorBase class.
    virtual ~CompilationExecutorBase() = default;
};

