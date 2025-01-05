#include "Renderer.h"

core::Renderer::Renderer() : m_renderer(nullptr)
{
}

core::Renderer::~Renderer()
{
    SDL_DestroyRenderer(m_renderer);
}

bool core::Renderer::init(SDL_Window *window)
{
    m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_renderer == nullptr)
        return false;

    // Enable blending for transparency
    if (SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND) != 0)
        return false;
    
    return true;
}

void core::Renderer::clear(const SDL_Color &color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(m_renderer);
}

void core::Renderer::drawRect(const SDL_Rect &rect, const SDL_Color &color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(m_renderer, &rect);
}

void core::Renderer::drawRectBorder(const SDL_Rect &rect, const SDL_Color &color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(m_renderer, &rect);
}

void core::Renderer::drawTexture(const core::Texture &texture, const SDL_Rect *srcRect, const SDL_Rect *dstRect)
{
    SDL_RenderCopy(m_renderer, texture.m_texture, srcRect, dstRect);
}

void core::Renderer::present()
{
    SDL_RenderPresent(m_renderer);
}
