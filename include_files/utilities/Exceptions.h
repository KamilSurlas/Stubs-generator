#pragma once

#include <exception>
#include <string>

namespace Utilities
{
    /// @brief Base class for custom exceptions.
    class CustomException : public std::exception
    {
    private:
        /// @brief exception message.
        std::string m_errorMessage;

    public:
        CustomException(const std::string& message)
            : m_errorMessage(message) {}

        const char* what() const noexcept override
        {
            return m_errorMessage.c_str();
        }
    };

    /// @brief custom exception class to represent safety validation exception.
    class safety_validation_exception : public CustomException 
    {
    public:
        safety_validation_exception(const std::string& message) : CustomException(message) {}        
    };

    /// @brief custom exception class to represent unsupported operation exception.
    class operation_not_supported_exception : public CustomException 
    {
    public:
        operation_not_supported_exception(const std::string& message) : CustomException(message) {}        
    };
}
