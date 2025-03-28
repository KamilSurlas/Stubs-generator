#include <stdio.h>

static const char* usage = "USAGE: StubsGenerator [fileName] [path to compile_commands.json]\n";

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("%s", usage);
        return 1;
    }
    return 0;
}