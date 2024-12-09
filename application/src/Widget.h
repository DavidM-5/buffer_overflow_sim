#pragma once

#include "SDL2/SDL.h"

#include "../../core/src/config.h"
#include "../../core/src/InputManager.h"
#include "../../core/src/Renderer.h"

namespace application
{

    class Widget
    {
    public:
        Widget(int posX = 0, int posY = 0, int w = 100, int h = 100, SDL_Color color = {255, 255, 255, 255});
        virtual ~Widget() = default;
    
        virtual void handleEvents(const core::InputManager& inputMngr) = 0;
        virtual void render(core::Renderer& renderer);

    protected:
        SDL_Rect m_transform;

        SDL_Color m_mainColor;

    };
    
} // namespace application
