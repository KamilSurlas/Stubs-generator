#include <string>


class ContainerHandler
{
private:
    /// @brief The container name, unique per sytstem
    std::string m_containerName;

    /// @brief The name of the image
    std::string m_imageName;

    /// @brief Helper function to execute shell commands and return output.
    /// @param command The command to execute.
    /// @return The output of the command.
    std::string executeCommand(const std::string &command) const;

    /// @brief initialize the container and execute the command
    void initializeContainer() const;

    void dockerRun() const;
    void dockerStart() const;

public:
    std::string executeInsideContainer(const std::string& command) const;
    ContainerHandler(const std::string &containerName, const std::string &imageName);
};
