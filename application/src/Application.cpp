#include "Application.h"

application::Application::Application() : m_window("Attack Simulator", WINDOW_WIDTH, WINODW_HEIGHT),
                                          widget_asm(WINDOW_WIDTH/2, 0, WINDOW_WIDTH/2, WINODW_HEIGHT, {0x60, 0x5f, 0x5f, 0xff}),
                                          txtb(400)
{
    // temporary
    TTF_Init();
    font = TTF_OpenFont("resources/fonts/Arial.ttf", 30); // temporary
    if (font == nullptr)
        std::cout << "font not loaded: " << TTF_GetError() << std::endl;
    // temporary end

    /*
    std::string str = "Hello, world!\nDoes this work? awdawdawdjaoiwjfijawf awfjawfjpaiwnf awnfawjofijaoiegnj rgjarehjoaidrjohi asjrh ojtenohsnstrh st.bernard, jhonathan";
    txtb.setText(str);
    */
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
        widget_asm.handleEvents(inptmng); // temporary
        widget_asm.handleWindowResize(WINDOW_WIDTH/2, 0, 0.5, 1);
        inptmng.update(); // temporary


        Application::update();
        Application::render();
        
    }
    
}

void application::Application::render() // temporary implementation
{
    m_renderer.clear({0, 0, 0, 255});

    widget_asm.render(m_renderer);

   //  txtb.render(400, 200, m_renderer);

    m_renderer.present();
}
