#include "PEApplication.h"

application::PEApplication::PEApplication() : m_window("Privilege Escalation Simulator", WINDOW_WIDTH, WINDOW_HEIGHT),
                                              m_mainPanel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {0x2D, 0x2D, 0x2D, 0xFF}),
                                              testTable(50, 150, 750, 150, SDL_Color{255, 255, 255, 255}, 4, 4),
                                              iline(100, 550, 600)
{
    initPanels();

    testTable.setText({1, 0}, "1, 0");
    testTable.setText({2, 0}, "2, 0");
    testTable.setText({3, 0}, "3, 0");
    testTable.setText({0, 1}, "0, 1");
    testTable.setText({0, 2}, "0, 2");
    testTable.setText({0, 3}, "0, 3");

    runBLP();
    loadCapabilityTable();
}

bool application::PEApplication::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

    return true;
}

void application::PEApplication::run()
{
    SDL_Event event;

    int fps; // Desired FPS

    // Get the screen refresh rate
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
        fps = displayMode.refresh_rate / 1.5;
    }
    else {
        fps = 60;
    }

    
    const int frameDelay = 1000 / fps; // Frame duration in ms
    Uint32 frameStart;
    Uint32 frameTime;


    while (m_window.isRunning())
    {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            m_window.handleEvents(event);

            PEApplication::update(event);
        }

        PEApplication::render();

        // Frame timing and capping
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void application::PEApplication::update(SDL_Event &event)
{
    m_inputMngr.update(event);

    iline.handleEvents(m_inputMngr);

    m_mainPanel.handleEvents(m_inputMngr);
}

void application::PEApplication::render()
{
    m_renderer.clear({0, 0, 0, 255});

    m_mainPanel.render(m_renderer);

    testTable.render(m_renderer);

    iline.render(m_renderer);

    m_renderer.present();
}

void application::PEApplication::initPanels()
{
    auto topBar = std::make_unique<application::Panel>(0, 0, WINDOW_WIDTH, 40, SDL_Color{0x1E, 0x1E, 0x1E, 0xFF});

    loadButtons(*topBar);

    m_mainPanel.addWidget("Panel-top_bar", std::move(topBar));
}

void application::PEApplication::loadButtons(application::Panel &parentPanel)
{
    std::vector<std::string> models;

    std::string path = "./access_manager/models";
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        std::cerr << "Failed to open directory: " << path << '\n';
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;

        // Skip "." and ".."
        if (name == "." || name == "..")
            continue;

        std::string fullPath = path + "/" + name;
        if (isDirectory(fullPath)) {
            models.push_back(name);
        }
    }

    closedir(dir);

    for (int i = 0; i < models.size(); i++) {
        auto btn = std::make_unique<application::Button>(10 + 160 * i, 5, 150, 30, SDL_Color{0x1B, 0x51, 0xCC, 0xFF}, models[i], vector2i{15 + 160 * i, 5}, 140);

        std::string model = models[i];
        btn->onClick([this, model]() { m_accessManager = std::make_unique<ConsoleProccess>("./main", "./access_manager", model); });

        std::string btnName = "Button-" + model;
        parentPanel.addWidget(btnName, std::move(btn));
    }
}

void application::PEApplication::loadCapabilityTable()
{
    if (!m_accessManager)
        return;

    usleep(100000);
    m_accessManager->getOutput();

    m_accessManager->sendInput("list");

    usleep(100000);
    std::string input = m_accessManager->getOutput();

    // std::cout << input << std::endl;


    // Parse the capability data
    std::vector<FileCapability> capabilities;
    std::istringstream stream(input);
    std::string line;
    
    FileCapability* currentFile = nullptr;
    
    while (std::getline(stream, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Check if this is a user permission line
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            if (!currentFile) {
                // If we encounter a user without a file, skip
                continue;
            }
            
            // Parse user and permissions
            std::string username = line.substr(0, colonPos);
            
            // Trim whitespace from username
            username.erase(0, username.find_first_not_of(" \t"));
            username.erase(username.find_last_not_of(" \t") + 1);
            
            // Extract permissions
            std::istringstream permStream(line.substr(colonPos + 1));
            int read = 0, write = 0, execute = 0;
            permStream >> read >> write >> execute;
            
            // Add user to current file
            std::array<bool, 2> permissions = {read == 1, write == 1};
            currentFile->users.emplace_back(username, permissions);
        } else {
            // This is a filename
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            capabilities.emplace_back(line);
            currentFile = &capabilities.back();
        }
    }
    
    // Collect unique usernames across all files
    std::set<std::string> uniqueUsers;
    for (const auto& file : capabilities) {
        for (const auto& [username, _] : file.users) {
            uniqueUsers.insert(username);
        }
    }
    
    // Calculate table dimensions
    int numFiles = capabilities.size();
    int numUsers = uniqueUsers.size();
    
    // Resize table (numFiles+1 columns, numUsers+1 rows)
    // First column for usernames, first row for filenames
    // Note: I'm using testTable as in your example
    
    // Set the first cell (0,0) to be empty or label it as "User/File"
    testTable.setText({0, 0}, "User/File");
    
    // Set file names in the top row
    for (int col = 0; col < numFiles; ++col) {
        testTable.setText({col + 1, 0}, capabilities[col].filename);
    }
    
    // Set user names in the first column
    int row = 1;
    for (const auto& username : uniqueUsers) {
        testTable.setText({0, row}, username);
        row++;
    }
    
    // Fill in permissions for each user-file combination
    for (int fileIdx = 0; fileIdx < numFiles; ++fileIdx) {
        const auto& file = capabilities[fileIdx];
        
        row = 1; // Start from the first row after headers
        for (const auto& username : uniqueUsers) {
            // Find if this user has permissions for this file
            bool permissionFound = false;
            std::string permStr = "--"; // Default: no permissions
            
            for (const auto& [fileUser, permissions] : file.users) {
                if (fileUser == username) {
                    permStr.clear();
                    permStr += permissions[0] ? "R" : "-";
                    permStr += permissions[1] ? "W" : "-";
                    permissionFound = true;
                    break;
                }
            }
            
            // Set the permission text
            testTable.setText({fileIdx + 1, row}, permStr);
            row++;
        }
    }

}

void application::PEApplication::runBLP() {
    m_accessManager = std::make_unique<ConsoleProccess>("./main", "./access_manager", "BLP");
}

bool application::PEApplication::isDirectory(const std::string &path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;

    return S_ISDIR(statbuf.st_mode);
}
