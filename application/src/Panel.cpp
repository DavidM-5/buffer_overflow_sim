#include "Panel.h"

application::Panel* application::Panel::s_activePanel = nullptr;

application::Panel::Panel(int posX, int posY, int w, int h, SDL_Color color) : 
                            Widget(posX, posY, w, h, color), m_scaleX(1.f), m_scaleY(1.f),
                            m_BORDER_THICKNESS(7), m_isResizing(false), m_isResizingLeft(false),
                            m_isResizingRight(false), m_isResizingTop(false),
                            m_isResizingBottom(false), m_isActive(false)
{
}

application::Panel::~Panel()
{
    if (m_isActive) {
        s_activePanel = nullptr;
    }
}

void application::Panel::handleEvents(const core::InputManager &inputMngr)
{
    vector2i mousePos = inputMngr.getMousePosition();

    // Handle mouse press
    if (inputMngr.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!s_activePanel && isPointInBorder(mousePos)) {
            s_activePanel = this;
            m_isActive = true;
        }
    }
    // Handle mouse release
    else if (!inputMngr.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (s_activePanel == this) {
            s_activePanel = nullptr;
            m_isActive = false;
        }
    }

    // Only resize if this panel is active
    if (m_isActive) {
        resize(inputMngr);
    }
}

bool application::Panel::isPointInBorder(const vector2i &point)
{
   if (!isPointInPanel(point)) return false;
    
    // Check if point is in any border area
    m_isResizingLeft = point.x <= m_transform.x + m_BORDER_THICKNESS;
    m_isResizingRight = point.x >= m_transform.x + m_transform.w - m_BORDER_THICKNESS;
    m_isResizingTop = point.y <= m_transform.y + m_BORDER_THICKNESS;
    m_isResizingBottom = point.y >= m_transform.y + m_transform.h - m_BORDER_THICKNESS;
    
    return m_isResizingLeft || m_isResizingRight || m_isResizingTop || m_isResizingBottom;
}

bool application::Panel::isPointInPanel(const vector2i &point)
{
    return point.x >= m_transform.x && point.x <= m_transform.x + m_transform.w &&
           point.y >= m_transform.y && point.y <= m_transform.y + m_transform.h;
}

void application::Panel::resize(const core::InputManager &inputMngr)
{   
    vector2i mousePos = inputMngr.getMousePosition();
    vector2i mouseDelta = inputMngr.getMousePosDelta();
    SDL_Rect resizeOffset = {0, 0, 0, 0};

    if (inputMngr.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // If not already resizing, check if we should start
        if (!m_isResizing) {
            m_isResizingLeft = (mousePos.x >= m_transform.x && 
                              mousePos.x <= m_transform.x + m_BORDER_THICKNESS);
            
            m_isResizingRight = (mousePos.x <= m_transform.x + m_transform.w && 
                               mousePos.x >= m_transform.x + m_transform.w - m_BORDER_THICKNESS);
            
            m_isResizingTop = (mousePos.y >= m_transform.y && 
                             mousePos.y <= m_transform.y + m_BORDER_THICKNESS);
            
            m_isResizingBottom = (mousePos.y <= m_transform.y + m_transform.h && 
                                mousePos.y >= m_transform.y + m_transform.h - m_BORDER_THICKNESS);
            
            m_isResizing = m_isResizingLeft || m_isResizingRight || m_isResizingTop || m_isResizingBottom;
        }

        if (m_isResizing) {
            if (m_isResizingLeft) {
                resizeOffset.x += mouseDelta.x;
                resizeOffset.w -= mouseDelta.x;
            }
            if (m_isResizingRight) {
                resizeOffset.w += mouseDelta.x;
            }
            if (m_isResizingTop) {
                resizeOffset.y += mouseDelta.y;
                resizeOffset.h -= mouseDelta.y;
            }
            if (m_isResizingBottom) {
                resizeOffset.h += mouseDelta.y;
            }
        }
    }
    else {
        m_isResizing = false;
        m_isResizingLeft = false;
        m_isResizingRight = false;
        m_isResizingTop = false;
        m_isResizingBottom = false;
    }

    if (m_isResizing) {
        int newWidth = m_transform.w + resizeOffset.w;
        int newHeight = m_transform.h + resizeOffset.h;
        int newX = m_transform.x + resizeOffset.x;
        int newY = m_transform.y + resizeOffset.y;

        int minSize = m_BORDER_THICKNESS * 2;
        if (newWidth >= minSize) {
            m_transform.w = newWidth;
            m_transform.x = newX;
        }
        if (newHeight >= minSize) {
            m_transform.h = newHeight;
            m_transform.y = newY;
        }
    }

}
