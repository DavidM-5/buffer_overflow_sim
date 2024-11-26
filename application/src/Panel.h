#pragma once

#include "../../core/src/config.h"
#include "Widget.h"

namespace application
{
    
    class Panel : public Widget
    {
    public:
        Panel(int posX = 0, int posY = 0, int w = 100, int h = 100);
        ~Panel() = default;

        void resize(int newW, int newH);
        void resizeByRatio(float newW, float newH);

        void move(int posX, int posY);
    
    private:
        /* data */
    };

} // namespace application

