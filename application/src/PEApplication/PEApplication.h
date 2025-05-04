#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <map>
#include <set>
#include <dirent.h>
#include <sys/stat.h>
#include "../../../core/src/config.h"
#include "../../../core/src/Window.h"
#include "../../../core/src/Renderer.h"
#include "../../../core/src/Texture.h"
#include "../../../core/src/InputManager.h"
#include "../BOApplication/ConsoleProccess.h"
#include "../Panel.h"
#include "../Table.h"
#include "../Button.h"
#include "../InputLine.h"


namespace application
{
    
    class PEApplication
    {
    public:
        PEApplication(/* args */);
        ~PEApplication() = default;
        
        bool init();
        void run();
    
    private:
        core::Window m_window;
        core::Renderer m_renderer;
        core::InputManager m_inputMngr;

        struct FileCapability {
            std::string filename;
            std::vector<std::pair<std::string, std::array<bool, 2>>> users; // username, [read, write]
            
            FileCapability(const std::string& name) : filename(name) {}
        };

        application::Panel m_mainPanel;

        std::unique_ptr<ConsoleProccess> m_accessManager;

        application::Table testTable;
        application::InputLine iline;
        
    private:
        void update(SDL_Event& event);
        void render();

        void initPanels();
        void loadButtons(application::Panel& parentPanel);

        void loadCapabilityTable();

        void runBLP();

        bool isDirectory(const std::string& path);

    };

} // namespace application

