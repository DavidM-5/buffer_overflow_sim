#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>

#include <SDL2/SDL.h>

#include "../../core/src/config.h"
#include "../../core/src/Window.h"
#include "../../core/src/Renderer.h"
#include "../../core/src/Texture.h"
#include "../../core/src/InputManager.h"
#include "TextBlock.h"
#include "TextLine.h"
#include "Panel.h"
#include "Border.h"
#include "StackVisualizer.h"

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


        std::vector<std::unique_ptr<application::Widget>> m_widgets;

        application::Border m_bordVert;
        application::Border m_bordHor;

        int count = 0;// temporary
        application::StackVisualizer m_stack; // temporary

        application::TextLine line; // temporary


    private:
        void update(); // not implemented
        void render(); // temp implementation

        void initPanels();
    };

} // namespace application

