#include "Application.h"

application::Application::Application() : m_window("Buffer Overflow Simulator", WINDOW_WIDTH, WINODW_HEIGHT),
                                          m_codeWidget(2*WINDOW_WIDTH/3-100, 0, WINDOW_WIDTH/3+100, WINODW_HEIGHT, {0x60, 0x5f, 0x5f, 0xff}),
                                          m_stackWidget(0, 0, 2*WINDOW_WIDTH/3-100, 2*WINODW_HEIGHT/3, {0x60, 0x5f, 0xff, 0xff}),
                                          m_consoleWidget(0, 2*WINODW_HEIGHT/3, 2*WINDOW_WIDTH/3-100, WINODW_HEIGHT/3, {0x60, 0xff, 0x5f, 0xff}),
                                          txtb(400)
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
        m_codeWidget.handleEvents(inptmng); // temporary
        //m_codeWidget.handleWindowResize(WINDOW_WIDTH/2, 0, 0.5, 1); // temporary?
        inptmng.update(); // temporary


        Application::update();
        Application::render();
        
    }
    
}

void application::Application::render() // temporary implementation
{
    m_renderer.clear({0, 0, 0, 255});

    m_codeWidget.render(m_renderer);
    m_stackWidget.render(m_renderer);
    m_consoleWidget.render(m_renderer);

   //  txtb.render(400, 200, m_renderer);

    m_renderer.present();
}
