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
    return regex_match(m_fileToTest, safeRegex) && regex_match(m_makefileWithPath, safeRegex);
}
bool FilesValidator::validate() const
{
    return validateFileToTestExtention() && safetyCheck() && validateIfmakefileExists();
}
string FilesValidator::getPathToMakefile() const
{
    return m_pathToMakefile;
}
string FilesValidator::getFileToTest() const
{
    return m_fileToTest;
}
FilesValidator::FilesValidator(const string &fileToTest, const string &pathToMakefile)
    : m_fileToTest(fileToTest),m_pathToMakefile(pathToMakefile), m_makefileWithPath(concatenate())
{
}
}


