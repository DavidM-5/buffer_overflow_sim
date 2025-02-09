#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <regex>
#include <map>

class GDBController {
public:
    GDBController(const std::string& programPath);
    ~GDBController();

    void sendCommand(const std::string& command, bool clearBufferBeforeSend = true);

    // Read the output (with an optional delay to allow GDB to process the command)
    std::string readOutput(int delayMicroseconds = 100000);
    std::string formatGDBOutput(const std::string& rawOutput);

    bool isWaitingForInput();

    void killGDB();


private:
    std::string programPath;
    pid_t gdbPid;
    int gdbToParentPipe[2]; // Pipe for GDB -> Parent communication
    int parentToGdbPipe[2]; // Pipe for Parent -> GDB communication

private:
    std::string unescapeString(const std::string& input);

    void clearBuffer();

};


/*

int main() {
    GDBController gdbController("./your_program");

    // Send a GDB command
    gdbController.sendCommand("break main");
    gdbController.sendCommand("run");

    // Read GDB output
    std::string output = gdbController.readOutput();
    std::cout << "GDB Output: " << output << std::endl;

    // Continue doing other work...
    std::cout << "Main program continues to run..." << std::endl;

    return 0;
}

*/