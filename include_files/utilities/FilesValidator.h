#include <regex>
#include <string>

#pragma once

namespace Utilities {
/// @brief Files validator class for validating the file to test and the path to the makefile.
/// @details This class checks if the file to test has a valid extension, if the files exist, and performs safety checks on the file paths.
class FilesValidator
{   
public:
    /// @brief Concatenates two filesystem paths.
    /// @param first The first path.
    /// @param second The second path.
    /// @return The concatenated path.
    static std::string concatenate(const std::string& first, const std::string& second);
    
    /// @brief Validates if the given file exist and is regular file.
    static bool validateIfFileExists(const std::string& file);

    /// @brief Performs safety checks on the file path.
    static bool safetyCheck(const std::string& path);   
};
}
