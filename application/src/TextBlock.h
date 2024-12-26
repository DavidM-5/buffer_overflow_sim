#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <sstream>
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

        void render(core::Renderer& renderer, int x, int y); // change parameters?

        int getWidth() { return m_lineWidth; }
        int getHeight() { return m_lineHeight; }

    private:
        const int m_GUTTER_WIDTH;

        struct Line
        {
            uint32_t lineNumber;
            bool breakpoint = false;
            application::TextLine textTexture;
        };
        
        std::vector<Line> m_lines;

    };

} // namespace application
