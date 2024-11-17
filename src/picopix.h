#ifndef _PICOPIX_H_
#define _PICOPIX_H_

#include <raylib.h>

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
