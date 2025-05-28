#include "TypeTraits.h"
#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;

namespace Utilities
{
    namespace TypeTraits
    {
        std::string getCompundTypeName(const std::string &type)
        {
            vector<string> types = separateTypes(type);
            for (const auto &t : types)
            {
                if (find(undeterminedTypes.begin(), undeterminedTypes.end(), t) == undeterminedTypes.end())
                {
                    return t;
                }
            }
            return "";
        }

        bool isFundamentalType(const string &type)
        {
            vector<string> types = separateTypes(type); 
            for (const auto &t : types)
            {
                if (find(undeterminedTypes.begin(), undeterminedTypes.end(), t) != undeterminedTypes.end())
                {
                    continue;
                }
                if (find(fundamentalTypes.begin(), fundamentalTypes.end(), t) != fundamentalTypes.end())
                {
                    return true;
                }
            }
            return false;
        }
        
        bool isPointerType(const string &type)
        {
            return type.find('*') != string::npos;
        }

        bool isReferenceType(const string &type)
        {
            return type.find('&') != string::npos;
        }

        bool isVoidType(const string &type)
        {
            return (type.find("void") != string::npos);
        }

        bool isUndeterminedType(const string &type)
        {
            return find(undeterminedTypes.begin(), undeterminedTypes.end(), type) != undeterminedTypes.end();
        }

        vector<string> separateTypes(const string& type)
        {
            istringstream ss(type);
            string word;
            vector<string> types;
            while (ss >> word){
                types.push_back(word);
            }
            return types;
        }

        Type getType(const string &type)
        {
            if (isFundamentalType(type))
            {
                return Type::FUNDAMENTAL;
            }
            else if (isPointerType(type))
            {
                return Type::POINTER;
            }
            else if (isReferenceType(type))
            {
                return Type::REFERENCE;
            }
            else if (isVoidType(type))
            {
                return Type::VOID;
            }
            else
            {
                return Type::COMPOUND;
            }
        }
    }
}
