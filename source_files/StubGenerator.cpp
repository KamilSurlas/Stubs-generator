#include "StubGenerator.h"
#include "FilesValidator.h"
#include <filesystem>
#include <iostream>

using namespace std;
namespace fs = filesystem;
using namespace Utilities;

StubGenerator::StubGenerator(const std::vector<UndefinedReferenceError>& errors)
    : m_errors(errors)
{
}

string StubGenerator::prepareDirectories() const
{
    auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm localTime;
    localtime_s(&localTime, &currentTime);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &localTime);

    string directoryName = "output/" + string(buffer);
    if (!fs::create_directories(directoryName)) {
        throw fs::filesystem_error("Failed to create output directory", error_code());
    }
    return directoryName;
}

bool StubGenerator::appendStub(const ofstream &stubFile, const vector<string>& functionSygnatures, const string& headerFile) const
{
    ifstream headerStream(headerFile);
    if (!headerStream.is_open()) {
        return false;
    }
    
    
}

bool StubGenerator::isFunctionOverloaded(ifstream &headerFile, const string &functionName) const
{
    int overloadCounter = 0;
    string line;
    while(getline(headerFile, line)){
         if (line.find(functionName) != string::npos && line.find('(') != string::npos && line.find(')') != string::npos) {
        overloadCounter++;
    }
    }
    return (overloadCounter > 1);
}

bool StubGenerator::isNeedToCheckTypes(ifstream &headerFile, string functionName, int argumentsCount) const
{
    int overloadCounter = 0;
    string line;
    while (getline(headerFile, line)) {
        if (line.find(functionName) != string::npos) {
            size_t start = line.find('(');
            size_t end = line.find(')');
            if (start != string::npos && end != string::npos && start < end) {
                string arguments = line.substr(start + 1, end - start - 1);
                int count = getFunctionArgumentsCount(headerFile, functionName);
                if (count == argumentsCount) {
                    overloadCounter++;
                }
            }
        }
    }

    // > 1 because the one is the function itself
    return (overloadCounter > 1);
}

vector<FunctionInfo> StubGenerator::retrieveFunctionsInfo(ifstream& headerFile, const vector<string> &functionSygnatures) const
{
    vector<FunctionInfo> functions;
    for (const auto& functionSygnature : functionSygnatures) {
        size_t start = functionSygnature.find(' ') + 1;
        size_t end = functionSygnature.find('(');
        string functionName = functionSygnature.substr(start, end - start);

        FunctionInfo info;
        info.m_functionName = functionName;
        bool isOverload = isFunctionOverloaded(headerFile, functionName);
        if(isOverload){
            info.m_isOverloaded = true;
            int argumentsCount = getFunctionArgumentsCount(headerFile, functionSygnature);
            info.m_argumentsCounter = argumentsCount;
            if(isNeedToCheckTypes(headerFile, functionName, argumentsCount)){
                
            }
        } else {
            info.m_isOverloaded = false;
            // If function is not overloaded there is no need to calculate arguments
        }
    }


}

int StubGenerator::getFunctionArgumentsCount(ifstream &headerFile, const string &functionSygnature) const
{
    size_t start = functionSygnature.find('(') + 1;
    size_t end = functionSygnature.find(')');
    if (start == string::npos || end == string::npos || start >= end) {
        return 0;
    }

    string arguments = functionSygnature.substr(start, end - start);
    if (arguments.empty()) {
        return 0;
    }

    int count = 1;
    for (char c : arguments) {
        if (c == ',') {
            count++;
        }
    }

    return count;
}

void StubGenerator::generateStubs() const
{
    string outputPath;
    try {
        outputPath = prepareDirectories();
    } catch (const fs::filesystem_error& e) {
        // Throw exception further to the caller
        throw;
    }

    for (const auto& error : m_errors) {
        for (const auto& dependency : error.m_dependencies.m_dependencies) {
            bool generateStub = false;
            fs::path path(dependency);
            string stemedFile = path.stem().string();
            string stubFile = stemedFile + "Stub.cpp";

            for (const auto& sygnature : error.m_functionSygnatures) {
                if (sygnature.m_namespaceOrClassName == stemedFile) {
                    generateStub = true;
                    continue;
                }
                for (const auto& functionSygnature : sygnature.m_functionSygnatures) {
                    if (functionSygnature.rfind(stemedFile, 0) == 0) {
                        generateStub = true;
                        break;
                    }
                }
            }

            if (generateStub) {
                fs::path filePath = FilesValidator::concatenate(fs::path(outputPath).string(), stubFile);
                if (!fs::exists(filePath)) {
                    // CHnage logic to if the file exist just do not create next
                    ofstream file(filePath);
                    if (!file) {
                        throw fs::filesystem_error("Failed to create stub file: " + filePath.string(), error_code());
                    }
                    for (const auto& sygnature : error.m_functionSygnatures) {
                        // for (const auto& functionSygnature : sygnature.m_functionSygnatures) {
                        //     // if (!appendStub(file, functionSygnature)) {
                                
                        //     // }
                        // }
                    }
                    file.close();
                }
            }
        }
    }
}
