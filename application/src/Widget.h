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
    
        virtual void handleEvents(const core::InputManager& inputMngr);
        virtual void handleWindowResize(int posX, int posY, float windowScaleX, float windowScaleY);
        virtual void render(core::Renderer& renderer);

    protected:
        SDL_Rect m_position;
        SDL_Rect m_dragArea;

        SDL_Color m_mainColor;
        SDL_Color m_dragColor;

        bool m_isDragging;
        vector2i m_dragOffset;


    private:
        

    };
    
} // namespace application
