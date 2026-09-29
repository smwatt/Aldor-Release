#include "alar.h"
#include <cstdio>
#include <string>
#include <vector>

static int usage()
{
        std::fprintf(stderr, "usage: uniar {r|q|d|t|x}[cv] archive [members ...]\n");
        return 2;
}

int main(int argc, char **argv)
{
        if (argc < 3) return usage();
        std::string flags = argv[1];
        bool verbose = flags.find('v') != std::string::npos;
        char op = 0;
        for (char c: flags) if (c=='r'||c=='q'||c=='d'||c=='t'||c=='x') { op=c; break; }
        if (!op) return usage();
        std::vector<std::string> args;
        for (int i=3; i<argc; ++i) args.emplace_back(argv[i]);
        bool ok = false;
        if (op=='r' || op=='q') ok = alarCreateReplace(argv[2], args, verbose);
        else if (op=='d') ok = alarDelete(argv[2], args, verbose);
        else if (op=='x') ok = alarExtract(argv[2], args, verbose);
        else if (op=='t') {
                std::vector<AlarMember> ms;
                ok = alarList(argv[2], &ms);
                if (ok) for (auto &m: ms) std::puts(m.name.c_str());
        }
        if (!ok) { std::perror("uniar"); return 1; }
        return 0;
}
