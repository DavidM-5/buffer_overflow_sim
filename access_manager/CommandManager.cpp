#include "CommandManager.h"

void CommandManager::run()
{
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        parseAndExecute(input);
    }
}

void CommandManager::handleCreate(const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        std::cout << "Usage: create <filename>" << std::endl;
        return;
    }

    const std::string& filename = args[0];
    std::string path = "files/" + filename;

    namespace fs = std::filesystem;

    // Check if files folder exists
    if (!fs::exists("files")) {
        if (!fs::create_directory("files")) {
            std::cout << "Failed to create 'files' directory!" << std::endl;
            return;
        }
    }

    // Check if file already exists
    if (fs::exists(path)) {
        std::cout << "Error: File '" << filename << "' already exists." << std::endl;
        return;
    }

    // Create the file
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cout << "Error: Failed to create file." << std::endl;
        return;
    }

    file.close();

    Inode newInode;
    newInode.filename = filename;
    
    inodes.push_back(newInode);
    updateConfigFile();
}

void CommandManager::handleChmod(const std::vector<std::string> &args)
{
    if (args.size() != 3 || args[2].length() != 6) {
        std::cout << "Usage: chmod <filename> <user> <-r+w-x>" << std::endl;
        return;
    }

    std::string file = args[0];
    std::string user = args[1];
    std::string permissions = args[2];
    
    Inode* inode = nullptr;
    // Check if file exists
    for (Inode& node : inodes) {
        if (node.filename == file) {
            inode = &node;
        }
    }

    if (!inode) {
        std::cout << "No such file exist." << std::endl;
        return;
    }


    if (permissions[1] == 'r') {
        if (permissions[0] == '+') inode->userPermissions[user][READ] = true;
        else if (permissions[0] == '-') inode->userPermissions[user][READ] = false;
    }
    if (permissions[3] == 'w') {
        if (permissions[2] == '+') inode->userPermissions[user][WRITE] = true;
        else if (permissions[2] == '-') inode->userPermissions[user][WRITE] = false;
    }
    if (permissions[5] == 'x') {
        if (permissions[4] == '+') inode->userPermissions[user][EXECUTE] = true;
        else if (permissions[4] == '-') inode->userPermissions[user][EXECUTE] = false;
    }

    updateConfigFile();
}

void CommandManager::handleDelete(const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        std::cout << "Usage: delete <filename>" << std::endl;
        return;
    }

    std::string filename = args[0];

    // Find inode
    auto it = std::find_if(inodes.begin(), inodes.end(), [&](const Inode& inode) {
        return inode.filename == filename;
    });

    if (it == inodes.end()) {
        std::cout << "No such file: " << filename << std::endl;
        return;
    }

    // Try to delete the file from the filesystem
    std::string path = "files/" + filename;
    if (std::remove(path.c_str()) != 0) {
        std::cout << "Failed to delete file from disk." << std::endl;
        return;
    }

    // Remove inode from memory
    inodes.erase(it);

    // Update the config file
    updateConfigFile();
}

void CommandManager::handleList(const std::vector<std::string> &args)
{
    for (auto& i : inodes) {
        std::cout << i.filename << std::endl;
        
        for (const auto& pair : i.userPermissions) {
            std::cout << pair.first << ": ";
            std::cout << pair.second[READ] << " ";
            std::cout << pair.second[WRITE] << " ";
            std::cout << pair.second[EXECUTE] << std::endl;
        }

        std::cout << std::endl;
    }
}

void CommandManager::handleExit(const std::vector<std::string> &args)
{
    exit(0);
}

void CommandManager::handleEcho(const std::vector<std::string> &args)
{
    if (args.empty()) {
        std::cout << "Usage:\n";
        std::cout << "  echo \"<text>\" or echo <file>\n";
        std::cout << "  echo <user> \"<text>\" >> <file>\n";
        std::cout << "  echo <user> <file1> >> <file2>\n";
        return;
    }

    // Find ">>" if it exists
    bool appendMode = false;
    int appendIndex = -1;
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == ">>") {
            appendMode = true;
            appendIndex = i;
            break;
        }
    }

    if (appendMode) {
        // Check minimum required arguments
        if (appendIndex < 2 || appendIndex + 1 >= args.size()) {
            std::cout << "Invalid usage for append mode." << std::endl;
            return;
        }

        std::string user = args[0];
        std::string targetFile = args[appendIndex + 1];
        std::string content;

        // Check if content is quoted
        if (args[1].size() >= 2 && args[1][0] == '"') {
            // Rebuild quoted content
            content = args[1].substr(1);  // Remove opening quote
            if (content.back() == '"') {
                content.pop_back();  // Remove closing quote if in first arg
            } else {
                // Find closing quote in subsequent args
                for (int i = 2; i < appendIndex; ++i) {
                    content += " " + args[i];
                    if (args[i].back() == '"') {
                        content.pop_back();  // Remove closing quote
                        break;
                    }
                }
            }
        } else {
            // Treat as filename
            content = args[1];
        }

        // Find destination inode
        Inode* destInode = nullptr;
        for (Inode& node : inodes) {
            if (node.filename == targetFile) {
                destInode = &node;
                break;
            }
        }

        if (!destInode) {
            std::cout << "Destination file does not exist: " << targetFile << std::endl;
            return;
        }

        if (!destInode->userPermissions[user][WRITE]) {
            std::cout << "Permission denied: user '" << user << "' cannot write to " << targetFile << std::endl;
            return;
        }

        // Check if content is a file (only if not quoted)
        if (args[1][0] != '"') {
            for (Inode& node : inodes) {
                if (node.filename == content) {
                    // Found source file - check permissions
                    if (!node.userPermissions[user][READ]) {
                        std::cout << "Permission denied: user '" << user << "' cannot read from " << content << std::endl;
                        return;
                    }

                    // Append file content
                    std::ifstream src("files/" + content);
                    std::ofstream dst("files/" + targetFile, std::ios::app);
                    if (!src.is_open() || !dst.is_open()) {
                        std::cout << "Failed to open files." << std::endl;
                        return;
                    }
                    dst << src.rdbuf();
                    return;
                }
            }
        }

        // Append text content
        std::ofstream dst("files/" + targetFile, std::ios::app);
        if (!dst.is_open()) {
            std::cout << "Failed to open file: " << targetFile << std::endl;
            return;
        }
        dst << content << std::endl;
    }
    else {
        // Simple echo - check if quoted
        if (args[0].size() >= 2 && args[0][0] == '"') {
            // Process quoted text
            std::string content = args[0].substr(1);  // Remove opening quote
            
            // Search for closing quote
            bool foundClosingQuote = false;
            if (content.back() == '"') {
                content.pop_back();
                foundClosingQuote = true;
            } else {
                for (size_t i = 1; i < args.size(); ++i) {
                    content += " " + args[i];
                    if (args[i].back() == '"') {
                        content.pop_back();  // Remove closing quote
                        foundClosingQuote = true;
                        break;
                    }
                }
            }
            
            if (foundClosingQuote) {
                std::cout << content << std::endl;
            } else {
                std::cout << "Error: Unclosed quote in text." << std::endl;
            }
        } else {
            // Try as filename
            std::string filename = args[0];
            bool isFile = false;
            
            for (Inode& node : inodes) {
                if (node.filename == filename) {
                    isFile = true;
                    std::ifstream file("files/" + filename);
                    if (!file.is_open()) {
                        std::cout << "Failed to open file: " << filename << std::endl;
                        return;
                    }

                    std::string line;
                    while (std::getline(file, line)) {
                        std::cout << line << std::endl;
                    }
                    break;
                }
            }
            
            if (!isFile) {
                // Provide error for non-quoted text
                std::string content;
                for (size_t i = 0; i < args.size(); ++i) {
                    if (i > 0) content += " ";
                    content += args[i];
                }
                std::cout << "Text must be in quotes. Did you mean: echo \"" << content << "\"?" << std::endl;
            }
        }
    }
}

void CommandManager::parseAndExecute(const std::string &input)
{
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) return;

    const std::string command = tokens[0];
    tokens.erase(tokens.begin()); // remove the command from args

    
    if (command == "create") {
        handleCreate(tokens);
    } else if (command == "rm") {
        handleDelete(tokens);
    } else if (command == "list") {
        handleList(tokens);
    } else if (command == "chmod") {
        handleChmod(tokens);
    } else if (command == "echo") {
        handleEcho(tokens);
    } else if (command == "e" || command == "exit") {
        handleExit(tokens);
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}

void CommandManager::updateConfigFile()
{
    std::ofstream configFile("config/config.txt", std::ios::trunc);  // Open config file for writing (overwrite mode)
    if (!configFile.is_open()) {
        std::cout << "Error: Failed to open config file for writing!" << std::endl;
        return;
    }

    for (const auto& inode : inodes) {
        // Write file header (BEGIN filename)
        configFile << "BEGIN " << inode.filename << std::endl;

        // Write user permissions for each file
        for (const auto& userPermissions : inode.userPermissions) {
            const std::string& user = userPermissions.first;
            const auto& permissions = userPermissions.second;
            
            // Write permissions in format: <user> <read> <write> <execute>
            configFile << user << " " 
                       << (permissions[0] ? 1 : 0) << " "
                       << (permissions[1] ? 1 : 0) << " "
                       << (permissions[2] ? 1 : 0) << std::endl;
        }

        // Write file footer (END)
        configFile << "END" << std::endl;
    }
}
