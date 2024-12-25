#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

namespace core
{
    class Renderer;
    
    class Texture
    {
        friend class Renderer;

    public:
        Texture(/* args */);
        ~Texture();

        bool loadFromText(const std::string& text, TTF_Font* font, const SDL_Color& color, core::Renderer& renderer);
        bool loadFromSurface(SDL_Surface* surface, const core::Renderer& renderer);

        bool isValid();

        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }
    
    private:
        SDL_Texture* m_texture;
        int m_width, m_height;

        void destroy();

    };

} // namespace core

#include "Renderer.h"
