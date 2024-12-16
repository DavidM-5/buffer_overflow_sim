#pragma once

#include <SDL2/SDL.h>

#include "../../core/src/config.h"
#include "../../core/src/Renderer.h"
#include "Widget.h"

namespace application
{
    
    class Panel : public Widget
    {
    public:
        Panel(int posX = 0, int posY = 0, int w = 100, int h = 100, SDL_Color color = {255, 255, 255, 255});
        ~Panel() = default;

        void handleEvents(const core::InputManager& inputMngr);
        void render(core::Renderer& renderer);
    
    private:
        // the scale is between 0-1 (percent) of the screen width/height
        float m_scaleX, m_scaleY;

    };

} // namespace application
