#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include <cstring>
#include <cerrno>

class ConsoleProccess
{
public:
    ConsoleProccess(const std::string& filename, const std::string &targetDir, const std::string& args = "");
    ~ConsoleProccess();

    void sendInput(const std::string& input);
    std::string getOutput() const;

    bool isTargetAlive() const;
    pid_t getTargetPid() const { return m_childPid; }

private:
    int m_in_pipe[2];  // Parent writes, child reads (stdin redirection)
    int m_out_pipe[2]; // Child writes, parent reads (stdout redirection)

    pid_t m_childPid;

private:
    void set_nonblocking(int fd);

};
