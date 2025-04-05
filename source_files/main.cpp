#include "LinkerCommandParser.h"
#include "FilesValidator.h"
#include <stdio.h>
#include <iostream>

static const char* usage = "USAGE: StubsGenerator [fileName] [path to makefile] --container=[script/command]\n"
                           "fileName: The name of the file to test.\n"
                           "path to makefile: The path to the makefile.\n"
                           "--container=[script/command]: name of script/command for running container.\n";

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("%s", usage); 
        return 1;
    }
    if (argc > 3)
    {
        std::string containerArg = argv[3];
        std::string containerPrefix = "--container=";
        if (containerArg.rfind(containerPrefix, 0) == 0) {
            std::string containerName = containerArg.substr(containerPrefix.length());
            Utilities::FilesValidator filesValidator(argv[1], argv[2], containerName);
            if (filesValidator.validate())
            {
                try
                {
                    LinkerCommandParser linkerCommandsParser(argv[1], argv[2], containerName);
                    std::cout << linkerCommandsParser.invoke();
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }

            }
        } else {
            std::cerr << "Invalid argument for container. Expected format: --container=[name]\n";
            return 1;
        }
    } else
    {
        Utilities::FilesValidator filesValidator(argv[1], argv[2]);
        if (filesValidator.validate())
        {
            try
            {
                LinkerCommandParser linkerCommandsParser(argv[1], argv[2]);
                std::cout << linkerCommandsParser.invoke();
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }
   
    return 0;    
}