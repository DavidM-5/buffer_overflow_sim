#include "StackVisualizer.h"

application::StackVisualizer::StackVisualizer(int posX, int posY, int w, int h, SDL_Color color) : 
                                               Widget(posX, posY, w, h, color),
                                               m_stackSlotDims({w, 50})
{
}

void application::StackVisualizer::render(core::Renderer &renderer)
{
    renderer.drawRect(m_transform, m_mainColor);

    int currRenderY = m_transform.y + (m_stackSlotDims.y / 2);
    
    for (application::TextBlock& textTexture : m_slotsTextTextures) {
        textTexture.render(renderer, m_transform.x + m_transform.h/2 - textTexture.getWidth()/2, currRenderY - textTexture.getHeight()/2);
        currRenderY += m_stackSlotDims.y;
    }
}

void application::StackVisualizer::push(std::string str)
{
    m_slotsTextTextures.emplace_back(m_transform.w - 10);

    std::string text = std::to_string(m_slotsTextTextures.size());

    m_slotsTextTextures[m_slotsTextTextures.size() - 1].setText(text);
}

void application::StackVisualizer::pop()
{
    if (!m_slotsTextTextures.empty())
        m_slotsTextTextures.pop_back();
}
