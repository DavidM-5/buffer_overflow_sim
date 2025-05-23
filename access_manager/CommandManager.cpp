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

Inode *CommandManager::findInode(std::unordered_map<std::string, Inode> &inodes, const std::string &filename)
{
    auto it = inodes.find(filename);
    if (it != inodes.end())
        return &it->second;

    return nullptr;
}

void CommandManager::handleCreate(const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        std::cout << "Usage: create <filename>" << std::endl;
        return;
    }

    const std::string& filename = args[0];
    std::string path = PATH_ROOT + "/files/" + filename;

    namespace fs = std::filesystem;

    // Check if files folder exists
    if (!fs::exists("files")) {
        if (!fs::create_directory("files")) {
            std::cout << "Failed to create 'files' directory!" << std::endl;
            logAction("Failed to create 'files' directory");
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
        logAction("Failed to create '" + filename + "' file");
        return;
    }

    file.close();

    Inode newInode;
    newInode.filename = filename;
    
    inodes[filename] = newInode;
    updateConfigFile();

    logAction("Created file '" + filename + "'");
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
    
    Inode* inode = findInode(inodes, file);

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
    logAction("Chmod success: set permissions '" + permissions + "' for user '" + user + "' on file '" + file + "'");
}

void CommandManager::handleDelete(const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        std::cout << "Usage: delete <filename>" << std::endl;
        return;
    }

    std::string filename = args[0];

    // Find inode by key
    auto it = inodes.find(filename);

    if (it == inodes.end()) {
        std::cout << "No such file: " << filename << std::endl;
        return;
    }

    // Try to delete the file from the filesystem
    std::string path = PATH_ROOT + "/files/" + filename;
    if (std::remove(path.c_str()) != 0) {
        std::cout << "Failed to delete file from disk." << std::endl;
        logAction("Failed to delete file '" + filename + "'");
        return;
    }

    // Remove inode from memory
    inodes.erase(it);

    // Update the config file
    updateConfigFile();
    logAction("Deleted file '" + filename + "'");
}

void CommandManager::handleList(const std::vector<std::string> &args)
{
    for (const auto& [filename, inode] : inodes) {
        std::cout << filename << std::endl;
        
        for (const auto& pair : inode.userPermissions) {
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
        std::cout << "  echo \"<text>\"\n";
        std::cout << "  echo <user> <file>\n";
        std::cout << "  echo <user> \"<text>\" >> <file>\n";
        std::cout << "  echo <user> <file1> >> <file2>\n";
        return;
    }

    // ——— 1) echo <user> <file> : permission-checked read
    if (args.size() == 2
        && args[0].find_first_of("\"") == std::string::npos
        && args[1].find_first_of("\"") == std::string::npos)
    {
        const std::string &user     = args[0];
        const std::string &filename = args[1];

        // locate inode
        Inode* inode = findInode(inodes, filename);
        
        if (!inode) {
            std::cout << "No such file: " << filename << std::endl;
            logAction("Echo failed: read, file not found: " + filename);
            return;
        }

        // check READ permission
        if (!inode->userPermissions[user][READ]) {
            std::cout << "Permission denied: user '"
                      << user << "' cannot read " << filename << std::endl;
            logAction("Echo failed: read-permission denied for user '" + user + "' on '" + filename + "'");
            return;
        }

        // dump file
        std::ifstream file("files/" + filename);
        if (!file.is_open()) {
            std::cout << "Failed to open file: " << filename << std::endl;
            logAction("Echo failed: could not open '" + filename + "'");
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }
        logAction("Echo success: read file '" + filename + "' by user '" + user + "'");
        return;
    }

    // ——— 2) detect append-mode ("echo <user> ... >> <file>")
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
        // validate
        if (appendIndex < 2 || appendIndex + 1 >= (int)args.size()) {
            std::cout << "Invalid usage for append mode." << std::endl;
            logAction("Echo failed: invalid append-mode usage");
            return;
        }

        const std::string user       = args[0];
        const std::string targetFile = args[appendIndex + 1];
        std::string content;

        // quoted text?
        if (args[1].size() >= 2 && args[1][0] == '"') {
            content = args[1].substr(1);
            if (content.back() == '"') {
                content.pop_back();
            } else {
                for (int i = 2; i < appendIndex; ++i) {
                    content += " " + args[i];
                    if (args[i].back() == '"') {
                        content.pop_back();
                        break;
                    }
                }
            }
        } else {
            content = args[1];  // treat as filename
        }

        // locate destination inode
        Inode* destInode = findInode(inodes, targetFile);
        
        if (!destInode) {
            std::cout << "Destination file does not exist: "
                      << targetFile << std::endl;
            logAction("Echo failed: destination file not found: " + targetFile);
            return;
        }

        // check WRITE permission
        if (!destInode->userPermissions[user][WRITE]) {
            std::cout << "Permission denied: user '"
                      << user << "' cannot write to "
                      << targetFile << std::endl;
            logAction("Echo failed: write-permission denied for user '" + user + "' on '" + targetFile + "'");
            return;
        }

        // if unquoted, could be source file → append file contents
        if (args[1][0] != '"') {
            bool foundSrc = false;
            for (auto& [filename, node] : inodes) {
                if (node.filename == content) {
                    foundSrc = true;
                    if (!node.userPermissions[user][READ]) {
                        std::cout << "Permission denied: user '"
                                  << user << "' cannot read from "
                                  << content << std::endl;
                        logAction("Echo failed: read-permission denied for user '" + user + "' on source file '" + content + "'");
                        return;
                    }
                    std::ifstream src("files/" + content);
                    std::ofstream dst("files/" + targetFile,
                                      std::ios::app);
                    if (!src.is_open() || !dst.is_open()) {
                        std::cout << "Failed to open files." << std::endl;
                        logAction("Echo failed: unable to open source or destination for append");
                        return;
                    }
                    dst << src.rdbuf();
                    logAction("Echo success: appended content of '" + content + "' to '" + targetFile + "' by user '" + user + "'");
                    return;
                }
            }

            // source file not found in inodes
            std::cout << "No such source file: " << content << std::endl;
            logAction("Echo failed: source file not found: " + content);
            return;
        }

        // append literal text
        std::ofstream dst("files/" + targetFile, std::ios::app);
        if (!dst.is_open()) {
            std::cout << "Failed to open file: " << targetFile << std::endl;
            logAction("Echo failed: unable to open '" + targetFile + "' for append text");
            return;
        }
        dst << content << std::endl;
        logAction("Echo success: appended text to '" + targetFile + "' by user '" + user + "': \"" + content + "\"");
    }
    else {
        // ——— 3) non-append: quoted-text or fallback file dump
        if (args[0].size() >= 2 && args[0][0] == '"') {
            std::string content = args[0].substr(1);
            bool foundClosing = false;
            if (content.back() == '"') {
                content.pop_back();
                foundClosing = true;
            } else {
                for (size_t i = 1; i < args.size(); ++i) {
                    content += " " + args[i];
                    if (args[i].back() == '"') {
                        content.pop_back();
                        foundClosing = true;
                        break;
                    }
                }
            }
            if (!foundClosing) {
                std::cout << "Error: Unclosed quote in text" << std::endl;
                logAction("Echo failed: unclosed quote");
            } else {
                std::cout << content << std::endl;
                logAction("Echo success: printed text: \"" + content + "\"");
            }
        } else {
            std::cout << "Usage:\n";
            std::cout << "  echo \"<text>\"";
            std::cout << "  echo <user> <file>\n";
            std::cout << "  echo <user> \"<text>\" >> <file>\n";
            std::cout << "  echo <user> <file1> >> <file2>\n";
            return;
        }
    }
}

void CommandManager::handleHelp(const std::vector<std::string> &args)
{
    std::cout << "List of all the commands: "<< std::endl;
    std::cout << "create" << std::endl;
    std::cout << "chmod" << std::endl;
    std::cout << "delete" << std::endl;
    std::cout << "list" << std::endl;
    std::cout << "exit" << std::endl;
    std::cout << "echo" << std::endl;
    std::cout << "reset" << std::endl;
    std::cout << "help" << std::endl;
}

void CommandManager::handleReset(const std::vector<std::string>& args)
{
    resetInodeList();
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
    } else if (command == "reset") {
        handleReset(tokens);
    } else if (command == "help") {
        handleHelp(tokens); 
    } else if (command == "e" || command == "exit" || command == "q") {
        handleExit(tokens);
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}

void CommandManager::updateConfigFile()
{
    std::ofstream configFile(PATH_ROOT + "/config/config.txt", std::ios::trunc);  // Open config file for writing (overwrite mode)
    if (!configFile.is_open()) {
        std::cout << "Error: Failed to open config file for writing!" << std::endl;
        return;
    }

    for (const auto& [filename, inode] : inodes) {
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

void CommandManager::logAction(const std::string &action)
{
    std::ofstream log(PATH_ROOT + "/config/log.txt", std::ios::app);
    if (!log.is_open()) {
        std::cerr << "Failed to open log file!" << std::endl;
        return;
    }

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    log << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] " << action << std::endl;
}
