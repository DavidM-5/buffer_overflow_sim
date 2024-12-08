#pragma once

extern int WINDOW_WIDTH;
extern int WINODW_HEIGHT;


typedef enum {
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_WHEEL,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_TOTAL
} Mouse_buttons;


struct vector2i
{
    int x, y;
};

struct vector2f
{
    float x, y;
};

