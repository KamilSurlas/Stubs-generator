#include "FilesValidator.h"
#include "fstream"
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;
using namespace std;

namespace Utilities
{
string FilesValidator::concatenate(const string &pathToCompileCommandsJson)
{
    return pathToCompileCommandsJson + compile_commands_json;
}
bool FilesValidator::validateFileToStubExtention()
{
    regex pattern(R"(.*\.(cpp|c)$)", regex::icase);
    return regex_match(m_fileToStub, pattern);
}
bool FilesValidator::validateIfJsonFileExist()
{
    struct stat buffer;   
    return (stat (m_jsonFile.c_str(), &buffer) == 0); 
}
bool FilesValidator::validateFileToStub()
{
    if (!validateIfJsonFileExist())
    {
        return false;
    }

    ifstream file(m_jsonFile);
    json data = json::parse(file);

    for (const auto& entry : data) {
        if (entry.contains("file") && entry.at("file").get<string>().find(m_fileToStub) != string::npos) {
            return true;
        }
    }
    return false;
}
bool FilesValidator::validate()
{
    return validateFileToStub() && validateFileToStubExtention() && validateIfJsonFileExist();
}
FilesValidator::FilesValidator(const string &fileToStub, const string &pathToCompileCommandsJson)
    : m_fileToStub(fileToStub), m_jsonFile(concatenate(pathToCompileCommandsJson))
{
}
}


