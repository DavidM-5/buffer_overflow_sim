#include "TextLine.h"

std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> application::TextLine::s_fonts;
bool application::TextLine::s_ttfInitialized = false;

application::TextLine::TextLine(int posX, int posY, int w, int h, SDL_Color color, bool applyFormatting): 
                                Widget(posX, posY, w, h, color),
                                m_text(""), m_updated(false), m_fontSize(0),
                                m_ignoreNotFitted(false), m_applyFormatting(applyFormatting)
{
}

application::TextLine::TextLine(const TextLine &other) : Widget(other),  // Copy base Widget properties
                                                        m_formatMap(other.m_formatMap),
                                                        m_text(other.m_text),
                                                        m_fontName(other.m_fontName),
                                                        m_fontSize(other.m_fontSize),
                                                        m_texture(),  // Create new empty texture
                                                        m_updated(true),  // Mark as needing update
                                                        m_applyFormatting(other.m_applyFormatting),
                                                        m_ignoreNotFitted(other.m_ignoreNotFitted)
{
}

application::TextLine &application::TextLine::operator=(const TextLine &other)
{
    if (this != &other) {
        Widget::operator=(other);  // Copy base Widget properties
        
        m_formatMap = other.m_formatMap;
        m_text = other.m_text;
        m_fontName = other.m_fontName;
        m_fontSize = other.m_fontSize;
        m_ignoreNotFitted = other.m_ignoreNotFitted;
        
        m_updated = false;  // Mark as needing update
        // m_texture will be updated on next render
    }
    return *this;
}

void application::TextLine::render(core::Renderer &renderer, const SDL_Rect *srcRect, const SDL_Rect *dstRect)
{
    if (m_text.empty()) {
        return;
    }

    if (m_updated || !m_texture.isValid()) {
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

    // If ignoring fit constraints, simply append the text and return
    if (ignoreNotFitted) {
        m_text += text;
        m_updated = true;
        return 0;
    }

    // Get the font
    TTF_Font* font = s_fonts[m_fontName][m_fontSize];
    if (!font) return -1;

    // Measure the current width of the text
    int currentWidth = 0;
    if (!m_text.empty()) {
        TTF_SizeText(font, m_text.c_str(), &currentWidth, nullptr);
    }

    // Try adding characters one by one to find where it stops fitting
    std::string textToAdd;
    for (size_t i = 0; i < text.size(); i++) {
        char c = text[i];
        std::string charToAdd = (c == '\t') ? "  " : std::string(1, c);
        
        // Test if adding this character would exceed width
        int testWidth;
        TTF_SizeText(font, (m_text + textToAdd + charToAdd).c_str(), &testWidth, nullptr);
        
        if (testWidth <= m_transform.w) {
            // Character fits, add it
            textToAdd += charToAdd;
        } else {
            // Character doesn't fit - return the exact position where it stops fitting
            m_text += textToAdd;
            m_updated = true;
            return i;
        }
    }
    
    // All text fit
    m_text += textToAdd;
    m_updated = true;
    return text.size();
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
    m_texture = core::Texture();

    m_updated = true;
}

void application::TextLine::fitWidthToText()
{
    if (m_fontName.empty() || m_fontSize <= 0)
        return;

    if (!m_texture.isValid())
        return;

    m_transform.w = m_texture.getWidth();
    m_updated = true;
}

void application::TextLine::fitHeightToText()
{
    if (m_fontName.empty() || m_fontSize <= 0)
        return;

    if (!m_texture.isValid())
        return;

    m_transform.h = m_texture.getHeight();
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

void application::TextLine::setColor(const SDL_Color &color)
{
    Widget::setColor(color);

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
    if (m_fontName.empty() || m_fontSize <= 0 ||
        s_fonts.find(m_fontName) == s_fonts.end() ||
        s_fonts[m_fontName].find(m_fontSize) == s_fonts[m_fontName].end()) {

        m_updated = false;
        return;
    }

    TTF_Font* font = s_fonts[m_fontName][m_fontSize];
    if (!font) return;

    // Handle empty text case
    if (m_text.empty()) {
        // Create an empty surface with the dimensions of the text line
        SDL_Surface* emptySurface = SDL_CreateRGBSurface(0, m_transform.w, m_transform.h, 32, 
            0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
        if (!emptySurface) return;

        // Convert the empty surface to a texture
        m_texture.loadFromSurface(emptySurface, renderer);
        SDL_FreeSurface(emptySurface);

        m_updated = false;
        return;
    }

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
    bool inDoubleQuotes = false; // Track if we're inside double quotes
    bool inSingleQuotes = false; // Track if we're inside single quotes

    while (pos < m_text.length()) {
        // Handle double quotes
        if (m_text[pos] == '"') {
            std::string quoteChar = "\"";
            SDL_Color quoteColor = m_mainColor; // Default color

            // Apply formatting only if enabled
            if (m_applyFormatting) {
                quoteColor = m_formatMap.find(quoteChar) != m_formatMap.end() ? 
                             m_formatMap[quoteChar] : m_mainColor;
            }

            // Render the quote character
            SDL_Surface* quoteSurface = TTF_RenderText_Blended(font, quoteChar.c_str(), quoteColor);
            if (quoteSurface) {
                SDL_Rect dstRect = { currentX, 0, quoteSurface->w, quoteSurface->h };
                SDL_BlitSurface(quoteSurface, nullptr, completeSurface, &dstRect);
                currentX += quoteSurface->w;
                SDL_FreeSurface(quoteSurface);
            }

            inDoubleQuotes = !inDoubleQuotes; // Toggle quote state
            pos++;
            continue;
        }

        // Handle single quotes
        if (m_text[pos] == '\'') {
            std::string singleQuoteChar = "'";
            SDL_Color singleQuoteColor = m_mainColor; // Default color

            // Apply formatting only if enabled
            if (m_applyFormatting) {
                singleQuoteColor = m_formatMap.find(singleQuoteChar) != m_formatMap.end() ? 
                                   m_formatMap[singleQuoteChar] : m_mainColor;
            }

            // Render the single quote character
            SDL_Surface* singleQuoteSurface = TTF_RenderText_Blended(font, singleQuoteChar.c_str(), singleQuoteColor);
            if (singleQuoteSurface) {
                SDL_Rect dstRect = { currentX, 0, singleQuoteSurface->w, singleQuoteSurface->h };
                SDL_BlitSurface(singleQuoteSurface, nullptr, completeSurface, &dstRect);
                currentX += singleQuoteSurface->w;
                SDL_FreeSurface(singleQuoteSurface);
            }

            inSingleQuotes = !inSingleQuotes; // Toggle quote state
            pos++;
            continue;
        }

        // Handle spaces
        if (m_text[pos] == ' ') {
            currentX += spaceWidth;
            pos++;
            continue;
        }

        // Extract the current segment
        size_t segmentEnd = m_text.find_first_of(" \"'\t\n\r", pos); // Split on whitespace or special characters
        if (segmentEnd == std::string::npos) {
            segmentEnd = m_text.length();
        }

        std::string segment = m_text.substr(pos, segmentEnd - pos);

        if (!segment.empty()) {
            SDL_Surface* segmentSurface;
            SDL_Color color = m_mainColor; // Default color

            // Apply formatting only if enabled
            if (m_applyFormatting) {
                // Check if we're inside quotes
                if (inDoubleQuotes) {
                    std::string quoteKey = "\"";
                    color = m_formatMap.find(quoteKey) != m_formatMap.end() ? 
                            m_formatMap[quoteKey] : m_mainColor;
                } else if (inSingleQuotes) {
                    std::string singleQuoteKey = "'";
                    color = m_formatMap.find(singleQuoteKey) != m_formatMap.end() ? 
                            m_formatMap[singleQuoteKey] : m_mainColor;
                } else if (m_formatMap.find(segment) != m_formatMap.end()) {
                    // Handle other formatted segments
                    color = m_formatMap[segment];
                }
            }

            // Render the segment
            segmentSurface = TTF_RenderText_Blended(font, segment.c_str(), color);
            if (segmentSurface) {
                SDL_Rect dstRect = { currentX, 0, segmentSurface->w, segmentSurface->h };
                SDL_BlitSurface(segmentSurface, nullptr, completeSurface, &dstRect);
                currentX += segmentSurface->w;
                SDL_FreeSurface(segmentSurface);
            }
        }

        // Move to the next segment
        pos = segmentEnd;
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
