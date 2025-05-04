#include <iostream>
#include <vector>
#include "config.h"
#include "CommandManager.h"

std::vector<Inode> inodes;

std::vector<Inode> loadInodesFromFile(const std::string& filepath);

int main(int argc, char const *argv[])
{
    inodes = loadInodesFromFile("config/config.txt");

    CommandManager mngr;

    mngr.run();

    return 0;
}


std::vector<Inode> loadInodesFromFile(const std::string& filepath) {
    std::vector<Inode> inodes;
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
            iss >> currentInode.filename;
            currentInode.userPermissions.clear();
            readingInode = true;
        }
        else if (token == "END") {
            if (readingInode) {
                inodes.push_back(currentInode);
                readingInode = false;
            }
        }
        else if (readingInode) {
            std::string username = token;
            int p1, p2, p3;
            if (iss >> p1 >> p2 >> p3) {
                currentInode.userPermissions[username] = { static_cast<bool>(p1), static_cast<bool>(p2), static_cast<bool>(p3) };
            }
        }
    }
    
    return inodes;
}
