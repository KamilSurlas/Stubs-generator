#include "json.hpp"
#include <string>

using json = nlohmann::json;

class CompileCommandsParser
{
private:
    std::string m_fileToTest;
    std::string m_compileCommandsJsonFile;
public:
    std::string retrieveCompilationCommand();
    CompileCommandsParser(const std::string& fileToTest, const std::string& compileCommandsJsonFile);
};

