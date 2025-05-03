#pragma once

#include <exception>
#include <string>

namespace Utilities
{
    class CustomException : public std::exception
    {
    private:
        std::string m_errorMessage;

    public:
        CustomException(const std::string& message)
            : m_errorMessage(message) {}

        const char* what() const noexcept override
        {
            return m_errorMessage.c_str();
        }
    };

    class safety_validation_exception : public CustomException 
    {
    public:
        safety_validation_exception(const std::string& message) : CustomException(message) {}        
    };

    class operation_not_supported_exception : public CustomException 
    {
    public:
        operation_not_supported_exception(const std::string& message) : CustomException(message) {}        
    };
}
