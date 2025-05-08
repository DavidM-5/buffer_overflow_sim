#include "BOApplication.h"

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

application::BOApplication::BOApplication() : m_window("Buffer Overflow Simulator", WINDOW_WIDTH, WINDOW_HEIGHT),
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
                                          m_borderWidth(10), m_innerBorderWidth(10),
                                          m_userLatestBreakpoint(0),
                                          m_userInLoginFunction(false),
                                          m_targetConsole(nullptr),
                                          m_stackView(nullptr)
{
    initFormatMap();
}

bool application::BOApplication::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

    initPanels();

    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-center_bottom");
    application::Widget* w = parentWidget->getWidget("Console-console");
    m_targetConsole = static_cast<application::Console*>(w);

    parentWidget = m_mainPanel.getWidget("Panel-right_bottom");
    w = parentWidget->getWidget("StackVisualizer-stack_view");
    m_stackView = static_cast<application::StackVisualizer*>(w);

    if (!m_targetConsole || !m_stackView)
        return false;


    if (!loadTargetSourceCodeFromPath("targets/src/Task_one/vulnerable_system/main.c")) {
        std::cerr << "Failed to load source code." << std::endl;
        return false;
    }

    // Compile the target program if not already compiled.
    if (!fileExists("./targets/compiled/t1")) {
        if (!compileFile("targets/src/Task_one/compile_command_linux.txt")) {
            std::cerr << "Failed to compile target code." << std::endl;
            return false;
        }
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

void application::BOApplication::run()
{
    /*
    ============================
        IDEA!!!
        Instead of passing the actual string value to gdb,
        just claculate the number of 'A', needed ( ($rbp+8) - (&username) ),
        and when the user enters the correct string run the command:
        set *((int *)($rbp + 8)) = 0x55555555e867
        in gdb.
    ============================
    */

    gdb = std::make_shared<GDBController>("./t1", "./targets/compiled");

    gdb->sendCommand("break gets");
    showFunctionsAddresses();

    m_requieredAddressInput = getRequieredAddressInput();
    std::cout << "REQ ADDRESS: " << m_requieredAddressInput << std::endl;

    gdb->sendCommand("continue");
    std::string out = gdb->getTargetOutput(); // clear buffer

    // Attach the gdb proccess to the console widget,
    // so that the console will be able to show gdb output.
    m_targetConsole->attachGDB(gdb);
    gdb->sendTargetInput("9");


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


    while (m_window.isRunning())
    {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            m_window.handleEvents(event);
            /*
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

                if (m_inputMngr.getPressedKey() == "r") {
                    gdb->sendCommand("info registers rbp");

                    std::string rawOutput = gdb->getGdbOutput();

                    std::cout << "registers:\n" << rawOutput << std::endl;
                }

                if (m_inputMngr.getPressedKey() == "n") {
                    std::cout << "-> Memory dump" << std::endl;

                    fillStackViewLoginFunc();
                }
                
                // temporary end /\/\/\.
            }
            */
            BOApplication::update(event);
        }

        BOApplication::render();

        // Frame timing and capping
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void application::BOApplication::update(SDL_Event& event)
{
    m_inputMngr.update(event);

    m_mainPanel.handleEvents(m_inputMngr);


    m_borderVerticalLeft.handleEvents(m_inputMngr);
    m_borderVerticalRight.handleEvents(m_inputMngr);

    static bool sentContinueComm = false;
    
    std::string consoleInput = m_targetConsole->getLastInput(2);
    if (!consoleInput.empty()) {
        std::string_view reqLoginText = "1\n"
                                        "Username: ";
        
        if (consoleInput == reqLoginText) {
            m_userInLoginFunction = true;
        }
        else {
            m_userInLoginFunction = false;
            sentContinueComm = false;
        }
    }
    
    // Check if the user put the breakpoint at 
    // the correct line and act accordingly.
    if (!m_userTasksStatus[PUT_BREAKPOINT_AT_PROBLEM_LINE]) {
        if (m_userLatestBreakpoint == 115) {
            m_userTasksStatus[PUT_BREAKPOINT_AT_PROBLEM_LINE] = true;
            markTaskDone(1);
        }
        else {
            if (!sentContinueComm && m_userInLoginFunction) {
                gdb->sendCommand("continue");
                sentContinueComm = true;
            }
        }
    }
    else if (!m_userTasksStatus[ENTER_PAYLOAD] && m_userInLoginFunction) { // Check if user did not complete the second step
        if (!m_targetConsole->isLocked())
            m_targetConsole->lock();

        if (m_stackView->empty()) {
            fillStackViewLoginFunc();
        }

        std::string userInput = m_targetConsole->getCurrentInput();
        int neededLenght = 10 + m_requieredAddressInput.length(); // 10 A's (dummy input) + the function address

        if (userInput.length() < neededLenght) { /* do nothing */ }
        else if (userInput.substr(10, m_requieredAddressInput.length()) == m_requieredAddressInput) {
            if (m_inputMngr.getPressedKey() == "\n") { // the user sent the correct input
                std::string userCorrectInput = m_targetConsole->getCurrentInput();
                m_targetConsole->printToConsole(userCorrectInput);
                m_targetConsole->clearInputLine();

                std::stringstream ss;
                ss << "0x" << std::hex << std::setw(16) << std::setfill('0') << m_requieredFunctionAddress;
                m_stackView->editSlot(0, ss.str());
                
                std::string gdbComm = "set *((void **)($rbp + 8)) = (void *)" + ss.str();
                // set *((int *)($rbp + 8)) = 0x55555555e867
                gdb->sendCommand(gdbComm);
                gdb->sendCommand("continue");
                
                
                gdb->sendTargetInput(userCorrectInput.substr(0, 10));
                m_targetConsole->unlock();
                m_userTasksStatus[ENTER_PAYLOAD] = true;
                markTaskDone(2);
                

                usleep(300000);
                m_targetConsole->printToConsole(" ");
                m_targetConsole->printToConsole("Program crashed. Restarting...");
                m_targetConsole->printToConsole(" ");
                m_targetConsole->detachGDB();
                
                gdb.reset();
                gdb = std::make_shared<GDBController>("./t1", "./targets/compiled");
                m_targetConsole->attachGDB(gdb);

                gdb->sendCommand("continue");
            }
        }
    }
    else if (!m_userTasksStatus[LOGIN_AS_MANAGER] && m_userTasksStatus[ENTER_PAYLOAD]) { // Check if user did not complete the third step and completed the second
        std::string consoleInput = m_targetConsole->getLastInput(8);
        if (!consoleInput.empty()) {
            std::string_view reqLoginText = "Hello, manager!\n"
                                            "-> Select operation:\n";
            
            if (reqLoginText == consoleInput.substr(0, reqLoginText.length())) {
                markTaskDone(3);
                showCompletionText();
                m_userTasksStatus[LOGIN_AS_MANAGER] = true;
            }
        }
    }
}

void application::BOApplication::render()
{
    m_renderer.clear({0, 0, 0, 255});

    m_mainPanel.render(m_renderer);
    
    // They are transparent
    // m_borderVerticalLeft.render(m_renderer);
    // m_borderVerticalRight.render(m_renderer);

    m_renderer.present();
}

void application::BOApplication::initFormatMap()
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

void application::BOApplication::initPanels()
{
    initLeftPanels();
    initCenterPanels();
    initRightPanels();
}


void application::BOApplication::initLeftPanels()
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

    
    auto task1 = std::make_unique<application::Paragraph>(
        label->getPosition().x, label->getPosition().y + label->getHeight() + 15,
        label->getWidth(), 85,
        SDL_Color{0xFF, 0xFF, 0xFF, 0x00},
        vector2i{m_borderWidth, m_borderWidth},
        "JetBrainsMono-Light.ttf", 16, 20
    );
    task1->setText("1) put a breakpoint at the line with unsafe input.");


    auto task2 = std::make_unique<application::Paragraph>(
        label->getPosition().x, task1->getPosition().y + task1->getHeight() + 15,
        label->getWidth(), 205,
        SDL_Color{0xFF, 0xFF, 0xFF, 0x00},
        vector2i{m_borderWidth, m_borderWidth},
        "JetBrainsMono-Light.ttf", 16, 20
    );
    task2->setText("2) Enter the payload to overflow the buffer, followed by the function address you want to replace the return address with. (In little endian format)");

    /*
    auto task3 = std::make_unique<application::Paragraph>(
        label->getPosition().x, task2->getPosition().y + task2->getHeight() + 15,
        label->getWidth(), 135,
        SDL_Color{0xFF, 0xFF, 0xFF, 0x00},
        vector2i{m_borderWidth, m_borderWidth},
        "JetBrainsMono-Light.ttf", 16, 20
    );
    task3->setText("3) Enter the function address you want to replace the return address with.");
    */

    auto task3 = std::make_unique<application::Paragraph>(
        label->getPosition().x, task2->getPosition().y + task2->getHeight() + 15,
        label->getWidth(), 100,
        SDL_Color{0xFF, 0xFF, 0xFF, 0x00},
        vector2i{m_borderWidth, m_borderWidth},
        "JetBrainsMono-Light.ttf", 16, 20
    );
    task3->setText("3) Read the output and login as a manager.");


    auto completionText = std::make_unique<application::Paragraph>(
        label->getPosition().x, task3->getPosition().y + task3->getHeight() + 15,
        label->getWidth(), 100,
        SDL_Color{0xFF, 0xFF, 0xFF, 0x00},
        vector2i{m_borderWidth, m_borderWidth},
        "JetBrainsMono-Light.ttf", 20, 26
    );
    completionText->setText("");


    leftPanel->addWidget("Label", std::move(label));
    leftPanel->addWidget("Paragraph-Task1", std::move(task1));
    leftPanel->addWidget("Paragraph-Task2", std::move(task2));
    // leftPanel->addWidget("Paragraph-Task3", std::move(task3));
    leftPanel->addWidget("Paragraph-Task3", std::move(task3));
    leftPanel->addWidget("Paragraph-completionText", std::move(completionText));

    // ===========================
    // ===========================
    m_mainPanel.addWidget("Panel-left", std::move(leftPanel));
}

void application::BOApplication::initCenterPanels()
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
        30,
        &m_userLatestBreakpoint
    );

    centerTopPanel->addWidget("Label", std::move(label));
    centerTopPanel->addWidget("TextBlock-Source_code", std::move(sourceCode));

    // ===========================
    // ===========================
    /*auto centerMiddlePanel = std::make_unique<application::Panel>(
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

    // Bind the function to the button
    continueBtn->onClick([this]() { gdb->sendCommand("continue"); });

    centerMiddlePanel->addWidget("Buttom-continue_btn", std::move(continueBtn));
    */
    // ===========================
    // ===========================
    auto centerBottomPanel = std::make_unique<application::Panel>(
        centerTopPanel->getPosition().x, centerTopPanel->getPosition().y + centerTopPanel->getHeight() + m_borderWidth,
        centerTopPanel->getWidth(), totalHeight - centerTopPanel->getHeight() - m_borderWidth * 3,
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
    label->appendText("Console - Vulnerable Program", true);

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
    // m_mainPanel.addWidget("Panel-center_middle", std::move(centerMiddlePanel));
    m_mainPanel.addWidget("Panel-center_bottom", std::move(centerBottomPanel));
}

void application::BOApplication::initRightPanels()
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
        200, rightBottomPanel->getHeight() - label->getPosition().y - label->getHeight() - 25 - m_innerBorderWidth - 40,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xDF},
        10, "JetBrainsMono-Bold.ttf", 17
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


void application::BOApplication::readFileToString(const std::string &filepath, std::string& dstString)
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

bool application::BOApplication::loadTargetSourceCodeFromPath(const std::string &filepath)
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

std::set<std::string> application::BOApplication::extractFunctionNamesFromFile(const std::string &filePath)
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

bool application::BOApplication::fileExists(const std::string &filePath)
{
    return std::filesystem::exists(filePath);
}

bool application::BOApplication::compileFile(const std::string &commandPath)
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

void application::BOApplication::memoryDumpToStackView(const std::string &startAddr, int numOfAddresses)
{
    std::vector<std::string> addresses = gdb->getMemoryDump("$rbp", numOfAddresses);

    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-right_bottom");
    application::Widget* w = parentWidget->getWidget("StackVisualizer-stack_view");
    application::StackVisualizer* stackV = static_cast<application::StackVisualizer*>(w);

    for (const std::string& addr : addresses) {
        stackV->push(addr);
    }

    gdb->getGdbOutput(); // Clear buffers
}

void application::BOApplication::fillStackViewLoginFunc()
{
    // std::vector<std::string> addresses = gdb->getMemoryDump("$rbp", 2); // clear gdb buffers
    
    gdb->getGdbOutput(); // clear buffers
    usleep(50000);
    
    gdb->sendCommand("x/1xg $rbp+8");
    usleep(200000);
    std::string rawOutput = gdb->getGdbOutput();

    std::istringstream iss(rawOutput);
    std::string word;
    int count = 0;

    while (iss >> word) {
        count++;
        if (count == 2) {
            m_stackView->push(word);
            break;
        }
    }

    usleep(100000);
    
    gdb->sendCommand("info registers rbp");
    usleep(200000);
    rawOutput = gdb->getGdbOutput();

    std::istringstream iss2(rawOutput);
    std::string word2;
    count = 0;

    while (iss2 >> word2) {
        count++;
        if (count == 2) {
            m_stackView->push(word2);
            break;
        }
    }

    m_stackView->push("0x0000000000000000");
    m_stackView->push("0x0000000000000000");
    m_stackView->push("...");
    m_stackView->push("...");

    m_stackView->selectBPSlot(1);

    // gdb->getGdbOutput(); // Clear buffers
}

void application::BOApplication::showFunctionsAddresses()
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

std::string application::BOApplication::getRequieredAddressInput()
{
    std::string printFunctionAddress = gdb->getAddress("print_users");

    if (printFunctionAddress.empty())
        return "";
    
    std::string hexPart = printFunctionAddress.substr(2); // Remove "0x" prefix
    hexPart.pop_back(); // Remove "." at the end

    std::stringstream ss;
    ss << std::hex << hexPart;
    ss >> m_requieredFunctionAddress;

    // Pad with '0' at the front to make it 16 characters
    if (hexPart.length() < 16) {
        hexPart.insert(0, 16 - hexPart.length(), '0');
    }

    std::string result;

    // Process 2 characters at a time in reverse
    for (int i = hexPart.length() - 2; i >= 0; i -= 2) {
        result += "\\0x";
        result += hexPart.substr(i, 2);
    }

    return result;
}

void application::BOApplication::markTaskDone(int taskNum)
{
    std::string widgetName;

    switch (taskNum)
    {
    case 1:
        widgetName = "Paragraph-Task1";
        break;
    case 2:
        widgetName = "Paragraph-Task2";
        break;
    case 3:
        widgetName = "Paragraph-Task3";
        break;
    
    default:
        return;
    }

    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-left");
    application::Widget* w = parentWidget->getWidget(widgetName);
    application::Paragraph* task = static_cast<application::Paragraph*>(w);

    task->setColor(SDL_Color{0x05, 0xCD, 0x00, 0xFF});
    task->appendText("  DONE!");

}

void application::BOApplication::showCompletionText()
{
    application::Widget* parentWidget = m_mainPanel.getWidget("Panel-left");
    application::Widget* w = parentWidget->getWidget("Paragraph-completionText");
    application::Paragraph* completionText = static_cast<application::Paragraph*>(w);

    completionText->setColor(SDL_Color{0x6D, 0xE8, 0x6A, 0xFF});
    completionText->appendText("GOOD JOB!!!");
}

bool application::BOApplication::is_number(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
