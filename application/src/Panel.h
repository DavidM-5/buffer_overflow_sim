#pragma once

#include <SDL2/SDL.h>
#include <array>
#include <optional>
#include <algorithm> 
#include <iostream>
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

        void setResizeBorders(const SDL_Rect* topBorder, const SDL_Rect* rightBorder, const SDL_Rect* bottomBorder, const SDL_Rect* leftBorder);

        static Panel* getActivePanel() { return s_activePanel; }
    
    private:
        // the scale is between 0-1 (percent) of the screen width/height
        float m_scaleX, m_scaleY;
        
        // [0]-top, [1]-right, [2]-bottom, [3]-left
        std::array<std::optional<SDL_Rect>, 4> m_borders;

        bool m_isResizing;
        int m_bordersResizeFlags; // 1000-top | 0100-right | 0010-bottom | 0001-left
        static Panel* s_activePanel; // Track which panel is currently active

    private:
        void resize(const core::InputManager& inputMngr);
        void updateBorderPosition(vector2i mouseDelta);
        bool isPointInRect(const vector2i& point, const SDL_Rect* rect);

    };

} // namespace application



// for dragging:
/*
SDL_Rect m_dragArea;
SDL_Color m_dragColor;
bool m_isDragging;
vector2i m_dragOffset; 

void application::Widget::handleEvents(const core::InputManager &inputMngr)
{
    vector2i mousePos = inputMngr.getMousePosition();

    if (inputMngr.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

        if (mousePos.x > m_dragArea.x && mousePos.x < (m_dragArea.x + m_dragArea.w) &&
            mousePos.y > m_dragArea.y && mousePos.y < (m_dragArea.y + m_dragArea.h)) {
                
                if (!m_isDragging) {
                    m_isDragging = true;

                    m_dragOffset.x = mousePos.x - m_position.x;
                    m_dragOffset.y = mousePos.y - m_position.y;
                }

            }
    }
    else {
        m_isDragging = false;
    }

    if (m_isDragging) {
        // Calculate the new position of the widget relative to the mouse movement
        m_position.x = mousePos.x - m_dragOffset.x;
        m_position.y = mousePos.y - m_dragOffset.y;

        // Also update the drag area to keep it aligned with the widget's new position
        m_dragArea.x = m_position.x;
        m_dragArea.y = m_position.y;
    }
}
*/