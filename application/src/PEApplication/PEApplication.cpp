#include "PEApplication.h"

application::PEApplication::PEApplication() : m_window("Privilege Escalation Simulator", WINDOW_WIDTH, WINDOW_HEIGHT),
                                              m_mainPanel(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {0x2D, 0x2D, 0x2D, 0xFF}),
                                              testTable(50, 50, 450, 150, SDL_Color{255, 255, 255, 255}, 4, 4)
{
    testTable.setText({1, 0}, "1, 0");
    testTable.setText({2, 0}, "2, 0");
    testTable.setText({3, 0}, "3, 0");
    testTable.setText({0, 1}, "0, 1");
    testTable.setText({0, 2}, "0, 2");
    testTable.setText({0, 3}, "0, 3");
}

bool application::PEApplication::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

    return true;
}

void application::PEApplication::run()
{
    SDL_Event event;

    int fps; // Desired FPS

    // Get the screen refresh rate
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
        fps = displayMode.refresh_rate / 1.5;
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

            PEApplication::update(event);
        }

        PEApplication::render();

        // Frame timing and capping
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void application::PEApplication::update(SDL_Event &event)
{
    m_inputMngr.update(event);

    m_mainPanel.handleEvents(m_inputMngr);
}

void application::PEApplication::render()
{
    m_renderer.clear({0, 0, 0, 255});

    m_mainPanel.render(m_renderer);

    testTable.render(m_renderer);

    m_renderer.present();
}
