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
                                          m_borderWidth(10), m_innerBorderWidth(10)
{
    initFormatMap();
}

bool application::Application::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

    initPanels();

    loadTargetSourceCode("targets/src/vulnerable_system/main.c");
    
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
            
            // temporary end /\/\/\.

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
}

void application::Application::render()
{
    m_renderer.clear({0, 0, 0, 255});

    m_mainPanel.render(m_renderer);
    
    m_borderVerticalLeft.render(m_renderer);
    m_borderVerticalRight.render(m_renderer);

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

    /*
    // code panel
    // Create the panel
    auto codePanel = std::make_unique<application::Panel>(
        2*WINDOW_WIDTH/3-100, 0, 
        WINDOW_WIDTH/3+100, WINDOW_HEIGHT,
        SDL_Color{0x60, 0x5f, 0x5f, 0xff}
    );

    // Create and add the text block
    auto textBlock = std::make_unique<application::TextBlock>(
        0, 0,  // Relative to panel
        WINDOW_WIDTH/3+100, WINDOW_HEIGHT,
        SDL_Color{0, 0, 0, 0}
    );
    
    // temporary \/\/\/
    // std::string str = "Lorem Ipsum\n is simply \ndummy text \nof the \nprinting\n and typ\nesett\ning indu\nstry.\nLorem Ipsum\n has\n been the \nindustry's\n standard dummy\n text\n ever \nsin\nce the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book.\nIt has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged.\nIt was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.\n";
    // textBlock->setText(str);
    // temporary /\/\/\
    
    codePanel->addWidget("codeText", std::move(textBlock));
    codePanel->addRightBottomBorder(m_bordVert);

    // Add panel to widgets
    m_panels.emplace_back(std::move(codePanel));



    // stack panel
    // Create the panel
    auto stackPanel = std::make_unique<application::Panel>(
        0, 0,
        2*WINDOW_WIDTH/3-100, 2*WINDOW_HEIGHT/3,
        SDL_Color{0x60, 0x5f, 0xff, 0xff}
    );

    // Create and add the stack
    auto stackVisualize = std::make_unique<application::StackVisualizer>(
        50, 50,  // Relative to panel
        150, 250,
        SDL_Color{255, 255, 255, 255},
        10
    );

    stackVisualize->push(std::to_string(count++)); // temporary
    stackVisualize->push(std::to_string(count++)); // temporary

    stackPanel->addWidget("stackVisualize", std::move(stackVisualize));
    stackPanel->addLeftTopBorder(m_bordVert);
    stackPanel->addLeftTopBorder(m_bordHor);
    
    // Add panel to widgets
    m_panels.emplace_back(std::move(stackPanel));

    

    // console panel
    // Create the panel
    auto consolePanel = std::make_unique<application::Panel>(
        0, 2*WINDOW_HEIGHT/3,
        2*WINDOW_WIDTH/3-100, WINDOW_HEIGHT/3,
        SDL_Color{0xff, 0x60, 0x5f, 0xff}
    );
    
    // Create and add the stack
    auto console = std::make_unique<application::Console>(
        0, 0,  // Relative to panel
        consolePanel->getWidth(), consolePanel->getHeight()
    );


    consolePanel->addWidget("console", std::move(console));
    consolePanel->addLeftTopBorder(m_bordVert);
    consolePanel->addRightBottomBorder(m_bordHor);
    

    // Add panel to widgets
    m_panels.emplace_back(std::move(consolePanel));


    m_bordVert.addLeftTopWidget(&m_bordHor);
    // m_bordVert.addLeftTopWidget(m_widgets[1].get());
    // m_bordVert.addLeftTopWidget(m_panels[2].get());
    // m_bordVert.addRightBottomWidget(m_widgets[0].get());

    // m_bordHor.addLeftTopWidget(m_widgets[1].get());
    // m_bordHor.addRightBottomWidget(m_panels[2].get());
    */
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
        SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}
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

    centerBottomPanel->addWidget("Label", std::move(label));

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
    
    label->useFont("JetBrainsMono-Bold.ttf", 18);
    label->appendText("Memory Addresses", true);

    rightTopPanel->addWidget("Label", std::move(label));

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
        14, 18
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

bool application::Application::loadTargetSourceCode(const std::string &filepath)
{
    std::filesystem::path fsPath(filepath);
    if (!std::filesystem::exists(fsPath))
        return false;

    std::ifstream file(filepath); // Open the file

    if (!file.is_open()) {
        return false;
    }

    // Use a stringstream to read the file's contents into a string
    std::ostringstream ss;
    ss << file.rdbuf(); // Read the whole file into the stringstream
    std::string fileContents = ss.str(); // Convert the stringstream into a string

    // Close the file
    file.close();

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
