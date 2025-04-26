#include <string>
#include "CompilationExecutorBase.h"

#pragma once

/// @brief Compilation executor class for invoking the compilation from a makefile.
class CompilationExecutor : public CompilationExecutorBase
{
public:
    /// @brief Reads the compilation output
    std::string compile() const override;
    /// @brief Constructor for the CompilationExecutor class.
    /// @param fileToTest The file to test.
    /// @param pathToMakefile path to Makefile.
    CompilationExecutor(const std::string& fileToTest, const std::string& pathToMakefile);
};

