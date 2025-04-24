#include "CompilationExecutorBase.h"
#include "ContainerHandler.h"
#include <memory>

#pragma once

class ContainerCompilationExecutor : public CompilationExecutorBase
{
private:
    /// @brief Handler for the container
    std::unique_ptr<ContainerHandler> m_containerHandler;
public:
    std::string compile() const override;
    /// @brief Constructor for the ContainerCompilationExecutor class.
    /// @param fileToTest The file to test.
    /// @param pathToMakefile The path to the Makefile.
    /// @param container The name of the container.
    /// @param image The name of the container image.
    ContainerCompilationExecutor(const std::string& fileToTest, const std::string& pathToMakefile, const std::string& container, const std::string& image);
};


