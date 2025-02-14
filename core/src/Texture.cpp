#include "Texture.h"

core::Texture::Texture() : m_texture(nullptr), m_width(0), m_height(0)
{
}

core::Texture::~Texture()
{
    destroy();
}

bool core::Texture::loadFromText(const std::string &text, TTF_Font *font, const SDL_Color &color, core::Renderer& renderer)
{
    destroy();

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (textSurface == nullptr)
        return false;
    

    m_texture = SDL_CreateTextureFromSurface(renderer.m_renderer, textSurface);
    if (m_texture == nullptr) {
        SDL_FreeSurface(textSurface);
        return false;
    }

    m_width = textSurface->w;
    m_height = textSurface->h;

    SDL_FreeSurface(textSurface);

    return true;
}

bool core::Texture::loadFromSurface(SDL_Surface *surface, const core::Renderer &renderer)
{
    if (!surface)
        return false;

    Texture::destroy();

    m_texture = SDL_CreateTextureFromSurface(renderer.m_renderer, surface);
    if (!m_texture)
        return false;

    m_width = surface->w;
    m_height = surface->h;

    return true;
}

bool core::Texture::isValid() const
{
    return m_texture && m_width != 0 && m_height != 0;
}

void core::Texture::destroy()
{
    if (m_texture != nullptr) {
        SDL_DestroyTexture(m_texture);

        m_texture = nullptr;
        m_width = 0;
        m_height = 0;
    }
}
