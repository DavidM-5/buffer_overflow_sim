#pragma once

#include <SDL2/SDL.h>
#include <iostream>

#include "config.h"
#include "Renderer.h"

namespace core
{
    
    class Window
    {
    public:
        Window(const std::string& title, int width, int height);
        ~Window();

        bool init();
        bool initRenderer(core::Renderer* renderer);
        // void clear();
        // void present();
        void handleEvents(SDL_Event& event);

        bool isRunning() const { return m_running; }

    private:
        int m_width, m_height;
        bool m_running;

        const std::string m_title;

        SDL_Window* m_window;
    };

} // namespace core

