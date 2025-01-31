#include "TextLine.h"

std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> application::TextLine::s_fonts;
bool application::TextLine::s_ttfInitialized = false;

application::TextLine::TextLine(int posX, int posY, int w, int h, SDL_Color color): 
                                Widget(posX, posY, w, h, color),
                                m_text(""), m_updated(false), m_fontSize(0),
                                m_ignoreNotFitted(false)
{
}

void application::TextLine::render(core::Renderer &renderer, const SDL_Rect* srcRect, const SDL_Rect* dstRect)
{
    if (m_text.empty()) {
        return;
    }

    if (m_updated) {
        updateTexture(renderer);
        m_updated = false;
    }

    // If no destination rect provided, use the transform
    SDL_Rect defaultDstRect = { 
        m_transform.x, 
        m_transform.y, 
        m_texture.getWidth(), 
        m_texture.getHeight() 
    };

    // If no source rect provided, use the full texture
    SDL_Rect defaultSrcRect = { 
        0, 
        0, 
        m_texture.getWidth(), 
        m_texture.getHeight() 
    };
    
    // Use provided rects if available, otherwise use defaults
    renderer.drawTexture(
        m_texture,
        srcRect ? srcRect : &defaultSrcRect,
        dstRect ? dstRect : &defaultDstRect
    );
}

int application::TextLine::appendText(const std::string& text, bool ignoreNotFitted)
{
    if (m_fontName.empty() || m_fontSize <= 0)
        return -1;

    // Handle empty input text
    if (text.empty()) {
        return 0;
    }

    m_ignoreNotFitted = ignoreNotFitted;
    
    if (ignoreNotFitted) {
        m_text += text;
        m_updated = true;

        return 0;
    }

    std::istringstream wordStream(text);
    std::string word;

    int wordsNotAdded = 0;
    bool hasAddedWords = false;
    std::string newText = m_text; // Work with a copy until we're sure all words fit
    
    int letterWidth;
    TTF_SizeText(s_fonts[m_fontName][m_fontSize], "A", &letterWidth, nullptr);

    int currentWidth;
    TTF_SizeText(s_fonts[m_fontName][m_fontSize], newText.c_str(), &currentWidth, nullptr);

    int spaceIndex = 0;
    std::vector<int> spaces = countSpaces(text);
    
    // First pass: count total words
    std::string tempText = text;
    std::istringstream countStream(tempText);
    int totalWords = spaces.size() - 1;

    int i = 0;
    while (i < text.length() - 1 && text[i] == ' ') {
        newText += " ";
        i++;
    }
    
    // Second pass: try to add words
    while (wordStream >> word) {
        std::string testWord = (newText.empty() ? "" : std::string(spaces[spaceIndex], ' ')) + word;
        int wordWidth;
        
        if (TTF_SizeText(s_fonts[m_fontName][m_fontSize], testWord.c_str(), &wordWidth, nullptr) != 0) {
            return -1;
        }

        // If m_text is empty, add the first word regardless of width
        if (m_text.empty() && !hasAddedWords) {
            newText += word;
            currentWidth += wordWidth;
            hasAddedWords = true;
        }
        else if (currentWidth + wordWidth <= m_transform.w - letterWidth) {
            newText += testWord;
            currentWidth += wordWidth;
            hasAddedWords = true;
        } else {
            // If we can't add this word, we can't add any more words
            wordsNotAdded = totalWords - (spaceIndex);
            break;
        }
        
        spaceIndex++;
    }

    // Only update m_text if we successfully added at least one word
    if (hasAddedWords) {
        // Add trailing spaces only if all words fit
        if (wordsNotAdded == 0 && spaceIndex < spaces.size()) {
            newText += std::string(spaces[spaceIndex], ' ');
        }
        m_text = newText;
        m_updated = true;
    }
    
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

    m_updated = true;
}

bool application::TextLine::useFont(const std::string &fontName, int size)
{
    if (!loadFont(fontName, size))
        return false;

    m_fontName = fontName;
    m_fontSize = size;

    return true;
}

void application::TextLine::clear()
{
    m_text.clear();

    m_updated = true;
}

void application::TextLine::fitWidthToText()
{
    if (m_fontName.empty() || m_fontSize <= 0)
        return;

    int width;
    TTF_SizeText(s_fonts[m_fontName][m_fontSize], m_text.c_str(), &width, nullptr);

    m_transform.w = width;
    m_updated = true;
}

void application::TextLine::fitHeightToText()
{
    if (m_fontName.empty() || m_fontSize <= 0)
        return;

    int height;
    TTF_SizeText(s_fonts[m_fontName][m_fontSize], m_text.c_str(), &height, nullptr);

    m_transform.h = height;
    m_updated = true;
}

void application::TextLine::addDeltaTransform(int x, int y, int w, int h)
{
    Widget::addDeltaTransform(x, y, w, h);

    m_updated = true;
}

void application::TextLine::setWidth(int newW)
{
    Widget::setWidth(newW);
    
    m_updated = true;
}

void application::TextLine::setHeight(int newH)
{
    Widget::setHeight(newH);

    m_updated = true;
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

void application::TextLine::updateTexture(core::Renderer &renderer, int centerHorizontally)
{
    // Early validation checks
    if (m_text.empty() || m_fontName.empty() || m_fontSize <= 0 ||
        s_fonts.find(m_fontName) == s_fonts.end() ||
        s_fonts[m_fontName].find(m_fontSize) == s_fonts[m_fontName].end()) {

        m_updated = false;
        return;
    }

    TTF_Font* font = s_fonts[m_fontName][m_fontSize];
    if (!font) return;

    // Create surface for the complete text line
    SDL_Surface* completeSurface = SDL_CreateRGBSurface(0, m_transform.w, m_transform.h, 32, 
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    if (!completeSurface) return;

    // Calculate space width
    SDL_Surface* spaceSurface = TTF_RenderText_Blended(font, " ", m_mainColor);
    int spaceWidth = spaceSurface ? spaceSurface->w : 0;
    SDL_FreeSurface(spaceSurface);

    size_t pos = 0;
    int currentX = 0;
    std::string word;
    
    while (pos < m_text.length()) {
        // Handle consecutive spaces
        if (m_text[pos] == ' ') {
            while (pos < m_text.length() && m_text[pos] == ' ') {
                currentX += spaceWidth;
                pos++;
            }
            continue;
        }

        // Extract word
        size_t wordEnd = m_text.find(' ', pos);
        if (wordEnd == std::string::npos) {
            word = m_text.substr(pos);
            pos = m_text.length();
        } else {
            word = m_text.substr(pos, wordEnd - pos);
            pos = wordEnd;
        }

        if (!word.empty()) {
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
    }

    // Convert final surface to texture
    m_texture.loadFromSurface(completeSurface, renderer);
    
    SDL_FreeSurface(completeSurface);

    m_updated = false;
}

std::vector<int> application::TextLine::countSpaces(const std::string &str)
{
    std::istringstream iss(str);
    int wordCount = 0;
    std::string word;

    while (iss >> word) {
        wordCount++;
    }
    
    std::vector<int> spaces;

    int i = 0;
    int currentCount = 0;
    int vecindex = 0;

    if (str[0] != ' ')
        spaces.push_back(0);

    do {
        if (str[i] == ' ') {
            ++currentCount;
        }
        else if (currentCount > 0){
            spaces.push_back(currentCount);
            currentCount = 0;
        }

        ++i;
    } while (i < str.length());
    

    spaces.push_back(currentCount);

    return spaces;
}
