#pragma once

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

        bool isPointInBorder(const vector2i& point);
        void setActive(bool active) { m_isActive = active; }
        static Panel* getActivePanel() { return s_activePanel; }
    
    private:
        // the scale is between 0-1 (percent) of the screen width/height
        float m_scaleX, m_scaleY;
        
        const int m_BORDER_THICKNESS;

        bool m_isResizing;
        
        // Track which borders are being resized
        bool m_isResizingLeft = false;
        bool m_isResizingRight = false;
        bool m_isResizingTop = false;
        bool m_isResizingBottom = false;

        bool m_isActive; // Track if this panel is the one being interacted with
        static Panel* s_activePanel; // Track which panel is currently active

    private:
        void resize(const core::InputManager& inputMngr);

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