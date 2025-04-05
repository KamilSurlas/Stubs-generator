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

    /// @brief Parses the makefile into string and checks if m_fileToTest exists in the Makefile.
    /// @return Parsed makefile as a string or NULL string if parsing fails.
    std::string parse() const;

    /// @brief Parses the linker command from the cmake's link.txt file.
    /// @param pathToLinkTxt The path to the link.txt file.
    /// @return The linker command as a string or NULL string if parsing fails.
    std::string retrieveFromLinkTxt(const std::string& pathToLinkTxt) const;
public:
    /// @brief Retrieves the linker command from the makefile.
    /// @return The linker command as a string or NULL string if retrieving fails.
    /// @throws std::runtime_error if parsed makefiles is empty.
    std::string retrieveLinkerCommand() const;
    
    /// @brief Constructor for the LinkerCommandParser class.
    /// @param fileToTest The file to test.
    /// @param pathToMakefile path to Makefile.
    LinkerCommandParser(const std::string& fileToTest, const std::string& pathToMakefile);
};

