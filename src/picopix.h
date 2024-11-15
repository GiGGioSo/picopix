#ifndef _PICOPIX_H_
#define _PICOPIX_H_

#include "pix_mathy.h"

typedef struct PIX_Canvas {
    PIX_Layer *layers;
    uint32 layers_count;
} PIX_Canvas;

typedef struct PIX_Layer {
    PIX_Pixel *pixels;
    uint32 pixels_width;
    uint32 pixels_height;
} PIX_Layer;

typedef struct PIX_Pixel {
    vec4f color;
    uint8 selected;
} PIX_Pixel;

#endif // _PICOPIX_H_
