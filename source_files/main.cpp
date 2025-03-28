#include "CompileCommandsParser.h"
#include "FilesValidator.h"
#include <stdio.h>
#include <iostream>

static const char* usage = "USAGE: StubsGenerator [fileName] [path to compile_commands.json]\n";

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("%s", usage); 
        return 1;
    }
    Utilities::FilesValidator filesValidator(argv[1], argv[2]);
    if (filesValidator.validate())
    {
        CompileCommandsParser compileCommandsParser(filesValidator.getFileToTest(), filesValidator.getCompileCommandsJsonFile());
        std::cout << compileCommandsParser.retrieveCompilationCommand();
    }
    return 0;    
}