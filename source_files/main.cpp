#include "ContainerCompilationExecutor.h"
#include "CompilationExecutor.h"
#include "FilesValidator.h"
#include "CompilationOutputParser.h"
#include <stdio.h>
#include <iostream>
#include <map>

enum class Configuration
{
    USE_CONTAINER
}; 

std::map<Configuration, bool> configMap = {
    {Configuration::USE_CONTAINER, false}
};

struct ContainerParams
{
    std::string m_containerName;
    std::string m_containerImage;
};

ContainerParams configContainerParams(){
    printf("Enter the container name: ");
    std::string containerName;
    std::cin >> containerName;
    printf("Enter the container image: ");
    std::string containerImage;
    std::cin >> containerImage;
    return {containerName, containerImage};
}

static const char* usage = "USAGE: StubsGenerator [fileName] [path to makefile] --use_container\n"
                           "fileName: The name of the file to test.\n"
                           "path to makefile: The path to the makefile.\n"
                           "--use_container: Add this flag if container should be used.\n";

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("%s", usage); 
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--use_container") {
            configMap[Configuration::USE_CONTAINER] = true;
        }
    }
    if (configMap[Configuration::USE_CONTAINER])
    {
        ContainerParams containerParams = configContainerParams();
        try
        {
            ContainerCompilationExecutor executor(argv[1], argv[2], containerParams.m_containerName, containerParams.m_containerImage);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    } else
    {
        Utilities::FilesValidator filesValidator(argv[1], argv[2]);
        if (filesValidator.validate())
        {
            try
            {
                CompilationExecutor executor(argv[1], argv[2]);
                CompilationOutputParser parser(executor.compile());
                parser.parse();
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }
   
    return 0;    
}