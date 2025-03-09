#pragma once

#include <string>
#include <functional>
#include "Widget.h"
#include "TextLine.h"

namespace application
{
    
    class Button : public Widget
    {
    public:
        using Callback = std::function<void()>;

        Button(int posX = 0, int posY = 0, int w = 100, int h = 100, SDL_Color color = {255, 255, 255, 255}, const std::string& label = "Button", vector2i globalPos = {0, 0});
        ~Button() = default;

        void handleEvents(const core::InputManager& inputMngr) override;
        void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);

        void onClick(Callback callback);

        void addDeltaTransform(int x = 0, int y = 0, int w = 0, int h = 0);
    
    private:
        Callback m_onClick;

        application::TextLine m_textLine;

    private:
        bool isMouseInsideTransform(vector2i mousePos);

    };

} // namespace application
