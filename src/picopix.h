#ifndef _PICOPIX_H_
#define _PICOPIX_H_

#include <raylib.h>

typedef struct PIX_Button {
    bool press;
    bool click;
} PIX_Button;

typedef struct PIX_UIContext {
    Vector2 mouse_pos_screen;
    Vector2 mouse_pos_world;
    PIX_Button mouse_left;
    PIX_Button mouse_right;
} PIX_UIContext;

typedef enum PIX_UIState {
    BUTTON_NOTHING = 0,
    BUTTON_HOVERED = (1 << 0),
    BUTTON_PRESSED = (1 << 1),
    BUTTON_CLICKED = (1 << 2)
} PIX_UIState;

typedef struct PIX_Pixel {
    Color color;
    bool selected;
} PIX_Pixel;

typedef struct PIX_Layer {
    char name[512];
    PIX_Pixel *pixels;
    size_t pixels_width;
    size_t pixels_height;
} PIX_Layer;

typedef struct PIX_Canvas {
    PIX_Layer *layers;
    size_t layers_count;
} PIX_Canvas;


#endif // _PICOPIX_H_
