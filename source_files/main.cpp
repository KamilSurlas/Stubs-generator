#include "LinkerCommandParser.h"
#include "FilesValidator.h"
#include <stdio.h>
#include <iostream>

static const char* usage = "USAGE: StubsGenerator [fileName] [path to makefile]\n";

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("%s", usage); 
        return 1;
    }
    Utilities::FilesValidator filesValidator(argv[1], argv[2]);
    if (filesValidator.validate())
    {
        try
        {
            LinkerCommandParser linkerCommandsParser(filesValidator.getFileToTest(), filesValidator.getPathToMakefile());
            std::cout << linkerCommandsParser.retrieveLinkerCommand();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
       
    }
    return 0;    
}