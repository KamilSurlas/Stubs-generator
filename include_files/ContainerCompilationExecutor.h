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
    /// @brief Function responsible for invoking compilation inside container.
    /// @return The compilation output.
    std::string compile() const override;
    /// @brief Constructor for the ContainerCompilationExecutor class.
    /// @param pathToMakefile The path to the Makefile.
    /// @param container The name of the container.
    /// @param image The name of the container image.
    ContainerCompilationExecutor(const std::string& pathToMakefile, const std::string& container, const std::string& image);
};


