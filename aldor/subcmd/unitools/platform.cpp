#include <stdio.h>
#include <string.h>
#include "platform.h"

static const char *objectSuffix() {
#if defined(ENV_MSVC)
    return "obj";
#else
    return "o";
#endif
}

static const char *executableSuffix() {
#if defined(OS_WINDOWS)
    return ".exe";
#else
    return "";
#endif
}

static const char *mathLibrary() {
#if defined(OS_MAC_OSX) || defined(ENV_MSVC)
    return "";
#else
    return "-lm";
#endif
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("%s - %s\n", CONFIGSYS, CONFIG);
        return 0;
    }
    if (argc != 2) return 2;
    if (!strcmp(argv[1], "--object-suffix")) {
        puts(objectSuffix());
        return 0;
    }
    if (!strcmp(argv[1], "--executable-suffix")) {
        puts(executableSuffix());
        return 0;
    }
    if (!strcmp(argv[1], "--math-library")) {
        puts(mathLibrary());
        return 0;
    }
    fprintf(stderr, "usage: platform [--object-suffix|--executable-suffix|--math-library]\n");
    return 2;
}
