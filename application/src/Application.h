#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>

#include "../../core/src/config.h"
#include "../../core/src/Window.h"
#include "../../core/src/Renderer.h"
#include "../../core/src/Texture.h"
#include "../../core/src/InputManager.h"
#include "TextBlock.h"
#include "TextLine.h"
#include "Panel.h"
#include "Border.h"
#include "StackVisualizer.h"
#include "Console.h"

namespace application
{
    
    class Application
    {
    public:
        Application(/* args */);
        ~Application() = default;
    
        bool init();
        void run();

    private:
        core::Window m_window;
        core::Renderer m_renderer;
        core::InputManager m_inputMngr;

        std::vector<std::unique_ptr<application::Panel>> m_panels;
        application::Panel m_mainPanel;

        int m_borderWidth;
        int m_innerBorderWidth;

        application::Border m_bordVert;
        application::Border m_bordHor;


        std::unordered_map<std::string, SDL_Color> formatMap; // temp


        uint64_t count = 0xAC342BFC178205A0;// temporary

    private:
        void update(SDL_Event& event); // temp implementation
        void render(); // temp implementation

        void initPanels();
        bool loadCodeText(const std::string& filename);

        void initLeftPanels();
        void initCenterPanels();
        void initRightPanels();

        bool loadTargetSourceCode(const std::string& filepath);
    };

} // namespace application

