#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <memory>
#include "../../core/src/config.h"
#include "../../core/src/Renderer.h"
#include "../../core/src/InputManager.h"
#include "TextLine.h"
#include "Widget.h"

namespace application
{

    class StackVisualizer : public Widget
    {
    public:
        StackVisualizer(int posX = 0, int posY = 0, int w = 150, int h = 50, SDL_Color color = {255, 255, 255, 255});
        ~StackVisualizer() = default;

        void render(core::Renderer& renderer);
        void handleEvents(const core::InputManager& inputMngr) {};

        void setMaxHeight(int h) { m_maxHeight = h; }

        void push(std::string str);
        void pop();

    private:
        std::vector<std::unique_ptr<application::TextLine>> m_slots;
        
        int m_maxHeight;
        int m_slotHeight;

    };
    
} // namespace application
