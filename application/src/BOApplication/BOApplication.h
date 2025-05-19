#pragma once

#include <vector>
#include <set>
#include <regex>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <functional>
#include <SDL2/SDL.h>

#include "../../../core/src/config.h"
#include "../../../core/src/Window.h"
#include "../../../core/src/Renderer.h"
#include "../../../core/src/Texture.h"
#include "../../../core/src/InputManager.h"
#include "../TextBlock.h"
#include "../TextLine.h"
#include "../Panel.h"
#include "../Border.h"
#include "../StackVisualizer.h"
#include "../Console.h"
#include "../Button.h"
#include "../Paragraph.h"
#include "GDBController.h"


namespace application
{
    
    class BOApplication
    {
    public:
        BOApplication(/* args */);
        ~BOApplication() = default;
    
        bool init();
        void run();

    private:
        enum UserTask {
            PUT_BREAKPOINT_AT_PROBLEM_LINE = 0,
            ENTER_PAYLOAD,
            LOGIN_AS_MANAGER,
            TOTAL_TASKS
        };

        core::Window m_window;
        core::Renderer m_renderer;
        core::InputManager m_inputMngr;

        std::vector<std::unique_ptr<application::Panel>> m_panels;
        application::Panel m_mainPanel;

        
        // List of all the tasks the user should do.
        // True if the task completed, False otherwise.
        bool m_userTasksStatus[TOTAL_TASKS] = {0};

        bool m_userInLoginFunction;
        std::string m_requieredAddressInput;
        uint64_t m_requieredFunctionAddress;


        int m_borderWidth;
        int m_innerBorderWidth;

        application::Console* m_targetConsole;
        application::StackVisualizer* m_stackView;

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

        int m_userLatestBreakpoint;


        std::shared_ptr<GDBController> gdb;

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

        bool fileExists(const std::string& filePath);
        bool compileFile(const std::string& commandPath);

        void memoryDumpToStackView(const std::string& startAddr = "$rbp", int numOfAddresses = 10);
        void fillStackViewLoginFunc();
        void showFunctionsAddresses();

        std::string padHexAddress(const std::string& addr, std::size_t width = 16);
        std::string repeatPattern(const std::string& input);
        std::string toHexAscii(const std::string& input);

        std::string getRequieredAddressInput();

        void markTaskDone(int taskNum);
        void showCompletionText();

        bool is_number(const std::string& s);

    };

} // namespace application

