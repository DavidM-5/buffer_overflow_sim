#include "GDBController.h"

GDBController::GDBController(const std::string &filename, const std::string &targetDir) : 
                               m_hitBreakpoint(false)
{
    m_targetAppProcess = std::make_unique<ConsoleProccess>(filename, targetDir);

    pid_t targetPid = m_targetAppProcess->getTargetPid();

    usleep(200000);
    std::string args = filename + " " + std::to_string(targetPid);
    m_gdbProcess = std::make_unique<ConsoleProccess>("gdb", "/", args);
}

GDBController::~GDBController()
{
    m_gdbProcess.~unique_ptr();
    m_targetAppProcess.~unique_ptr();
}

void GDBController::sendCommand(const std::string &command)
{
    m_gdbProcess->sendInput(command);
}

void GDBController::sendTargetInput(const std::string &input)
{
    m_targetAppProcess->sendInput(input);
}

std::string GDBController::getGdbOutput()
{
    return m_gdbProcess->getOutput();
}

std::string GDBController::getTargetOutput()
{
    std::string out = m_targetAppProcess->getOutput();

    std::regex breakpointPattern(R"(Breakpoint\s+(\d+),\s+([\w:~]+)\s*\(.*\)\s+at\s+([^\s]+):(\d+))");
    if (std::regex_search(out, breakpointPattern)) {
        m_hitBreakpoint = true;
        return out;
    }

    m_hitBreakpoint = false;
    return out;
}

std::string GDBController::getAddress(const std::string &name)
{
    usleep(200000); // Give time for the previous commands to process
    getGdbOutput(); // clear the buffer
    
    std::string command = "info address " + name;

    sendCommand(command);
    usleep(150000); // Give time for the command to process
    std::string output = getGdbOutput();
    
    // if (output.length() < 6) return "";
    
    if (output.size() > 6)
        output.resize(output.size()-6);

    output.erase(0, output.find_first_not_of(" \n\r\t"));  // Remove leading whitespaces
    output.erase(output.find_last_not_of(" \n\r\t") + 1);  // Remove trailing whitespaces
    
    if (output.find("Symbol \"") == 0) {
        int start = output.length() - 1;
        while (start > 0 && output.at(start) != ' ') {
            start--;
        }
        
        return output.substr(start+1, output.length());
    }

    return "";
}
