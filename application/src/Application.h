#pragma once

#include <vector>
#include <set>
#include <regex>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdlib>
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
#include "Button.h"
#include "GDBController.h"


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
        
        application::Border m_borderVerticalLeft;
        application::Border m_borderVerticalRight;
        
        /*
        application::Border m_borderHorizontalCenterPanelTop;
        application::Border m_borderHorizontalCenterPanelBottom;
        application::Border m_borderHorizontalRightPanel;
        */

        std::unordered_map<std::string, SDL_Color> formatMap; // temp

        u_int32_t m_latestBreakpointLine;
        std::unordered_set<u_int32_t> m_userBreakpoints;
        std::unordered_set<u_int32_t> m_requiredBreakpoints;


        std::unique_ptr<GDBController> gdb;

        uint64_t count = 0xAC342BFC178205A0;// temporary
        uint64_t count2 = 0; // temporary

    private:
        void update(SDL_Event& event);
        void render();

        void initFormatMap();
        
        void initPanels();
        bool loadCodeText(const std::string& filename);

        void initLeftPanels();
        void initCenterPanels();
        void initRightPanels();

        void readFileToString(const std::string& filepath, std::string& dstString);

        bool loadTargetSourceCodeFromPath(const std::string& filepath);
        std::set<std::string> extractFunctionNamesFromFile(const std::string& filePath);

        bool compileFile(const std::string& commandPath);

    };

} // namespace application

