#include "Panel.h"

application::Panel::Panel(int posX, int posY, int w, int h, SDL_Color color) : 
                          Widget(posX, posY, w, h, color)
{
}

void application::Panel::handleEvents(const core::InputManager &inputMngr)
{
    for (auto& [name, widget] : m_widgets) {
        widget->handleEvents(inputMngr);
    }
}

void application::Panel::render(core::Renderer &renderer)
{
    renderer.drawRect(m_transform, m_mainColor);

    for (auto& [name, widget] : m_widgets) {
        widget->render(renderer);
    }
}

bool application::Panel::addWidget(std::string name, std::unique_ptr<Widget> widget)
{
    if (m_widgets.find(name) != m_widgets.end())
        return false;

    m_widgets.emplace(name, std::move(widget));

    vector2i widPos = m_widgets[name]->getPosition();
    m_widgets[name]->setPosition( {m_transform.x + widPos.x, m_transform.y + widPos.y} );

    return true;
}

void application::Panel::addLeftTopBorder(application::Border &border)
{
    border.addLeftTopWidget(this);

    for (auto& [name, widget] : m_widgets) {
        border.addLeftTopWidget(widget.get());
    }
}

void application::Panel::addRightBottomBorder(application::Border &border)
{
    border.addRightBottomWidget(this);

    for (auto& [name, widget] : m_widgets) {
        border.addRightBottomWidget(widget.get());
    }
}

void application::Panel::setPosition(vector2i newPos)
{
    int dx = newPos.x - m_transform.x;
    int dy = newPos.y - m_transform.y;

    m_transform.x = newPos.x;
    m_transform.y = newPos.y;

    for (auto& [name, widget] : m_widgets) {
        widget->addDeltaTransform(dx, dy, 0, 0);
    }
}

void application::Panel::setWidth(int newW)
{
    int dw = newW - m_transform.w;

    m_transform.w = newW;

    for (auto& [name, widget] : m_widgets) {
        widget->addDeltaTransform(0, 0, dw, 0);
    }
}

void application::Panel::setHeight(int newH)
{
    int dh = newH - m_transform.h;

    m_transform.w = newH;

    for (auto& [name, widget] : m_widgets) {
        widget->addDeltaTransform(0, 0, 0, dh);
    }
}
