#include "SystemCommandExecutor.h"
#include <array>
#include <memory>
#include <stdexcept>

using namespace std;

string SystemCommandExecutor::execute(const string &command)
{
    array<char, 128> buffer;
    string output;
#ifdef _WIN32
    unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
#else
    unique_ptr<FILE, int (*)(FILE*)> pipe(popen(command.c_str(), "r"), pclose);
#endif
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        output += buffer.data();
    }
    return output;
}