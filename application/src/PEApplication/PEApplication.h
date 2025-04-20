#pragma once

#include <SDL2/SDL.h>

#include "../../../core/src/config.h"
#include "../../../core/src/Window.h"
#include "../../../core/src/Renderer.h"
#include "../../../core/src/Texture.h"
#include "../../../core/src/InputManager.h"
#include "../Panel.h"
#include "../Table.h"


namespace application
{
    
    class PEApplication
    {
    public:
        PEApplication(/* args */);
        ~PEApplication() = default;
        
        bool init();
        void run();
    
    private:
        core::Window m_window;
        core::Renderer m_renderer;
        core::InputManager m_inputMngr;

        application::Panel m_mainPanel;

        application::Table testTable;
        
    private:
        void update(SDL_Event& event);
        void render();

    };

} // namespace application

