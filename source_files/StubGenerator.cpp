#include "StubGenerator.h"
#include "FilesValidator.h"
#include "TypeTraits.h"
#include "HeaderFileAnalyzer.h"
#include <filesystem>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <sstream>

using namespace std;
namespace fs = filesystem;
using namespace Utilities;

StubGenerator::StubGenerator(const vector<UndefinedReferenceError>& errors)
    : m_errors(errors)
{
}

std::string &StubGenerator::trim(std::string &str) const
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char c) {
        return !std::isspace(c);
    }));

    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char c) {
        return !std::isspace(c);
    }).base(), str.end());

    return str;
}

string StubGenerator::prepareDirectories() const
{
    auto currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &currentTime);
#else
    localtime_r(&currentTime, &localTime);
#endif

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &localTime);

    string directoryName = "output/" + string(buffer);
    if (!fs::create_directories(directoryName)) {
        throw fs::filesystem_error("Failed to create output directory", error_code());
    }
    return directoryName;
}

bool StubGenerator::prepareStubFileContent(ostringstream& stubbedStream, const UndefinedReferenceError& errorData, const string& headerFile) const
{
    string headerFileName;
    size_t lastSlashPos = headerFile.find_last_of("/\\");
    if (lastSlashPos != string::npos) {
        headerFileName = headerFile.substr(lastSlashPos + 1);
    } else {
        headerFileName = headerFile;
    }

    ifstream headerFileStream(headerFile);
    if (!headerFileStream.is_open()) {
        return false;
    }

    headerFileStream.clear();
    headerFileStream.seekg(0, ios::beg);

    vector<FunctionInfo> functions = retrieveFunctionsInfo(errorData.m_functionSygnatures);

    string line;

    while (getline(headerFileStream, line)) {
        // Skip pragma once or include guards
        if (line.find("#pragma once") != string::npos ||
            line.find("#ifndef") != string::npos ||
            line.find("#define") != string::npos) {
            continue;
        }

        stubbedStream << "#include \"" << headerFileName << "\"\n";

        // Handle include directives
        if (line.find("#include") != string::npos) {
            stubbedStream << line << "\n";
            continue;
        }

        for (auto& function : functions) {
            size_t start = line.find('(');
            size_t end = line.rfind(' ', start);
            if (start != string::npos && end != string::npos && end < start) {
                string extractedFunctionName = line.substr(end + 1, start - end - 1);

                if (extractedFunctionName == function.m_functionName) {
#ifdef DEBUG
                    cout << "Function found: " << function.m_functionName << endl;
                    cout << "Function namespaces: " << function.m_namespaces << endl;
                    cout << "Function arguments: " << function.m_argumentsList << endl;
#endif
                    size_t returnTypeStartPos = line.find_first_not_of(' ');
                    if (returnTypeStartPos == string::npos) returnTypeStartPos = 0;
                    size_t funcPos = line.find(function.m_functionName, returnTypeStartPos);
                    string returnType = line.substr(returnTypeStartPos, funcPos - returnTypeStartPos);

                    size_t staticPos = line.find("static");
                    if (staticPos != std::string::npos) {
                        line.erase(staticPos, 7); // Remove "static "
                    }

                    size_t openParenPos = line.find('(');
                    size_t closeParenPos = line.find(')', openParenPos);
                    if (openParenPos != string::npos && closeParenPos != string::npos && closeParenPos > openParenPos) {
                        string swapArgs = line.substr(0, openParenPos + 1) + function.m_argumentsList + line.substr(closeParenPos);
                        line = swapArgs;
                    }

                    string returnFromStub;
                    switch (TypeTraits::getType(returnType)) {
                        case TypeTraits::Type::FUNDAMENTAL: {
                            returnFromStub = "return 0;";
                            break;
                        }
                        case TypeTraits::Type::POINTER: {
                            returnFromStub = "return nullptr;";
                            break;
                        }
                        case TypeTraits::Type::REFERENCE: {
                            string refType = returnType.substr(0, returnType.find('&'));
                            returnFromStub = "static " + refType + " stub;\n" + "return stub;";
                            break;
                        }
                        case TypeTraits::Type::COMPOUND: {
                            string actualType = TypeTraits::getCompundTypeName(returnType);
                            if (HeaderFileAnalyzer::defaultCtorExists(errorData.m_dependencies.m_dependencies, actualType)){
                                returnFromStub = "return " + actualType + "();";
                            } else {
                                returnFromStub = "\nreturn " + actualType + "();\n // Default constructor is not accessible, stub may not work correctly\n";
                            }
                            break;
                        }
                        case TypeTraits::Type::VOID: {
                            returnFromStub = "";
                            break;
                        }
                        default: {
                            returnFromStub = "return 0;";
                            break;
                        }
                    }

                    // Find the function name in the line and insert namespaces before it
                    size_t funcNamePos = line.find(function.m_functionName);
                    if (!function.m_namespaces.empty() && funcNamePos != std::string::npos) {
                        string lineWithNamespace = line;
                        lineWithNamespace.insert(funcNamePos, function.m_namespaces + "::");
                        stubbedStream << lineWithNamespace.substr(0, lineWithNamespace.find(';'))
                                      << " { " << returnFromStub << " }\n";
                    } else {
                        stubbedStream << line.substr(0, line.find(';'))
                                      << " { " << returnFromStub << " }\n";
                    }
                }
            }
        }
    }

    return true;
}

vector<FunctionInfo> StubGenerator::retrieveFunctionsInfo(const vector<UndefinedReferenceErrorMap>& sygnatures) const
{
    vector<FunctionInfo> functions;

    for (const auto& sygnature : sygnatures) {
        for (const auto& functionSygnature : sygnature.m_functionSygnatures) {
            FunctionInfo info;

            size_t argumentListStart = functionSygnature.find('(');
            size_t argumentListEnd = functionSygnature.rfind(')');
            info.m_argumentsList = functionSygnature.substr(argumentListStart + 1, argumentListEnd - argumentListStart - 1);

            string functionName = functionSygnature.substr(0, argumentListStart);
            size_t namespacesEnd = functionName.rfind("::");
            if (namespacesEnd != string::npos) {
                info.m_namespaces = functionName.substr(0, namespacesEnd);
                info.m_functionName = functionName.substr(namespacesEnd + 2);
            } else {
                info.m_functionName = functionName;  
                info.m_namespaces = sygnature.m_namespaceOrClassName;
            }
            if (!sygnature.m_namespaceOrClassName.empty() && !info.m_namespaces.empty() && info.m_namespaces != sygnature.m_namespaceOrClassName) {
                info.m_namespaces = sygnature.m_namespaceOrClassName + "::" + info.m_namespaces;
            } 
            functions.push_back(info);
            cout << "Added FunctionInfo: " << info.m_functionName << ", Namespaces: " << info.m_namespaces << ", Arguments: " << info.m_argumentsList << endl;
        }
    }

    return functions;
}

FunctionInfo StubGenerator::retrieveFunctionInfo(const string &sygnature, const string &ns) const
{
    FunctionInfo info;

    size_t argumentListStart = sygnature.find('(');
    size_t argumentListEnd = sygnature.rfind(')');
    info.m_argumentsList = sygnature.substr(argumentListStart + 1, argumentListEnd - argumentListStart - 1);

    string functionName = sygnature.substr(0, argumentListStart);
    size_t namespacesEnd = functionName.rfind("::");
    if (namespacesEnd != string::npos) {
        info.m_namespaces = functionName.substr(0, namespacesEnd);
        info.m_functionName = functionName.substr(namespacesEnd + 2);
    } else {
        info.m_functionName = functionName;
        if (ns.empty()) {
            info.m_namespaces = "";
        } else {
            info.m_namespaces = ns;
        }
    }
    if (!ns.empty() && !info.m_namespaces.empty() && info.m_namespaces != ns) {
        info.m_namespaces = ns + "::" + info.m_namespaces;
    }
    return info;
}

void StubGenerator::writeStubFile(ostringstream &stubbedStream, const string &stubFilePath) const
{
    unordered_set<string> lines;
    ostringstream result;
    ostringstream includes;
    ostringstream functions;
    string line;

    ifstream stubFile(stubFilePath);
    if (stubFile.is_open()) {
        while (getline(stubFile, line)) {
            trim(line);
            lines.insert(line);
        }
        stubFile.close();
    }

    istringstream ss(stubbedStream.str());
    while (getline(ss, line)) {
        trim(line);
        if (lines.find(line) == lines.end()) {
            lines.insert(line);
            if (line.find("#include") != string::npos) {
                includes << line << "\n";
            } else {
                functions << line << "\n";
            }
        }
    }

    if (!includes.str().empty()) {
        result << includes.str() << "\n";
    }

    result << functions.str();

    ofstream finalStubFile(stubFilePath, ios::app);
    if (!finalStubFile) {
        throw runtime_error("Failed to open stub file: " + stubFilePath);
    }

    finalStubFile << result.str();
    finalStubFile.close();
}



void StubGenerator::generateStubs() const
{
    string outputPath;
    try {
        outputPath = prepareDirectories();
    } catch (const fs::filesystem_error& e) {
        throw;
    }

    for (const auto& error : m_errors) {
        for (const auto& dependency : error.m_dependencies.m_dependencies) {
            fs::path path(dependency);
            string stemedFile = path.stem().string();
            string stubFile = stemedFile + "Stub.cpp";

            vector<UndefinedReferenceErrorMap> relevantSygnatures;
            
            for (const auto& sygnature : error.m_functionSygnatures) {
                
                if (sygnature.m_namespaceOrClassName == stemedFile) {
                    relevantSygnatures.push_back(sygnature);
                } else {
                    for (const auto& functionSygnature : sygnature.m_functionSygnatures) {
                        if (functionSygnature.find(stemedFile) != string::npos) {
                            relevantSygnatures.push_back(sygnature);
                            break;
                        } else {
                            FunctionInfo funcInfo = retrieveFunctionInfo(functionSygnature, sygnature.m_namespaceOrClassName);
                            if (HeaderFileAnalyzer::foundFunctionDeclaration(dependency, funcInfo)) {
                                relevantSygnatures.push_back(sygnature);
                                break;
                            }
                        }
                    }
                } 
            }
            
            if (relevantSygnatures.empty()) {
                continue;
            }

            fs::path filePath = FilesValidator::concatenate(fs::path(outputPath).string(), stubFile);
            ofstream file;

            file.open(filePath, fs::exists(filePath) ? ios::app : ios::out);
            if (!file) {
                throw fs::filesystem_error("Failed to open or create stub file: " + filePath.string(), error_code());
            }
           
            ostringstream stubbedStream;
            UndefinedReferenceError errorData(error.m_dependencies, relevantSygnatures);
            if (!prepareStubFileContent(stubbedStream, errorData, dependency)) {
                throw runtime_error("Error during generation");
            }
            writeStubFile(stubbedStream, filePath.string());

            file.close();
            cout << "Stub file has been created: " << filePath.string() << endl;
        }
    }
}
