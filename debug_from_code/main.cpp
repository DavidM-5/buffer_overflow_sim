#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

class GDBInterface {
private:
    FILE *gdb_pipe;
    pid_t gdb_pid;

    void cleanup() {
        if (gdb_pipe) {
            pclose(gdb_pipe);
            gdb_pipe = nullptr;
        }
    }

public:
    GDBInterface(const std::string& program) {
        std::string cmd = "gdb " + program + " -q";
        gdb_pipe = popen(cmd.c_str(), "w");
        if (!gdb_pipe) {
            throw std::runtime_error("Failed to start GDB");
        }
    }

    ~GDBInterface() {
        cleanup();
    }

    void sendCommand(const std::string& cmd) {
        if (gdb_pipe) {
            fprintf(gdb_pipe, "%s\n", cmd.c_str());
            fflush(gdb_pipe);
            // Give GDB time to process the command
            usleep(100000);
        }
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

        std::cout << "GDB Interface Started. Type 'quit' to exit.\n";
        
        while (true) {
            std::cout << "(gdb)-> ";
            std::getline(std::cin, command);
            
            gdb.sendCommand(command);

            if (command == "quit") {
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