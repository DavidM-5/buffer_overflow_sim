#include "Widget.h"

application::Widget::Widget(int posX, int posY, int w, int h, SDL_Color color) : 
                            m_transform({posX, posY, w, h}), m_mainColor(color)
{
}

void application::Widget::render(core::Renderer &renderer)
{
    renderer.drawRect(m_transform, m_mainColor);
}

void application::Widget::addDeltaTransform(int x, int y, int w, int h)
{
    m_transform.x += x;
    m_transform.y += y;
    m_transform.w += w;
    m_transform.h += h;
}

void application::Widget::setPosition(vector2i newPos)
{
    m_transform.x = newPos.x;
    m_transform.y = newPos.y;
}
