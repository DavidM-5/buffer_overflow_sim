#include "Panel.h"

application::Panel::Panel(int posX, int posY, int w, int h) : 
                            Widget(posX, posY, w, h)
{
    m_mainColor = {0x60, 0x5f, 0x5f, 0xff};
    m_dragColor = {0x49, 0x48, 0x48, 0xff};
}

void application::Panel::resize(int newW, int newH)
{
    m_position.w = newW;
    m_position.h = newH;

    m_dragArea.w = m_position.w;
}

void application::Panel::resizeByRatio(float ratioW, float ratioH)
{
    if (ratioW >= 0) m_position.w = (int)(m_position.w * ratioW);
    if (ratioH >= 0) m_position.h = (int)(m_position.h * ratioH);

    m_dragArea.w = m_position.w;
}

void application::Panel::move(int posX, int posY)
{
    m_position.x = posX;
    m_position.y = posY;

    m_dragArea.x = m_position.x;
    m_dragArea.y = m_position.y;
}
