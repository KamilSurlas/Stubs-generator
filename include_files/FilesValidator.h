#include <regex>
#include <string>

#pragma once

namespace Utilities {
/// @brief Files validator class for validating the file to test and the path to the makefile.
/// @details This class checks if the file to test has a valid extension, if the files exist, and performs safety checks on the file paths.
class FilesValidator
{
private:
    /// @brief The file to test.
    std::string m_fileToTest;

    /// @brief The path to the makefile.
    std::string m_pathToMakefile;

    /// @brief Concatenated makefile with the path.
    std::string m_makefileWithPath;

    /// @brief The name of script for running container.
    std::string m_container;
    
    /// @brief Concatenates the path to the makefile with the makefile name.
    /// @details The makefile name is assumed to be "Makefile".
    /// @return The concatenated makefile path.
    std::string concatenate() const;

    /// @brief Validates the file to test extension (.c or .cpp).
    bool validateFileToTestExtention() const;

    /// @brief Validates if the makefile exist and is regular file.
    bool validateIfmakefileExists() const;

    /// @brief Performs safety checks on the file paths.
    bool safetyCheck() const;
public:
    /// @brief Validates the file to test and the path to the makefile.
    bool validate() const;

    /// @brief Constructor for the FilesValidator class.
    /// @param fileToTest The file to test.
    /// @param pathToMakefile The path to the makefile.
    /// @param container The name of script for running container.
    FilesValidator(const std::string& fileToTest, const std::string& pathToMakefile, const std::string& container = "");
};
}
