#pragma once

#include <memory>
#include <string>
#include <iostream>

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
        application::Panel panelasm; // temporary
        application::Panel panelstack; // temporary
        application::Panel panelterminal; // temporary
        application::TextBlock txtb; // temporary


        void update() {}; // not implemented
        void render(); // temp implementation

    };

} // namespace application

