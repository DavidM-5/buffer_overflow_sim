#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include "../../core/src/Renderer.h"
#include "TextLine.h"

namespace application
{
    
    class Table : public Widget
    {
    public:
        Table(int posX = 0, int posY = 0, int w = 150, int h = 50, SDL_Color color = {255, 255, 255, 255}, int cols = 2, int rows = 2, const std::string& font = "JetBrainsMono-Bold.ttf", int fontSize = 16);
        ~Table() = default;

        void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);
        void handleEvents(const core::InputManager& inputMngr) {};

        std::string getText(const vector2i& coord);
        void setText(const vector2i& coord, const std::string& text);

        void resize(int cols, int rows);

        vector2i getDim();
    
    private:
        int m_rows, m_columns;
        int m_slotW, m_slotH;

        std::string m_font;
        int m_fontSize;
        
        std::vector<std::vector<application::TextLine>> m_table;

    private:
        bool isInBounds(const vector2i& coord);
        
    };

} // namespace application

