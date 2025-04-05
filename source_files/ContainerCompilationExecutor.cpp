#include "ContainerCompilationExecutor.h"
#include <iostream>
#include <array>

using namespace std;

std::string ContainerCompilationExecutor::compile() const
{
    string compileCommand = "\"make -C \"" + m_pathToMakefile;
    return m_containerHandler->executeInsideContainer(compileCommand);
}

ContainerCompilationExecutor::ContainerCompilationExecutor(const std::string &fileToTest, const std::string &pathToMakefile, const std::string &container, const std::string &image)
    : CompilationExecutorBase(fileToTest, pathToMakefile), m_containerHandler(std::make_unique<ContainerHandler>(container, image))
{
}
