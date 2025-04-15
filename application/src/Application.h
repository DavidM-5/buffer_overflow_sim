#pragma once

#include <iostream>
#include <memory>
#include <SDL2/SDL.h>
#include "BOApplication/BOApplication.h"

namespace application
{
    
    class Application
    {
    public:
        Application(/* args */);
        ~Application() = default;

        bool init();
        void run();
    
    private:
        core::Window m_window;
        core::Renderer m_renderer;
        core::InputManager m_inputMngr;

        application::Panel m_guiPanel;

        pid_t m_BOchildPID = -1;
        pid_t m_PEchildPID = -1;

        bool m_BOInitialized = false;
        bool m_PEInitialized = false;
        
    private:
        void update(SDL_Event& event);
        void render();

        void initButtons();

        void createBufferOverflow();
        void createPrivilegeEscalation();

    };

} // namespace application

