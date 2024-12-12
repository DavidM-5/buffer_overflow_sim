#include "Application.h"

application::Application::Application() : m_window("Buffer Overflow Simulator", WINDOW_WIDTH, WINODW_HEIGHT), txtb(400)
{
    // temporary
    std::string str = "Hello, world!\nDoes this work? awdawdawdjaoiwjfijawf awfjawfjpaiwnf awnfawjofijaoiegnj rgjarehjoaidrjohi asjrh ojtenohsnstrh st.bernard, jhonathan";
    txtb.setText(str);
    // temporary
}

bool application::Application::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

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
    for (const auto& panel : m_panels) {
        panel->handleEvents(inptmng);
    } 
}

void application::Application::render() // temporary implementation
{
    m_renderer.clear({0, 0, 0, 255});

    // the selected panel will be moved to the end, if its not already
    if (application::Panel::getActivePanel() != m_panels[m_panels.size()-1].get()) {
        for (size_t i = 0; i < m_panels.size(); ++i) {
            if (m_panels[i].get() == application::Panel::getActivePanel()) {
                std::rotate(m_panels.begin() + i, m_panels.begin() + i + 1, m_panels.end());
                break;
            }
        }
    }

    // render panels
    for (const auto& panel : m_panels) {
        panel->render(m_renderer);
    }


   //  txtb.render(400, 200, m_renderer);

    m_renderer.present();
}

void application::Application::initPanels()
{
    // code panel
    m_panels.emplace_back(
        std::make_unique<application::Panel>(2*WINDOW_WIDTH/3-100, 0, WINDOW_WIDTH/3+100, WINODW_HEIGHT, SDL_Color{0x60, 0x5f, 0x5f, 0xff})
    );
    
    // stack panel
    m_panels.emplace_back(
        std::make_unique<application::Panel>(0, 0, 2*WINDOW_WIDTH/3-100, 2*WINODW_HEIGHT/3, SDL_Color{0x60, 0x5f, 0xff, 0xff})
    );

    // console panel
    m_panels.emplace_back(
        std::make_unique<application::Panel>(0, 2*WINODW_HEIGHT/3, 2*WINDOW_WIDTH/3-100, WINODW_HEIGHT/3, SDL_Color{0x60, 0xff, 0x5f, 0xff})
    );

    
}
