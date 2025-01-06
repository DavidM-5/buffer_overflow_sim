#pragma once

#include <array>
#include <vector>
#include "../../core/src/config.h"
#include "../../core/src/InputManager.h"
#include "Widget.h"

namespace application
{

    class Border : public Widget
    {
    public:
        Border(bool isVertical, int posX = 0, int posY = 0, int w = 10, int h = 100, SDL_Color color = {255, 255, 255, 255});
        ~Border() = default;

        void handleEvents(const core::InputManager& inputMngr);
        
        void addLeftTopWidget(Widget* panel);
        void addRightBottomWidget(Widget* panel);

    private:
        bool m_isVertical;
        bool m_isResizing;

        // index 0 for widgets on left/top
        // index 1 for widgets on right/bottom
        std::array<std::vector<Widget*>, 2> m_widgets;

    private:
        bool isPointInside(const vector2i& point);

    };
    
} // namespace application
