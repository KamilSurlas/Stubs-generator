#include "ContainerHandler.h"
#include <fstream>
#include <cstdio>
#include <array>
#include <sstream>
#include <iostream>
#include <memory>

using namespace std;

void ContainerHandler::initializeContainer() const
{
    try {
        string checkIfContainerIsRunning = "docker container ls -f name=^/" + m_containerName + "$ | wc -l";
        int runningCount = stoi(executeCommand(checkIfContainerIsRunning));
        if (runningCount > 1) {
            return;
        }
        string checkIfContainerExists = "docker container ls -a -f name=^/" + m_containerName + "$ | wc -l";
        int existsCount = stoi(executeCommand(checkIfContainerExists));
        if (existsCount > 1) {
            dockerStart();
            return;
        }
        dockerRun();
    } catch (const exception &e) {
        cerr << "Error initializing container: " << e.what();
    }
}

string ContainerHandler::executeInsideContainer(const std::string &command) const
{
    string fullCommand = "docker exec -it " + m_containerName + " bash -c " + command;
    return executeCommand(fullCommand);
}

string ContainerHandler::executeCommand(const string &command) const
{
#ifndef _WIN32
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, int (*)(FILE*)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
#endif
}

void ContainerHandler::dockerRun() const
{
    string fullCommand = "docker run -it --name " + m_containerName;
    system(fullCommand.c_str());
}

void ContainerHandler::dockerStart() const
{
    string startCommand = "docker start -ai " + m_containerName;
    system(startCommand.c_str());
}

ContainerHandler::ContainerHandler(const string &containerName, const string &imageName)
    : m_containerName(containerName), m_imageName(imageName)
{
    initializeContainer();
}
