#pragma once

#include <SDL2/SDL.h>

#include "Texture.h"

namespace core
{
    
    class Renderer
    {
        friend class Texture;

    public:
        Renderer(/* args */);
        ~Renderer();

        bool init(SDL_Window* window);

        void clear(const SDL_Color& color);
        void drawRect(const SDL_Rect& rect, const SDL_Color& color);
        void drawRectBorder(const SDL_Rect& rect, const SDL_Color& color);
        void drawTexture(const core::Texture& texture, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);

        void present();
    
    private:
        SDL_Renderer* m_renderer;

    };

} // namespace core
