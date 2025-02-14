#include "Console.h"

application::Console::Console(int posX, int posY, int w, int h, SDL_Color color) :
                              Widget(posX, posY, w, h, color),
                              m_activeLine(posX, posY, w, 20, {255, 255, 255, 255}, false),
                              m_thisWidgetSelected(false)
{
    m_activeLine.useFont("JetBrainsMono-Medium.ttf", 14);
    m_activeLine.appendText("> ", true);
}

void application::Console::handleEvents(const core::InputManager &inputMngr)
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
        m_lines.push_back(m_activeLine);
        m_activeLine.clear();

        m_activeLine.appendText("> ", true);
    }
    else if (input == "\b") {
        if (m_activeLine.getLenght() > 2)
            m_activeLine.editText(m_activeLine.getLenght() - 1, m_activeLine.getLenght(), "");
    }
    else {
        m_activeLine.appendText(input);
    }
}

void application::Console::render(core::Renderer &renderer, const SDL_Rect *srcRect, const SDL_Rect *dstRect)
{
    Widget::render(renderer);

    // Calculate the maximum number of lines that can fit into the console's height
    int lineHeight = m_activeLine.getHeight();
    int maxLines = m_transform.h / lineHeight;
    
    // Determine the starting index for rendering lines
    int startIndex = std::max(0, static_cast<int>(m_lines.size()) - maxLines + 1);
    
    // Render the lines that fit into the console's height
    for (int i = startIndex; i < m_lines.size(); i++) {
        SDL_Rect lineDstRect = {
            m_transform.x,
            m_transform.y + (i - startIndex) * lineHeight,
            m_lines[i].getWidth(),
            lineHeight
        };
        m_lines[i].render(renderer, nullptr, &lineDstRect);
    }

    // Render the active line at the bottom
    SDL_Rect activeLineDstRect = {
        m_transform.x,
        m_transform.y + (maxLines - 1) * lineHeight,
        m_activeLine.getWidth(),
        lineHeight
    };
    m_activeLine.render(renderer, nullptr, &activeLineDstRect);
    
}

void application::Console::printToConsole(const std::string &str)
{
    int remainingWords = m_activeLine.appendText(str);

    while (remainingWords > 0) {
        m_lines.push_back(m_activeLine);
        m_activeLine.clear();

        std::string additionalText = trimToLastNWords(str, remainingWords);
        remainingWords = m_activeLine.appendText("  " + additionalText);
    }

    m_lines.push_back(m_activeLine);
    m_activeLine.clear();

    m_activeLine.appendText("> ", true);
}

void application::Console::addDeltaTransform(int x, int y, int w, int h)
{
    Widget::addDeltaTransform(x, y, w, h);

    m_activeLine.addDeltaTransform(x, y, w, h);

    for (application::TextLine& tline : m_lines) {
        tline.addDeltaTransform(x, y, w, h);
    }
}

void application::Console::setWidth(int newW)
{
    Widget::setWidth(newW);

    m_activeLine.setWidth(newW);

    for (application::TextLine& tline : m_lines) {
        tline.setWidth(newW);
    }
}

int application::Console::findNthWordFromEnd(const std::string &str, int n)
{
    if (n <= 0) {
        return -1; // Invalid input
    }

    int wordCount = 0;
    int index = str.length() - 1;

    // Skip trailing whitespace
    while (index >= 0 && std::isspace(str[index])) {
        index--;
    }

    // Iterate from the end to find the n-th word
    while (index >= 0) {
        if (std::isspace(str[index])) {
            wordCount++;
            if (wordCount == n) {
                // Return the starting index of the n-th word
                return index + 1;
            }
            // Skip consecutive spaces
            while (index >= 0 && std::isspace(str[index])) {
                index--;
            }
        } else {
            index--;
        }
    }

    // If the n-th word is the first word in the string
    if (wordCount + 1 == n) {
        return 0;
    }

    return -1; // n-th word not found
}

std::string application::Console::trimToLastNWords(const std::string &str, int n)
{
    int startIndex = findNthWordFromEnd(str, n);
    if (startIndex == -1) {
        return ""; // n-th word not found, return an empty string
    }

    // Extract the substring starting from the n-th word
    std::string result = str.substr(startIndex);

    // Ensure the result does not start with a space
    size_t firstNonSpace = result.find_first_not_of(' ');
    if (firstNonSpace != std::string::npos) {
        result = result.substr(firstNonSpace);
    }

    return result;
}

bool application::Console::isMouseInsideTransform(vector2i mousePos)
{
    return (mousePos.x > m_transform.x && mousePos.x < m_transform.x + m_transform.w &&
            mousePos.y > m_transform.y && mousePos.y < m_transform.y + m_transform.h);
}
