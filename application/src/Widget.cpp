#include "Widget.h"

application::Widget::Widget(int posX, int posY, int w, int h) : 
                            m_position({posX, posY, w, h}), m_dragArea({posX, posY, w, 15}),
                            m_mainColor({255, 255, 255, 255}), m_dragColor({0, 255, 255, 255}),
                            m_isDragging(false), m_dragOffset({0, 0})
{
}

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

void application::Widget::render(core::Renderer &renderer)
{
    renderer.drawRect(m_position, m_mainColor);
    renderer.drawRect(m_dragArea, m_dragColor);
}
