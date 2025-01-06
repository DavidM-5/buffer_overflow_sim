#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <array>
#include <memory>
#include <stdexcept>
#include <fcntl.h>
#include <signal.h>
#include <cstring>


namespace application
{
    
    class GDBInterface
    {
    public:
        GDBInterface(const std::string& program);
        ~GDBInterface();

        std::string sendCommand(const std::string& cmd);
    
    private:
        FILE *m_write_pipe;
        FILE *m_read_pipe;
        pid_t m_gdb_pid;

    private:
        void cleanup();
        std::string readUntilPrompt();

    };

} // namespace application

