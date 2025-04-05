#include "LinkerCommandParser.h"
#include "FilesValidator.h"
#include "Logger.h"
#include <stdio.h>
#include <iostream>

using namespace Utilities;

static const char* usage = "USAGE: StubsGenerator [fileName] [path to makefile] [--logs={CONSOLE, FILE, BOTH (default)}]\n";

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("%s", usage); 
        return 1;
    }
    Logger* const logger = Logger::getInstance();
    if (argc > 3) {
        logger->configure(argv[1], logger->charArrToLogOutput(argv[3]));
    } else {
        logger->configure(argv[1]);
    }
    FilesValidator filesValidator(argv[1], argv[2]);
    if (filesValidator.validate())
    {
        try
        {
            LinkerCommandParser linkerCommandsParser(filesValidator.getFileToTest(), filesValidator.getPathToMakefile());
            std::cout << linkerCommandsParser.retrieveLinkerCommand();
        }
        catch(const std::exception& e)
        {
            logger->log(__FILE__, __LINE__,e.what(), LogSeverity::ERROR);
        } 
    }
    else 
    {
        logger->log(__FILE__, __LINE__, "Invalid file or makefile path.\n Please check if Makefile exists in provided path.", LogSeverity::ERROR);
    }
    return 0;    
}