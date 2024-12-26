#include "TextBlock.h"

application::TextBlock::TextBlock(int posX, int posY, int w, int h, SDL_Color color) : 
                                  Widget(posX, posY, w, h, color)
{
}

void application::TextBlock::setText(std::string &text)
{
    m_lines.clear();
    std::string currentLine;
    std::string word;

    for (char c : text) {
        if (c == '\n') {
            if (!word.empty())
                currentLine += word;

            m_lines.push_back(currentLine);
            word.clear();
            currentLine.clear();
            continue;
        }
        else if (c == ' ') {
            int lineWidth = 0, wordWidth = 0;
            TTF_SizeText(m_font, word.c_str(), &wordWidth, nullptr);
            TTF_SizeText(m_font, currentLine.c_str(), &lineWidth, nullptr);

            if (lineWidth + wordWidth >= m_lineWidth) {
                m_lines.push_back(currentLine);
                currentLine.clear();
            }

            currentLine += (word += ' ');
            word.clear();
            continue;
        }
        else {
            word += c;
        }  
    }

    // if the text does not end with a new line character
    if (!word.empty()) {
        int lineWidth = 0, wordWidth = 0;
        TTF_SizeText(m_font, word.c_str(), &wordWidth, nullptr);
        TTF_SizeText(m_font, currentLine.c_str(), &lineWidth, nullptr);

        if (lineWidth + wordWidth >= m_lineWidth) {
            m_lines.push_back(currentLine);
            currentLine.clear();
            currentLine += (word += ' ');
        }
        else
            currentLine += (word += ' ');
    }
    m_lines.push_back(currentLine);

    m_updated = true;
}

void application::TextBlock::setColorFormat(const std::unordered_map<std::string, SDL_Color> &formatMap)
{
    m_colorFormatMap = formatMap;
    m_updated = true;
}

void application::TextBlock::render(core::Renderer& renderer, int x, int y)
{
    if (m_updated)
        updateTexture(renderer);

    // Render the cached texture
    SDL_Rect dstRect = {x, y, m_textTexture.getWidth(), m_textTexture.getHeight()};
    renderer.drawTexture(m_textTexture, nullptr, &dstRect);
}

void application::TextBlock::updateTexture(core::Renderer &renderer)
{
    // Create a large enough surface to hold the text block
    int totalHeight = m_lineHeight * static_cast<int>(m_lines.size());
    int totalWidth = m_lineWidth;

    SDL_Surface* textSurface = SDL_CreateRGBSurfaceWithFormat(0, totalWidth, totalHeight, 32, SDL_PIXELFORMAT_RGBA32);
    if (!textSurface) {
        std::cerr << "Failed to create surface for text block: " << SDL_GetError() << std::endl;
        return;
    }

    // Fill the surface with transparent background
    SDL_FillRect(textSurface, nullptr, SDL_MapRGBA(textSurface->format, 0, 0, 0, 0));

    // Render each line to the surface
    int yOffset = 0;
    for (const auto& line : m_lines) {
        int xOffset = 0;
        std::istringstream stream(line);
        std::string word;

        while (stream >> word) {
            SDL_Color wordColor = m_defaultColor;
            if (m_colorFormatMap.find(word) != m_colorFormatMap.end()) {
                wordColor = m_colorFormatMap[word];
            }

            // Render each word to a temporary surface
            SDL_Surface* wordSurface = TTF_RenderText_Blended(m_font, word.c_str(), wordColor);
            if (!wordSurface) {
                std::cerr << "Failed to render word: " << word << " Error: " << TTF_GetError() << std::endl;
                continue;
            }

            // Blit word onto the main surface
            SDL_Rect dstRect = {xOffset, yOffset, wordSurface->w, wordSurface->h};
            SDL_BlitSurface(wordSurface, nullptr, textSurface, &dstRect);

            // Update xOffset and free word surface
            xOffset += wordSurface->w + 5; // Add spacing between words
            SDL_FreeSurface(wordSurface);
        }

        yOffset += m_lineHeight;
    }

    if (!m_textTexture.loadFromSurface(textSurface, renderer)) {
        std::cerr << "Failed to create texture from surface!" << std::endl;
    }

    SDL_FreeSurface(textSurface);
    m_updated = false; // Texture is now up-to-date
}
