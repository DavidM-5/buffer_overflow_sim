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
    
        virtual void handleEvents(const core::InputManager& inputMngr) {};
        virtual void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);

        virtual void addDeltaTransform(int x = 0, int y = 0, int w = 0, int h = 0);

        virtual vector2i getPosition() { return vector2i{m_transform.x, m_transform.y}; }
        virtual void setPosition(vector2i newPos);

        virtual int getWidth() { return m_transform.w; }
        virtual void setWidth(int newW) { m_transform.w = newW; }
        
        virtual int getHeight() { return m_transform.h; }
        virtual void setHeight(int newH) { m_transform.h = newH; }

        virtual void setColor(const SDL_Color &color) { m_mainColor = color; }

        virtual Widget* getWidget(std::string name) { return nullptr; }
        
    protected:
        SDL_Rect m_transform;

        SDL_Color m_mainColor;

    };
    
} // namespace application
