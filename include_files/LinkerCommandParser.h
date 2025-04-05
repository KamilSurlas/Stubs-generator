#include <string>

#pragma once

/// @brief Linker command parser class for parsing the linker command from a makefile.
/// @details This class retrieves the linker command for a given file to test and makefile path.
class LinkerCommandParser
{
private:
    /// @brief The file to test.
    std::string m_fileToTest;

    /// @brief The path to the Makefile.
    std::string m_pathToMakefile;

    /// @brief The script name to run container.
    std::string m_container;

    /// @brief Reads the compilation output
    std::string compile() const;
public:
    /// @brief Invokes the compilation process with provided Makefile.
    /// @return The parsed compilation output.
    std::string invoke() const;
    
    /// @brief Constructor for the LinkerCommandParser class.
    /// @param fileToTest The file to test.
    /// @param pathToMakefile path to Makefile.
    /// @param container The name of script for running container.
    LinkerCommandParser(const std::string& fileToTest, const std::string& pathToMakefile, const std::string& container = "");
};

