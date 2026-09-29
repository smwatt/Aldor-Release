#ifndef _ALAR_H_
#define _ALAR_H_

#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>

struct AlarMember {
        std::string name;
        std::uint64_t headerOffset;
        std::uint64_t dataOffset;
        std::uint64_t size;
        std::uint64_t nextOffset;
};

bool alarReadMagic(FILE *);
bool alarReadMember(FILE *, std::uint64_t headerOffset, AlarMember *);
bool alarList(const char *, std::vector<AlarMember> *);
bool alarCreateReplace(const char *, const std::vector<std::string> &, bool verbose);
bool alarDelete(const char *, const std::vector<std::string> &, bool verbose);
bool alarExtract(const char *, const std::vector<std::string> &, bool verbose);

#endif
