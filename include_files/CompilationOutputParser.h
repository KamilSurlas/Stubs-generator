#include <string>
#include <vector>
#include <filesystem>

#pragma once

/// @brief Struct to represent file dependencies (include files)
struct Dependencies {
    std::string m_fileName;
    std::vector<std::string> m_dependencies;
};

/// @brief Struct to represent undefined reference error.
struct UndefinedReferenceErrorMap {
    /// @brief namespace or class name where the error occurs.
    std::string m_namespaceOrClassName;
    /// @brief vector of function signatures.
    std::vector<std::string> m_functionSygnatures;
    UndefinedReferenceErrorMap(const std::string &namespaceOrClassName, const std::vector<std::string> &functionSygnatures)
        : m_namespaceOrClassName(namespaceOrClassName), m_functionSygnatures(functionSygnatures) {}
    UndefinedReferenceErrorMap(const std::string &namespaceOrClassName)
        : m_namespaceOrClassName(namespaceOrClassName) {}
};

/// @brief Struct to represent undefined reference error with informations needed for stubs generation. 
struct UndefinedReferenceError {
    Dependencies m_dependencies;
    std::vector<UndefinedReferenceErrorMap> m_functionSygnatures;
    UndefinedReferenceError(const std::string& fileName)
        : m_dependencies({Dependencies{fileName, {}}}) {}
    UndefinedReferenceError(const Dependencies& dep, const std::vector<UndefinedReferenceErrorMap>& functionSygnatures);
};

/// @brief Class resposnible for parsing the compilation output into UndefinedReferenceError vector.
class CompilationOutputParser
{
private:
    /* 
        Helper functions.
    */
    /// @brief Retrieves compilation commands from a compile_commands.json.
    /// @param compileCommandsJsonFile Path to the compile_commands.json file.
    /// @param files List of files to filter the compilation commands.
    /// @return A vector of compilation commands relevant to the specified files.
    std::vector<std::string> retrieveCompilationCommands(const std::string& compileCommandsJsonFile, const std::vector<std::string>& files) const;

    /// @brief Prepares a compilation command by removing -o and -c and adding -MM flag.
    /// @param compilationCommand The compilation command to be prepared.
    void prepareCompilationCommand(std::string& compilationCommand) const;

    /// @brief Prepares function sygnatures by removing abi elements if presents.
    /// @param errors A vector of undefined reference errors to process.
    void prepareSygnatures(std::vector<UndefinedReferenceError> &errors) const;

    /// @brief Removes duplicate undefined reference errors from the provided list.
    /// @param errors A vector of undefined reference errors to process.
    void removeDuplicates(std::vector<UndefinedReferenceError> &errors) const;

    /// @brief Retrieves compilation commands for the specified files from Makefile.
    /// @param files List of files to retrieve compilation commands for.
    /// @return A vector of pairs, where each pair contains a file name and its corresponding compilation command.
    std::vector<std::pair<std::string, std::string>> getCompilationCommands(const std::vector<std::string>& files) const;

    /// @brief Invokes a preprocessor to read dependencies.
    /// @param errors A vector of undefined reference errors to process.
    void invokePreprocessor(std::vector<UndefinedReferenceError> &errors) const;

    /// @brief Retrieves the file name from a compilation line.
    /// @param compilationLine The compilation line to extract the file name from.
    /// @return The extracted file name.
    std::string retrieveFileName(const std::string& compilationLine) const;

    /// @brief Parses dependencies from preprocessor output to form accesiable for StubGenerator.
    /// @param dependencies dependecies for file.
    /// @return vector of parsed dependencies.
    std::vector<std::string> parseFileDependencies(std::vector<std::string>& dependencies) const; 

    /// @brief The output of the compilation process.
    std::string m_compilationOutput;

    /// @brief The path to the Makefile (Need for invoking dry run)
    std::string m_pathToMakefile;

    /// @brief Indicates whether the Makefile has been generated from CMake.
    bool m_isFromCmake;

public:
    /// @brief Parses the compilation output into undefined reference errors vector.
    /// @return A vector of undefined reference errors found in the compilation output.
    std::vector<UndefinedReferenceError> parse() const;

    /// @brief Constructs a CompilationOutputParser object.
    /// @param compilationOutput The output of the compilation process.
    /// @param pathToMakefile The path to the Makefile used in the compilation process.
    /// @param isFromCmake Indicates whether the compilation process is generated from CMake.
    CompilationOutputParser(const std::string& compilationOutput, const std::string& pathToMakefile, bool isFromCmake);
};

