#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "../../core/src/Renderer.h"
#include "../../core/src/InputManager.h"
#include "TextBlock.h"
#include "Widget.h"

namespace application
{

    class StackVisualizer : public Widget
    {
    public:
        StackVisualizer(int posX = 0, int posY = 0, int w = 150, int h = 50, SDL_Color color = {255, 255, 255, 255});
        ~StackVisualizer() = default;

        void render(core::Renderer& renderer);
        void handleEvents(core::InputManager& inputMngr);

        void push(std::string str);
        void pop();

    private:
        std::vector<application::TextBlock> m_slotsText;
        
    };
    
} // namespace application
