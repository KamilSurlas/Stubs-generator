#include <string>
#include <array>
#include <vector>

namespace Utilities
{
    namespace TypeTraits
    {
        enum class Type
        {
            FUNDAMENTAL,
            POINTER,
            REFERENCE,
            COMPOUND,
            VOID
        };

        static const std::array<std::string, 8> fundamentalTypes = {
            "int", "float", "double", "char", "bool", "short", "long", "unsigned"
        };

        static const std::array<std::string, 3> undeterminedTypes = {
            "const", "static", "volatile"
        };

        std::string getCompundTypeName(const std::string& type);
        bool isFundamentalType(const std::string& type);
        bool isPointerType(const std::string& type);
        bool isReferenceType(const std::string& type);
        bool isVoidType(const std::string& type);
        bool isUndeterminedType(const std::string& type);
        std::vector<std::string> separateTypes(const std::string& type);
        Type getType(const std::string& type);
    }
}