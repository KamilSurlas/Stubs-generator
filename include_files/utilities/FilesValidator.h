#include <regex>
#include <string>

#pragma once

namespace Utilities {
namespace FilesValidator
{
    /// @brief Concatenates two filesystem paths.
    /// @param first The first path.
    /// @param second The second path.
    /// @return The concatenated path.
    std::string concatenate(const std::string& first, const std::string& second);
    
    /// @brief Validates if the given file exist and is regular file.
    /// @param file file to validate.
    /// @return boolean value if validation has been successful. 
    bool validateIfFileExists(const std::string& file);

    /// @brief Performs safety checks on the file path.
    /// @param path path to be check.
    /// @return boolean value if validation has been successful. 
    bool safetyCheck(const std::string& path);   
}
}
