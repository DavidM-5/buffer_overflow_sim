#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include "../../core/src/config.h"
#include "../../core/src/Renderer.h"
#include "../../core/src/InputManager.h"
#include "TextLine.h"
#include "Widget.h"

namespace application
{

    class StackVisualizer : public Widget
    {
    public:
        StackVisualizer(int posX = 0, int posY = 0, int w = 150, int h = 50, SDL_Color color = {255, 255, 255, 255}, int slotsAmount = 2, const std::string& font = "JetBrainsMono-Bold.ttf", int fontSize = 16);
        ~StackVisualizer() = default;

        void render(core::Renderer& renderer, const SDL_Rect* srcRect = nullptr, const SDL_Rect* dstRect = nullptr);
        void handleEvents(const core::InputManager& inputMngr) {};

        void push(const std::string& str);
        void pop();

        void selectBPSlot(int slot);
        void editSlot(int slotNum, const std::string& newText);

        void clear();
        bool empty() { return m_slots.empty(); }

        void addDeltaTransform(int x = 0, int y = 0, int w = 0, int h = 0) override;

    private:
        std::vector<std::unique_ptr<application::TextLine>> m_slots;
        application::TextLine m_bpText;
        
        int m_slotsAmount;
        int m_slotHeight;

        int m_selectedBpSlot;
        
        std::string m_font;
        int m_fontSize;
    };
    
} // namespace application
