#include "Console.h"

application::Console::Console(int posX, int posY, int w, int h, SDL_Color color) : 
                              Widget(posX, posY, w, h, color),
                              m_label(posX + 5, posY + 5),
                              m_inputText(posX + 5, posY + 5),
                              m_selected(false)
{
    m_label.useFont("Arial.ttf", 16);
    m_inputText.useFont("Arial.ttf", 16);

    m_label.appendText(">>> ", true);
    m_label.fitWidthToText();

    m_inputText.setPosition( {m_label.getPosition().x + m_label.getWidth(), posY} );
    m_inputText.setWidth(m_transform.w - m_inputText.getPosition().x - 5);
}

void application::Console::handleEvents(const core::InputManager &inputMngr)
{
    if (inputMngr.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!(inputMngr.getMousePosition().x > m_transform.x && 
            inputMngr.getMousePosition().x < m_transform.x + m_transform.w &&
            inputMngr.getMousePosition().y > m_transform.y && 
            inputMngr.getMousePosition().y < m_transform.y + m_transform.h)) {
            
            m_selected = false;
        }
        else {
            m_selected = true;
        }
    }

    if (!m_selected)
        return;

    std::string letter = inputMngr.getPressedKey();

    if (letter == "\n") {
        // handle command send
    }
    else if (letter == "backspace") {
        m_inputText.editText(m_inputText.getLenght() - 1, m_inputText.getLenght(), "");
    }
    else if (letter == "\t") {
        m_inputText.appendText("    ", true);
    }
    else {
        m_inputText.appendText(letter, true);
    }
}

void application::Console::render(core::Renderer &renderer)
{
    Widget::render(renderer);

    m_label.render(renderer);

    m_inputText.render(renderer);
}

void application::Console::addToLabel(const std::string &text)
{
    m_label.appendText(text, true);
}

void application::Console::setLabel(const std::string &text)
{
    m_label.clear();

    m_label.appendText(text, true);
    m_label.fitWidthToText();

    m_inputText.setPosition({ m_label.getPosition().x + m_label.getWidth(), m_inputText.getPosition().y });
    m_inputText.setWidth(m_transform.w - m_inputText.getPosition().x - 5);
}

void application::Console::addDeltaTransform(int x, int y, int w, int h)
{
    Widget::addDeltaTransform(x, y, w, h);

    m_label.addDeltaTransform(x, y, 0, 0);

    m_inputText.addDeltaTransform(x, y, w, 0);
}

void application::Console::setPosition(vector2i newPos)
{
    Widget::setPosition(newPos);

    m_label.setPosition( {m_transform.x + 5, m_transform.y + 5} );

    m_inputText.setPosition( {m_label.getPosition().x + m_label.getWidth(), m_transform.y + 5} );
}

void application::Console::setWidth(int newW)
{
    Widget::setWidth(newW);

    m_inputText.setWidth(newW - m_inputText.getPosition().x - 5);
}
