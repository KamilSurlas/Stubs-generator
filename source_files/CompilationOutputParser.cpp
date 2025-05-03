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
    set<pair<string, string>> seenSignatures;
    vector<UndefinedReferenceError> filteredErrors;

    for (const auto& error : errors) {
        UndefinedReferenceError uniqueError(error.m_dependencies.m_fileName);
        for (const auto& sygnatureMap : error.m_functionSygnatures) {
            UndefinedReferenceErrorMap uniqueMap(sygnatureMap.m_namespaceOrClassName);
              for (const auto& currentSignature : sygnatureMap.m_functionSygnatures) {
                pair<string, string> key = {sygnatureMap.m_namespaceOrClassName, currentSignature};
                auto insertResult = seenSignatures.insert(key);
                if (insertResult.second) {
                    uniqueMap.m_functionSygnatures.push_back(currentSignature);
                }
            } 
            if (!uniqueMap.m_functionSygnatures.empty()) {
                uniqueError.m_functionSygnatures.push_back(move(uniqueMap));
            }
        } 
        if (!uniqueError.m_functionSygnatures.empty()) {
            filteredErrors.push_back(move(uniqueError));
        }
    } 
    errors = move(filteredErrors);
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
        while (getline(preprocessorOutputStream, dependencyLine)) {
            dependencies.push_back(dependencyLine);
        }
        errors.at(i).m_dependencies.m_dependencies = dependencies;
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

bool CompilationOutputParser::stringEndsWith(const string &str, const string &suffix) const
{
    if (suffix.size() > str.size()) return false;
    return equal(suffix.rbegin(), suffix.rend(), str.rbegin());
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
            string namespaceName = fullSignature.substr(1, namespaceStart - 1);

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
   
    return undefinedReferences;
}

CompilationOutputParser::CompilationOutputParser(const string &compilationOutput, const string &pathToMakefile, bool isFromCmake)
    : m_compilationOutput(compilationOutput), m_pathToMakefile(pathToMakefile), m_isFromCmake(isFromCmake)
{
}
