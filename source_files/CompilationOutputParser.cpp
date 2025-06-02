#include "CompilationOutputParser.h"
#include "SystemCommandExecutor.h"
#include "Exceptions.h"
#include "FilesValidator.h"
#include "json.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <set>
#include <vector>
#include <string>

using namespace std;
using namespace Utilities;
using json = nlohmann::json;
namespace fs = filesystem;

vector<string> CompilationOutputParser::retrieveCompilationCommands(const string &compileCommandsJsonFile, const vector<string> &files) const
{
    if (!FilesValidator::validateIfFileExists(compileCommandsJsonFile) || 
        !FilesValidator::safetyCheck(compileCommandsJsonFile)) {
                throw safety_validation_exception("Provide valid path to compile_commands.json file");
        }
    
    vector<string> compilationCommands;
    ifstream file(compileCommandsJsonFile);
    json data = json::parse(file);
    
    for (const auto& entry : data) {
        if (entry.contains("file") && entry.contains("command")) {
            string filePath = entry.at("file").get<string>();

            auto isFilePathMatching = [&files](const string& filePath) -> bool {
                return any_of(files.begin(), files.end(), [&filePath](const string& file) {
                    return filePath.find(file) != string::npos;
                });
            };

            if (isFilePathMatching(filePath)) {
                string command = entry.at("command").get<string>();
                compilationCommands.push_back(command);
            }
        }
    }

    if(compilationCommands.empty()){
        throw runtime_error("Compilation commands empty. Unable to invoke preprocessor.");
    }

    return compilationCommands;
}

void CompilationOutputParser::prepareCompilationCommand(string &compilationCommand) const
{
    size_t flagPos = compilationCommand.find("-c");
    if (flagPos != string::npos) {
        compilationCommand.erase(flagPos, 2);
    }

    flagPos = compilationCommand.find("-o");
    if (flagPos != string::npos) {
        const size_t outputFileStart = flagPos + 4;
        const size_t outputFileEnd = compilationCommand.find(" ", outputFileStart);
        if (outputFileEnd != string::npos) {
            compilationCommand.erase(flagPos, outputFileEnd - flagPos + 1);
        } else {
            compilationCommand.erase(flagPos);
        }
    }

    compilationCommand += " -MM";
}

void CompilationOutputParser::prepareSygnatures(vector<UndefinedReferenceError> &errors) const
{
    for (auto& error: errors){
        for (auto& sygnatures : error.m_functionSygnatures){
            for (auto& functionSygnature : sygnatures.m_functionSygnatures){
                if (functionSygnature.find("[") != string::npos && functionSygnature.find("]") != string::npos){
                    size_t start = functionSygnature.find("[");
                    size_t end =  functionSygnature.find("]", start);
                    functionSygnature.erase(start, end - start + 1);
                }               
            }
        }
    }
}

void CompilationOutputParser::removeDuplicates(vector<UndefinedReferenceError> &errors) const
{
    for (auto& error : errors) {
        map<string, set<string>> groupedSignatures; // Group by namespace/class name

        // Group function signatures by namespace/class name
        for (const auto& sygnatureMap : error.m_functionSygnatures) {
            for (const auto& currentSignature : sygnatureMap.m_functionSygnatures) {
                groupedSignatures[sygnatureMap.m_namespaceOrClassName].insert(currentSignature);
            }
        }

        // Rebuild the function signatures with grouped entries
        vector<UndefinedReferenceErrorMap> groupedFunctionSignatures;
        for (const auto& [namespaceOrClassName, signatures] : groupedSignatures) {
            groupedFunctionSignatures.emplace_back(namespaceOrClassName, vector<string>(signatures.begin(), signatures.end()));
        }

        error.m_functionSygnatures = move(groupedFunctionSignatures);
    }
}


vector<pair<string, string>> CompilationOutputParser::getCompilationCommands(const vector<string>& files) const
{    
    /* Run make with dry run in order to retrieve compilations commands:
    -nk - dry run
    -B - always rebuild
    -C - specify path to Makefile file
    */

#ifdef _WIN32
    string output = SystemCommandExecutor::execute("mingw32-make -nkB -C " + m_pathToMakefile + " 2>&1");
#else
    string output = SystemCommandExecutor::execute("make -nkB -C " + m_pathToMakefile + " 2>&1");
#endif
    vector<pair<string, string>> compilationCommands;
    istringstream outputStream(output);
    string line;

    if (!m_isFromCmake)
    {
        while (getline(outputStream, line))
        {
            for (const string& file : files)
            {
            if (line.find(file) != string::npos && 
                (line.find(".cpp") != string::npos || line.find(".cc") != string::npos || line.find(".c") != string::npos))
            {
                prepareCompilationCommand(line);
                compilationCommands.push_back({file, line});
                break;
            }
            }
        }
    }
    else {
        // For CMake generation will be from compile_commands.json file.
        // The function assumes that the compile_commands.json is located in the Makefile directory or one of its subdirectories.
        string compileCommandsPath;
        for (const auto& entry : fs::recursive_directory_iterator(m_pathToMakefile)) {
            if (entry.path().filename() == "compile_commands.json") {
            compileCommandsPath = entry.path().string();
            break;
            }
        }
        if (compileCommandsPath.empty()) {
            throw runtime_error("compile_commands.json file not found in the specified path or its subdirectories.");
        }
        for (size_t i = 0; i < files.size(); i++)
        {
            string preparedCommand = retrieveCompilationCommands(compileCommandsPath, files)[i];
            prepareCompilationCommand(preparedCommand);
            compilationCommands.push_back({files[i], preparedCommand});
        }
    }    

    return compilationCommands;
}

void CompilationOutputParser::invokePreprocessor(vector<UndefinedReferenceError> &errors) const
{
    vector<string> files;
    for (const auto& error : errors) {
        files.push_back(error.m_dependencies.m_fileName);
    }

    string currentDirectory = fs::current_path().string();
    fs::current_path(m_pathToMakefile);

    vector<pair<string, string>> compilationCommands = getCompilationCommands(files);
    vector<string> dependencies;
    for (size_t i = 0; i < compilationCommands.size(); i++)
    {
        dependencies.clear();
        string preprocessorOutput = SystemCommandExecutor::execute(compilationCommands.at(i).second);
        istringstream preprocessorOutputStream(preprocessorOutput);
        string dependencyLine;
        string combinedLine;
        while (getline(preprocessorOutputStream, dependencyLine)) {
            // Check if preprocessor returned the output on multiple lines
            if (!dependencyLine.empty() && dependencyLine.back() == '\\') {
            dependencyLine.pop_back(); // Remove the trailing backslash
            combinedLine += dependencyLine;
            } else {
            combinedLine += dependencyLine;
            dependencies.push_back(combinedLine);
            combinedLine.clear();
            }
        }
        // If Makefile is not generated from Cmake add path to dependencies
        errors.at(i).m_dependencies.m_dependencies = parseFileDependencies(dependencies);
         // Remove source file's header from it is dependencies
        for (const auto& dep : errors.at(i).m_dependencies.m_dependencies){
            fs::path depPath(dep);
            fs::path srcPath(errors.at(i).m_dependencies.m_fileName);
            if(!depPath.stem().string().empty() && depPath.stem().string() == srcPath.stem().string()){
                errors.at(i).m_dependencies.m_dependencies.erase(
                    std::remove(errors.at(i).m_dependencies.m_dependencies.begin(),
                                errors.at(i).m_dependencies.m_dependencies.end(), dep),
                    errors.at(i).m_dependencies.m_dependencies.end());
            }
        }
        // At the end of process concatenate file name (it can not be done earlier due to preproccessor operations)
        if(!m_isFromCmake){
            errors.at(i).m_dependencies.m_fileName = FilesValidator::concatenate(m_pathToMakefile, errors.at(i).m_dependencies.m_fileName);
        }
    }

    fs::current_path(currentDirectory);
}


string CompilationOutputParser::retrieveFileName(const string &compilationLine) const
{
    size_t fileNameEnd = min({compilationLine.find(".cpp"), compilationLine.find(".cc"), 
                                   compilationLine.find(".cxx"), compilationLine.find(".c")});
    if (fileNameEnd == string::npos) {
        return string();
    }
    fileNameEnd += 4;
    
    size_t fileNameStart = fileNameEnd;
    while (fileNameStart > 0 && 
           (isalnum(compilationLine[fileNameStart - 1]) || 
            compilationLine[fileNameStart - 1] == '_' || 
            compilationLine[fileNameStart - 1] == '/' || 
            compilationLine[fileNameStart - 1] == '\\' || 
            compilationLine[fileNameStart - 1] == '.' || 
            // Cmake provides full paths so do not cut [Disk letter]:
            (m_isFromCmake && compilationLine[fileNameStart - 1] == ':'))) { 
        --fileNameStart;
    }

    return compilationLine.substr(fileNameStart, fileNameEnd - fileNameStart);
}

vector<string> CompilationOutputParser::parseFileDependencies(vector<string> &dependencies) const
{
    vector<string> parsed;

    for (const auto& dependencyLine : dependencies) {
        size_t colonPos = dependencyLine.find(':');
        if (colonPos != string::npos) {
            string objectPart = dependencyLine.substr(0, colonPos + 1);
            string dependenciesPart = dependencyLine.substr(colonPos + 1);

            istringstream iss(dependenciesPart);
            string parsedDependency;

            while (iss >> parsedDependency) {
                if (!parsedDependency.empty()) {
                    // Skip CPP file
                    if (parsedDependency.length() >= 4 && 
                        parsedDependency.rfind(".cpp") == parsedDependency.length() - 4) {
                        continue;
                    }
                    // skip C file
                    if (parsedDependency.length() >= 2 && 
                        parsedDependency.rfind(".c") == parsedDependency.length() - 2) {
                        continue;
                    }
                    m_isFromCmake ? parsed.push_back(parsedDependency) : parsed.push_back(Utilities::FilesValidator::concatenate(m_pathToMakefile, parsedDependency));
                }
            }
        }
    }
    return parsed;
}

vector<UndefinedReferenceError> CompilationOutputParser::parse() const
{
    istringstream stream(m_compilationOutput);
    string compilationLine;
    vector<UndefinedReferenceError> undefinedReferences;
    const char* const undefinedReferenceTo = "undefined reference to";
    string currentFileName;
    while (getline(stream, compilationLine))
    {
        if (compilationLine.find(undefinedReferenceTo) != string::npos)
        {
            string fileUnderCompilationName = retrieveFileName(compilationLine);
          
            size_t undefRefErrorStart = compilationLine.find(undefinedReferenceTo) + strlen(undefinedReferenceTo) + 1;
            size_t undefRefErrorEnd = compilationLine.find('\'', undefRefErrorStart);
            string fullSignature = compilationLine.substr(undefRefErrorStart, undefRefErrorEnd - undefRefErrorStart);

            size_t namespaceStart = fullSignature.find("::");
            string namespaceName;
            if (namespaceStart != string::npos) {
                namespaceName = fullSignature.substr(1, namespaceStart - 1);
            } else {
                namespaceName = ""; // No namespace/class for free function
            }

            if (fileUnderCompilationName != currentFileName && !fileUnderCompilationName.empty())
            {
                currentFileName = fileUnderCompilationName;
                undefinedReferences.push_back({fileUnderCompilationName});
            }

            fullSignature = fullSignature.substr(namespaceStart + 2, fullSignature.length() - namespaceStart - 1);

            undefinedReferences.back().m_functionSygnatures.push_back(UndefinedReferenceErrorMap(namespaceName, {fullSignature}));
        }
    }
    if (undefinedReferences.empty())
    {
        throw runtime_error("No undefined references found in the compilation output.");
    }
    removeDuplicates(undefinedReferences);
    prepareSygnatures(undefinedReferences);
    invokePreprocessor(undefinedReferences);
    for (const auto& error : undefinedReferences) {
        cout << "File: " << error.m_dependencies.m_fileName << endl;
        for (const auto& dp : error.m_dependencies.m_dependencies){
            cout << "dependency: " << dp << endl;
        }
        for (const auto& signatureMap : error.m_functionSygnatures) {
            cout << "  Namespace/Class: " << signatureMap.m_namespaceOrClassName << endl;
            for (const auto& functionSignature : signatureMap.m_functionSygnatures) {
                cout << "    Function Signature: " << functionSignature << endl;
            }
        }
    }
    return undefinedReferences;
}

CompilationOutputParser::CompilationOutputParser(const string &compilationOutput, const string &pathToMakefile, bool isFromCmake)
    : m_compilationOutput(compilationOutput), m_pathToMakefile(pathToMakefile), m_isFromCmake(isFromCmake)
{
}

UndefinedReferenceError::UndefinedReferenceError(const Dependencies &dep, const vector<UndefinedReferenceErrorMap> &functionSygnatures)
    : m_dependencies(dep), m_functionSygnatures(functionSygnatures)
{
}
