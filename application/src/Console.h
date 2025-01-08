#pragma once

#include <string>
#include "Widget.h"
#include "TextLine.h"

namespace application
{
    
    class Console : public Widget
    {
    public:
        Console(int posX = 0, int posY = 0, int w = 100, int h = 20, SDL_Color color = {255, 255, 255, 0});
        ~Console() = default;

        void handleEvents(const core::InputManager& inputMngr);
        void render(core::Renderer& renderer);

        void addToLabel(const std::string& text);
        void setLabel(const std::string& text);

        std::string getLabel() { return m_label.getText(); }

        void addDeltaTransform(int x = 0, int y = 0, int w = 0, int h = 0);
        void setPosition(vector2i newPos);
        void setWidth(int newW);

    private:
        application::TextLine m_label;

        application::TextLine m_inputText;

    };

} // namespace application

