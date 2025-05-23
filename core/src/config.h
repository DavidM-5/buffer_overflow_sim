#pragma once

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

extern int WINDOW_WIDTH_DELTA;
extern int WINDOW_HEIGHT_DELTA;

extern bool WINODW_RESIZED;


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
