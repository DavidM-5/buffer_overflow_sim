#include "TextBlock.h"

application::TextBlock::TextBlock(int posX, int posY, int w, int h, SDL_Color color) : 
                                  Widget(posX, posY, w, h, color), m_GUTTER_WIDTH(30),
                                  m_topLineForRender(1)
{
}

void application::TextBlock::setText(std::string &text)
{
    m_lines.clear();
    
    std::vector<std::string> lines = splitIntoLines(text);

    if (lines.empty())
        return;

    int currentLine = 0;
    while (currentLine < lines.size()) {
        Line ln = {
            .lineNumber = currentLine+1, 
            .breakpoint = false,
            .textLine = application::TextLine(m_transform.x + m_GUTTER_WIDTH, 
                                              m_transform.y + currentLine*20,
                                              m_transform.w - m_GUTTER_WIDTH, 
                                              20)
        };

        ln.textLine.useFont("Arial.ttf", 16);
        int remainingWords = ln.textLine.appendText(lines[currentLine]);
        
        if (remainingWords > 0) {
            bool spaceEncountered = false;

            int i = lines[currentLine].length() - 1;
            
            while (lines[currentLine][i] == ' ')
                i--;
            
            for (i; i >= 0; i--) {
                if (lines[currentLine][i] == ' ') {
                    if (spaceEncountered)
                        continue;
                    
                    spaceEncountered = true;
                    remainingWords--;
                }
                else {
                    spaceEncountered = false;
                }

                if (remainingWords == 0)
                    break;
            }

            std::string remaining = lines[currentLine].substr(i, lines[currentLine].size()-i);
            lines.insert(lines.begin() + currentLine + 1, remaining);
        }

        m_lines.push_back(ln);
        currentLine++;
    }

    m_text = text;
    
}

void application::TextBlock::setColorFormat(const std::unordered_map<std::string, SDL_Color> &formatMap)
{
    for (Line& line : m_lines) {
        line.textLine.addFormatMap(formatMap);
    }
}

// TODO: only render the lines that are activly shown
void application::TextBlock::render(core::Renderer &renderer)
{
    renderer.drawRect(m_transform, m_mainColor);
    
    for (Line& line : m_lines) {
        if (line.textLine.getPosition().y < m_transform.y + m_transform.h &&
            line.textLine.getPosition().y >= m_transform.y)
            line.textLine.render(renderer);
    }
}

void application::TextBlock::addDeltaTransform(int dx, int dy, int dw, int dh)
{
    Widget::addDeltaTransform(dx, dy, dw, dh);

    setText(m_text);
}

void application::TextBlock::setPosition(vector2i newPos)
{
    m_transform.x = newPos.x;
    m_transform.y = newPos.y;

    for (Line& line : m_lines) {
        line.textLine.setPosition( { m_transform.x + m_GUTTER_WIDTH, m_transform.y + (line.textLine.getHeight() * (line.lineNumber - 1)) } );
    }
}

void application::TextBlock::setWidth(int newW)
{
    m_transform.w = newW;

    for (Line& line : m_lines) {
        line.textLine.setWidth(newW);
    }
}

std::vector<std::string> application::TextBlock::splitIntoLines(std::string &text)
{
    std::vector<std::string> lines;
    int startIdx = 0;
    
    for (int i = 0; i < text.length(); i++) {
        if (text[i] == '\n') {
            lines.push_back(text.substr(startIdx, i - startIdx));
            startIdx = i+1;
        }
    }

    // Push the last line if there is any remaining text
    if (startIdx != text.length()) {
        lines.push_back(text.substr(startIdx, text.length()));
    }

    return lines;
}
