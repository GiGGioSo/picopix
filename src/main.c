#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <raymath.h>

#include "picopix.h"

void update_mouse(PIX_UIContext *ctx, Camera2D camera);
PIX_UIState button(PIX_UIContext *ctx, int x, int y, int width, int height);
PIX_UIState screen_button(PIX_UIContext *ctx, int x, int y, int width, int height);
PIX_UIState pixel_button(PIX_UIContext *ctx, Vector2 pos, Vector2 dim, Color col);
PIX_Canvas *create_canvas(size_t width, size_t height);
PIX_UIState draw_canvas(PIX_UIContext *ctx, PIX_Canvas *canvas, Vector2 pos, Vector2 pixel_size);
void erase_canvas(PIX_Canvas *canvas);

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
    Vector2 pixel_size = { 10, 10 };
    Vector2 canvas_origin = { -width * 0.35, -height * 0.35 };
    Vector2 opposite = Vector2Negate(canvas_origin);
    PIX_Canvas *canvas = create_canvas(
            opposite.x*2 / (pixel_size.x),
            opposite.y*2 / (pixel_size.y));

    Vector2 mouse_screen = Vector2Zero();
    Vector2 mouse_camera = Vector2Zero();

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();
        float time = GetTime();

        camera.offset.x = (width / 2) + sinf(time) * 200;
        camera.zoom = 1.f + sinf(time + PI * 0.5f) * 0.5f;

        update_mouse(&ctx, camera);

        BeginDrawing();
        BeginMode2D(camera);

        ClearBackground(RAYWHITE);

        PIX_UIState canvas_feed = draw_canvas(&ctx, canvas, canvas_origin, pixel_size);
        EndMode2D();

        if (screen_button(&ctx, 10, 10, 100, 40) & BUTTON_CLICKED) {
            erase_canvas(canvas);
        }

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

PIX_UIState screen_button(PIX_UIContext *ctx, int x, int y, int width, int height) {
    PIX_UIState result = NOTHING;
    Color button_col = LIGHTGRAY;

    Vector2 button_pos = { x, y };
    Vector2 button_dim = { width, height };

    Vector2 mouse_in_button = Vector2Subtract(ctx->mouse_pos_screen, button_pos);

    if (0 < mouse_in_button.x && mouse_in_button.x < button_dim.x &&
        0 < mouse_in_button.y && mouse_in_button.y < button_dim.y) {
        // the mouse is inside the button
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

PIX_UIState button(PIX_UIContext *ctx, int x, int y, int width, int height) {
    PIX_UIState result = NOTHING;
    Color button_col = LIGHTGRAY;

    Vector2 button_pos = { x, y };
    Vector2 button_dim = { width, height };

    Vector2 mouse_in_button = Vector2Subtract(ctx->mouse_pos_world, button_pos);

    if (0 < mouse_in_button.x && mouse_in_button.x < button_dim.x &&
        0 < mouse_in_button.y && mouse_in_button.y < button_dim.y) {
        // the mouse is inside the button
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

PIX_UIState pixel_button(PIX_UIContext *ctx, Vector2 pos, Vector2 dim, Color col) {
    PIX_UIState result = NOTHING;

    Vector2 mouse_in_button = Vector2Subtract(ctx->mouse_pos_world, pos);

    if (0 < mouse_in_button.x && mouse_in_button.x < dim.x &&
        0 < mouse_in_button.y && mouse_in_button.y < dim.y) {
        // the mouse is inside the button
        if (ctx->mouse_left.click) {
            result = BUTTON_CLICKED | BUTTON_HOVERED;
        } else if (ctx->mouse_left.press) {
            result = BUTTON_PRESSED | BUTTON_HOVERED;
        } else {
            result = BUTTON_HOVERED;
        }
    }

    Vector2 pixel_center_pos = Vector2AddValue(pos, dim.x * 0.11f);
    Vector2 pixel_center_dim = Vector2SubtractValue(dim, dim.x * 0.11f * 2);
    DrawRectangleV(pos, dim, DARKGRAY);
    DrawRectangleV(pixel_center_pos, pixel_center_dim, col);

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
        pixel->color = LIGHTGRAY;
    }

    return canvas;
}

PIX_UIState draw_canvas(PIX_UIContext *ctx, PIX_Canvas *canvas, Vector2 pos, Vector2 pixel_size) {
    PIX_UIState result = NOTHING;
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
                    (pixel_index % layer->pixels_width) * (pixel_size.x),
                .y = pos.y +
                    (pixel_index / layer->pixels_width) * (pixel_size.y)
            };

            if (pixel_button(ctx, pixel_pos, pixel_size, pixel->color) &
                    BUTTON_PRESSED) {
                pixel->color = RED;
                result = CANVAS_DRAW;
            }
            // DrawRectangleV(pixel_pos, pixel_size, pixel->color);
        }
    }
    return result;
}

void erase_canvas(PIX_Canvas *canvas) {
    for(size_t layer_index = 0;
        layer_index < canvas->layers_count;
        ++layer_index) {

        PIX_Layer *layer = &canvas->layers[layer_index];
        for(size_t pixel_index = 0;
            pixel_index < layer->pixels_width * layer->pixels_height;
            ++pixel_index) {

            PIX_Pixel *pixel = &layer->pixels[pixel_index];
            pixel->color = LIGHTGRAY;
            // DrawRectangleV(pixel_pos, pixel_size, pixel->color);
        }
    }
}
