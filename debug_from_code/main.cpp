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


class GDBInterface {
private:
    FILE *write_pipe;
    FILE *read_pipe;
    pid_t gdb_pid;

    void cleanup() {
        if (write_pipe) {
            fclose(write_pipe);
            write_pipe = nullptr;
        }
        if (read_pipe) {
            fclose(read_pipe);
            read_pipe = nullptr;
        }
    }

    std::string readUntilPrompt() {
        std::string output;
        char buffer[4096];
        fd_set readfds;
        struct timeval tv;
        int fd = fileno(read_pipe);

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
                if (fgets(buffer, sizeof(buffer), read_pipe) == nullptr) {
                    if (feof(read_pipe)) break;
                    if (ferror(read_pipe)) {
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

public:
    GDBInterface(const std::string& program) {
        int in_pipe[2];
        int out_pipe[2];
        
        if (pipe(in_pipe) == -1 || pipe(out_pipe) == -1) {
            throw std::runtime_error("Failed to create pipes");
        }

        // Set non-blocking mode for the read pipe
        fcntl(out_pipe[0], F_SETFL, O_NONBLOCK);

        gdb_pid = fork();
        if (gdb_pid == -1) {
            throw std::runtime_error("Failed to fork process");
        }

        if (gdb_pid == 0) {  // Child process
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

        write_pipe = fdopen(in_pipe[1], "w");
        read_pipe = fdopen(out_pipe[0], "r");

        if (!write_pipe || !read_pipe) {
            cleanup();
            throw std::runtime_error("Failed to open pipe streams");
        }

        // Read initial GDB output
        readUntilPrompt();
    }

    ~GDBInterface() {
        if (write_pipe) {
            fprintf(write_pipe, "quit\ny\n");
            fflush(write_pipe);
        }
        cleanup();
        if (gdb_pid > 0) {
            kill(gdb_pid, SIGTERM);
            waitpid(gdb_pid, nullptr, 0);
        }
    }

    std::string sendCommand(const std::string& cmd) {
        if (!write_pipe) {
            throw std::runtime_error("GDB pipe not available");
        }

        fprintf(write_pipe, "%s\n", cmd.c_str());
        fflush(write_pipe);
        
        return readUntilPrompt();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <program>\n";
        return 1;
    }

    try {
        GDBInterface gdb(argv[1]);
        std::string command;

        std::cout << "GDB Interface Started. Type 'q' or 'quit' to exit.\n";
        
        // Set up some initial debugging parameters
        gdb.sendCommand("set pagination off");
        
        while (true) {
            std::cout << "(gdb)-> ";
            std::getline(std::cin, command);
            
            std::string output = gdb.sendCommand(command);
            std::cout << output;

            if (command == "q" || command == "quit") {
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
