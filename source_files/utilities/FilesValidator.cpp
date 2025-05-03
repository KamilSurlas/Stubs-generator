#include "FilesValidator.h"
#include "fstream"
#include <iostream>
#include <filesystem>

using namespace std;

namespace Utilities
{
string FilesValidator::concatenate(const string &first, const string &second)
{
#ifdef _WIN32        
    if (first.back() != '\\')
    {
        return first + '\\' + second;
    }
#else
    if (first.back() != '/')
    {
        return first + '/' + second;
    }
#endif
    return first + second;
}
bool FilesValidator::validateIfFileExists(const string &file)
{
    return (filesystem::exists(file) && filesystem::is_regular_file(file));
}
bool FilesValidator::safetyCheck(const string &path)
{
    const regex safeRegex(R"(^[a-zA-Z0-9_\-./\\:]+$)");   
    return !path.empty() && regex_match(path, safeRegex);
}
}
