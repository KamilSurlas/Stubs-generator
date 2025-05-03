#include <string>

class SystemCommandExecutor
{
public:
    /// @brief Executes a system command and returns the output.
    /// @param command The command to execute
    /// @return The output of the command.
    static std::string execute(const std::string& command);
};
