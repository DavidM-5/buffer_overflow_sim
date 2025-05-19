#include "PEApplication.h"

application::PEApplication::PEApplication() : m_window("Access Control Simulator", WINDOW_WIDTH, WINDOW_HEIGHT),
                                              m_mainPanel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {0x2D, 0x2D, 0x2D, 0xFF}),
                                              m_capabilityTable(50, 75, 750, 250, SDL_Color{255, 255, 255, 255}, 4, 4)
{
    initPanels();
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

    m_mainPanel.handleEvents(m_inputMngr);
}

void application::PEApplication::render()
{
    m_renderer.clear({0, 0, 0, 255});

    m_mainPanel.render(m_renderer);

    m_capabilityTable.render(m_renderer);

    m_renderer.present();
}

void application::PEApplication::initPanels()
{
    auto topBar = std::make_unique<application::Panel>(0, 0, WINDOW_WIDTH, 40, SDL_Color{0x1E, 0x1E, 0x1E, 0xFF});

    loadButtons(*topBar);

    // Bypass access manager
    auto bypassLabel = std::make_unique<application::TextLine>(85, 400, 400, 24, SDL_Color{255, 255, 255, 0}, false);
    bypassLabel->useFont("JetBrainsMono-Bold.ttf", 18);
    bypassLabel->appendText("Bypass access manager");

    auto inputLineUser = std::make_unique<application::InputLine>(100, 450, 400, 22, SDL_Color{255, 255, 255, 0}, "User: ");
    auto inputLineFrom = std::make_unique<application::InputLine>(100, 480, 400, 22, SDL_Color{255, 255, 255, 0}, "From: ");
    auto inputLineTo = std::make_unique<application::InputLine>(100, 510, 400, 22, SDL_Color{255, 255, 255, 0}, "To: ");

    auto btnSendBypass = std::make_unique<application::Button>(100, 560, 150, 30, SDL_Color{0x1B, 0x51, 0xCC, 0xFF}, "Send", vector2i{100, 560});
    btnSendBypass->onClick([this]() { 
                sendUserInputBypass();
            });


    auto resLine = std::make_unique<application::TextLine>(85, 600, 600, 22, SDL_Color{255, 255, 255, 0}, false);
    resLine->useFont("JetBrainsMono-Medium.ttf", 16);


    // fix the model
    auto fixLabel = std::make_unique<application::TextLine>(585, 400, 400, 24, SDL_Color{255, 255, 255, 0}, false);
    fixLabel->useFont("JetBrainsMono-Bold.ttf", 18);
    fixLabel->appendText("Fix access manager (R/W/RW)");

    auto fixInputLineUser = std::make_unique<application::InputLine>(600, 450, 400, 22, SDL_Color{255, 255, 255, 0}, "User: ");
    auto fixInputLineFile = std::make_unique<application::InputLine>(600, 480, 400, 22, SDL_Color{255, 255, 255, 0}, "File: ");
    auto fixInputLinePerm = std::make_unique<application::InputLine>(600, 510, 400, 22, SDL_Color{255, 255, 255, 0}, "Permissions: ");

    auto btnSendFix = std::make_unique<application::Button>(600, 560, 150, 30, SDL_Color{0x1B, 0x51, 0xCC, 0xFF}, "Send", vector2i{600, 560});
    btnSendFix->onClick([this]() { 
                sendUserInputFix();
            });
    
    auto btnReset = std::make_unique<application::Button>(800, 560, 150, 30, SDL_Color{0x1B, 0x51, 0xCC, 0xFF}, "Reset", vector2i{800, 560});
    btnReset->onClick([this]() { 
                resetModel();
            });

    auto resLineFix = std::make_unique<application::TextLine>(585, 630, 600, 22, SDL_Color{255, 255, 255, 0}, false);
    resLineFix->useFont("JetBrainsMono-Medium.ttf", 16);


    m_mainPanel.addWidget("Panel-top_bar", std::move(topBar));
    m_mainPanel.addWidget("Bypass_Label", std::move(bypassLabel));
    m_mainPanel.addWidget("Input_User", std::move(inputLineUser));
    m_mainPanel.addWidget("Input_From", std::move(inputLineFrom));
    m_mainPanel.addWidget("Input_To", std::move(inputLineTo));
    m_mainPanel.addWidget("Btn_SendBypass", std::move(btnSendBypass));
    m_mainPanel.addWidget("Result_Line", std::move(resLine));

    m_mainPanel.addWidget("Fix_Label", std::move(fixLabel));
    m_mainPanel.addWidget("Input_FixUser", std::move(fixInputLineUser));
    m_mainPanel.addWidget("Input_FixFile", std::move(fixInputLineFile));
    m_mainPanel.addWidget("Input_FixPerm", std::move(fixInputLinePerm));
    m_mainPanel.addWidget("Btn_SendFix", std::move(btnSendFix));
    m_mainPanel.addWidget("Btn_reset", std::move(btnReset));
    m_mainPanel.addWidget("Result_LineFix", std::move(resLineFix));
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
        btn->onClick([this, model]() { 
                m_accessManager = std::make_unique<ConsoleProccess>("./main", "./access_manager", model);

                loadCapabilityTable();
            });

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
    m_capabilityTable.resize(numFiles, numUsers+1);
    
    // Set the first cell (0,0) to be empty or label it as "User/File"
    m_capabilityTable.setText({0, 0}, "User/File");
    
    // Set file names in the top row
    for (int col = 0; col < numFiles; ++col) {
        m_capabilityTable.setText({col + 1, 0}, capabilities[col].filename);
    }
    
    // Set user names in the first column
    int row = 1;
    for (const auto& username : uniqueUsers) {
        m_capabilityTable.setText({0, row}, username);
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
            m_capabilityTable.setText({fileIdx + 1, row}, permStr);
            row++;
        }
    }

}

bool application::PEApplication::isDirectory(const std::string &path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;

    return S_ISDIR(statbuf.st_mode);
}

std::string application::PEApplication::normalize(const std::string &in)
{
    std::string tmp;
    tmp.reserve(in.size());
    for (char c : in) {
        if (!std::isspace(static_cast<unsigned char>(c)))
            tmp += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return tmp;
}

bool application::PEApplication::parseAccessString(const std::string &input, std::string &outPerms)
{
    // 1) strip spaces & lowercase
    std::string s = normalize(input);
    if (s.empty()) 
        return false;

    // 2) ensure every char is 'r' or 'w'
    bool hasR = false, hasW = false;
    for (char c : s) {
        if (c == 'r')      hasR = true;
        else if (c == 'w') hasW = true;
        else               return false;  // invalid char found
    }

    // 3) build the +r/-r +w/-w string
    outPerms.clear();
    outPerms += (hasR ? "+r" : "-r");
    outPerms += (hasW ? "+w" : "-w");
    outPerms += "--";   // execute bit always off

    return true;
}

void application::PEApplication::sendUserInputBypass()
{
    if (!m_accessManager)
        return;    

    application::Widget* widget = m_mainPanel.getWidget("Input_User");
    application::InputLine* inputLine = static_cast<application::InputLine*>(widget);
    std::string user = inputLine->getInput();

    widget = m_mainPanel.getWidget("Input_From");
    inputLine = static_cast<application::InputLine*>(widget);
    std::string FromFile = inputLine->getInput();

    widget = m_mainPanel.getWidget("Input_To");
    inputLine = static_cast<application::InputLine*>(widget);
    std::string ToFile = inputLine->getInput();
    
    widget = m_mainPanel.getWidget("Result_Line");
    application::TextLine* resultLine = static_cast<application::TextLine*>(widget);
    
    std::string command = "echo " + user + " " + FromFile + " >> " + ToFile;

    if (user.empty() || FromFile.empty() || ToFile.empty()) {
        resultLine->clear();
        resultLine->appendText("Must enter input.", true);
        return;
    }

    m_accessManager->sendInput(command);

    usleep(100000);
    std::string out = m_accessManager->getOutput();

    if (out.length() >= 2) {
        out.pop_back();
        out.pop_back();

        if (out.length() >= 1)
            out.pop_back();
    }

    resultLine->clear();
    if (!out.empty()) {
        resultLine->appendText(out, true);
    }
    else {
        std::string successOutput = "Written '" + FromFile + "' contents into '" + ToFile + "'";
        resultLine->appendText(successOutput, true);
    }
}

void application::PEApplication::sendUserInputFix()
{
    if (!m_accessManager)
        return;

    application::Widget* widget = m_mainPanel.getWidget("Input_FixUser");
    application::InputLine* inputLine = static_cast<application::InputLine*>(widget);
    std::string user = inputLine->getInput();

    widget = m_mainPanel.getWidget("Input_FixFile");
    inputLine = static_cast<application::InputLine*>(widget);
    std::string File = inputLine->getInput();

    widget = m_mainPanel.getWidget("Input_FixPerm");
    inputLine = static_cast<application::InputLine*>(widget);
    std::string Perm = inputLine->getInput();

    widget = m_mainPanel.getWidget("Result_LineFix");
    application::TextLine* resultLine = static_cast<application::TextLine*>(widget);

    if (user.empty() || File.empty() || Perm.empty()) {
        resultLine->clear();
        resultLine->appendText("Must enter input.", true);
        return;
    }

    std::string finalizedPerms;
    if (!parseAccessString(Perm, finalizedPerms)) {
        resultLine->clear();
        resultLine->appendText("Valid permissions: R/W/RW.", true);
        return;
    }

    std::string command = "chmod " + File + " " + user + " " + finalizedPerms;

    m_accessManager->sendInput(command);

    usleep(100000);
    std::string out = m_accessManager->getOutput();

    if (out.length() >= 2) {
        out.pop_back();
        out.pop_back();

        if (out.length() >= 1)
            out.pop_back();
    }

    resultLine->clear();
    if (out.empty()) {
        loadCapabilityTable();
        return;
    }
}

void application::PEApplication::resetModel()
{
    if (!m_accessManager)
        return;

    m_accessManager->sendInput("reset");

    loadCapabilityTable();
}
