#include "Application.h"

application::Application::Application() : m_window("Attack Simulator", 1080, 720),
                                          m_guiPanel(0, 0, 1080, 720, {0x2D, 0x2D, 0x2D, 0xFF})
{
}

bool application::Application::init()
{
    if (!m_window.init()) return false;

    if (!m_window.initRenderer(&m_renderer)) return false;

    initButtons();

    return true;
}

void application::Application::run()
{
    SDL_Event event;

    int fps; // Desired FPS

    // Get the screen refresh rate
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
        fps = displayMode.refresh_rate / 1.5;
    }
    else {
        fps = 45;
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

void application::Application::update(SDL_Event &event)
{
    m_inputMngr.update(event);

    if (WINODW_RESIZED) {
        m_guiPanel.addDeltaTransform(0, 0, WINDOW_WIDTH_DELTA, WINDOW_HEIGHT_DELTA);

        WINODW_RESIZED = false;
    }

    m_guiPanel.handleEvents(m_inputMngr);
}

void application::Application::render()
{
    m_renderer.clear({0, 0, 0, 255});

    m_guiPanel.render(m_renderer);

    m_renderer.present();
}

void application::Application::initButtons()
{
    auto BObtn = std::make_unique<application::Button>(
        600, 320,
        350, 50,
        SDL_Color{0x1B, 0x51, 0xCC, 0xFF},
        "Buffer Overflow",
        vector2i{600, 320},
        150
    );

    // Bind the function to the button
    BObtn->onClick([this]() { createBufferOverflow(); });

    m_guiPanel.addWidget("Buttom-BO_btn", std::move(BObtn));

    auto PEbtn = std::make_unique<application::Button>(
        200, 320,
        350, 50,
        SDL_Color{0x1B, 0x51, 0xCC, 0xFF},
        "Access Control",
        vector2i{220, 320},
        180
    );

    // Bind the function to the button
    PEbtn->onClick([this]() { createPrivilegeEscalation(); });

    m_guiPanel.addWidget("Buttom-PE_btn", std::move(PEbtn));
}

void application::Application::createBufferOverflow()
{
    m_BOchildPID = fork();

    if (m_BOchildPID < 0) {
        std::cerr << "Failed to fork process for BOApplication." << std::endl;
        return;
    }

    if (m_BOchildPID == 0) {
        // --- Child process ---
        setsid(); // detach from terminal
        char* args[] = { (char*)"./BOAppExec", nullptr };
        execvp(args[0], args);
        std::cerr << "Failed to launch BOAppExec" << std::endl;
        exit(1);
    }
    else {
        // --- Parent process ---
        m_BOInitialized = true;
        // Don't wait, continue main app
    }
}

void application::Application::createPrivilegeEscalation()
{
    m_PEchildPID = fork();

    if (m_PEchildPID < 0) {
        std::cerr << "Failed to fork process for PEApplication." << std::endl;
        return;
    }

    if (m_PEchildPID == 0) {
        // --- Child process ---
        setsid(); // detach from terminal
        char* args[] = { (char*)"./PEAppExec", nullptr };
        execvp(args[0], args);
        std::cerr << "Failed to launch PEAppExec" << std::endl;
        exit(1);
    }
    else {
        // --- Parent process ---
        m_PEInitialized = true;
        // Don't wait, continue main app
    }
}
