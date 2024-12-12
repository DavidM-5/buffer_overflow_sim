#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> // temporary

#include "../../core/src/config.h"
#include "../../core/src/Window.h"
#include "../../core/src/Renderer.h"
#include "../../core/src/Texture.h"
#include "../../core/src/InputManager.h"
#include "TextBlock.h"
#include "Panel.h"

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

        core::InputManager inptmng; // temporary
        TTF_Font* font; // temporary


        std::vector<std::unique_ptr<application::Panel>> m_panels;

        
        application::TextBlock txtb; // temporary


    private:
        void update(); // not implemented
        void render(); // temp implementation

        void initPanels();
    };

} // namespace application

