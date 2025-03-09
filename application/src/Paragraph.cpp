#include "Paragraph.h"

application::Paragraph::Paragraph(int posX, int posY, int w, int h, SDL_Color color, vector2i globalPos, const std::string &fontName, int fontSize, int lineHeight) :
                                    Widget(posX, posY, w, h, color),
                                    m_globalPosition(globalPos),
                                    m_fontName(fontName), m_fontSize(fontSize),
                                    m_lineHeight(lineHeight),
                                    m_numOfLines(0),
                                    m_lineSpacing(5)
{
}

application::Paragraph::Paragraph(const Paragraph &other) :
                                    Widget(other),
                                    m_lines(other.m_lines),
                                    m_fontName(other.m_fontName),
                                    m_fontSize(other.m_fontSize),
                                    m_text(other.m_text),
                                    m_numOfLines(other.m_numOfLines),
                                    m_lineHeight(other.m_lineHeight)
{
}

void application::Paragraph::render(core::Renderer &renderer, const SDL_Rect *srcRect, const SDL_Rect *dstRect)
{
    Widget::render(renderer);

    for (application::TextLine& tline : m_lines) {
        tline.render(renderer);
    }
}

void application::Paragraph::setText(const std::string &text)
{
    clear();
    appendText(text);
}

void application::Paragraph::appendText(const std::string &text)
{
    if (text.empty())
        return;
        
    m_text += text;
    std::string textToAdd = text;
    
    // If we don't have any lines yet, create the first one
    if (m_lines.empty()) {
        createNewLine(0);
    }
    
    while (!textToAdd.empty()) {
        // Get the last line
        TextLine& currentLine = m_lines.back();
        
        // Try to add text to the current line
        int charsAdded = currentLine.appendText(textToAdd);
        
        // Error or no characters added
        if (charsAdded <= 0) {
            // Create a new line and try again
            createNewLine(m_lines.size() * (m_lineHeight + m_lineSpacing));
            continue;
        }
        
        // All text fit
        if (static_cast<size_t>(charsAdded) == textToAdd.length()) {
            break;
        }
        
        // Some text didn't fit, find the last complete word
        std::string addedText = currentLine.getText();
        int lastWordBoundary = addedText.length() - 1;
        
        // Find the last space character
        while (lastWordBoundary >= 0 && addedText[lastWordBoundary] != ' ') {
            lastWordBoundary--;
        }
        
        if (lastWordBoundary >= 0) {
            // Found a space, remove the partial word from current line
            std::string partialWord = addedText.substr(lastWordBoundary + 1);
            currentLine.editText(lastWordBoundary + 1, addedText.length(), "");
            
            // Adjust the remaining text to include the partial word
            textToAdd = partialWord + textToAdd.substr(charsAdded);
        } else {
            // No space found, just continue with remaining text
            textToAdd = textToAdd.substr(charsAdded);
        }
        
        // Create a new line for the next part
        createNewLine(m_lines.size() * (m_lineHeight + m_lineSpacing));
    }
}

void application::Paragraph::createNewLine(int yOffset)
{
    TextLine newLine(
        m_globalPosition.x + m_transform.x,                // X position same as paragraph
        m_globalPosition.y + m_transform.y + yOffset,      // Y position based on line number
        m_transform.w,                                     // Width same as paragraph
        m_lineHeight,                                      // Height from settings
        m_mainColor,                                       // Use same color
        false                                              // Don't apply formatting by default
    );
    
    // Apply font settings
    newLine.useFont(m_fontName, m_fontSize);
    
    m_lines.push_back(newLine);
}

void application::Paragraph::clear()
{
    for (application::TextLine& tline : m_lines) {
        tline.clear();
    }

    m_text.clear();
}

void application::Paragraph::addDeltaTransform(int x, int y, int w, int h)
{
    Widget::addDeltaTransform(x, y, w, h);

    for (TextLine& tline : m_lines) {
        tline.addDeltaTransform(x, y, w, h);
    }
}

void application::Paragraph::setWidth(int newW)
{
    Widget::setWidth(newW);

    for (TextLine& tline : m_lines) {
        tline.setWidth(newW);
    }
}

void application::Paragraph::setHeight(int newH)
{
    Widget::setHeight(newH);

    for (TextLine& tline : m_lines) {
        tline.setHeight(newH);
    }
}
