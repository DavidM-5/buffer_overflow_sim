#include <iostream>
#include <vector>
#include <unordered_map>
#include "config.h"
#include "CommandManager.h"


std::string PATH;
std::unordered_map<std::string, Inode> inodes;


std::unordered_map<std::string, Inode> loadInodesFromFile(const std::string& filepath);

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        PATH = "config/config.txt";
    }
    else {
        std::string modelName = argv[1];
        PATH = "models/" + modelName + "/config/config.txt";
    }

    inodes = loadInodesFromFile(PATH);

    if (inodes.size() == 0)
        return 1;

    CommandManager mngr;
    mngr.run();

    return 0;
}


std::unordered_map<std::string, Inode> loadInodesFromFile(const std::string& filepath) {
    std::unordered_map<std::string, Inode> inodes;
    std::ifstream file(filepath);
    
    if (!file) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return inodes;
    }
    
    std::string line;
    Inode currentInode;
    bool readingInode = false;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        
        if (token == "BEGIN") {
            // start a new inode
            iss >> currentInode.filename;
            currentInode.userPermissions.clear();
            readingInode = true;
        }
        else if (token == "END") {
            // finish this inode and insert into map
            if (readingInode) {
                inodes.emplace(currentInode.filename, std::move(currentInode));
                readingInode = false;
            }
        }
        else if (readingInode) {
            // parse a permission line:  username p1 p2 p3
            std::string username = token;
            int p1, p2, p3;
            if (iss >> p1 >> p2 >> p3) {
                currentInode.userPermissions[username] = {
                    static_cast<bool>(p1),
                    static_cast<bool>(p2),
                    static_cast<bool>(p3)
                };
            }
        }
    }
    
    return inodes;
}

void replaceFileContents(const std::string& srcPath, const std::string& destPath) {
    // 1) Open source file for binary reading
    std::ifstream src(srcPath, std::ios::in | std::ios::binary);
    if (!src.is_open()) {
        std::cerr << "Error: cannot open source file '" << srcPath << "' for reading.\n";
        return;
    }

    // 2) Open (or create) destination file for binary writing, truncating any existing data
    std::ofstream dst(destPath, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!dst.is_open()) {
        std::cerr << "Error: cannot open destination file '" << destPath << "' for writing.\n";
        return;
    }

    // 3) Copy all data from source to destination
    dst << src.rdbuf();

    // 4) Check for write errors
    if (!dst.good()) {
        std::cerr << "Error: failed while writing to '" << destPath << "'.\n";
        return;
    }
}

void resetInodeList() {
    replaceFileContents(PATH + "backup", PATH);

    inodes = loadInodesFromFile(PATH);
}
