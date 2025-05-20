#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include "config.h"

class CommandManager
{
public:
    CommandManager() = default;
    ~CommandManager() = default;

    void run();

private:
    Inode* findInode(std::unordered_map<std::string, Inode>& inodes, const std::string& filename);

    void handleCreate(const std::vector<std::string>& args);
    void handleChmod(const std::vector<std::string>& args);
    void handleDelete(const std::vector<std::string>& args);
    void handleList(const std::vector<std::string>& args);
    void handleExit(const std::vector<std::string>& args);
    void handleEcho(const std::vector<std::string>& args);
    void handleHelp(const std::vector<std::string>& args);
    void handleReset(const std::vector<std::string>& args);

    void parseAndExecute(const std::string& input);
    void updateConfigFile();

    void logAction(const std::string& action);
};
