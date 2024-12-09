#include "Widget.h"

application::Widget::Widget(int posX, int posY, int w, int h, SDL_Color color) : 
                            m_transform({posX, posY, w, h}), m_mainColor(color)
{
}

void application::Widget::render(core::Renderer &renderer)
{
    renderer.drawRect(m_transform, m_mainColor);
}
