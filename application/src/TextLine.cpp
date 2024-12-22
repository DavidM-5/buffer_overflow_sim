#include "TextLine.h"

std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> application::TextLine::s_fonts;
bool application::TextLine::s_ttfInitialized = false;

application::TextLine::TextLine(int posX, int posY, int w, int h, SDL_Color color): 
                                Widget(posX, posY, w, h, color),
                                m_text(""), m_updated(false), m_fontSize(0)
{
}

// TODO: make the render() function only reload the texture when the text updated
void application::TextLine::render(core::Renderer &renderer)
{
    // If no text or font is set, nothing to render
    if (m_text.empty() || m_fontName.empty() || m_fontSize <= 0) {
        return;
    }

    // Check if we have the font loaded
    if (s_fonts.find(m_fontName) == s_fonts.end()) {
        return;
    }

    if (s_fonts[m_fontName].find(m_fontSize) == s_fonts[m_fontName].end()) {
        return;
    }

    TTF_Font* font = s_fonts[m_fontName][m_fontSize];
    if (!font) {
        return;
    }


    std::vector<std::string> words;

    std::istringstream stream(m_text);
    std::string word;
    
    // Use getline with space as the delimiter
    while (std::getline(stream, word, ' ')) {
        if (!word.empty())
            words.push_back(word);
    }

    vector2i currPos = {m_transform.x, m_transform.y};

    for (std::string& w : words) {
        w += " ";
        core::Texture texture;

        // If no format map exists for this word, render with default color
        if (m_formatMap.find(w) == m_formatMap.end()) {
            if (!texture.loadFromText(w, font, m_mainColor, renderer))
                return;
        }
        else {
            if (!texture.loadFromText(w, font, m_formatMap[w], renderer))
                return;
        }

        SDL_Rect dstRect = { currPos.x, currPos.y, texture.getWidth(), texture.getHeight() };
        renderer.drawTexture(texture, nullptr, &dstRect);

        currPos.x += texture.getWidth();
    }
}

bool application::TextLine::appendText(const std::string text)
{
    if (m_fontName.empty() || m_fontSize <= 0)
        return false;

    std::string combinedText = m_text + text;
    
    // Get the width and height of the combined text
    int width;
    if (TTF_SizeText(s_fonts[m_fontName][m_fontSize], combinedText.c_str(), &width, nullptr) != 0)
        return false;

    if (width > m_transform.w)
        return false;

    m_text = combinedText;
    m_updated = true;
    return true;
}

void application::TextLine::editText(int start, int end, const std::string &newText)
{
    // Validate input parameters
    if (start < 0)
        start = 0;
    
    if (end > static_cast<int>(m_text.length()))
        end = static_cast<int>(m_text.length());
    
    if (start > end)
        std::swap(start, end);


    m_text.replace(start, end - start, newText);
    
    m_updated = true;
}

void application::TextLine::addFormatMap(const std::unordered_map<std::string, SDL_Color> &formatMap)
{
    m_formatMap = formatMap;
}

bool application::TextLine::useFont(const std::string &fontName, int size)
{
    if (s_fonts.find(fontName) == s_fonts.end())
        return false;
    
    if (s_fonts[fontName].find(size) == s_fonts[fontName].end())
        return false;

    m_fontName = fontName;
    m_fontSize = size;

    return true;
}

void application::TextLine::adjustWidhtToFont()
{
    if (m_fontName.empty() || m_fontSize <= 0)
        return;

    int width;
    TTF_SizeText(s_fonts[m_fontName][m_fontSize], m_text.c_str(), &width, nullptr);

    m_transform.w = width;
}

void application::TextLine::adjustHeightToFont()
{
    if (m_fontName.empty() || m_fontSize <= 0)
        return;

    int height;
    TTF_SizeText(s_fonts[m_fontName][m_fontSize], m_text.c_str(), &height, nullptr);

    m_transform.h = height;
}

bool application::TextLine::loadFont(const std::string &fontName, int size)
{
    if (!s_ttfInitialized) {
        TTF_Init();
        s_ttfInitialized = true;
    }
    
    // the font already loaded
    if (s_fonts[fontName].find(size) != s_fonts[fontName].end())
        return true;

    std::string fontPath = "resources/fonts/" + fontName;

    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), size);
    if (!font)
        return false;

    s_fonts[fontName][size] = font;

    return true;
}
