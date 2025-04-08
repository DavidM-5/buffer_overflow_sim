#pragma once

#include <vector>
#include <string>
#include "TextLine.h"

namespace application
{
    
    class Paragraph : public Widget
    {
    public:
        Paragraph(int posX = 0, int posY = 0, int w = 100, int h = 20, SDL_Color color = {255, 255, 255, 255}, vector2i globalPos = {0, 0}, 
                    const std::string& fontName = "JetBrainsMono-Bold.ttf", int fontSize = 16, 
                    int lineHeight = 20);

        Paragraph(const Paragraph& other); // Copy constructor
        ~Paragraph() = default;

        void handleEvents(const core::InputManager& inputMngr) override {};
        void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);

        void setText(const std::string& text);
        void appendText(const std::string& text);
        // void editText(int start, int end, const std::string& newText);

        void clear();

        std::string getText() { return m_text; }

        void addDeltaTransform(int x = 0, int y = 0, int w = 0, int h = 0);
        void setWidth(int newW);
        void setHeight(int newH);

        void setColor(const SDL_Color &color);

    private:
        std::vector<TextLine> m_lines;

        std::string m_text;

        std::string m_fontName;
        int m_fontSize;

        int m_lineHeight;
        int m_numOfLines;
        int m_lineSpacing;

        vector2i m_globalPosition;

    private:
        void createNewLine(int yOffset);

    };

} // namespace application

