#include "json.hpp"
#include <string>

using json = nlohmann::json;

class CompileCommandsParser
{
private:
    std::string m_jsonFile;
    std::string m_compilationCommand;
    std::string m_fileToStub;
public:
    CompileCommandsParser(std::string jsonFile, std::string fileToStub);
    ~CompileCommandsParser();
};

