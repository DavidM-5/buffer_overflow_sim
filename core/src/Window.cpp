#include "Window.h"

int WINDOW_WIDTH = 1080;
int WINDOW_HEIGHT = 720;


core::Window::Window(const std::string& title, int width, int height) :
                        m_title(title), m_width(width), m_height(height),
                        m_running(true), m_window(nullptr)
{
}

core::Window::~Window()
{
    SDL_DestroyWindow(m_window);

    SDL_Quit();
}

bool core::Window::init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "SDL could not initialize -> " << SDL_GetError() << std::endl;
        success = false;
    }
    else {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            std::cerr << "Warning: Linear texture filtering not enabled" << std::endl;
        }

        // create window
        m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (m_window == nullptr){
            std::cerr << "Window could not be created -> " << SDL_GetError() << std::endl;
            success = false;
        }
        else {
            // create renderer
            // ...
        }
    }

    return success;
}

bool core::Window::initRenderer(core::Renderer *renderer)
{
    if (!renderer->init(m_window))
        return false;
    
    return true;
}

void core::Window::handleEvents(SDL_Event& event)
{
    if (event.type == SDL_QUIT) {
        m_running = false;  // Exit the loop when the window close event is detected
    }
    else if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
            WINDOW_WIDTH = event.window.data1; // updated width
            WINDOW_HEIGHT = event.window.data2; // updated height

        }
    }
    
}
