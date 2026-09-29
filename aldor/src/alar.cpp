#include "alar.h"

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <system_error>

namespace {
constexpr char Magic[] = "!<arch>\n";
constexpr std::size_t HeaderSize = 60;

static std::string trimRight(std::string s)
{
        while (!s.empty() && s.back() == ' ') s.pop_back();
        return s;
}

static bool parseDecimal(const char *p, std::size_t n, std::uint64_t *v)
{
        std::string s(p, n);
        s = trimRight(s);
        if (s.empty()) return false;
        char *end = nullptr;
        errno = 0;
        unsigned long long x = std::strtoull(s.c_str(), &end, 10);
        if (errno || !end || *end) return false;
        *v = static_cast<std::uint64_t>(x);
        return true;
}

static std::string baseName(const std::string &p)
{
        return std::filesystem::path(p).filename().string();
}

static bool readBytes(FILE *f, std::uint64_t off, std::uint64_t n,
                      std::vector<unsigned char> *out)
{
        if (std::fseek(f, static_cast<long>(off), SEEK_SET)) return false;
        out->resize(static_cast<std::size_t>(n));
        return n == 0 || std::fread(out->data(), 1, out->size(), f) == out->size();
}

static bool writeHeader(FILE *f, const std::string &name, std::uint64_t size)
{
        char h[HeaderSize];
        std::memset(h, ' ', sizeof h);
        if (name.size() > 16) return false;
        std::memcpy(h, name.data(), name.size());
        std::snprintf(h + 16, 13, "%-12d", int{});
        std::snprintf(h + 28, 7,  "%-6d", int{});
        std::snprintf(h + 34, 7,  "%-6d", int{});
        std::snprintf(h + 40, 9,  "%-8o", 0644);
        std::snprintf(h + 48, 11, "%-10llu", static_cast<unsigned long long>(size));
        h[58] = '`'; h[59] = '\n';
        return std::fwrite(h, 1, sizeof h, f) == sizeof h;
}

static bool writeMember(FILE *out, const std::string &name,
                        const unsigned char *data, std::uint64_t size)
{
        const bool extended = name.size() > 16 || name.find(' ') != std::string::npos;
        std::string hdrName = extended ? "#1/" + std::to_string(name.size()) : name;
        std::uint64_t stored = size + (extended ? name.size() : 0);
        if (!writeHeader(out, hdrName, stored)) return false;
        if (extended && std::fwrite(name.data(), 1, name.size(), out) != name.size()) return false;
        if (size && std::fwrite(data, 1, static_cast<std::size_t>(size), out) != size) return false;
        if (stored & 1) {
                const unsigned char nl = '\n';
                if (std::fwrite(&nl, 1, 1, out) != 1) return false;
        }
        return true;
}

struct StoredMember {
        std::string name;
        std::vector<unsigned char> data;
};

static bool loadArchive(const char *path, std::vector<StoredMember> *members)
{
        FILE *f = std::fopen(path, "rb");
        if (!f) return errno == ENOENT;
        if (!alarReadMagic(f)) { std::fclose(f); return false; }
        std::uint64_t off = sizeof(Magic) - 1;
        for (;;) {
                AlarMember m;
                if (std::fseek(f, 0L, SEEK_END)) { std::fclose(f); return false; }
                long end = std::ftell(f);
                if (end < 0) { std::fclose(f); return false; }
                if (off == static_cast<std::uint64_t>(end)) break;
                if (off > static_cast<std::uint64_t>(end) || !alarReadMember(f, off, &m)) {
                        std::fclose(f); return false;
                }
                StoredMember sm; sm.name = m.name;
                if (!readBytes(f, m.dataOffset, m.size, &sm.data)) { std::fclose(f); return false; }
                members->push_back(std::move(sm));
                off = m.nextOffset;
        }
        std::fclose(f);
        return true;
}

static bool writeArchive(const char *path, const std::vector<StoredMember> &members)
{
        std::string tmp = std::string(path) + ".tmp";
        FILE *f = std::fopen(tmp.c_str(), "wb");
        if (!f) return false;
        bool ok = std::fwrite(Magic, 1, sizeof(Magic)-1, f) == sizeof(Magic)-1;
        for (const auto &m: members)
                ok = ok && writeMember(f, m.name, m.data.data(), m.data.size());
        ok = ok && std::fclose(f) == 0;
        if (!ok) { std::remove(tmp.c_str()); return false; }
        std::remove(path); /* needed on platforms where rename won't replace */
        if (std::rename(tmp.c_str(), path)) { std::remove(tmp.c_str()); return false; }
        return true;
}
}

bool alarReadMagic(FILE *f)
{
        char b[sizeof(Magic)-1];
        if (std::fseek(f, 0L, SEEK_SET)) return false;
        return std::fread(b, 1, sizeof b, f) == sizeof b &&
               std::memcmp(b, Magic, sizeof b) == 0;
}

bool alarReadMember(FILE *f, std::uint64_t headerOffset, AlarMember *m)
{
        char h[HeaderSize];
        if (std::fseek(f, static_cast<long>(headerOffset), SEEK_SET)) return false;
        if (std::fread(h, 1, sizeof h, f) != sizeof h) return false;
        if (h[58] != '`' || h[59] != '\n') return false;
        std::uint64_t storedSize;
        if (!parseDecimal(h + 48, 10, &storedSize)) return false;
        std::string n = trimRight(std::string(h, 16));
        std::uint64_t nameBytes = 0;
        if (n.rfind("#1/", std::string::size_type{}) == 0) {
                if (!parseDecimal(n.data()+3, n.size()-3, &nameBytes) || nameBytes > storedSize)
                        return false;
                std::vector<char> nb(static_cast<std::size_t>(nameBytes));
                if (nameBytes && std::fread(nb.data(), 1, nb.size(), f) != nb.size()) return false;
                n.assign(nb.begin(), nb.end());
        }
        else if (n == "/" || n == "//" || (!n.empty() && n[0] == '/')) {
                /* GNU/SVR4 special-name machinery is deliberately unsupported. */
                return false;
        }
        m->name = n;
        m->headerOffset = headerOffset;
        m->dataOffset = headerOffset + HeaderSize + nameBytes;
        m->size = storedSize - nameBytes;
        m->nextOffset = headerOffset + HeaderSize + storedSize + (storedSize & 1);
        return true;
}

bool alarList(const char *path, std::vector<AlarMember> *members)
{
        FILE *f = std::fopen(path, "rb");
        if (!f) return false;
        if (!alarReadMagic(f)) { std::fclose(f); return false; }
        if (std::fseek(f, 0L, SEEK_END)) { std::fclose(f); return false; }
        long end = std::ftell(f);
        std::uint64_t off = sizeof(Magic)-1;
        while (end >= 0 && off < static_cast<std::uint64_t>(end)) {
                AlarMember m;
                if (!alarReadMember(f, off, &m)) { std::fclose(f); return false; }
                members->push_back(m); off = m.nextOffset;
        }
        bool ok = end >= 0 && off == static_cast<std::uint64_t>(end);
        std::fclose(f); return ok;
}

bool alarCreateReplace(const char *path, const std::vector<std::string> &files, bool verbose)
{
        std::vector<StoredMember> old;
        if (!loadArchive(path, &old)) return false;
        std::map<std::string, StoredMember> repl;
        std::vector<std::string> order;
        for (const auto &file: files) {
                std::ifstream in(file, std::ios::binary);
                if (!in) return false;
                StoredMember sm; sm.name = baseName(file);
                sm.data.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
                repl[sm.name] = sm; order.push_back(sm.name);
        }
        std::set<std::string> seen, oldNames;
        for (const auto &m: old) oldNames.insert(m.name);
        std::vector<StoredMember> out;
        for (auto &m: old) {
                auto it = repl.find(m.name);
                if (it != repl.end()) { out.push_back(it->second); seen.insert(m.name); }
                else out.push_back(std::move(m));
        }
        for (const auto &n: order) if (!seen.count(n)) { out.push_back(repl[n]); seen.insert(n); }
        if (verbose)
                for (const auto &n: order)
                        std::printf("%c - %s\n", oldNames.count(n) ? 'r' : 'a',
                                    n.c_str());
        return writeArchive(path, out);
}

bool alarDelete(const char *path, const std::vector<std::string> &names, bool verbose)
{
        std::vector<StoredMember> old, out;
        if (!loadArchive(path, &old)) return false;
        std::set<std::string> del;
        for (auto &n: names) del.insert(baseName(n));
        for (auto &m: old) {
                if (del.count(m.name)) { if (verbose) std::printf("d - %s\n", m.name.c_str()); }
                else out.push_back(std::move(m));
        }
        return writeArchive(path, out);
}

bool alarExtract(const char *path, const std::vector<std::string> &names, bool verbose)
{
        std::vector<StoredMember> ms;
        if (!loadArchive(path, &ms)) return false;
        std::set<std::string> wanted;
        for (auto &n: names) wanted.insert(baseName(n));
        for (auto &m: ms) if (wanted.empty() || wanted.count(m.name)) {
                std::ofstream out(m.name, std::ios::binary | std::ios::trunc);
                if (!out) return false;
                out.write(reinterpret_cast<const char *>(m.data.data()), m.data.size());
                if (!out) return false;
                if (verbose) std::printf("x - %s\n", m.name.c_str());
        }
        return true;
}
