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
    if (pathToCompileCommandsJson.back() != '\\')
    {
        return pathToCompileCommandsJson + '\\' + m_compile_commands_json;
    }
    
    return pathToCompileCommandsJson + m_compile_commands_json;
}
bool FilesValidator::validateFileToTestExtention()
{
    regex pattern(R"(.*\.(cpp|c)$)", regex::icase);
    return regex_match(m_fileToTest, pattern);
}
bool FilesValidator::validateIfJsonFileExist()
{
    struct stat buffer;   
    return (stat (m_compileCommandsJsonFile.c_str(), &buffer) == 0); 
}
bool FilesValidator::validateFileToTest()
{
    if (!validateIfJsonFileExist())
    {
        return false;
    }

    ifstream file(m_compileCommandsJsonFile);
    json data = json::parse(file);

    for (const auto& entry : data) {
        if (entry.contains("file") && entry.at("file").get<string>().find(m_fileToTest) != string::npos) {
            return true;
        }
    }
    return false;
}
bool FilesValidator::validate()
{
    return validateFileToTestExtention() && validateFileToTest() && validateIfJsonFileExist();
}
std::string FilesValidator::getCompileCommandsJsonFile()
{
    return m_compileCommandsJsonFile;
}
std::string FilesValidator::getFileToTest()
{
    return m_fileToTest;
}
FilesValidator::FilesValidator(const string &fileToTest, const string &pathToCompileCommandsJson)
    : m_fileToTest(fileToTest), m_compileCommandsJsonFile(concatenate(pathToCompileCommandsJson))
{
}
}


