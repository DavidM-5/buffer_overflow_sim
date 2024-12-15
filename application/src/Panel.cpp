#include "Panel.h"

application::Panel* application::Panel::s_activePanel = nullptr;

application::Panel::Panel(int posX, int posY, int w, int h, SDL_Color color) : 
                            Widget(posX, posY, w, h, color), m_scaleX(1.f), m_scaleY(1.f),
                            m_isResizing(false), m_bordersResizeFlags(0)
{
}

void application::Panel::handleEvents(const core::InputManager &inputMngr)
{
    // vector2i mousePos = inputMngr.getMousePosition();

    resize(inputMngr);
    /*
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
    */
}

void application::Panel::render(core::Renderer &renderer)
{
    Widget::render(renderer);

    if (m_borders[0].has_value())
        renderer.drawRect(m_borders[0].value(), {255,255,255,255});
    if (m_borders[1].has_value())
        renderer.drawRect(m_borders[1].value(), {255,255,255,255});
    if (m_borders[2].has_value())
        renderer.drawRect(m_borders[2].value(), {255,255,255,255});
    if (m_borders[3].has_value())
        renderer.drawRect(m_borders[3].value(), {255,255,255,255});
}

void application::Panel::setResizeBorders(const SDL_Rect* topBorder, const SDL_Rect* rightBorder, const SDL_Rect* bottomBorder, const SDL_Rect* leftBorder)
{
    if (topBorder) {
        m_borders[0] = *topBorder;
    }
    if (rightBorder) {
        m_borders[1] = *rightBorder;
    }
    if (bottomBorder) {
        m_borders[2] = *bottomBorder;
    }
    if (leftBorder) {
        m_borders[3] = *leftBorder;
    }
}

bool application::Panel::isPointInRect(const vector2i& point, const SDL_Rect* rect)
{
    if (!rect) return false;

    return point.x >= rect->x && point.x <= rect->x + rect->w &&
           point.y >= rect->y && point.y <= rect->y + rect->h;
}

void application::Panel::resize(const core::InputManager &inputMngr)
{
    vector2i mousePos = inputMngr.getMousePosition();
    vector2i mouseDelta = inputMngr.getMousePosDelta();
    SDL_Rect resizeOffset = {0, 0, 0, 0};

    if (inputMngr.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!m_isResizing) {
            if (m_borders[0].has_value() && isPointInRect(mousePos, &m_borders[0].value())) {
                m_isResizing = true;
                m_bordersResizeFlags |= 0b1000;
            }
            if (m_borders[1].has_value() && isPointInRect(mousePos, &m_borders[1].value())) {
                m_isResizing = true;
                m_bordersResizeFlags |= 0b0100;
            }
            if (m_borders[2].has_value() && isPointInRect(mousePos, &m_borders[2].value())) {
                m_isResizing = true;
                m_bordersResizeFlags |= 0b0010;
            }
            if (m_borders[3].has_value() && isPointInRect(mousePos, &m_borders[3].value())) {
                m_isResizing = true;
                m_bordersResizeFlags |= 0b0001;
            }
        }

        if (m_isResizing) {
            // Clamp mouse delta to prevent extreme values
            const int maxDelta = 200;
            mouseDelta.x = std::clamp(mouseDelta.x, -maxDelta, maxDelta);
            mouseDelta.y = std::clamp(mouseDelta.y, -maxDelta, maxDelta);

            if (m_borders[0].has_value() && m_bordersResizeFlags & 0b1000) {
                resizeOffset.y += mouseDelta.y;
                resizeOffset.h -= mouseDelta.y;
            }
            if (m_borders[1].has_value() && m_bordersResizeFlags & 0b0100) {
                resizeOffset.w += mouseDelta.x;
            }
            if (m_borders[2].has_value() && m_bordersResizeFlags & 0b0010) {
                resizeOffset.h += mouseDelta.y;
            }
            if (m_borders[3].has_value() && m_bordersResizeFlags & 0b0001) {
                resizeOffset.x += mouseDelta.x;
                resizeOffset.w -= mouseDelta.x;
            }
        }
    }
    else {
        m_isResizing = false;
        m_bordersResizeFlags = 0;
    }

    if (m_isResizing) {
        const int minSize = 10;
        const int maxSize = 2000;

        // Calculate new dimensions
        int newWidth = std::clamp(m_transform.w + resizeOffset.w, minSize, maxSize);
        int newHeight = std::clamp(m_transform.h + resizeOffset.h, minSize, maxSize);
        int newX = m_transform.x + resizeOffset.x;
        int newY = m_transform.y + resizeOffset.y;

        if (newWidth != m_transform.w) {
            m_transform.w = newWidth;
            m_transform.x = newX;
        }
        if (newHeight != m_transform.h) {
            m_transform.h = newHeight;
            m_transform.y = newY;
        }

        updateBorderPosition(mouseDelta);
    }
}

void application::Panel::updateBorderPosition(vector2i mouseDelta)
{
    if (m_borders[0].has_value()) {
        m_borders[0].value().x = m_transform.x;
        m_borders[0].value().y = m_transform.y;
        m_borders[0].value().w = m_transform.w;
    }
    if (m_borders[1].has_value()) {
        m_borders[1].value().x = m_transform.x + m_transform.w - m_borders[1].value().w;
        m_borders[1].value().y = m_transform.y;
        m_borders[1].value().h = m_transform.h;
    }
    if (m_borders[2].has_value()) {
        m_borders[2].value().x = m_transform.x;
        m_borders[2].value().y = m_transform.y + m_transform.h - m_borders[2].value().h;
        m_borders[2].value().w = m_transform.w;
    }
    if (m_borders[3].has_value()) {
        m_borders[3].value().x = m_transform.x;
        m_borders[3].value().y = m_transform.y;
        m_borders[3].value().h = m_transform.h;
    }
}
