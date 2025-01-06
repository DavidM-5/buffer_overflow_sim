#include "Border.h"

application::Border::Border(bool isVertical, int posX, int posY, int w, int h, SDL_Color color) :
                            Widget(posX, posY, w, h, color), m_isVertical(isVertical),
                            m_isResizing(false)
{
}

void application::Border::handleEvents(const core::InputManager &inputMngr)
{
    if (inputMngr.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        vector2i mousePos = inputMngr.getMousePosition();

        if (!m_isResizing) {
            if (isPointInside(mousePos)) {
                m_isResizing = true;
            }
        }
    }
    else {
        m_isResizing = false;
    }

    if (m_isResizing) {
        vector2i mouseDelta = inputMngr.getMousePosDelta();

        if (m_isVertical) {
            m_transform.x += mouseDelta.x;

            for (Widget* widget : m_widgets[0]) {
                widget->addDeltaTransform(0, 0, mouseDelta.x, 0);
            }
            for (Widget* widget : m_widgets[1]) {
                widget->addDeltaTransform(mouseDelta.x, 0, -mouseDelta.x, 0);
            }
        }
        else {
            m_transform.y += mouseDelta.y;

            for (Widget* widget : m_widgets[0]) {
                widget->addDeltaTransform(0, 0, 0, mouseDelta.y);
            }
            for (Widget* widget : m_widgets[1]) {
                widget->addDeltaTransform(0, mouseDelta.y, 0, -mouseDelta.y);
            }
        }
    }
}

void application::Border::addLeftTopWidget(Widget *panel)
{
    if (panel) {
        m_widgets[0].push_back(panel);
    }
}

void application::Border::addRightBottomWidget(Widget *panel)
{
    if (panel) {
        m_widgets[1].push_back(panel);
    }
}

bool application::Border::isPointInside(const vector2i &point)
{
    return point.x >= m_transform.x && point.x <= m_transform.x + m_transform.w &&
           point.y >= m_transform.y && point.y <= m_transform.y + m_transform.h;
}
