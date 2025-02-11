#pragma once

#include <string>
#include "Widget.h"
#include "TextLine.h"

namespace application
{
    
    class Button : public Widget
    {
    public:
        Button(int posX = 0, int posY = 0, int w = 100, int h = 100, SDL_Color color = {255, 255, 255, 255}, const std::string& label = "Button");
        ~Button() = default;

        void handleEvents(const core::InputManager& inputMngr) override;
        void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr) override;

        void onClick(void (*callback)());
    
    private:
        void (*m_onClick)();

        application::TextLine m_textLine;

    private:
        bool isMouseInsideTransform(vector2i mousePos);

    };

} // namespace application
