#ifndef _PICOPIX_H_
#define _PICOPIX_H_

#include <raylib.h>

#define SIGN(x) (((x) > 0) ? 1.f : -1.f)

typedef struct PIX_Button {
    bool press;
    bool click;
    bool down_click;
} PIX_Button;

typedef struct PIX_Mouse {
    Vector2 pos;
    Vector2 world_pos;
    Vector2 press_pos;
    Vector2 world_press_pos;
    PIX_Button left;
    PIX_Button middle;
    PIX_Button right;
    float wheel;
} PIX_Mouse;

typedef struct PIX_UIContext {
    PIX_Mouse mouse;
    bool canvas_dragging;
    Vector2 camera_drag_start_offset;
} PIX_UIContext;

typedef enum PIX_UIState {
    NOTHING = 0,
    BUTTON_HOVERED = (1 << 0),
    BUTTON_PRESSED = (1 << 1),
    BUTTON_CLICKED = (1 << 2),
    CANVAS_DRAW    = (1 << 3),
} PIX_UIState;

typedef struct PIX_Pixel {
    Color color;
    bool selected;
} PIX_Pixel;

typedef struct PIX_Layer {
    char name[512];
    PIX_Pixel *pixels;
} PIX_Layer;

typedef struct PIX_Canvas {
    PIX_Layer *layers;
    size_t layers_count;
    size_t pixels_width;
    size_t pixels_height;
    Color erase_color;
} PIX_Canvas;


#endif // _PICOPIX_H_
