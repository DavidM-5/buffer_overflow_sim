#include "StackVisualizer.h"

application::StackVisualizer::StackVisualizer(int posX, int posY, int w, int h, SDL_Color color) : 
                                               Widget(posX, posY, w, h, color)
{
}

void application::StackVisualizer::render(core::Renderer &renderer)
{
    int currRenderY = m_transform.y + 25;

    
}

void application::StackVisualizer::push(std::string str)
{
    m_slotsTextTextures.emplace_back(m_transform.w - 10);
    std::string text = std::to_string(m_slotsTextTextures.size());
    m_slotsTextTextures[m_slotsTextTextures.size() - 1].setText(text);
}

void application::StackVisualizer::pop()
{
    m_slotsTextTextures.pop_back();
}
