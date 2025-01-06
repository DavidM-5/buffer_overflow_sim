#include "GDBInterface.h"

application::GDBInterface::GDBInterface(const std::string &program)
{
    int in_pipe[2];
    int out_pipe[2];
    
    if (pipe(in_pipe) == -1 || pipe(out_pipe) == -1) {
        throw std::runtime_error("Failed to create pipes");
    }

    // Set non-blocking mode for the read pipe
    fcntl(out_pipe[0], F_SETFL, O_NONBLOCK);

    m_gdb_pid = fork();
    if (m_gdb_pid == -1) {
        throw std::runtime_error("Failed to fork process");
    }

    if (m_gdb_pid == 0) {  // Child process
        dup2(in_pipe[0], STDIN_FILENO);
        dup2(out_pipe[1], STDOUT_FILENO);
        dup2(out_pipe[1], STDERR_FILENO);

        close(in_pipe[1]);
        close(out_pipe[0]);

        execlp("gdb", "gdb", "-q", "--nx", program.c_str(), nullptr);
        exit(1);
    }

    close(in_pipe[0]);
    close(out_pipe[1]);

    m_write_pipe = fdopen(in_pipe[1], "w");
    m_read_pipe = fdopen(out_pipe[0], "r");

    if (!m_write_pipe || !m_read_pipe) {
        cleanup();
        throw std::runtime_error("Failed to open pipe streams");
    }

    // Read initial GDB output
    readUntilPrompt();
}

application::GDBInterface::~GDBInterface()
{
    if (m_write_pipe) {
        fprintf(m_write_pipe, "quit\ny\n");
        fflush(m_write_pipe);
    }
    cleanup();
    if (m_gdb_pid > 0) {
        kill(m_gdb_pid, SIGTERM);
        waitpid(m_gdb_pid, nullptr, 0);
    }
}

std::string application::GDBInterface::sendCommand(const std::string &cmd)
{
    if (!m_write_pipe) {
        throw std::runtime_error("GDB pipe not available");
    }

    fprintf(m_write_pipe, "%s\n", cmd.c_str());
    fflush(m_write_pipe);
    
    return readUntilPrompt();
}

void application::GDBInterface::cleanup()
{
    if (m_write_pipe) {
        fclose(m_write_pipe);
        m_write_pipe = nullptr;
    }
    if (m_read_pipe) {
        fclose(m_read_pipe);
        m_read_pipe = nullptr;
    }
}

std::string application::GDBInterface::readUntilPrompt()
{
    std::string output;
    char buffer[4096];
    fd_set readfds;
    struct timeval tv;
    int fd = fileno(m_read_pipe);

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ready = select(fd + 1, &readfds, nullptr, nullptr, &tv);
        if (ready < 0) {
            throw std::runtime_error("Select error");
        }
        
        if (ready == 0) {
            // Timeout occurred
            if (!output.empty()) break;
            continue;
        }

        if (FD_ISSET(fd, &readfds)) {
            if (fgets(buffer, sizeof(buffer), m_read_pipe) == nullptr) {
                if (feof(m_read_pipe)) break;
                if (ferror(m_read_pipe)) {
                    throw std::runtime_error("Read error");
                }
            }
            output += buffer;
            
            // Check for GDB prompt
            if (strstr(buffer, "(gdb)")) {
                break;
            }
        }
    }
    return output;
}
