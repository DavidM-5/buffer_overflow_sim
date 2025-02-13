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
    m_mouseClicked[0] = m_mouseClicked[1] = m_mouseClicked[2] = false;
    m_prevMousePosition = m_mousePosition;
    m_currentKeyPress = "";

    if (event.type == SDL_TEXTINPUT) {
        // Handle text input (including special characters like :"|_+)
        m_currentKeyPress = event.text.text;
    }
    else if (event.type == SDL_KEYDOWN) {
        SDL_Keycode keycode = event.key.keysym.sym;

        // Handle special keys like enter, tab, etc.
        if (keycode == SDLK_RETURN || keycode == SDLK_RETURN2) {
            m_currentKeyPress = "\n";
        }
        else if (keycode == SDLK_TAB) {
            m_currentKeyPress = "\t";
        }
        else if (keycode == SDLK_BACKSPACE) {
            m_currentKeyPress = "\b";
        }
    }

    // Detect mouse wheel scrolling
    if (event.type == SDL_MOUSEWHEEL) {
        m_mouseWheelScroll = event.wheel.y;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            m_mouseClicked[MOUSE_BUTTON_LEFT] = true;
        }
        if (event.button.button == SDL_BUTTON_RIGHT) {
            m_mouseClicked[MOUSE_BUTTON_RIGHT] = true;
        }
        if (event.button.button == SDL_BUTTON_MIDDLE) {
            m_mouseClicked[MOUSE_BUTTON_WHEEL] = true;
        }
    }

    // Update key states
    const uint8_t* currentKeyStates = SDL_GetKeyboardState(nullptr);
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        m_keyStates[i] = currentKeyStates[i] != 0;
    }

    // Update mouse position and button states
    uint32_t currentMouseState = SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);
    m_mouseButtons[MOUSE_BUTTON_LEFT] = (currentMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    m_mouseButtons[MOUSE_BUTTON_WHEEL] = (currentMouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    m_mouseButtons[MOUSE_BUTTON_RIGHT] = (currentMouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
}

vector2i core::InputManager::getMousePosDelta() const
{
    int x = m_mousePosition.x - m_prevMousePosition.x;
    int y = m_mousePosition.y - m_prevMousePosition.y;

    return vector2i{x, y};
}
