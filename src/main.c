#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <raymath.h>

#include "picopix.h"

void update_mouse(PIX_UIContext *ctx, Camera2D camera);
PIX_UIState button(PIX_UIContext *ctx, int x, int y, int width, int height);
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
    PIX_UIContext ctx = {};
    PIX_Canvas *canvas = create_canvas(4, 3);

    Vector2 mouse_screen = Vector2Zero();
    Vector2 mouse_camera = Vector2Zero();

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();
        float time = GetTime();

        update_mouse(&ctx, camera);

        BeginDrawing();
        BeginMode2D(camera);

        ClearBackground(BLACK);

        Vector2 pixel_size = { 20, 20 };
        // DrawRectangleV(Vector2Zero(), pixel_size, RED);
        draw_canvas(canvas, Vector2Zero(), pixel_size);

        if (button(&ctx, -200, -100, 200, 100) & BUTTON_CLICKED) {
            printf("CLICK!\n");
        }

        EndMode2D();
        DrawFPS(2, 2);
        EndDrawing();
    }
}

void update_mouse(PIX_UIContext *ctx, Camera2D camera) {
    if (!ctx) return;
    if (IsCursorOnScreen) {
        // mouse cursor
        ctx->mouse_pos_screen = GetMousePosition();
        ctx->mouse_pos_world = GetScreenToWorld2D(ctx->mouse_pos_screen, camera);
        // mouse left button
        bool new_mouse_left_press = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        ctx->mouse_left.click =
            ctx->mouse_left.press && !new_mouse_left_press;
        ctx->mouse_left.press = new_mouse_left_press;
        // mouse right button
        bool new_mouse_right_press = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
        ctx->mouse_right.click =
            ctx->mouse_right.press && !new_mouse_right_press;
        ctx->mouse_right.press = new_mouse_right_press;
    }
}

PIX_UIState button(PIX_UIContext *ctx, int x, int y, int width, int height) {
    PIX_UIState result = BUTTON_NOTHING;
    Color button_col = LIGHTGRAY;

    Vector2 button_pos = { x, y };
    Vector2 button_dim = { width, height };

    Vector2 mouse_in_button = Vector2Subtract(ctx->mouse_pos_world, button_pos);

    if (0 < mouse_in_button.x && mouse_in_button.x < button_dim.x &&
        0 < mouse_in_button.y && mouse_in_button.y < button_dim.y) {
        if (ctx->mouse_left.click) {
            result = BUTTON_CLICKED | BUTTON_HOVERED;
            button_col = RED;
        } else if (ctx->mouse_left.press) {
            result = BUTTON_PRESSED | BUTTON_HOVERED;
            button_col = GREEN;
        } else {
            result = BUTTON_HOVERED;
            button_col = DARKGRAY;
        }
    }

    DrawRectangleV(button_pos, button_dim, button_col);

    return result;
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
