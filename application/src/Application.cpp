#include "Application.h"

application::Application::Application() : m_window("Buffer Overflow Simulator", WINDOW_WIDTH, WINDOW_HEIGHT),
                                          m_bordVert(true, 2*WINDOW_WIDTH/3-105, 0, 10, WINDOW_HEIGHT),
                                          m_bordHor(false, 0, 2*WINDOW_HEIGHT/3-5, 2*WINDOW_WIDTH/3-100, 10),
                                          m_mainPanel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {0x2D, 0x2D, 0x2D, 0xFF}),
                                          m_borderWidth(10), m_innerBorderWidth(10)
{
    formatMap["#include"] = {255, 123, 23, 255};
    formatMap["while"] = {123, 245, 123, 255};
}

bool application::Application::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

    initPanels();

    // loadCodeText("targets/src/main.c");

    return true;
}

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
                application::StackVisualizer* stackV = static_cast<application::StackVisualizer*>(m_panels[1]->getWidget("stackVisualize"));
                stackV->push(std::to_string(count++));
            }
            if (m_inputMngr.getPressedKey() == "i") {
                application::StackVisualizer* stackV = static_cast<application::StackVisualizer*>(m_panels[1]->getWidget("stackVisualize"));
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

    for (const auto& panel : m_panels) {
        panel->handleEvents(m_inputMngr);
    }

    m_bordVert.handleEvents(m_inputMngr);
    m_bordHor.handleEvents(m_inputMngr);
}

void application::Application::render()
{
    m_renderer.clear({0, 0, 0, 255});

    /*
    // render panels
    for (const auto& panel : m_panels) {
        panel->render(m_renderer);
    }
    */

    m_mainPanel.render(m_renderer);
    
    /*
    m_bordVert.render(m_renderer);
    m_bordHor.render(m_renderer);
    */

    m_renderer.present();
}

void application::Application::initPanels()
{
    auto leftPanel = std::make_unique<application::Panel>(
        m_mainPanel.getPosition().x + m_borderWidth, m_mainPanel.getPosition().y + m_borderWidth,
        WINDOW_WIDTH / 3 - m_borderWidth * 2, WINDOW_HEIGHT - m_borderWidth * 2,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    auto label = std::make_unique<application::TextLine>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        leftPanel->getWidth() - m_innerBorderWidth * 2, 22,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}
    );

    label->useFont("JetBrainsMono-Bold.ttf", 20);
    label->appendText("Tasks", true);

    leftPanel->addWidget("Label", std::move(label));


    auto centerTopPanel = std::make_unique<application::Panel>(
        leftPanel->getPosition().x + leftPanel->getWidth() + m_borderWidth, leftPanel->getPosition().y,
        WINDOW_WIDTH / 3 - m_borderWidth * 2, 1.8 * WINDOW_HEIGHT / 3 - m_borderWidth * 2,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    label = std::make_unique<application::TextLine>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        leftPanel->getWidth() - m_innerBorderWidth * 2, 22,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}
    );
    
    label->useFont("JetBrainsMono-Bold.ttf", 20);
    label->appendText("Source Code", true);

    centerTopPanel->addWidget("Label", std::move(label));


    auto centerMiddlePanel = std::make_unique<application::Panel>(
        centerTopPanel->getPosition().x, centerTopPanel->getPosition().y + centerTopPanel->getHeight() + m_borderWidth,
        centerTopPanel->getWidth(), 50,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );


    auto centerBottomPanel = std::make_unique<application::Panel>(
        centerMiddlePanel->getPosition().x, centerMiddlePanel->getPosition().y + centerMiddlePanel->getHeight() + m_borderWidth,
        centerMiddlePanel->getWidth(), WINDOW_HEIGHT - centerTopPanel->getHeight() - centerMiddlePanel->getHeight() - m_borderWidth * 4,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    label = std::make_unique<application::TextLine>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        leftPanel->getWidth() - m_innerBorderWidth * 2, 22,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}
    );
    
    label->useFont("JetBrainsMono-Bold.ttf", 20);
    label->appendText("Console", true);

    centerBottomPanel->addWidget("Label", std::move(label));


    auto rightTopPanel = std::make_unique<application::Panel>(
        centerTopPanel->getPosition().x + centerTopPanel->getWidth() + m_borderWidth, leftPanel->getPosition().y,
        WINDOW_WIDTH - leftPanel->getWidth() - centerTopPanel->getWidth() - m_borderWidth * 4, WINDOW_HEIGHT / 3 - m_borderWidth * 2,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    label = std::make_unique<application::TextLine>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        leftPanel->getWidth() - m_innerBorderWidth * 2, 22,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}
    );
    
    label->useFont("JetBrainsMono-Bold.ttf", 20);
    label->appendText("Memory Addresses", true);

    rightTopPanel->addWidget("Label", std::move(label));


    auto rightBottomPanel = std::make_unique<application::Panel>(
        rightTopPanel->getPosition().x, rightTopPanel->getPosition().y + rightTopPanel->getHeight() + m_borderWidth,
        rightTopPanel->getWidth(), 2 * WINDOW_HEIGHT / 3 - m_borderWidth,
        SDL_Color{0x1E, 0x1E, 0x1E, 0xFF}
    );

    label = std::make_unique<application::TextLine>(
        0 + m_innerBorderWidth, 0 + m_innerBorderWidth,
        leftPanel->getWidth() - m_innerBorderWidth * 2, 22,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}
    );
    
    label->useFont("JetBrainsMono-Bold.ttf", 20);
    label->appendText("Stack View", true);

    rightBottomPanel->addWidget("Label", std::move(label));


    m_mainPanel.addWidget("1", std::move(leftPanel));
    m_mainPanel.addWidget("3", std::move(centerTopPanel));
    m_mainPanel.addWidget("4", std::move(centerMiddlePanel));
    m_mainPanel.addWidget("5", std::move(centerBottomPanel));
    m_mainPanel.addWidget("2", std::move(rightTopPanel));
    m_mainPanel.addWidget("6", std::move(rightBottomPanel));


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

bool application::Application::loadCodeText(const std::string &path)
{
    return true;

    std::filesystem::path fsPath(path);
    if (!std::filesystem::exists(fsPath))
        return false;

    std::ifstream file(path); // Open the file

    if (!file.is_open()) {
        return false;
    }

    // Use a stringstream to read the file's contents into a string
    std::ostringstream ss;
    ss << file.rdbuf(); // Read the whole file into the stringstream
    std::string fileContents = ss.str(); // Convert the stringstream into a string

    // Close the file
    file.close();

    // set the text to the code textblock
    application::TextBlock* codeBlock = static_cast<application::TextBlock*>(m_panels[0]->getWidget("codeText"));

    

    codeBlock->setText(fileContents, true);
    codeBlock->setColorFormat(formatMap);

    return true;
}
