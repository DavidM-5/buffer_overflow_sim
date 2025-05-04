#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include "config.h"

class CommandManager
{
public:
    CommandManager() = default;
    ~CommandManager() = default;

    void run();

private:
    void handleCreate(const std::vector<std::string>& args);
    void handleChmod(const std::vector<std::string>& args);
    void handleDelete(const std::vector<std::string>& args);
    void handleList(const std::vector<std::string>& args);
    void handleExit(const std::vector<std::string>& args);
    void handleEcho(const std::vector<std::string>& args);

    void parseAndExecute(const std::string& input);
    void updateConfigFile();
};
