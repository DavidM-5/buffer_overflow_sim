#include "TextBlock.h"

application::TextBlock::TextBlock(int posX, int posY, int w, int h, SDL_Color color, u_int32_t* latesBreakpointLine) : 
                                  Widget(posX, posY, w, h, color), m_GUTTER_WIDTH(30),
                                  m_renderStartLine(0), m_ignoreNotFittedLine(false),
                                  m_latesBreakpointLine(*latesBreakpointLine)
{
}

void application::TextBlock::setText(std::string &text, bool ignoreNotFittedLine, bool clearBreakpoints)
{
    std::vector<Line> newLines;
    
    std::vector<std::string> lines = splitIntoLines(text);

    if (lines.empty())
        return;

    m_ignoreNotFittedLine = ignoreNotFittedLine;

    if (ignoreNotFittedLine) {
        for (int i = 0; i < lines.size(); i++) {
            Line ln = {
            .lineNumber = i+1, 
            .breakpoint = (!clearBreakpoints && i < m_lines.size()) ? m_lines[i].breakpoint : false,
            .textLine = application::TextLine(m_transform.x + m_GUTTER_WIDTH, 
                                              m_transform.y + i*20,
                                              m_transform.w - m_GUTTER_WIDTH, 
                                              20)
            };

            ln.textLine.useFont("JetBrainsMono-Medium.ttf", 14);
            ln.textLine.appendText(lines[i], true);

            newLines.push_back(ln);
        }

        m_text = text;

        for (Line& line : newLines) {
            line.textLine.addFormatMap(m_formatMap);
        }

        m_lines = newLines;
        return;
    }

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

        ln.textLine.useFont("JetBrainsMono-Medium.ttf", 14);
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

        newLines.push_back(ln);
        currentLine++;
    }
    
    m_text = text;
    for (Line& line : newLines) {
        line.textLine.addFormatMap(m_formatMap);
    }

    m_lines = newLines;
}

void application::TextBlock::setColorFormat(const std::unordered_map<std::string, SDL_Color> &formatMap)
{
    m_formatMap = formatMap;

    for (Line& line : m_lines) {
        line.textLine.addFormatMap(formatMap);
    }
}

void application::TextBlock::handleEvents(const core::InputManager &inputMngr)
{
    vector2i mousePos = inputMngr.getMousePosition();

    if (isMouseInsideTransform(mousePos)) {
        if (inputMngr.getMouseWheelScroll() != 0) {
            m_renderStartLine -= inputMngr.getMouseWheelScroll() * 3;

            if (m_renderStartLine < 0)
                m_renderStartLine = 0;
            else if (m_renderStartLine >= m_lines.size())
                m_renderStartLine = m_lines.size() - 1;
        }
        
        if (m_lines.size() > 0 && inputMngr.mouseClicked(MOUSE_BUTTON_LEFT)) {
            if (mousePos.x > m_transform.x && mousePos.x < m_transform.x + m_GUTTER_WIDTH) { // Is mouse in gutter
                int lineHeight = m_lines[0].textLine.getHeight();

                int lineNumber = (mousePos.y - m_transform.y) / lineHeight + 1;

                if (m_renderStartLine + lineNumber <= m_lines.size()) {
                    m_lines[m_renderStartLine + lineNumber - 1].breakpoint = !m_lines[m_renderStartLine + lineNumber - 1].breakpoint;

                    m_latesBreakpointLine = m_renderStartLine + lineNumber;
                }
            }
        }
    }
}

void application::TextBlock::render(core::Renderer &renderer, const SDL_Rect* srcRect, const SDL_Rect* dstRect)
{
    int ln = 0;
    for (int i = m_renderStartLine; m_transform.y + m_lines[0].textLine.getHeight() * ln < m_transform.y + m_transform.h - m_lines[0].textLine.getHeight(); i++, ln++) {
        if (i >= m_lines.size())
            continue;

        if (m_lines[i].breakpoint) {
            renderer.drawRect(SDL_Rect{m_transform.x,
                                m_transform.y + m_lines[0].textLine.getHeight() * ln,
                                m_lines[i].textLine.getHeight(),
                                m_lines[i].textLine.getHeight()},
                        SDL_Color{255, 0, 0, 255});
        }

        SDL_Rect dstRect{
            m_lines[i].textLine.getPosition().x,
            m_transform.y + m_lines[0].textLine.getHeight() * ln,
            m_lines[i].textLine.getWidth(),
            m_lines[i].textLine.getHeight()
        };
        m_lines[i].textLine.render(renderer, nullptr, &dstRect);
    }
}

void application::TextBlock::addDeltaTransform(int dx, int dy, int dw, int dh)
{
    Widget::addDeltaTransform(dx, dy, dw, dh);
    
    setText(m_text, m_ignoreNotFittedLine, false);
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
            lines.push_back(text.substr(startIdx, i - startIdx - 1));
            startIdx = i+1;
        }
    }

    // Push the last line if there is any remaining text
    if (startIdx != text.length()) {
        lines.push_back(text.substr(startIdx, text.length()));
    }

    return lines;
}

bool application::TextBlock::isMouseInsideTransform(vector2i mousePos)
{
    return (mousePos.x >= m_transform.x && mousePos.x < m_transform.x + m_transform.w &&
            mousePos.y >= m_transform.y && mousePos.y < m_transform.y + m_transform.h);
}
