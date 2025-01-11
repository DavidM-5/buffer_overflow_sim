#include "InputManager.h"

core::InputManager::InputManager() : m_mousePosition({WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}),
                                     m_prevMousePosition({WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}),
                                     m_currentKeyPress(""),
                                     m_mouseWheelScroll(0)
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

void core::InputManager::update(SDL_Event& event)
{
    m_mouseWheelScroll = 0;
    m_prevMousePosition = m_mousePosition;
    m_currentKeyPress = "";

    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode keycode = event.key.keysym.sym;
        SDL_Keymod modState = SDL_GetModState();
        
        // Handle numbers and symbols with shift
        if (keycode >= SDLK_0 && keycode <= SDLK_9 && (modState & KMOD_SHIFT)) {
            const char shiftNums[] = ")!@#$%^&*(";
            m_currentKeyPress = shiftNums[keycode - SDLK_0];
        } 
        else {
            // Get the character considering keyboard layout
            std::string keyName = SDL_GetKeyName(keycode);

            if (keycode == SDLK_SPACE) {
                m_currentKeyPress = " ";
            }
            else if (keycode == SDLK_RETURN || keycode == SDLK_RETURN2) {
                m_currentKeyPress = "\n";
            }
            else if (keycode == SDLK_TAB) {
                m_currentKeyPress = "\t";
            }
            else {
                m_currentKeyPress = keyName;
                
                // Handle case sensitivity
                if (!(modState & KMOD_SHIFT)) {
                    std::transform(m_currentKeyPress.begin(), m_currentKeyPress.end(), 
                                m_currentKeyPress.begin(), ::tolower);
                }
            }
        }
    }

    // Detect mouse wheel scrolling
    if (event.type == SDL_MOUSEWHEEL) {
        m_mouseWheelScroll = event.wheel.y;
    }

    // Rest of the update function remains the same
    const uint8_t* currentKeyStates = SDL_GetKeyboardState(nullptr);
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        m_keyStates[i] = currentKeyStates[i] != 0;
    }

    uint32_t currentMouseState = SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);
    m_mouseButtons[MOUSE_BUTTON_LEFT] = (currentMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    m_mouseButtons[MOUSE_BUTTON_WHEEL] = (currentMouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    m_mouseButtons[MOUSE_BUTTON_RIGHT] = (currentMouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
}

std::string core::InputManager::getPressedKey() const
{
    return m_currentKeyPress;
}

vector2i core::InputManager::getMousePosDelta() const
{
    int x = m_mousePosition.x - m_prevMousePosition.x;
    int y = m_mousePosition.y - m_prevMousePosition.y;

    return vector2i{x, y};
}
