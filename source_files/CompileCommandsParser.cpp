#include "CompileCommandsParser.h"
#include "fstream"

using namespace std;

string CompileCommandsParser::retrieveCompilationCommand() const
{
    ifstream file(m_compileCommandsJsonFile);
    json data = json::parse(file);

    for (const auto& entry : data) {
        if (entry.contains("file") && entry.at("file").get<string>().find(m_fileToTest) != string::npos) {
            return entry.value("command", ""); 
        }
    }
}

CompileCommandsParser::CompileCommandsParser(const string& fileToTest, const string& compileCommandsJsonFile)
    : m_fileToTest(fileToTest), m_compileCommandsJsonFile(compileCommandsJsonFile)
{
}
