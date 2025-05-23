#pragma once

#include <string>
#include <unordered_map>
#include <array>

enum Permission {
    READ = 0,
    WRITE = 1,
    EXECUTE = 2
};


struct Inode {
    std::string filename;
    std::unordered_map<std::string, std::array<bool, 3>> userPermissions;
};


extern std::string PATH_CONFIG;
extern std::string PATH_ROOT;
extern std::unordered_map<std::string, Inode> inodes;

void resetInodeList();
