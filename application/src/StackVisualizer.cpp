#include "StackVisualizer.h"

application::StackVisualizer::StackVisualizer(int posX, int posY, int w, int h, SDL_Color color) : 
                                               Widget(posX, posY, w, h, color)
{
}

void application::StackVisualizer::pop()
{
    m_slotsText.pop_back();
}
