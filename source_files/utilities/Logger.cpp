#include "Logger.h"
#include <chrono>
#include <cstring>
#include <iomanip>
#include <algorithm>

using namespace std;

namespace Utilities {
string Logger::prepareDirectory() const
{
#ifdef __WIN32__
    const char* const logsFileLocationDirecotry = "logs\\";
#else
    const char* const logsFileLocationDirecotry = "logs/";
#endif
    if (!filesystem::exists(logsFileLocationDirecotry))
    {
        if(!filesystem::create_directory(logsFileLocationDirecotry)){
            throw filesystem::filesystem_error("Unable to create log directory", error_code());
        } 
    }
    return logsFileLocationDirecotry;
}

ostringstream Logger::getDateTime() const
{
    const auto now = chrono::system_clock::now();
    const auto timeT = chrono::system_clock::to_time_t(now);
      
    ostringstream dateTime;
    dateTime << put_time(localtime(&timeT), "%Y-%m-%d %H:%M:%S");
    return dateTime;
}

string Logger::prepareFileName() const
{
    ostringstream dateTime = getDateTime();
    // Adjust the file name for actual OS 
    string tmp = dateTime.str();
    replace(tmp.begin(), tmp.end(), ' ', '_');
    replace(tmp.begin(), tmp.end(), '-', '_');
    replace(tmp.begin(), tmp.end(), ':', '_');
    string logFileName = tmp + ".txt";
    return logFileName;
}

void Logger::logToFile(string message)
{
    m_logsFile.open(m_logFileName, ios::app);
    if (!m_logsFile.good() && m_output != LogOutput::CONSOLE)
    { 
        /* If there was an error during log file creation/opening print information on console and print actual log */
        cerr << "Error opening log file. Printing error on console: \n" + message;
        return;
    }
    else
    {
        m_logsFile << message << endl;
        m_logsFile.close();
    }
}

string Logger::replaceChars(const char* const fileName, const char toReplace, const char replacement) const
{
    string modifiedName(fileName); 
    for (char &c : modifiedName) {
        if (c == toReplace) {
            c = replacement;
        }
    }
    return modifiedName;
}

void Utilities::Logger::configure(const LogOutput& logOutput)
{
    try
    {
        m_output = logOutput;
        if (m_output != LogOutput::CONSOLE)
        {
            m_logFileName = prepareDirectory() + prepareFileName();
        }  
    }
    catch(const filesystem::filesystem_error& e)
    {
        m_output = LogOutput::CONSOLE;
        string message = string("Error creating log file. The logs output is console. ") + e.what();
        log(__FILE__, __LINE__, message,LogSeverity::WARNING);
    }

}
void Logger::log(const string &caller, const int line, string message, const LogSeverity& severity)
{   
    ostringstream logDateTime = getDateTime();

    string logPrefix = logDateTime.str() + " [" + caller + ":" + to_string(line) + "] ";

    switch (severity)
    {
    case LogSeverity::INFO:
        logPrefix += "INFO: ";
        break;
    case LogSeverity::WARNING:
        logPrefix += "WARNING: ";
        break;
    case LogSeverity::ERROR:
        logPrefix += "ERROR: ";
        break;
    default:
        logPrefix += "LOG: ";
        break;
    }

    message.insert(0, logPrefix);

    switch (m_output)
    {
    case LogOutput::BOTH: {
        cerr << message << endl;
        logToFile(message);
    }
    break;
    case LogOutput::CONSOLE:
        cerr << message << endl;
        break;  
    case LogOutput::FILE: {
        logToFile(message);
    }
    break;    
    default:
        break;
    }
}


LogOutput Logger::charArrToLogOutput(const char *const output) const
{
    if (strcmp(output, "--logs=CONSOLE") == 0){
        return LogOutput::CONSOLE;
    } 
    if (strcmp(output, "--logs=FILE") == 0){
        return LogOutput::FILE;
    } 
    return LogOutput::BOTH;
}
Logger *Logger::getInstance()
{
    static Logger instance;
    return &instance;
}
}