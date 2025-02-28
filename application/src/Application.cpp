#include "Application.h"

/*
m_borderHorizontalCenterPanelTop(false,
                                m_mainPanel.getPosition().x + m_mainPanel.getWidth() / 3 - 50,
                                1.8 * m_mainPanel.getHeight() / 3 - m_borderWidth,
                                m_mainPanel.getWidth() / 3 - m_borderWidth + 50,
                                m_borderWidth,
                                SDL_Color{255, 120, 255, 120}),
m_borderHorizontalCenterPanelBottom(false,
                                    m_mainPanel.getPosition().x + m_mainPanel.getWidth() / 3 - 50,
                                    1.8 * m_mainPanel.getHeight() / 3 + 50,
                                    m_mainPanel.getWidth() / 3 - m_borderWidth + 50,
                                    m_borderWidth,
                                    SDL_Color{255, 120, 255, 120}),
*/

/*

Tasks:
1) Put a breakpoint on a suspected function
    .
    .
    .

*/

// TEMPORARY \/\/\/
void printHello() {
    std::cout << "Hello, World!" << std::endl;
}
// TEMPORARY /\/\/\

application::Application::Application() : m_window("Buffer Overflow Simulator", WINDOW_WIDTH, WINDOW_HEIGHT),
                                          m_bordVert(true, 2*WINDOW_WIDTH/3-105, 0, 10, WINDOW_HEIGHT),
                                          m_bordHor(false, 0, 2*WINDOW_HEIGHT/3-5, 2*WINDOW_WIDTH/3-100, 10),
                                          m_borderVerticalLeft(true, 
                                                               m_mainPanel.getPosition().x + m_mainPanel.getWidth() / 3 - m_borderWidth - 50,
                                                               m_mainPanel.getPosition().y + m_borderWidth,
                                                               m_borderWidth,
                                                               m_mainPanel.getHeight() - m_borderWidth * 2,
                                                               SDL_Color{0xFF, 0xFF, 0xFF, 0x00}),
                                          m_borderVerticalRight(true,
                                                                m_mainPanel.getPosition().x + 2 * m_mainPanel.getWidth() / 3 - m_borderWidth,
                                                                m_mainPanel.getPosition().y + m_borderWidth,
                                                                m_borderWidth,
                                                                m_mainPanel.getHeight() - m_borderWidth * 2,
                                                                SDL_Color{0xFF, 0xFF, 0xFF, 0x00}),
                                          m_mainPanel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {0x2D, 0x2D, 0x2D, 0xFF}),
                                          m_borderWidth(10), m_innerBorderWidth(10), m_latestBreakpointLine(0),
                                          m_requiredBreakpoints({82})
{
    initFormatMap();
}

bool application::Application::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

    initPanels();

    if (!loadTargetSourceCodeFromPath("targets/src/Task_one/vulnerable_system/main.c")) {
        std::cerr << "Failed to load source code." << std::endl;
        return false;
    }

    return true;
}

// temp
std::string hexToString(uint64_t hexValue) {
    std::stringstream ss;
    ss << std::hex << hexValue;  // Convert to hex
    return ss.str();
}
// temp /\/\

void application::Application::run()
{
    /*
    if (!compileFile("targets/src/Task_one/compile_command_linux.txt")) {
        std::cerr << "Failed to compile target code." << std::endl;
        return;
    }*/
    gdb = std::make_shared<GDBController>("./t1", "./targets/compiled");

    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-center_bottom");
    application::Widget* w = parentWidget->getWidget("Console-console");
    application::Console* con = static_cast<application::Console*>(w);

    con->attachGDB(gdb);

    gdb->sendCommand("break gets");
    showFunctionsAddresses();

    // gdb->sendCommand("continue");
    // gdb->sendCommand("continue");
    
    // Read program output
    // std::string output = gdb->getTargetOutput();
    // std::cout << "Target output: \n" << output << std::endl;

    // gdb->sendTargetInput("1");

    // output = gdb->getTargetOutput();
    // std::cout << "Target output: \n" << output << std::endl;

    

    /*
    gdb->sendCommand("break main");

    std::string rawOutput = gdb->readOutput();
    std::string formattedOutput = gdb->formatGDBOutput(rawOutput);
    std::cout << "\n\nGDB Output 1: \n" << formattedOutput << std::endl;
    std::cout << "\n\n\n\n" << std::endl;
    */

    /*
    gdb->sendCommand("run");

    // Read GDB output
    rawOutput = gdb->readOutput();
    formattedOutput = gdb->formatGDBOutput(rawOutput);
    std::cout << "GDB Output 2: " << formattedOutput << std::endl;
    */

    SDL_Event event;

    int fps; // Desired FPS

    // Get the screen refresh rate
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
        fps = displayMode.refresh_rate;
    }
    else {
        fps = 60;
    }

    
    const int frameDelay = 1000 / fps; // Frame duration in ms
    Uint32 frameStart;
    Uint32 frameTime;

    
    // showFunctionsAddresses is non-static and requires an instance of the class to be called.
    // Use a lambda to pass the instance to the thread.
    /*std::thread setupThread([this]() {
        showFunctionsAddresses();  // Accessing 'this' to call a member function
    });

    // Detach the thread, allowing it to run independently
    setupThread.detach();*/


    while (m_window.isRunning())
    {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            m_window.handleEvents(event);
            
            if (!m_inputMngr.getPressedKey().empty()) {
                // temporary \/\/\/
                if (m_inputMngr.getPressedKey() == "k") {
                    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-right_bottom");
                    application::Widget* w = parentWidget->getWidget("StackVisualizer-stack_view");
                    application::StackVisualizer* stackV = static_cast<application::StackVisualizer*>(w);
                    // ABCDEFAB  CDEFABCD
                    stackV->push(hexToString(count++));
                }
                if (m_inputMngr.getPressedKey() == "i") {
                    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-right_bottom");
                    application::Widget* w = parentWidget->getWidget("StackVisualizer-stack_view");
                    application::StackVisualizer* stackV = static_cast<application::StackVisualizer*>(w);
                    stackV->pop();
                } 
                
                if (m_inputMngr.getPressedKey() == "m") {
                    // std::cout << "Memory dump:" << std::endl;
                    gdb->sendCommand("i b");

                    std::string rawOutput = gdb->getGdbOutput();

                    std::cout << rawOutput << std::endl;
                }

                if (m_inputMngr.getPressedKey() == "b") {
                    std::cout << "breakpoint: " << gdb->isAtBreakpoint() << std::endl;
                }

                if (m_inputMngr.getPressedKey() == "d") {
                    std::cout << "-> Memory dump" << std::endl;

                    memoryDumpToStackView("$rbp", 13);

                    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-right_bottom");
                    application::Widget* w = parentWidget->getWidget("StackVisualizer-stack_view");
                    application::StackVisualizer* stackV = static_cast<application::StackVisualizer*>(w);

                    stackV->selectSlot(1);
                }

                if (m_inputMngr.getPressedKey() == "p") {
                    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-center_bottom");
                    application::Widget* w = parentWidget->getWidget("Console-console");
                    application::Console* con = static_cast<application::Console*>(w);

                    con->printToConsole("abcdefg");
                    con->printToConsole("012345678901234567890123456789012345678901234567890123456789");
                    con->printToConsole("Hello, \n World!");

                }
                // temporary end /\/\/\.
            }

            Application::update(event);
        }

        Application::render();

        // Frame timing and capping
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void application::Application::update(SDL_Event& event)
{
    m_inputMngr.update(event);

    m_mainPanel.handleEvents(m_inputMngr);

    m_borderVerticalLeft.handleEvents(m_inputMngr);
    m_borderVerticalRight.handleEvents(m_inputMngr);

    if (m_latestBreakpointLine > 0) {
        if (m_userBreakpoints.find(m_latestBreakpointLine) != m_userBreakpoints.end()) {
            m_userBreakpoints.erase(m_latestBreakpointLine);

            std::ostringstream command;

            command << "info line targets/src/Task_one/vulnerable_system/main.c:" << m_latestBreakpointLine;
            gdb->sendCommand(command.str());

            // std::string rawOutput = gdb->readOutput();
            std::string formattedOutput = gdb->getGdbOutput();

            if (formattedOutput.find("but contains no code.") == std::string::npos) {
                std::ostringstream command2;

                command2 << "clear " << m_latestBreakpointLine;
                gdb->sendCommand(command2.str());

                std::cout << "Command: " << command2.str() << std::endl;
            }
        }
        else {
            m_userBreakpoints.insert(m_latestBreakpointLine);

            std::ostringstream command;

            command << "info line targets/src/Task_one/vulnerable_system/main.c:" << m_latestBreakpointLine;
            gdb->sendCommand(command.str());

            // std::string rawOutput = gdb->readOutput();
            std::string formattedOutput = gdb->getGdbOutput();

            if (formattedOutput.find("but contains no code.") == std::string::npos) {
                std::ostringstream command2;
                command2 << "break " << m_latestBreakpointLine;
                gdb->sendCommand(command2.str());
                
                std::cout << "Command: " << command2.str() << std::endl;
            }
        }

        // std::string rawOutput = gdb->readOutput();
        std::string formattedOutput = gdb->getGdbOutput();

        std::cout << formattedOutput << std::endl;

        m_latestBreakpointLine = 0;
    }

}

void application::Application::render()
{
    m_renderer.clear({0, 0, 0, 255});

    m_mainPanel.render(m_renderer);
    
    m_borderVerticalLeft.render(m_renderer);
    m_borderVerticalRight.render(m_renderer);

    m_renderer.drawRect({630, 874, 200, 30}, {255,255,255,255});

    m_renderer.present();
}

void application::Application::initFormatMap()
{
    formatMap["#include"] = {255, 123, 23, 255};  // Light Orange
    formatMap["#ifdef"] = {255, 123, 23, 255};  // Light Orange
    formatMap["#else"] = {255, 123, 23, 255};  // Light Orange
    formatMap["#endif"] = {255, 123, 23, 255};  // Light Orange
    formatMap["while"] = {123, 245, 123, 255};   // Light Green
    formatMap["void"] = {0, 192, 255, 255};      // Sky Blue
    formatMap["{"] = {163, 127, 54, 255};     // Olive Green
    formatMap["}"] = {163, 127, 54, 255};     // Olive Green
    formatMap[":"] = {163, 127, 54, 255};     // Olive Green
    formatMap["int"] = {255, 215, 0, 255};       // Gold
    formatMap["return"] = {70, 130, 180, 255};   // Steel Blue
    formatMap["if"] = {50, 205, 50, 255};        // Lime Green
    formatMap["else"] = {50, 205, 50, 255};      // Lime Green
    formatMap["for"] = {50, 205, 50, 255};       // Lime Green
    formatMap["break"] = {220, 20, 60, 255};     // Crimson
    formatMap["continue"] = {220, 20, 60, 255};  // Crimson
    formatMap["//"] = { 128, 128, 128, 255 }; // Grey
    formatMap["sizeof"] = {255, 215, 0, 255};    // Gold
    formatMap["switch"] = {50, 205, 50, 255};    // Lime Green
    formatMap["case"] = {0, 123, 255, 255};      // Dodger Blue
    formatMap["default:"] = {0, 192, 255, 255};      // Sky Blue
    formatMap["system"] = {50, 205, 50, 255};      // Lime Green
    formatMap["get"] = {0, 102, 204, 255};       // Dodger Blue
    formatMap["gtes"] = {0, 102, 204, 255};      // Dodger Blue
    formatMap["printf"] = {255, 69, 0, 255};     // Red-Orange
    formatMap["scanf"] = {255, 69, 0, 255};      // Red-Orange
    formatMap["malloc"] = {255, 69, 0, 255};     // Red-Orange
    formatMap["free"] = {255, 69, 0, 255};       // Red-Orange
    formatMap["char"] = {255, 215, 0, 255};      // Gold
    formatMap["\""] = {85, 207, 85, 255};      // Green
    formatMap["\'"] = {85, 207, 85, 255};      // Green
    formatMap["]"] = {255, 215, 0, 255};         // Gold
    formatMap["["] = {255, 215, 0, 255};         // Gold
    formatMap["("] = {255, 215, 0, 255};         // Gold
    formatMap[")"] = {255, 215, 0, 255};         // Gold
    formatMap["<"] = {255, 215, 0, 255};         // Gold
    formatMap[">"] = {255, 215, 0, 255};         // Gold
    formatMap["float"] = {255, 215, 0, 255};     // Gold
    formatMap["double"] = {255, 215, 0, 255};    // Gold
    formatMap["long"] = {255, 215, 0, 255};      // Gold
    formatMap["const"] = {173, 216, 230, 255};     // Light Blue
    formatMap["static"] = {0, 128, 128, 255};    // Teal
    formatMap["extern"] = {0, 128, 128, 255};    // Teal
    formatMap["volatile"] = {0, 128, 128, 255};  // Teal
    formatMap["sizeof"] = {255, 215, 0, 255};    // Gold
    formatMap["typedef"] = {0, 128, 128, 255};   // Teal
    formatMap["struct"] = {67, 143, 236, 255};     // Blue
    formatMap["bool"] = {146, 68, 247, 255};       // Purple
    formatMap["="] = {219, 6, 136, 255};       // Rose
    formatMap["=="] = {219, 6, 136, 255};       // Rose
    formatMap["!="] = {219, 6, 136, 255};       // Rose
    formatMap["+"] = {219, 6, 136, 255};        // Rose
    formatMap["-"] = {219, 6, 136, 255};        // Rose
    formatMap["&&"] = {219, 6, 136, 255};        // Rose
    formatMap["||"] = {219, 6, 136, 255};        // Rose
    formatMap["operator"] = {219, 6, 136, 255};        // Rose  +, -, =, !, |, &
    formatMap["function"] = {80, 130, 240, 255}; // Orange
}

void application::Application::initPanels()
{
    initLeftPanels();
    initCenterPanels();
    initRightPanels();
}


void application::Application::initLeftPanels()
{
    const int totalWidth = m_mainPanel.getWidth();
    const int totalHeight = m_mainPanel.getHeight();

    auto leftPanel = std::make_unique<application::Panel>(
        m_mainPanel.getPosition().x + m_borderWidth, m_mainPanel.getPosition().y + m_borderWidth,
        totalWidth / 3 - m_borderWidth * 2 - 50, totalHeight - m_borderWidth * 2,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    leftPanel->addLeftTopBorder(m_borderVerticalLeft);

    auto label = std::make_unique<application::TextLine>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        leftPanel->getWidth() - m_innerBorderWidth * 2, 22,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xCF}
    );

    label->useFont("JetBrainsMono-Bold.ttf", 18);
    label->appendText("Tasks", true);

    leftPanel->addWidget("Label", std::move(label));

    // ===========================
    // ===========================
    m_mainPanel.addWidget("Panel-left", std::move(leftPanel));
}

void application::Application::initCenterPanels()
{
    const int totalWidth = m_mainPanel.getWidth();
    const int totalHeight = m_mainPanel.getHeight();

    auto centerTopPanel = std::make_unique<application::Panel>(
        m_mainPanel.getPosition().x + totalWidth / 3 - 50, m_mainPanel.getPosition().y + m_borderWidth,
        totalWidth / 3 - m_borderWidth + 50, 1.8 * totalHeight / 3 - m_borderWidth * 2,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    centerTopPanel->addRightBottomBorder(m_borderVerticalLeft);
    centerTopPanel->addLeftTopBorder(m_borderVerticalRight);

    auto label = std::make_unique<application::TextLine>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        centerTopPanel->getWidth() - m_innerBorderWidth * 2, 22,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xCF}
    );
    
    label->useFont("JetBrainsMono-Bold.ttf", 18);
    label->appendText("Source Code", true);
    
    auto sourceCode = std::make_unique<application::TextBlock>(
        0 + m_innerBorderWidth, label->getPosition().y + label->getHeight() + 15,
        label->getWidth(), centerTopPanel->getHeight() - label->getPosition().y - label->getHeight() - 25 - m_innerBorderWidth,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xFF},
        &m_latestBreakpointLine
    );

    centerTopPanel->addWidget("Label", std::move(label));
    centerTopPanel->addWidget("TextBlock-Source_code", std::move(sourceCode));

    // ===========================
    // ===========================
    auto centerMiddlePanel = std::make_unique<application::Panel>(
        centerTopPanel->getPosition().x, centerTopPanel->getPosition().y + centerTopPanel->getHeight() + m_borderWidth,
        centerTopPanel->getWidth(), 50,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    centerMiddlePanel->addRightBottomBorder(m_borderVerticalLeft);
    centerMiddlePanel->addLeftTopBorder(m_borderVerticalRight);

    auto continueBtn = std::make_unique<application::Button>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        150, centerMiddlePanel->getHeight() - m_innerBorderWidth * 2,
        SDL_Color{0x1B, 0x51, 0xCC, 0xFF},
        "Continue",
        vector2i{centerMiddlePanel->getPosition().x + m_innerBorderWidth, centerMiddlePanel->getPosition().y + m_innerBorderWidth}
    );

    centerMiddlePanel->addWidget("Buttom-continue_btn", std::move(continueBtn));

    // ===========================
    // ===========================
    auto centerBottomPanel = std::make_unique<application::Panel>(
        centerMiddlePanel->getPosition().x, centerMiddlePanel->getPosition().y + centerMiddlePanel->getHeight() + m_borderWidth,
        centerMiddlePanel->getWidth(), totalHeight - centerTopPanel->getHeight() - centerMiddlePanel->getHeight() - m_borderWidth * 4,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    centerBottomPanel->addRightBottomBorder(m_borderVerticalLeft);
    centerBottomPanel->addLeftTopBorder(m_borderVerticalRight);

    label = std::make_unique<application::TextLine>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        centerBottomPanel->getWidth() - m_innerBorderWidth * 2, 22,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xCF}
    );
    
    label->useFont("JetBrainsMono-Bold.ttf", 18);
    label->appendText("Console", true);

    auto console = std::make_unique<application::Console>(
        0 + m_innerBorderWidth, label->getPosition().y + label->getHeight() + 10,
        label->getWidth(), centerBottomPanel->getHeight() - label->getPosition().y - label->getHeight() - m_innerBorderWidth * 2,
        SDL_Color{0x18, 0x18, 0x18, 0xFF}
    );

    centerBottomPanel->addWidget("Label", std::move(label));
    centerBottomPanel->addWidget("Console-console", std::move(console));

    // ===========================
    // ===========================
    m_mainPanel.addWidget("Panel-center_top", std::move(centerTopPanel));
    m_mainPanel.addWidget("Panel-center_middle", std::move(centerMiddlePanel));
    m_mainPanel.addWidget("Panel-center_bottom", std::move(centerBottomPanel));
}

void application::Application::initRightPanels()
{
    const int totalWidth = m_mainPanel.getWidth();
    const int totalHeight = m_mainPanel.getHeight();

    auto rightTopPanel = std::make_unique<application::Panel>(
        m_mainPanel.getPosition().x + 2 * totalWidth / 3, m_mainPanel.getPosition().y + m_borderWidth,
        totalWidth / 3 - m_borderWidth, totalHeight / 3 - m_borderWidth * 2,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    rightTopPanel->addRightBottomBorder(m_borderVerticalRight);

    auto label = std::make_unique<application::TextLine>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        rightTopPanel->getWidth() - m_innerBorderWidth * 2, 22,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xCF}
    );

    auto functionsAddresses = std::make_unique<application::TextBlock>(
        0 + m_innerBorderWidth + 15, label->getPosition().y + label->getHeight() + 10,
        label->getWidth() - 15, rightTopPanel->getHeight() - label->getPosition().y - label->getHeight() - m_innerBorderWidth * 2,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xFF},
        nullptr,
        0
    );
    
    label->useFont("JetBrainsMono-Bold.ttf", 18);
    label->appendText("Memory Addresses", true);

    rightTopPanel->addWidget("Label", std::move(label));
    rightTopPanel->addWidget("TextBlock-functions_addresses", std::move(functionsAddresses));

    // ===========================
    // ===========================
    auto rightBottomPanel = std::make_unique<application::Panel>(
        rightTopPanel->getPosition().x, rightTopPanel->getPosition().y + rightTopPanel->getHeight() + m_borderWidth,
        rightTopPanel->getWidth(), 2 * totalHeight / 3 - m_borderWidth,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    rightBottomPanel->addRightBottomBorder(m_borderVerticalRight);

    label = std::make_unique<application::TextLine>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        rightBottomPanel->getWidth() - m_innerBorderWidth * 2, 22,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xCF}
    );
    
    auto stackView = std::make_unique<application::StackVisualizer>(
        rightBottomPanel->getWidth() / 2 - 200 / 2, label->getPosition().y + 45,
        200, rightBottomPanel->getHeight() - label->getPosition().y - label->getHeight() - 25 - m_innerBorderWidth,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xDF},
        14, "JetBrainsMono-Bold.ttf", 16
    );

    label->useFont("JetBrainsMono-Bold.ttf", 18);
    label->appendText("Stack View", true);

    rightBottomPanel->addWidget("Label", std::move(label));
    rightBottomPanel->addWidget("StackVisualizer-stack_view", std::move(stackView));

    // ===========================
    // ===========================
    m_mainPanel.addWidget("Panel-right_top", std::move(rightTopPanel));
    m_mainPanel.addWidget("Panel-right_bottom", std::move(rightBottomPanel));
}


void application::Application::readFileToString(const std::string &filepath, std::string& dstString)
{
    std::ifstream file(filepath); // Open the file

    if (!file.is_open()) {
        dstString = "";
        return;
    }
    
    // Use a stringstream to read the file's contents into a string
    std::ostringstream ss;
    ss << file.rdbuf(); // Read the whole file into the stringstream
    dstString = ss.str(); // Convert the stringstream into a string

    // Close the file
    file.close();
}

bool application::Application::loadTargetSourceCodeFromPath(const std::string &filepath)
{
    std::string fileContents;
    readFileToString(filepath, fileContents);
    
    // Set the text to the source code widget
    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-center_top");
    if (parentWidget == nullptr)
        return false;
    
    application::Widget* w = parentWidget->getWidget("TextBlock-Source_code");
    if (w == nullptr)
        return false;
    
    application::TextBlock* textBlock = static_cast<application::TextBlock*>(w);

    textBlock->setText(fileContents, true);
    textBlock->setColorFormat(formatMap);

    return true;
}

std::set<std::string> application::Application::extractFunctionNamesFromFile(const std::string &filePath)
{
    std::set<std::string> functionNames;
    std::ifstream file(filePath);
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Regex to match function definitions with complex return types
    std::regex functionRegex(R"(([\w\s\*]+)\s+(\w+)\s*\([^)]*\)\s*\{)");

    std::smatch matches;
    std::string::const_iterator searchStart(fileContent.cbegin());

    while (std::regex_search(searchStart, fileContent.cend(), matches, functionRegex)) {
        std::string potentialFunctionName = matches[2];

        // Check if the matched name is not a control statement (e.g., if, while, for)
        if (potentialFunctionName != "if" && potentialFunctionName != "while" && potentialFunctionName != "for" &&
            potentialFunctionName != "else" && potentialFunctionName != "switch") {
            functionNames.insert(potentialFunctionName);
        }

        searchStart = matches.suffix().first;
    }

    return functionNames;
}

bool application::Application::compileFile(const std::string& commandPath)
{
    std::string compileCommand;

    readFileToString(commandPath, compileCommand);

    int result = system(compileCommand.c_str());

    if (result == 0) {
        return true;
    }

    std::cerr << "Compilation failed with error code: " << result << std::endl;
    return false;
}

void application::Application::memoryDumpToStackView(const std::string &startAddr, int numOfAddresses)
{
    std::vector<std::string> addresses = gdb->getMemoryDump("$rbp", 11);

    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-right_bottom");
    application::Widget* w = parentWidget->getWidget("StackVisualizer-stack_view");
    application::StackVisualizer* stackV = static_cast<application::StackVisualizer*>(w);

    for (const std::string& addr : addresses) {
        stackV->push(addr);
    }
}

void application::Application::showFunctionsAddresses()
{
    std::set<std::string> functions = extractFunctionNamesFromFile("targets/src/Task_one/vulnerable_system/main.c");

    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-right_top");
    application::Widget* w = parentWidget->getWidget("TextBlock-functions_addresses");
    application::TextBlock* textBlock = static_cast<application::TextBlock*>(w);
    
    for (const std::string& funcName : functions) {
        std::string fullLine = funcName + " - " + gdb->getAddress(funcName);
        
        textBlock->addLine(fullLine, true);
        textBlock->addLine("\n", true);
    }
}
