#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <cmath>
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
        // TODO: fix rendring and handle when input="\n"
        void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);

        void printToConsole(const std::string& str);

        void addDeltaTransform(int x = 0, int y = 0, int w = 0, int h = 0);
        void setWidth(int newW);

    private:
        std::vector<application::TextLine> m_lines;

        application::TextLine m_activeLine;

        bool m_thisWidgetSelected;

    private:
        int findNthWordFromEnd(const std::string& str, int n);
        std::string trimToLastNWords(const std::string& str, int n);

        bool isMouseInsideTransform(vector2i mousePos);

    };

} // namespace application

