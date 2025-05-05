#include <string>

namespace Utilities
{
    namespace SystemCommandExecutor
    {
        /// @brief Executes a system command and returns the output.
        /// @param command The command to execute
        /// @return The output of the command.
        std::string execute(const std::string& command);
    }
}
