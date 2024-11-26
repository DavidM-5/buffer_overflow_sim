#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>
#include <string>

#include "config.h"

namespace core
{
    
    class InputManager
    {
    public:
        InputManager(/* args */);
        ~InputManager() = default;

        void update();

        bool isKeyPressed(SDL_Scancode key) const { return m_keyStates[key]; }
        bool isMouseButtonPressed(uint8_t button) const { return m_mouseButtons[button]; }

        vector2i getMousePosition() const { return m_mousePosition; }
        std::string getPressedKey(SDL_Event& event);
    
    private:
        bool m_keyStates[SDL_NUM_SCANCODES];
        bool m_mouseButtons[MOUSE_BUTTON_TOTAL];
        
        vector2i m_mousePosition;

    };

} // namespace core
