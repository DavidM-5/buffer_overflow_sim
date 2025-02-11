#include "Button.h"

application::Button::Button(int posX, int posY, int w, int h, SDL_Color color, const std::string &label) :
                            Widget(posX, posY, w, h, color),
                            m_textLine()
{
    m_textLine.useFont("JetBrainsMono-Bold.ttf", 14);
    m_textLine.appendText(label, true);

    m_textLine.fitHeightToText();
    m_textLine.fitWidthToText();

    m_textLine.setPosition({m_transform.x + w / 2 - m_textLine.getWidth() / 2,
                            m_transform.y + h / 2 - m_textLine.getHeight() / 2});
}

void application::Button::handleEvents(const core::InputManager &inputMngr)
{
    vector2i mousePos = inputMngr.getMousePosition();

    if (inputMngr.mouseClicked(MOUSE_BUTTON_LEFT) && isMouseInsideTransform(mousePos)) {        
        if (m_onClick) {
            m_onClick();
        }
    }
}

void application::Button::render(core::Renderer &renderer, const SDL_Rect *srcRect, const SDL_Rect *dstRect)
{
    Widget::render(renderer);

    m_textLine.render(renderer);
}

void application::Button::onClick(void (*callback)())
{
    m_onClick = callback;
}

bool application::Button::isMouseInsideTransform(vector2i mousePos)
{
    return (mousePos.x >= m_transform.x && mousePos.x < m_transform.x + m_transform.w &&
            mousePos.y >= m_transform.y && mousePos.y < m_transform.y + m_transform.h);
}
