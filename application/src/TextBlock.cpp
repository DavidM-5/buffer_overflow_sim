#include "TextBlock.h"

application::TextBlock::TextBlock(int posX, int posY, int w, int h, SDL_Color color) : 
                                  Widget(posX, posY, w, h, color), m_GUTTER_WIDTH(30)
{
}

void application::TextBlock::setText(std::string &text)
{
    // split the text by lines
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    for (int i = 0; i < lines.size(); i++) {
        m_lines.emplace_back(Line{ i+1,
                                   false,
                                   application::TextLine(m_transform.x + m_GUTTER_WIDTH, 
                                                         m_transform.y + i*20,
                                                         m_transform.w - m_GUTTER_WIDTH, 
                                                         20)
                                  });

        m_lines[i].textLine.useFont("Arial.ttf", 16);
        int remainingWords = m_lines[i].textLine.appendText(lines[i]);

        if (remainingWords > 0) {
            int spaceCount = 0;
            bool continuoseSpace = false;
            
            int j;
            for (j = lines[i].length()-1; j >= 0; j--) {
                if (spaceCount == remainingWords)
                    break;

                if (lines[i][j] == ' ') {
                    if (continuoseSpace) {
                        continue;
                    }
                    else if (!continuoseSpace) {
                        ++spaceCount;
                        continuoseSpace = true;
                    }  
                }
                else {
                    continuoseSpace = false;
                }
            }

            std::string remainingText = lines[i].substr(j+1);
            lines.insert(lines.begin() + i + 1, remainingText);
        }
    }
}

void application::TextBlock::setColorFormat(const std::unordered_map<std::string, SDL_Color> &formatMap)
{
    for (Line& line : m_lines) {
        line.textLine.addFormatMap(formatMap);
    }
}

void application::TextBlock::render(core::Renderer &renderer)
{
    renderer.drawRect(m_transform, SDL_Color{255, 123, 42, 192});
    
    for (Line& line : m_lines) {
        line.textLine.render(renderer);
    }
}
