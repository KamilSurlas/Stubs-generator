#include "ContainerCompilationExecutor.h"
#include "SystemCommandExecutor.h"
#include <iostream>
#include <array>

using namespace std;

std::string ContainerCompilationExecutor::compile() const
{
    string compileCommand = "\"make -B -C \"" + m_pathToMakefile + " 2>&1";
    return m_containerHandler->executeInsideContainer(compileCommand);
}

ContainerCompilationExecutor::ContainerCompilationExecutor(const std::string &pathToMakefile, const std::string &container, const std::string &image)
    : CompilationExecutorBase(pathToMakefile), m_containerHandler(std::make_unique<ContainerHandler>(container, image))
{
}
