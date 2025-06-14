#include "ContainerCompilationExecutor.h"
#include "CompilationExecutor.h"
#include "FilesValidator.h"
#include "CompilationOutputParser.h"
#include "StubGenerator.h"
#include <stdio.h>
#include <iostream>
#include <map>

enum class Configuration
{
    USE_CONTAINER,
    FROM_CMAKE
}; 

std::map<Configuration, bool> configMap = {
    {Configuration::USE_CONTAINER, false},
    {Configuration::FROM_CMAKE, false}
};

struct ContainerParams
{
    std::string m_containerName;
    std::string m_containerImage;
};

ContainerParams configContainerParams(){
    ContainerParams containerParams;
    printf("Enter the container name: ");
    std::cin >> containerParams.m_containerName;
    printf("Enter the container image: ");
    std::cin >> containerParams.m_containerImage;
    return containerParams;
}
static const char* const makefileName = "Makefile";
static const char* const usage = "USAGE: StubsGenerator [path to makefile] [path to project directory] --use_container\n"
                           "path to makefile: The path to the makefile.\n"
                           "path to project directory: The path to the project (needed for searching includes).\n"
                           "--use_container: Add this flag if container should be used.\n"
                           "--from_cmake: Use this flag if the Makefile have been generated from CMakeLists.txt\n";

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("%s", usage);
        return 1;
    }

    const std::string makefile = Utilities::FilesValidator::concatenate(argv[1], makefileName);
    if (!Utilities::FilesValidator::validateIfFileExists(makefile) || 
        !Utilities::FilesValidator::safetyCheck(makefile)) {
        printf("Invalid path to makefile: %s\n", makefile.c_str());
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--use_container") {
            configMap[Configuration::USE_CONTAINER] = true;
        }
        if (std::string(argv[i]) == "--from_cmake"){
            configMap[Configuration::FROM_CMAKE] = true;
        }
    }

    if (configMap[Configuration::USE_CONTAINER]) {
        ContainerParams containerParams = configContainerParams();
        try {
            ContainerCompilationExecutor executor(argv[1], containerParams.m_containerName, containerParams.m_containerImage);
            bool isFromCmake = configMap[Configuration::FROM_CMAKE] == true ? true : false;
            CompilationOutputParser parser(executor.compile(), argv[1], isFromCmake);
            const std::vector<UndefinedReferenceError> errors = parser.parse();
            StubGenerator stubGenerator(errors);
            stubGenerator.generateStubs();
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    } else {
        try {
            CompilationExecutor executor(argv[1]);
            bool isFromCmake = configMap[Configuration::FROM_CMAKE] == true ? true : false;
            CompilationOutputParser parser(executor.compile(), argv[1], isFromCmake);
            const std::vector<UndefinedReferenceError> errors = parser.parse();
            StubGenerator stubGenerator(errors);
            stubGenerator.generateStubs();
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }

    return 0;
}
