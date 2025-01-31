#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "../../core/src/config.h"
#include "../../core/src/Renderer.h"
#include "Widget.h"
#include "Border.h"

namespace application
{
    
    class Panel : public Widget
    {
    public:
        Panel(int posX = 0, int posY = 0, int w = 100, int h = 100, SDL_Color color = {255, 255, 255, 255});
        ~Panel() = default;

        void handleEvents(const core::InputManager& inputMngr);
        void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);

        bool addWidget(std::string name, std::unique_ptr<Widget> widget);
        void addLeftTopBorder(application::Border& border);
        void addRightBottomBorder(application::Border& border);

        application::Widget* getWidget(std::string name);

        void setPosition(vector2i newPos);
        void setWidth(int newW);
        void setHeight(int newH);
    
    private:
        std::unordered_map<std::string, std::unique_ptr<application::Widget>> m_widgets;

    };

} // namespace application
