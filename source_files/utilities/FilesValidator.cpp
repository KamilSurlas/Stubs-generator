#include "FilesValidator.h"
#include "fstream"
#include <iostream>
#include <filesystem>
#include <regex>

using namespace std;

namespace Utilities {
namespace FilesValidator {

string concatenate(const string &first, const string &second) {
#ifdef _WIN32
    if (first.back() != '\\') {
        return first + '\\' + second;
    }
#else
    if (first.back() != '/') {
        return first + '/' + second;
    }
#endif
    return first + second;
}

bool validateIfFileExists(const string &file) {
    return (filesystem::exists(file) && filesystem::is_regular_file(file));
}

bool safetyCheck(const string &path) {
    const regex safeRegex(R"(^[a-zA-Z0-9_\-./\\:]+$)");
    return !path.empty() && regex_match(path, safeRegex);
}

} 
} 
