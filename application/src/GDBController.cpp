#include "GDBController.h"

GDBController::GDBController(const std::string &programPath) : programPath(programPath), gdbPid(-1)
{
    // Create pipes for communication
    if (pipe(gdbToParentPipe) == -1 || pipe(parentToGdbPipe) == -1) {
        std::cerr << "Pipe creation failed!" << std::endl;
        exit(1);
    }

    // Fork to create a child process
    gdbPid = fork();
    if (gdbPid == -1) {
        std::cerr << "Fork failed!" << std::endl;
        exit(1);
    }

    if (gdbPid == 0) {
        // Child process (GDB)
        close(gdbToParentPipe[0]); // Close read end
        close(parentToGdbPipe[1]); // Close write end

        // Redirect stdin and stdout
        dup2(parentToGdbPipe[0], STDIN_FILENO);
        dup2(gdbToParentPipe[1], STDOUT_FILENO);

        // Create a new process group for the child
        setpgid(0, 0);

        // Execute GDB
        execlp("gdb", "gdb", "--interpreter=mi2", programPath.c_str(), nullptr);

        // If execlp fails
        std::cerr << "Failed to execute GDB!" << std::endl;
        exit(1);
    } else {
        // Parent process
        close(gdbToParentPipe[1]); // Close write end
        close(parentToGdbPipe[0]); // Close read end

        // Set the pipes to non-blocking mode
        fcntl(gdbToParentPipe[0], F_SETFL, O_NONBLOCK);
    }
}

GDBController::~GDBController()
{
    killGDB();
}

void GDBController::sendCommand(const std::string& command, bool clearBufferBeforeSend)
{
    if (clearBufferBeforeSend) {
        clearBuffer(); // Clear the buffer before sending the command
    }

    std::string fullCommand = command + "\n";
    write(parentToGdbPipe[1], fullCommand.c_str(), fullCommand.size());
}

std::string GDBController::readOutput(int delayMicroseconds)
{
    usleep(delayMicroseconds); // Wait for GDB to process the command

    char buffer[1024];
    std::string output;
    ssize_t bytesRead;

    while ((bytesRead = read(gdbToParentPipe[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        output += buffer;
    }

    return output;
}

std::string GDBController::formatGDBOutput(const std::string &rawOutput)
{
    std::istringstream stream(rawOutput);
    std::string line;
    std::string lastHumanReadableLine;

    while (std::getline(stream, line)) {
        if (line.empty()) continue;

        // Keep only human-readable messages (lines starting with '~')
        if (line[0] == '~') {
            // Remove the '~' prefix and store the line
            lastHumanReadableLine = line.substr(1);

            // Remove surrounding quotes if present
            if (!lastHumanReadableLine.empty() && lastHumanReadableLine.front() == '"' && lastHumanReadableLine.back() == '"') {
                lastHumanReadableLine = lastHumanReadableLine.substr(1, lastHumanReadableLine.size() - 2);
            }

            // Unescape the string
            lastHumanReadableLine = unescapeString(lastHumanReadableLine);
        }
    }

    return lastHumanReadableLine;
}

bool GDBController::isWaitingForInput()
{
    std::string output = readOutput();

    // Check if the output contains the GDB prompt
    std::regex promptRegex("\\(gdb\\)\\s*$");
    return std::regex_search(output, promptRegex);
}

void GDBController::killGDB()
{
    if (gdbPid > 0) {
        // Kill the entire process group
        killpg(gdbPid, SIGKILL);

        // Wait for the process to terminate
        int status;
        waitpid(gdbPid, &status, 0);

        // Reset the PID
        gdbPid = -1;
    }
}

std::string GDBController::unescapeString(const std::string &input)
{
    std::stringstream ss;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\\' && i + 1 < input.size()) {
            switch (input[i + 1]) {
                case 'n': ss << '\n'; ++i; break; // Convert \n to newline
                case 't': ss << '\t'; ++i; break; // Convert \t to tab
                case '\\': ss << '\\'; ++i; break; // Convert \\ to \
                case '"': ss << '"'; ++i; break; // Convert \" to "
                default: ss << input[i]; break; // Leave other escape sequences as-is
            }
        } else {
            ss << input[i];
        }
    }
    return ss.str();
}

void GDBController::clearBuffer()
{
    char buffer[1024];
    while (read(gdbToParentPipe[0], buffer, sizeof(buffer)) > 0) {
        // Discard the data
    }
}
