#include "CompileCommandsParser.h"

CompileCommandsParser::CompileCommandsParser(std::string jsonFile, std::string fileToStub)
    :m_jsonFile(jsonFile), m_fileToStub(fileToStub)
{
}
