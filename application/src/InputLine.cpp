#include "InputLine.h"

application::InputLine::InputLine(int posX, int posY, int w, int h, SDL_Color color, const std::string& label) : 
                                  Widget(posX, posY, w, h, color),
                                  m_input(""),
                                  m_label(label),
                                  m_lastInput(""),
                                  m_thisWidgetSelected(false)
{
    m_labelTextLine.useFont("JetBrainsMono-Medium.ttf", 14);
    m_inputTextLine.useFont("JetBrainsMono-Medium.ttf", 14);

    m_labelTextLine.appendText(m_label);

    m_labelTextLine.setPosition({posX, posY});
    m_labelTextLine.fitWidthToText();

    m_inputTextLine.appendText("test text");

    m_inputTextLine.setPosition({posX + m_labelTextLine.getWidth(), posY});
    m_inputTextLine.setWidth(m_transform.w - m_labelTextLine.getWidth());
}

void application::InputLine::handleEvents(const core::InputManager &inputMngr)
{
    vector2i mousePos = inputMngr.getMousePosition();

    if (inputMngr.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!isMouseInsideTransform(mousePos))
            m_thisWidgetSelected = false;
        else
            m_thisWidgetSelected = true;
    }

    if (!m_thisWidgetSelected)
        return;

    std::string input = inputMngr.getPressedKey();

    if (input == "\n") {
        m_lastInput = m_inputTextLine.getText();
        m_inputTextLine.clear();
    }
    else if (input == "\b") {
        if (m_inputTextLine.getLenght() > 0)
            m_inputTextLine.editText(m_inputTextLine.getLenght() - 1, m_inputTextLine.getLenght(), "");
    }
    else {
        m_inputTextLine.appendText(input, true);
    }
}

void application::InputLine::render(core::Renderer &renderer, const SDL_Rect *srcRect, const SDL_Rect *dstRect)
{
    m_labelTextLine.render(renderer);
    m_inputTextLine.render(renderer);
}

bool application::InputLine::isMouseInsideTransform(vector2i mousePos)
{
    return (mousePos.x > m_transform.x && mousePos.x < m_transform.x + m_transform.w &&
            mousePos.y > m_transform.y && mousePos.y < m_transform.y + m_transform.h);
}