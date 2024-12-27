#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include "../../core/src/config.h"
#include "Widget.h"

namespace application {

    class TextLine : public Widget
    {
    public:
        TextLine(int posX = 0, int posY = 0, int w = 100, int h = 20, SDL_Color color = {255, 255, 255, 255});
        ~TextLine() = default;

        void handleEvents(const core::InputManager& inputMngr) override {};
        void render(core::Renderer& renderer);

        int appendText(const std::string& text);
        void editText(int start, int end, const std::string& newText);

        void addFormatMap(const std::unordered_map<std::string, SDL_Color> &formatMap);
        bool useFont(const std::string& fontName, int size);

        void adjustWidhtToFont();
        void adjustHeightToFont();

        int getLenght() { return m_text.length(); }

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

    private:
        void updateTexture(core::Renderer& renderer);

        std::vector<int> countSpaces(const std::string& str);

    };

} // namespace application
