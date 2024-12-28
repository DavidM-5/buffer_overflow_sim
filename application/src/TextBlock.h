#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "../../core/src/Renderer.h"
#include "../../core/src/Texture.h"
#include "Widget.h"
#include "TextLine.h"

namespace application
{
    
    class TextBlock : public Widget
    {
    public:
        TextBlock(int posX = 0, int posY = 0, int w = 100, int h = 100, SDL_Color color = {255, 255, 255, 255});
        ~TextBlock() = default;

        void setText(std::string& text);
        // void addLine(std::string& text);
        void setColorFormat(const std::unordered_map<std::string, SDL_Color>& formatMap);

        void handleEvents(const core::InputManager& inputMngr) {};
        void render(core::Renderer& renderer);

    private:
        struct Line
        {
            int lineNumber;
            bool breakpoint = false;
            application::TextLine textLine;
        };

        const int m_GUTTER_WIDTH;
        
        std::vector<Line> m_lines;

    };

} // namespace application
