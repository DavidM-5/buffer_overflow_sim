#include "Application.h"

application::Application::Application() : m_window("Buffer Overflow Simulator", WINDOW_WIDTH, WINDOW_HEIGHT),
                                          m_bordVert(true, 2*WINDOW_WIDTH/3-105, 0, 10, WINDOW_HEIGHT),
                                          m_bordHor(false, 0, 2*WINDOW_HEIGHT/3-5, 2*WINDOW_WIDTH/3-100, 10)
                                          // m_stack(50, 50, 150, 250, {255, 255, 255, 255}, 10) // {0xff, 0xd8, 0x00, 0xff} - stack color
{
}

bool application::Application::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

    if (!application::TextLine::loadFont("Arial.ttf", 16)) return false; // temporary

    initPanels();

    return true;
}

void application::Application::run()
{
    SDL_Event event;

    while (m_window.isRunning())
    {
        while (SDL_PollEvent(&event))
        {
            m_window.handleEvents(event);
            
            // temporary
            std::string key = inptmng.getPressedKey(event);
            if (!key.empty()) {
                std::cout << key << std::flush;
            }
            /*
            if (inptmng.getPressedKey(event) == "k")
                m_stack.push(std::to_string(count++));
            if (inptmng.getPressedKey(event) == "i")
                m_stack.pop();
            */
            // temporary end
        }

        Application::update();
        Application::render();
    }
}

void application::Application::update() // temporary implementation
{
    inptmng.update(); // temporary

    for (const auto& panel : m_widgets) {
        panel->handleEvents(inptmng);
    }

    m_bordVert.handleEvents(inptmng);
    m_bordHor.handleEvents(inptmng);
}

void application::Application::render() // temporary implementation
{
    m_renderer.clear({0, 0, 0, 255});

    // render panels
    for (const auto& panel : m_widgets) {
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
    
    std::string str = "Lorem Ipsum\n is simply \ndummy text \nof the \nprinting\n and typ\nesett\ning indu\nstry.\nLorem Ipsum\n has\n been the \nindustry's\n standard dummy\n text\n ever \nsin\nce the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book.\nIt has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged.\nIt was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.\n";
    
    textBlock->setText(str);
    
    
    codePanel->addWidget("codeText", std::move(textBlock));
    codePanel->addRightBottomBorder(m_bordVert);

    // Add panel to widgets
    m_widgets.emplace_back(std::move(codePanel));



    // stack panel
    /*
    m_widgets.emplace_back(
        std::make_unique<application::TextBlock>(0, 0, 2*WINDOW_WIDTH/3-100, 2*WINDOW_HEIGHT/3, SDL_Color{0x60, 0x5f, 0xff, 0xff})
    );  m_stack.push(std::to_string(count++));
    m_stack.push(std::to_string(count++));
    */
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

    stackVisualize->push(std::to_string(count++));
    stackVisualize->push(std::to_string(count++));

    stackPanel->addWidget("stackVisualize", std::move(stackVisualize));
    stackPanel->addLeftTopBorder(m_bordVert);
    stackPanel->addLeftTopBorder(m_bordHor);
    
    // Add panel to widgets
    m_widgets.emplace_back(std::move(stackPanel));

    

    // console panel
    m_widgets.emplace_back(
        std::make_unique<application::TextBlock>(0, 2*WINDOW_HEIGHT/3, 2*WINDOW_WIDTH/3-100, WINDOW_HEIGHT/3, SDL_Color{0x60, 0xff, 0x5f, 0xff})
    );

    m_bordVert.addLeftTopWidget(&m_bordHor);
    // m_bordVert.addLeftTopWidget(m_widgets[1].get());
    m_bordVert.addLeftTopWidget(m_widgets[2].get());
    // m_bordVert.addRightBottomWidget(m_widgets[0].get());

    // m_bordHor.addLeftTopWidget(m_widgets[1].get());
    m_bordHor.addRightBottomWidget(m_widgets[2].get());
}
