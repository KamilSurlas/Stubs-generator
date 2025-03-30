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
    std::string concatenate(const std::string& pathToCompileCommandsJson) const;
    bool validateFileToTestExtention() const;
    bool validateIfJsonFileExist() const;
    bool validateFileToTest() const;
public:
    bool validate() const;
    std::string getCompileCommandsJsonFile() const;
    std::string getFileToTest() const;
    FilesValidator(const std::string& fileToTest, const std::string& pathToCompileCommandsJson);
};
}
