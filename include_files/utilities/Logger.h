#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

namespace Utilities
{
enum class LogSeverity {
    INFO, 
    WARNING, 
    ERROR  
};
enum class LogOutput {
    FILE,
    CONSOLE,
    BOTH
};
class Logger
{
private:
    const char* m_logsFileLocationDirecotry = "../../Logs/";
    std::ofstream m_logsFile; 
    std::string m_logFileName;
    LogOutput m_output;
    void prepareDirectory() const;
    std::ostringstream getDateTime() const;
    void logToFile(std::string message);
    std::string prepareFileName(const char* const fileToTest) const;
    std::string replaceChars(const char* const fileName, const char toReplace, const char replacement) const;
    Logger(){};
public:
    void configure(const std::string& logFileName, const LogOutput& logOutput = LogOutput::BOTH);
    void log(const std::string& caller, const int line, std::string message, const LogSeverity& severity);
    LogOutput charArrToLogOutput(const char* const output) const;
    static Logger* getInstance();
};
} 


