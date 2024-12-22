#include "StackVisualizer.h"

application::StackVisualizer::StackVisualizer(int posX, int posY, int w, int h, SDL_Color color) : 
                                              Widget(posX, posY, w, h, color), m_maxHeight(0),
                                              m_slotHeight(h)
{
}

void application::StackVisualizer::render(core::Renderer &renderer)
{
    for (application::TextLine txt : m_slots) {
        txt.render(renderer);
    }
}

void application::StackVisualizer::push(std::string str)
{
    application::TextLine txt(m_transform.x, m_transform.y + m_transform.h * m_slots.size(), 
                              m_transform.w, m_transform.h,
                              m_mainColor);
    txt.useFont("Arial.ttf", 16);
    txt.appendText(str);

    m_slots.push_back(txt);
}

void application::StackVisualizer::pop()
{
    if (m_slots.size() > 0) {
        m_slots.pop_back();
    }
}
