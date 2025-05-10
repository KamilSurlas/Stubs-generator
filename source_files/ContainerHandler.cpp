#include "ContainerHandler.h"
#include "SystemCommandExecutor.h"
#include "Exceptions.h"
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
    return Utilities::SystemCommandExecutor::execute(command);
#else
    throw Utilities::operation_not_supported_exception("This function is not supported on Windows.");
#endif
}

void ContainerHandler::dockerRun() const
{
    string fullCommand = "docker run -d --name " + m_containerName + " " + m_imageName + " > /dev/null 2>&1";
    system(fullCommand.c_str());
}

void ContainerHandler::dockerStart() const
{
    string startCommand = "docker start " + m_containerName + " > /dev/null 2>&1";
    system(startCommand.c_str());
}

ContainerHandler::ContainerHandler(const string &containerName, const string &imageName)
    : m_containerName(containerName), m_imageName(imageName)
{
    initializeContainer();
}