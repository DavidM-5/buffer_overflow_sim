#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "../../core/src/Renderer.h"
#include "../../core/src/Texture.h"

namespace application
{
    
    class TextBlock
    {
    public:
        TextBlock(int lineWidth);
        ~TextBlock();

        void setText(std::string& text);
        // void addLine(std::string& text);
        void setColorFormat(const std::unordered_map<std::string, SDL_Color>& formatMap);

        void render(core::Renderer& renderer, int x, int y); // change parameters?

    private:
        TTF_Font* m_font;

        SDL_Color m_defaultColor;

        std::vector<std::string> m_lines;
        std::unordered_map<std::string, SDL_Color> m_colorFormatMap;

        core::Texture m_textTexture;

        int m_lineWidth;
        int m_lineHeight;
        bool m_updated;
        
    private:
        void updateTexture(core::Renderer& renderer);

    };

} // namespace application
