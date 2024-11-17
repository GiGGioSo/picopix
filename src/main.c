#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <raymath.h>

#include "picopix.h"

PIX_Canvas *create_canvas(size_t width, size_t height);
void draw_canvas(PIX_Canvas *canvas, Vector2 pos, Vector2 pixel_size);

int main(void) {

    int width = 900;
    int height = 600;

    InitWindow(width, height, "PicoPix");
    // SetTargetFPS(60);
    SetExitKey(KEY_F4);

    Camera2D camera = {
        .offset = {width/2, height/2},
        .target = {0.f, 0.f},
        .rotation = 0.f,
        .zoom = 1.0f
    };
    PIX_Canvas *canvas = create_canvas(4, 3);

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();
        float time = GetTime();

        BeginDrawing();
        BeginMode2D(camera);

        ClearBackground(BLACK);


        Vector2 pixel_size = { 20, 20 };
        // DrawRectangleV(Vector2Zero(), pixel_size, RED);
        draw_canvas(canvas, Vector2Zero(), pixel_size);

        EndMode2D();
        DrawFPS(2, 2);
        EndDrawing();
    }
}

PIX_Canvas *create_canvas(size_t width, size_t height) {
    PIX_Canvas *canvas = malloc(sizeof(PIX_Canvas));
    if (!canvas) return NULL;

    canvas->layers_count = 1;
    canvas->layers = malloc(canvas->layers_count * sizeof(PIX_Layer));

    PIX_Layer *layer = &(canvas->layers[0]);
    strcpy(layer->name, "Default layer");
    layer->pixels_width = width;
    layer->pixels_height = height;
    layer->pixels =
        malloc(layer->pixels_width * layer->pixels_height * sizeof(PIX_Pixel));

    for(size_t pixel_index = 0;
        pixel_index < layer->pixels_width * layer->pixels_height;
        ++pixel_index) {
        PIX_Pixel *pixel = &layer->pixels[pixel_index];
        pixel->color = (Color){ 255, 0, 0, 255 };
    }

    return canvas;
}

void draw_canvas(PIX_Canvas *canvas, Vector2 pos, Vector2 pixel_size) {

    for(size_t layer_index = 0;
        layer_index < canvas->layers_count;
        ++layer_index) {

        PIX_Layer *layer = &canvas->layers[layer_index];
        for(size_t pixel_index = 0;
            pixel_index < layer->pixels_width * layer->pixels_height;
            ++pixel_index) {

            PIX_Pixel *pixel = &layer->pixels[pixel_index];
            Vector2 pixel_pos = {
                .x = pos.x +
                    (pixel_index % layer->pixels_width) * (pixel_size.x + 2),
                .y = pos.y +
                    (pixel_index / layer->pixels_width) * (pixel_size.y + 2)
            };

            DrawRectangleV(pixel_pos, pixel_size, pixel->color);
        }
    }

}
