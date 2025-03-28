#include "CompileCommandsParser.h"
#include "fstream"

using namespace std;

std::string CompileCommandsParser::retrieveCompilationCommand()
{
    ifstream file(m_compileCommandsJsonFile);
    json data = json::parse(file);

    for (const auto& entry : data) {
        if (entry.contains("file") && entry.at("file").get<string>().find(m_fileToTest) != string::npos) {
            return entry.value("command", ""); 
        }
    }
}

CompileCommandsParser::CompileCommandsParser(const std::string& fileToTest, const std::string& compileCommandsJsonFile)
    : m_fileToTest(fileToTest), m_compileCommandsJsonFile(compileCommandsJsonFile)
{
}
