#include "Application.h"

application::Application::Application() : m_window("Buffer Overflow Simulator", WINDOW_WIDTH, WINDOW_HEIGHT),
                                          m_bordVert(true, 2*WINDOW_WIDTH/3-105, 0, 10, WINDOW_HEIGHT),
                                          m_bordHor(false, 0, 2*WINDOW_HEIGHT/3-5, 2*WINDOW_WIDTH/3-100, 10),
                                          m_stack(50, 50, 150, 250, {255, 255, 255, 255}, 10), // {0xff, 0xd8, 0x00, 0xff} - stack color
                                          line(100, 400, 500, 20)
{
}

bool application::Application::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

    if (!application::TextLine::loadFont("Arial.ttf", 16)) return false; // temporary

    initPanels();

    line.useFont("Arial.ttf", 16);
    line.appendText("Hello World!");

    m_stack.push(std::to_string(count++));
    m_stack.push(std::to_string(count++));

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
            if (inptmng.getPressedKey(event) == "k")
                m_stack.push(std::to_string(count++));
            if (inptmng.getPressedKey(event) == "i")
                m_stack.pop();
            if (inptmng.getPressedKey(event) == "u") {
                line.appendText(" A B,     C D E");
            }
            if (inptmng.getPressedKey(event) == "j") {
                line.editText(0, 2, "");
            }
            // temporary end
        }

        inptmng.update(); // temporary


        //m_codeWidget.handleWindowResize(WINDOW_WIDTH/2, 0, 0.5, 1); // temporary?


        Application::update();
        Application::render();
        
    }
    
}

void application::Application::update()
{
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

    m_stack.render(m_renderer);
    line.render(m_renderer);

    m_renderer.present();
}

void application::Application::initPanels()
{
    // code panel
    m_widgets.emplace_back(
        std::make_unique<application::TextBlock>(2*WINDOW_WIDTH/3-100, 0, WINDOW_WIDTH/3+100, WINDOW_HEIGHT, SDL_Color{0x60, 0x5f, 0x5f, 0xff})
    );
    
    std::string str = "Lorem Ipsum\n is simply \ndummy text \nof the \nprinting\n and typ\nesett\ning indu\nstry.\nLorem Ipsum\n has\n been the \nindustry's\n standard dummy\n text\n ever \nsin\nce the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book.\nIt has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged.\nIt was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.\n";
    
    application::TextBlock* t = static_cast<application::TextBlock*>(m_widgets[0].get());
    t->setText(str);
    
    // stack panel
    m_widgets.emplace_back(
        std::make_unique<application::Panel>(0, 0, 2*WINDOW_WIDTH/3-100, 2*WINDOW_HEIGHT/3, SDL_Color{0x60, 0x5f, 0xff, 0xff})
    );

    // console panel
    m_widgets.emplace_back(
        std::make_unique<application::Panel>(0, 2*WINDOW_HEIGHT/3, 2*WINDOW_WIDTH/3-100, WINDOW_HEIGHT/3, SDL_Color{0x60, 0xff, 0x5f, 0xff})
    );

    m_bordVert.addLeftTopPanel(&m_bordHor);
    m_bordVert.addLeftTopPanel(m_widgets[1].get());
    m_bordVert.addLeftTopPanel(m_widgets[2].get());
    m_bordVert.addRightBottomPanel(m_widgets[0].get());

    m_bordHor.addLeftTopPanel(m_widgets[1].get());
    m_bordHor.addRightBottomPanel(m_widgets[2].get());
}
