#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#pragma once

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
    std::ofstream m_logsFile; 
    std::string m_logFileName;
    LogOutput m_output;
    std::string prepareDirectory() const;
    std::ostringstream getDateTime() const;
    std::string prepareFileName() const;
    void logToFile(std::string message);
    std::string replaceChars(const char* const fileName, const char toReplace, const char replacement) const;
    Logger(){};
public:
    void configure(const LogOutput& logOutput = LogOutput::BOTH);
    void log(const std::string& caller, const int line, std::string message, const LogSeverity& severity);
    LogOutput charArrToLogOutput(const char* const output) const;
    static Logger* getInstance();
};
} 