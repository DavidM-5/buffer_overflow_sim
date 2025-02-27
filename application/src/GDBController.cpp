#include "GDBController.h"

GDBController::GDBController(const std::string &filename, const std::string &targetDir) : 
                               m_lastGdbOutput("")
{
    m_targetAppProcess = std::make_unique<ConsoleProccess>(filename, targetDir);

    pid_t targetPid = m_targetAppProcess->getTargetPid();

    std::string args = filename + " " + std::to_string(targetPid);
    m_gdbProcess = std::make_unique<ConsoleProccess>("gdb", ".", args);
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

std::string GDBController::getRawOutput()
{
    m_lastGdbOutput = m_gdbProcess->getOutput();
    
    return m_lastGdbOutput;
}

std::string GDBController::getTargetOutput()
{
    return m_targetAppProcess->getOutput();
}

bool GDBController::isAtBreakpoint()
{
    getRawOutput();

    std::regex breakpointPattern(R"(Breakpoint\s+(\d+),\s+([\w:~]+)\s*\(.*\)\s+at\s+([^\s]+):(\d+))");
    std::smatch match;

    if (std::regex_search(m_lastGdbOutput, match, breakpointPattern)) {
        std::cout << "Hit Breakpoint #" << match[1] << " in function " << match[2] 
                  << " at " << match[3] << ":" << match[4] << std::endl;
        return true;
    }
    
    return false;
}

std::vector<std::string> GDBController::getMemoryDump(const std::string &startAddr, size_t numOfAddresses)
{
    // "x/10xg $rbp-80" <- print $rbp+8 (the ret address), $rbp, and 8 addresses below $rbp;
    std::ostringstream oss;
    oss << "x/" << numOfAddresses << "xg " << startAddr << "-" << (numOfAddresses - 2) * 8;

    std::string command = oss.str();

    m_gdbProcess->getOutput(); // clear the buffer
    m_gdbProcess->sendInput(command);

    std::vector<std::string> addressess;
    std::istringstream ss(m_gdbProcess->getOutput());
    std::string line;

    // Parse the string and extract the adressess values
    while (std::getline(ss, line)) {
        // Clean up any leading/trailing whitespace
        line.erase(0, line.find_first_not_of(" \n\r\t"));  // Remove leading whitespaces
        line.erase(line.find_last_not_of(" \n\r\t") + 1);  // Remove trailing whitespaces

        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        std::istringstream lineStream(line);
        std::string address, value1, value2;

        // Extract address and values
        lineStream >> address >> value1 >> value2;

        // Add values to vector
        if (!value1.empty())
            addressess.push_back(value1);

        if (!value2.empty())
            addressess.push_back(value2);
    }

    std::reverse(addressess.begin(), addressess.end());
    return addressess;
}

std::string GDBController::getAddress(const std::string &name)
{
    std::string command = "info address " + name;
    
    m_gdbProcess->getOutput(); // clear the buffer
    sendCommand(command);
    std::string output = m_gdbProcess->getOutput();
    
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
