#include <sys/stat.h>
#include <regex>
#include <string>

namespace Utilities {
class FilesValidator
{
private:
    const char* m_compile_commands_json = "compile_commands.json";
    std::string m_fileToTest;
    std::string m_compileCommandsJsonFile;
    std::string concatenate(const std::string& pathToCompileCommandsJson);
    bool validateFileToTestExtention();
    bool validateIfJsonFileExist();
    bool validateFileToTest();
public:
    bool validate();
    std::string getCompileCommandsJsonFile();
    std::string getFileToTest();
    FilesValidator(const std::string& fileToTest, const std::string& pathToCompileCommandsJson);
};
}
