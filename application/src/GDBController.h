#pragma once

#include <vector>
#include <algorithm>
#include <memory>
#include <regex>
#include <sstream>
#include "ConsoleProccess.h"

class GDBController
{
public:
    GDBController(const std::string& filename, const std::string &targetDir);
    ~GDBController();

    void sendCommand(const std::string& command);
    void sendTargetInput(const std::string& input);

    std::string getGdbOutput();
    std::string getTargetOutput();

    bool isAtBreakpoint();
    std::vector<std::string> getMemoryDump(const std::string& startAddr = "$rbp", size_t numOfAddresses = 10);

    std::string getAddress(const std::string& name);

private:
    std::unique_ptr<ConsoleProccess> m_targetAppProcess;
    std::unique_ptr<ConsoleProccess> m_gdbProcess;

    std::string m_lastGdbOutput;

    bool m_hitBreakpoint;

};
