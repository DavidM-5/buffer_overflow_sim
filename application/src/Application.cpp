#include "Application.h"

application::Application::Application() : m_window("Buffer Overflow Simulator", WINDOW_WIDTH, WINDOW_HEIGHT),
                                          m_bordVert(true, 2*WINDOW_WIDTH/3-105, 0, 10, WINDOW_HEIGHT),
                                          m_bordHor(false, 0, 2*WINDOW_HEIGHT/3-5, 2*WINDOW_WIDTH/3-100, 10)
{
}

bool application::Application::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

    initPanels();

    loadCodeText("main.c");

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

    // render panels
    for (const auto& panel : m_panels) {
        panel->render(m_renderer);
    }

    m_bordVert.render(m_renderer);
    m_bordHor.render(m_renderer);

    m_renderer.present();
}

void application::Application::initPanels()
{
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
}

bool application::Application::loadCodeText(const std::string &filename)
{
    std::string fullPath = "targets/src/" + filename;
    std::ifstream file(fullPath); // Open the file

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
    codeBlock->setText(fileContents);

    return true;
}
