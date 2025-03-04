#include "GDBController.h"

GDBController::GDBController(const std::string &filename, const std::string &targetDir) : 
                               m_lastGdbOutput(""),
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

bool GDBController::isAtBreakpoint()
{
    int status;
    pid_t result = waitpid(m_targetAppProcess->getTargetPid(), &status, WNOHANG);
    
    if (result > 0) {
        // Check if process is stopped due to a trap (breakpoint)
        if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> GDBController::getMemoryDump(const std::string &startAddr, size_t numOfAddresses)
{
    // "x/10xg $rbp-80" <- print $rbp+8 (the ret address), $rbp, and 8 addresses below $rbp;
    std::ostringstream oss;
    numOfAddresses--;
    oss << "x/" << numOfAddresses << "xg " << startAddr << "-" << (numOfAddresses - 2) * 8;

    std::string command = oss.str();

    // getGdbOutput(); // clear the buffer
    m_gdbProcess->sendInput(command);

    std::string out = getGdbOutput();

    std::vector<std::string> addressess;
    std::istringstream ss(out);
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

        if (value1.substr(0, 2) != "0x") {
            value1 = value2;

            if (lineStream.eof())
                value2.clear();
            else
                lineStream >> value2;
        }

        // Add values to vector
        if (!value1.empty() && value1.substr(0, 2) == "0x")
            addressess.push_back(value1);

        if (!value2.empty() && value2.substr(0, 2) == "0x")
            addressess.push_back(value2);
    }

    std::reverse(addressess.begin(), addressess.end());
    return addressess;
}

std::string GDBController::getAddress(const std::string &name)
{
    usleep(200000); // Give time for the previous commands to process
    getGdbOutput(); // clear the buffer
    
    std::string command = "info address " + name;

    sendCommand(command);
    usleep(100000); // Give time for the command to process
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
