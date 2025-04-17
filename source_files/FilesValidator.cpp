#include "FilesValidator.h"
#include "fstream"
#include <iostream>
#include <filesystem>

using namespace std;

namespace Utilities
{
string FilesValidator::concatenate() const
{
    const char* const makefileName = "Makefile";
#ifdef _WIN32
    if (m_pathToMakefile.back() != '\\')
    {
        return m_pathToMakefile + '\\' + makefileName;
    }
#else
    if (m_pathToMakefile.back() != '/')
    {
        return m_pathToMakefile + '/' + makefileName;
    }
#endif
    return m_pathToMakefile + makefileName;
}
bool FilesValidator::validateFileToTestExtention() const
{
    const regex pattern(R"(.*\.(cpp|c)$)", regex::icase);
    return regex_match(m_fileToTest, pattern);
}
bool FilesValidator::validateIfmakefileExists() const
{
    return (filesystem::exists(m_makefileWithPath) && filesystem::is_regular_file(m_makefileWithPath));
}
bool FilesValidator::safetyCheck() const
{
    const regex safeRegex(R"(^[a-zA-Z0-9_\-./\\:]+$)");    
    return regex_match(m_fileToTest, safeRegex) && regex_match(m_makefileWithPath, safeRegex)
    && (m_container.empty() || regex_match(m_container, safeRegex));
}
bool FilesValidator::validate() const
{
    return safetyCheck() && validateFileToTestExtention() && validateIfmakefileExists();
}
FilesValidator::FilesValidator(const std::string &fileToTest, const std::string &pathToMakefile, const std::string &container)
    :m_fileToTest(fileToTest), m_pathToMakefile(pathToMakefile), m_makefileWithPath(concatenate()), m_container(container)
{
}
}



