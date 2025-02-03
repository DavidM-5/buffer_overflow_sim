#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <unordered_set>
#include "../../core/src/Renderer.h"
#include "../../core/src/Texture.h"
#include "Widget.h"
#include "TextLine.h"

namespace application
{
    
    class TextBlock : public Widget
    {
    public:
        TextBlock(int posX = 0, int posY = 0, int w = 100, int h = 100, SDL_Color color = {255, 255, 255, 255}, std::unordered_set<int>* breakpointVector = nullptr);
        ~TextBlock() = default;

        void setText(std::string& text, bool ignoreNotFittedLine = false, bool clearBreakpoints = true);
        // void addLine(std::string& text);
        void setColorFormat(const std::unordered_map<std::string, SDL_Color>& formatMap);

        void handleEvents(const core::InputManager& inputMngr) override;
        void render(core::Renderer& renderer, const SDL_Rect* srcRect, const SDL_Rect* dstRect);

        void addDeltaTransform(int dx = 0, int dy = 0, int dw = 0, int dh = 0) override;

        void setPosition(vector2i newPos);
        void setWidth(int newW);

    private:
        struct Line
        {
            int lineNumber;
            bool breakpoint = false;
            application::TextLine textLine;
        };
        
        const int m_GUTTER_WIDTH;
        std::unordered_set<int>& m_breakpointVector;
        
        std::vector<Line> m_lines;

        std::unordered_map<std::string, SDL_Color> m_formatMap;

        std::string m_text;

        int m_renderStartLine;

        bool m_ignoreNotFittedLine;

    private:
        std::vector<std::string> splitIntoLines(std::string& text);

        bool isMouseInsideTransform(vector2i mousePos);

    };

} // namespace application
