#include <vector>
#include <string>
#include <fstream>
#include "CompilationOutputParser.h"

struct FunctionInfo {
    std::string m_functionName;
    std::string m_returnType;
    int m_argumentsCounter = 0;
    bool m_isOverloaded;
};

class StubGenerator
{
private:
    std::string prepareDirectories() const;
    bool appendStub(const std::ofstream& stubFile, const std::vector<std::string>& functionSygnatures, const std::string& headerFile) const;
    bool isFunctionOverloaded(std::ifstream& headerFile, const std::string& functionName) const;
    bool isNeedToCheckTypes(std::ifstream& headerFile, std::string functionName, int argumentsCount) const;
    std::vector<FunctionInfo> retrieveFunctionsInfo(std::ifstream& headerFile, const std::vector<std::string>& functionSygnatures) const;
    int getFunctionArgumentsCount(std::ifstream& headerFile, const std::string& functionSygnature) const;
    
    std::vector<UndefinedReferenceError> m_errors;
public:
    void generateStubs() const;
    StubGenerator(const std::vector<UndefinedReferenceError>& errors);
};
