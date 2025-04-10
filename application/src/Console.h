#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <cmath>
#include "Widget.h"
#include "TextLine.h"
#include "GDBController.h"

namespace application
{
    
    class Console : public Widget
    {
    public:
        Console(int posX = 0, int posY = 0, int w = 100, int h = 20, SDL_Color color = {255, 255, 255, 0});
        ~Console() = default;

        void handleEvents(const core::InputManager& inputMngr);
        void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);

        void attachGDB(const std::shared_ptr<GDBController>& gdb);
        void detachGDB();

        std::string getLastInput(size_t n = 1);
        std::string getCurrentInput() { return m_activeLine.getText().substr(2); }

        void printToConsole(const std::string& str);

        void lock() { m_locked = true; }
        void unlock() { m_locked = false; }
        bool isLocked() { return m_locked; }

        void clearInputLine();

        void addDeltaTransform(int x = 0, int y = 0, int w = 0, int h = 0);
        void setWidth(int newW);

    private:
        std::shared_ptr<GDBController> m_gdb;

        std::vector<application::TextLine> m_lines;

        application::TextLine m_activeLine;

        bool m_thisWidgetSelected;
        bool m_gdbAttached;

        bool m_locked;

        int m_renderStartLine;

    private:
        bool isMouseInsideTransform(vector2i mousePos);

    };

} // namespace application

