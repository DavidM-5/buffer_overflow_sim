#include "InputManager.h"

core::InputManager::InputManager() : m_mousePosition({WINODW_WIDTH / 2, WINODW_HEIGHT / 2})
{
    // initialize key states to false
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        m_keyStates[i] = false;
    }

    // initialize mouse button states to false
    for (int i = 0; i < MOUSE_BUTTON_TOTAL; ++i) {
        m_mouseButtons[i] = false;
    }
}

void core::InputManager::update()
{
    // update keyboard keys statess
    const uint8_t* currentKeyStates = SDL_GetKeyboardState(nullptr);
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        m_keyStates[i] = currentKeyStates[i] != 0;
    }

    // update mouse position and buttons states
    uint32_t currentMouseState = SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);
    m_mouseButtons[MOUSE_BUTTON_LEFT] = (currentMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    m_mouseButtons[MOUSE_BUTTON_WHEEL] = (currentMouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    m_mouseButtons[MOUSE_BUTTON_RIGHT] = (currentMouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
}

std::string core::InputManager::getPressedKey(SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        uint16_t mod = event.key.keysym.mod;

        bool isShift = (mod & KMOD_SHIFT);

        // Handle alphanumeric keys
        if (key >= SDLK_a && key <= SDLK_z) {
            char pressedKey = (isShift) ? toupper(key) : key;
            return std::string(1, pressedKey);
        }

        // Handle numbers (0-9)
        if (key >= SDLK_0 && key <= SDLK_9) {
            char pressedKey = (isShift) ? (key - SDLK_0 + '!') : key;  // Handle shift+number for special characters
            return std::string(1, pressedKey);
        }

        switch (key)
        {
        case SDLK_SPACE:
            return " ";

        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            return "\n";

        case SDLK_TAB:
            return "\t";

        case SDLK_BACKSPACE:
            return "<BACKSPACE>";

        case SDLK_ESCAPE:
            return "<ESCAPE>";

        default:
            return "";
        }
        
    }

    return "";  // Return an empty string if no key is pressed
}
