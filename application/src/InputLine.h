#pragma once

#include <string>
#include "Widget.h"
#include "TextLine.h"

namespace application
{
    
    class InputLine : public Widget
    {
    public:
        InputLine(int posX = 0, int posY = 0, int w = 100, int h = 20, SDL_Color color = {255, 255, 255, 0}, const std::string& label = "Input: ");
        ~InputLine() = default;

        void handleEvents(const core::InputManager& inputMngr);
        void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);

        std::string getInput() { return m_lastInput; }
    
    private:
        std::string m_input;
        std::string m_label;

        std::string m_lastInput;
        
        application::TextLine m_inputTextLine;
        application::TextLine m_labelTextLine;

        bool m_thisWidgetSelected;

    private:
        bool isMouseInsideTransform(vector2i mousePos);

    };

} // namespace application

