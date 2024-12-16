#include "Panel.h"

application::Panel::Panel(int posX, int posY, int w, int h, SDL_Color color) : 
                            Widget(posX, posY, w, h, color), m_scaleX(1.f), 
                            m_scaleY(1.f)
{
}

void application::Panel::handleEvents(const core::InputManager &inputMngr)
{
    
}

void application::Panel::render(core::Renderer &renderer)
{
    Widget::render(renderer);
}
