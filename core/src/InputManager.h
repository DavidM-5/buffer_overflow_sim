#pragma once

#include <SDL2/SDL.h>
#include <algorithm>
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

        void update(SDL_Event& event);

        bool isKeyPressed(SDL_Scancode key) const { return m_keyStates[key]; }
        bool isMouseButtonPressed(uint8_t button) const { return m_mouseButtons[button]; }
        bool mouseClicked(uint8_t button) const { return m_mouseClicked[button]; }

        vector2i getMousePosition() const { return m_mousePosition; }
        int getMouseWheelScroll() const { return m_mouseWheelScroll; };
        std::string getPressedKey() const { return m_currentKeyPress; };

        vector2i getMousePosDelta() const;
    
    private:
        bool m_keyStates[SDL_NUM_SCANCODES];
        bool m_mouseButtons[MOUSE_BUTTON_TOTAL];
        bool m_mouseClicked[MOUSE_BUTTON_TOTAL];

        int m_mouseWheelScroll;
        
        vector2i m_mousePosition;
        vector2i m_prevMousePosition;

        std::string m_currentKeyPress;

    };

} // namespace core
