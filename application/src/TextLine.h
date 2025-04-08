#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include <algorithm>
#include "../../core/src/config.h"
#include "Widget.h"

namespace application {

    class TextLine : public Widget
    {
    public:
        TextLine(int posX = 0, int posY = 0, int w = 100, int h = 20, SDL_Color color = {255, 255, 255, 255}, bool applyFormatting = true);
        TextLine(const TextLine& other); // Copy constructor
        ~TextLine() = default;

        // Add assignment operator
        TextLine& operator=(const TextLine& other);

        void handleEvents(const core::InputManager& inputMngr) override {};
        void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);

        int appendText(const std::string& text, bool ignoreNotFitted = false);
        void editText(int start, int end, const std::string& newText);

        void addFormatMap(const std::unordered_map<std::string, SDL_Color> &formatMap);
        bool useFont(const std::string& fontName, int size);

        void clear();

        void fitWidthToText();
        void fitHeightToText();

        int getFontSize() {return m_fontSize; }

        void addDeltaTransform(int x = 0, int y = 0, int w = 0, int h = 0);
        void setWidth(int newW);
        void setHeight(int newH);
        void setColor(const SDL_Color &color);

        int getLenght() { return m_text.length(); }
        std::string getText() { return m_text; }

        bool isTextureValid() const { return m_texture.isValid(); }

        static bool loadFont(const std::string& fontName, int size);

    private:
        // unordered map [fonat name][font size] = ttf_font
        static std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> s_fonts;
        static bool s_ttfInitialized;

        std::unordered_map<std::string, SDL_Color> m_formatMap;

        std::string m_text;
        std::string m_fontName;
        int m_fontSize;

        core::Texture m_texture;
        bool m_updated;

        bool m_applyFormatting;

        bool m_ignoreNotFitted;

    private:
        void updateTexture(core::Renderer& renderer, int centerHorizontally = 0);

        std::vector<int> countSpaces(const std::string& str);

    };

} // namespace application
