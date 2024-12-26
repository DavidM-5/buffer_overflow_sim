#include "TextLine.h"

std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> application::TextLine::s_fonts;
bool application::TextLine::s_ttfInitialized = false;

application::TextLine::TextLine(int posX, int posY, int w, int h, SDL_Color color): 
                                Widget(posX, posY, w, h, color),
                                m_text(""), m_updated(false), m_fontSize(0)
{
}

void application::TextLine::render(core::Renderer &renderer)
{
    if (m_updated) {
        updateTexture(renderer);
        m_updated = false;
    }

    SDL_Rect dstRect = { m_transform.x, m_transform.y, m_texture.getWidth(), m_texture.getHeight() };
    renderer.drawTexture(m_texture, nullptr, &dstRect);
}

int application::TextLine::appendText(const std::string text)
{
    if (m_fontName.empty() || m_fontSize <= 0)
        return -1;

    // Handle empty input text
    if (text.empty()) {
        return 0;
    }

    std::istringstream wordStream(text);
    std::string word;
    int wordsNotAdded = 0;
    bool isFirstWord = m_text.empty();

    while (wordStream >> word) {
        // Calculate width for this word with proper spacing
        std::string testText = isFirstWord ? word : m_text + " " + word;
        int width;
        
        if (TTF_SizeText(s_fonts[m_fontName][m_fontSize], testText.c_str(), &width, nullptr) != 0) {
            return -1;  // Font rendering error
        }

        if (width <= m_transform.w - 20) {
            // Word fits, update the text
            m_text = testText;
            isFirstWord = false;
        } else {
            // If this is the first word and it doesn't fit, it will never fit
            if (isFirstWord) {
                return 1;  // Return 1 to indicate this word couldn't be added (the word too long)
            }
            ++wordsNotAdded;
        }
    }

    m_updated = true;
    return wordsNotAdded;
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

void application::TextLine::updateTexture(core::Renderer &renderer)
{
    // Early validation checks
    if (m_text.empty() || m_fontName.empty() || m_fontSize <= 0 ||
        s_fonts.find(m_fontName) == s_fonts.end() ||
        s_fonts[m_fontName].find(m_fontSize) == s_fonts[m_fontName].end()) {
        return;
    }

    TTF_Font* font = s_fonts[m_fontName][m_fontSize];
    if (!font) return;

    // Create surface for the complete text line
    SDL_Surface* completeSurface = SDL_CreateRGBSurface(0, m_transform.w, m_transform.h, 32, 
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    if (!completeSurface) return;

    // Parse words
    std::istringstream stream(m_text);
    std::string word;
    int currentX = 0;
    
    while (std::getline(stream, word, ' ')) {
        if (word.empty()) continue;
        word += " ";

        // Create surface for single word
        SDL_Surface* wordSurface;
        SDL_Color color = (m_formatMap.find(word) != m_formatMap.end()) ? 
                            m_formatMap[word] : m_mainColor;
                         
        wordSurface = TTF_RenderText_Blended(font, word.c_str(), color);
        if (!wordSurface) continue;

        // Blit word surface to complete surface
        SDL_Rect dstRect = { currentX, 0, wordSurface->w, wordSurface->h };
        SDL_BlitSurface(wordSurface, nullptr, completeSurface, &dstRect);
        
        currentX += wordSurface->w;
        SDL_FreeSurface(wordSurface);
    }

    // Convert final surface to texture
    m_texture.loadFromSurface(completeSurface, renderer);
    SDL_FreeSurface(completeSurface);
}
