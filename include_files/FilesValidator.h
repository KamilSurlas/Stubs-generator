#include <sys/stat.h>
#include <regex>
#include <string>

namespace Utilities {
class FilesValidator
{
private:
    const char* compile_commands_json = "compile_commands.json";
    std::string m_fileToStub;
    std::string m_jsonFile;
    std::string concatenate(const std::string& pathToCompileCommandsJson);
    bool validateFileToStubExtention();
    bool validateIfJsonFileExist();
    bool validateFileToStub();
public:
    bool validate();
    FilesValidator(const std::string& fileToStub, const std::string& pathToCompileCommandsJson);
};
}
