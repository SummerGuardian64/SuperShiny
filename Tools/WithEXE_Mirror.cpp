// WithEXE_Mirror.cpp — minimal XP-friendly mirroring via env vars
// Env:
//   WITHEXE_SRC  = source folder (required)
//   WITHEXE_DST  = destination folder (required)
//   WITHEXE_KEEP = full path to a file inside DST to preserve (optional)
//
// Step 1: copy newer/missing files from SRC to DST
// Step 2: delete things in DST not in SRC, except WITHEXE_KEEP

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#ifndef WINVER
#define WINVER 0x0501
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

// Keep-matching PDB (lowercased fullpath); empty if not applicable
static std::string g_keepPdbLower;

static bool IsDirectory(const char* p)
{
    DWORD a = GetFileAttributesA(p);
    return (a != INVALID_FILE_ATTRIBUTES) && (a & FILE_ATTRIBUTE_DIRECTORY);
}

static bool EnsureDirectory(const std::string& path)
{
    if (path.empty())
    {
        return true;
    }
    DWORD a = GetFileAttributesA(path.c_str());
    if (a != INVALID_FILE_ATTRIBUTES)
    {
        return (a & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    char buf[MAX_PATH];
    size_t n = path.size();
    if (n >= MAX_PATH) return false;

    for (size_t i = 0; i < n; i++)
    {
        buf[i] = path[i];
        if ((buf[i] == '\\' || buf[i] == '/') && i > 2) // skip "X:\"
        {
            char save = buf[i];
            buf[i] = '\0';
            CreateDirectoryA(buf, NULL);
            buf[i] = save;
            buf[i] = '\\'; // normalize
        }
    }
    buf[n] = '\0';
    if (!CreateDirectoryA(buf, NULL))
    {
        DWORD e = GetLastError();
        if (e != ERROR_ALREADY_EXISTS) return false;
    }
    return true;
}

static std::string ToLower(const std::string& s)
{
    std::string r(s);
    for (size_t i = 0; i < r.size(); i++)
    {
        char c = r[i];
        if (c >= 'A' && c <= 'Z') r[i] = (char)(c - 'A' + 'a');
    }
    return r;
}

static std::string Join(const std::string& a, const char* name)
{
    if (a.empty()) return std::string(name);
    char last = a[a.size() - 1];
    if (last == '\\' || last == '/') return a + name;
    std::string out = a;
    out.push_back('\\');
    out += name;
    return out;
}

static bool DeleteTree(const std::string& path, const std::string& keepLower)
{
    WIN32_FIND_DATAA f;
    std::string pattern = path + "\\*";
    HANDLE h = FindFirstFileA(pattern.c_str(), &f);
    if (h != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (std::strcmp(f.cFileName, ".") == 0 || std::strcmp(f.cFileName, "..") == 0)
            {
                continue;
            }
            std::string child = Join(path, f.cFileName);
            if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (!DeleteTree(child, keepLower)) { FindClose(h); return false; }
            }
            else
            {
                std::string lower = ToLower(child);
                if (lower == keepLower || (!g_keepPdbLower.empty() && lower == g_keepPdbLower))
                {
                    continue;
                }
                if (!DeleteFileA(child.c_str()))
                {
                    // best-effort; report and continue
                    std::fprintf(stderr, "DeleteFile failed: %s (err %lu)\n", child.c_str(), GetLastError());
                }
            }
        }
        while (FindNextFileA(h, &f));
        FindClose(h);
    }
    // Don't remove a directory if it contains KEEP (parent of KEEP)
    if (!keepLower.empty())
    {
        std::string hereLower = ToLower(path);
        size_t m = hereLower.size();
        if (m > 0 && m <= keepLower.size())
        {
            if (std::memcmp(hereLower.c_str(), keepLower.c_str(), m) == 0)
            {
                if (keepLower.size() == m || keepLower[m] == '\\')
                {
                    return true; // KEEP is inside; don't remove this dir
                }
            }
        }
    }
    // existing keepLower guard stays as-is...
    // Then add this second guard for the PDB:
    if (!g_keepPdbLower.empty())
    {
        std::string hereLower = ToLower(path);
        size_t m = hereLower.size();
        if (m > 0 && m <= g_keepPdbLower.size())
        {
            if (std::memcmp(hereLower.c_str(), g_keepPdbLower.c_str(), m) == 0)
            {
                if (g_keepPdbLower.size() == m || g_keepPdbLower[m] == '\\')
                {
                    return true; // PDB is inside; don't remove this dir
                }
            }
        }
    }
    
    RemoveDirectoryA(path.c_str()); // ignore failure if not empty
    return true;
}

int main()
{
    const char* evSrc = std::getenv("WITHEXE_SRC");
    const char* evDst = std::getenv("WITHEXE_DST");
    const char* evKeep = std::getenv("WITHEXE_KEEP"); // optional

    if (!evSrc || !evDst)
    {
        std::puts("WITHEXE parameters missing. Please fix them or remove this command from post-build steps");
        return 1;
    }

    char srcFull[MAX_PATH];
    char dstFull[MAX_PATH];
    char keepFull[MAX_PATH];

    if (!GetFullPathNameA(evSrc, MAX_PATH, srcFull, NULL)) std::strncpy(srcFull, evSrc, MAX_PATH - 1), srcFull[MAX_PATH - 1] = '\0';
    if (!GetFullPathNameA(evDst, MAX_PATH, dstFull, NULL)) std::strncpy(dstFull, evDst, MAX_PATH - 1), dstFull[MAX_PATH - 1] = '\0';

    std::string keepLower;
    if (evKeep && evKeep[0])
    {
        if (!GetFullPathNameA(evKeep, MAX_PATH, keepFull, NULL)) std::strncpy(keepFull, evKeep, MAX_PATH - 1), keepFull[MAX_PATH - 1] = '\0';
        keepLower = ToLower(keepFull);
    }
    
    // If KEEP ends with .exe, also protect the side-by-side .pdb
    g_keepPdbLower.clear();
    if (!keepLower.empty())
    {
        size_t dot = keepLower.rfind('.');
        if (dot != std::string::npos)
        {
            if (keepLower.substr(dot) == ".exe")
            {
                g_keepPdbLower = keepLower.substr(0, dot) + ".pdb";
            }
        }
    }


    if (!IsDirectory(srcFull))
    {
        std::fprintf(stderr, "WITHEXE_SRC not found or not a directory: %s\n", srcFull);
        return 2;
    }
    if (!EnsureDirectory(dstFull))
    {
        std::fprintf(stderr, "Failed to create WITHEXE_DST: %s\n", dstFull);
        return 3;
    }

    // --- Step 1: copy newer/missing files from SRC to DST ---
    {
        std::vector<std::pair<std::string, std::string> > stack;
        stack.push_back(std::make_pair(std::string(srcFull), std::string(dstFull)));

        while (!stack.empty())
        {
            std::string curS = stack.back().first;
            std::string curD = stack.back().second;
            stack.pop_back();

            WIN32_FIND_DATAA f;
            std::string pattern = curS + "\\*";
            HANDLE h = FindFirstFileA(pattern.c_str(), &f);
            if (h == INVALID_HANDLE_VALUE) continue;

            do
            {
                if (std::strcmp(f.cFileName, ".") == 0 || std::strcmp(f.cFileName, "..") == 0)
                {
                    continue;
                }

                std::string sPath = Join(curS, f.cFileName);
                std::string dPath = Join(curD, f.cFileName);

                if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    EnsureDirectory(dPath);
                    stack.push_back(std::make_pair(sPath, dPath));
                }
                else
                {
                    std::string dLow = ToLower(dPath);
                    if ((!keepLower.empty() && dLow == keepLower) ||
                        (!g_keepPdbLower.empty() && dLow == g_keepPdbLower))
                    {
                        continue; // never touch KEEP EXE/PDB
                    }
                    
                    WIN32_FILE_ATTRIBUTE_DATA sa, da;
                    BOOL hasS = GetFileAttributesExA(sPath.c_str(), GetFileExInfoStandard, &sa);
                    BOOL hasD = GetFileAttributesExA(dPath.c_str(), GetFileExInfoStandard, &da);
                    bool needCopy = false;

                    if (!hasS)
                    {
                        continue; // shouldn't happen
                    }
                    if (!hasD)
                    {
                        needCopy = true;
                    }
                    else
                    {
                        // newer or different size
                        ULARGE_INTEGER us, ud;
                        us.LowPart = sa.ftLastWriteTime.dwLowDateTime; us.HighPart = sa.ftLastWriteTime.dwHighDateTime;
                        ud.LowPart = da.ftLastWriteTime.dwLowDateTime; ud.HighPart = da.ftLastWriteTime.dwHighDateTime;
                        if (us.QuadPart > ud.QuadPart) needCopy = true;
                        if (sa.nFileSizeHigh != da.nFileSizeHigh || sa.nFileSizeLow != da.nFileSizeLow) needCopy = true;
                    }

                    if (needCopy)
                    {
                        std::string parent = dPath.substr(0, dPath.find_last_of("\\/"));
                        EnsureDirectory(parent);
                        if (!CopyFileA(sPath.c_str(), dPath.c_str(), FALSE))
                        {
                            std::fprintf(stderr, "CopyFile failed: %s -> %s (err %lu)\n",
                                         sPath.c_str(), dPath.c_str(), GetLastError());
                        }
                    }
                }
            }
            while (FindNextFileA(h, &f));
            FindClose(h);
        }
    }

    // --- Step 2: delete things in DST that are not in SRC (except KEEP) ---
    {
        std::vector<std::pair<std::string, std::string> > stack;
        stack.push_back(std::make_pair(std::string(dstFull), std::string(srcFull)));

        while (!stack.empty())
        {
            std::string curD = stack.back().first;
            std::string curS = stack.back().second;
            stack.pop_back();

            WIN32_FIND_DATAA f;
            std::string pattern = curD + "\\*";
            HANDLE h = FindFirstFileA(pattern.c_str(), &f);
            if (h == INVALID_HANDLE_VALUE) continue;

            do
            {
                if (std::strcmp(f.cFileName, ".") == 0 || std::strcmp(f.cFileName, "..") == 0)
                {
                    continue;
                }

                std::string dPath = Join(curD, f.cFileName);
                std::string sPath = Join(curS, f.cFileName);

                if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (IsDirectory(sPath.c_str()))
                    {
                        stack.push_back(std::make_pair(dPath, sPath));
                    }
                    else
                    {
                        // If KEEP is inside this directory, skip deleting this subtree
                        if (!keepLower.empty())
                        {
                            std::string dLower = ToLower(dPath);
                            size_t m = dLower.size();
                            if (m <= keepLower.size() &&
                                std::memcmp(dLower.c_str(), keepLower.c_str(), m) == 0 &&
                                (keepLower.size() == m || keepLower[m] == '\\'))
                            {
                                continue; // KEEP lives here; don't delete this dir
                            }
                        }
                        DeleteTree(dPath, keepLower);
                    }
                }
                else
                {
                    std::string dLow = ToLower(dPath);
                    if ((!keepLower.empty() && dLow == keepLower) ||
                        (!g_keepPdbLower.empty() && dLow == g_keepPdbLower))
                    {
                        continue; // preserve EXE/PDB
                    }
                    
                    if (GetFileAttributesA(sPath.c_str()) == INVALID_FILE_ATTRIBUTES)
                    {
                        if (!DeleteFileA(dPath.c_str()))
                        {
                            std::fprintf(stderr, "DeleteFile failed: %s (err %lu)\n",
                                         dPath.c_str(), GetLastError());
                        }
                    }
                }
            }
            while (FindNextFileA(h, &f));
            FindClose(h);
        }
    }

    return 0;
}
