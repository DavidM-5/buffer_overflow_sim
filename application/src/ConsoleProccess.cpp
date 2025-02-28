#include "ConsoleProccess.h"

ConsoleProccess::ConsoleProccess(const std::string &filename, const std::string &targetDir, const std::string& args) :
                                         m_childPid(-1)
{
    if (pipe(m_in_pipe) == -1 || pipe(m_out_pipe) == -1) {
        std::cerr << "error in pipe in target controller" << std::endl;
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "error in fork in target controller" << std::endl;
        return;
    }

    if (pid == 0) { // Child process
        // Change directory to the target program's directory
        if (chdir(targetDir.c_str()) == -1) {
            std::cerr << "Error changing directory to " << targetDir << ": " << strerror(errno) << std::endl;
            _exit(1);  // Exit child if directory change fails
        }

        // Redirect stdin
        dup2(m_in_pipe[0], STDIN_FILENO);
        close(m_in_pipe[0]);
        close(m_in_pipe[1]);

        // Redirect stdout
        dup2(m_out_pipe[1], STDOUT_FILENO);
        close(m_out_pipe[0]);
        close(m_out_pipe[1]);


        // Prepare arguments for execvp
        std::stringstream ss(args);
        std::string arg;
        std::vector<const char*> execArgs = { filename.c_str() };

        // Convert the strings to a vector of char* (for execvp)
        while (ss >> arg) {
            execArgs.push_back(arg.c_str());
        }
        execArgs.push_back(nullptr);  // Null-terminate the argument list


        // Execute the target program
        execvp(filename.c_str(), const_cast<char**>(execArgs.data()));
        perror("error in execlp in target controller"); // Only runs if exec fails
        exit(1);
    }
    else { // Parent process
        m_childPid = pid;

        close(m_in_pipe[0]);  // Parent doesn't read from in_pipe
        close(m_out_pipe[1]); // Parent doesn't write to out_pipe

        // Set non-blocking mode for reading child's stdout
        set_nonblocking(m_out_pipe[0]);
    }
}

ConsoleProccess::~ConsoleProccess()
{
    if (m_childPid > 0) {
        close(m_in_pipe[1]); // Close pipe after sending input
        close(m_out_pipe[0]); // Close reading end
        kill(m_childPid, 9);
    }
}

void ConsoleProccess::sendInput(const std::string &input)
{
    std::string fullInput = input + "\n";
    write(m_in_pipe[1], fullInput.c_str(), fullInput.size());
}

std::string ConsoleProccess::getOutput() const
{
    // usleep(200000);

    std::string output;

    char buffer[256];
    ssize_t bytes_read;
    
    while (true) {
        // Check if there's output from the child
        bytes_read = read(m_out_pipe[0], buffer, sizeof(buffer));
        if (bytes_read <= 0)
            break;

        output.append(buffer, static_cast<size_t>(bytes_read));
    }

    return output;
}

bool ConsoleProccess::isTargetAlive() const
{
    return (kill(m_childPid, 0) == 0); // Return true if process is alive, false if not
}

void ConsoleProccess::set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}



/*
int main(int argc, char const *argv[])
{
    ConsoleProccess tar("./t1", "../../targets/src/Task_one/vulnerable_system");

    while (tar.isTargetAlive())
    {
        std::cout << tar.getOutput();

        std::string s;
        std::getline(std::cin, s);

        if (s == "q") break;

        tar.sendInput(s);
    }
    

    return 0;
}
*/
