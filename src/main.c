#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <raymath.h>

#include "picopix.h"

void update_context(PIX_UIContext *ctx, Camera2D camera);
PIX_UIState button(PIX_UIContext *ctx, int x, int y, int width, int height);
PIX_UIState screen_button(PIX_UIContext *ctx, int x, int y, int width, int height);
PIX_UIState pixel_button(PIX_UIContext *ctx, Vector2 pos, Vector2 dim, Color col);
PIX_Canvas *create_canvas(size_t width, size_t height, Color erase_color);
PIX_UIState draw_canvas(PIX_UIContext *ctx, PIX_Canvas *canvas, Vector2 pos, Vector2 pixel_size);
void draw_background(int width, int height);
void erase_canvas(PIX_Canvas *canvas);

int main(void) {

    int width = 900;
    int height = 600;
    InitWindow(width, height, "PicoPix");
    SetTargetFPS(256);
    SetExitKey(KEY_F4);

    Camera2D camera = {
        .offset = {width/2, height/2},
        .target = {0.f, 0.f},
        .rotation = 0.f,
        .zoom = 1.0f
    };
    PIX_UIContext ctx = {};
    Vector2 pixel_size = { 10, 10 };
    Vector2 canvas_origin = { -width * 0.25f, -height * 0.25f };
    Vector2 opposite = Vector2Negate(canvas_origin);
    PIX_Canvas *canvas = create_canvas(
            opposite.x*2 / (pixel_size.x),
            opposite.y*2 / (pixel_size.y),
            BLANK);

    Vector2 mouse_screen = Vector2Zero();
    Vector2 mouse_camera = Vector2Zero();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float time = GetTime();

        update_context(&ctx, camera);

        // camera movement
        if (ctx.canvas_dragging && ctx.mouse.left.press) {
            Vector2 displacement = Vector2Subtract(ctx.mouse.pos, ctx.mouse.press_pos);
            camera.offset = Vector2Add(ctx.camera_drag_start_offset, displacement);
        }

        // camera zoom
        if (ctx.mouse.wheel != 0) {
            camera.offset = ctx.mouse.pos;
            camera.target = ctx.mouse.world_pos;
            // camera.zoom += (50.f * dt * SIGN(ctx.mouse.wheel));
            camera.zoom *= (1.f + dt * 15.f * SIGN(ctx.mouse.wheel));
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_background(width, height);

        BeginMode2D(camera);

        draw_canvas(&ctx, canvas, canvas_origin, pixel_size);
        EndMode2D();

        if (screen_button(&ctx, 10, 30, 100, 40) & BUTTON_CLICKED) {
            erase_canvas(canvas);
        }

        DrawFPS(2, 2);
        EndDrawing();
    }
}

void update_context(PIX_UIContext *ctx, Camera2D camera) {
    if (!ctx) return;
    if (IsCursorOnScreen) {
        // mouse cursor
        ctx->mouse.pos = GetMousePosition();
        ctx->mouse.world_pos = GetScreenToWorld2D(ctx->mouse.pos, camera);
        // mouse left button
        bool new_mouse_left_press = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        //  update mouse position with starting press position
        if (!ctx->mouse.left.press && new_mouse_left_press) {
            ctx->mouse.press_pos = ctx->mouse.pos;
            ctx->mouse.world_press_pos = ctx->mouse.world_pos;
        }
        ctx->mouse.left.down_click =
            !ctx->mouse.left.press && new_mouse_left_press;
        ctx->mouse.left.click =
            ctx->mouse.left.press && !new_mouse_left_press;
        ctx->mouse.left.press = new_mouse_left_press;
        // mouse right button
        bool new_mouse_right_press = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
        ctx->mouse.right.down_click =
            !ctx->mouse.right.press && new_mouse_right_press;
        ctx->mouse.right.click =
            ctx->mouse.right.press && !new_mouse_right_press;
        ctx->mouse.right.press = new_mouse_right_press;
        // mouse wheel
        ctx->mouse.wheel = GetMouseWheelMove();
    }
    ctx->canvas_dragging = IsKeyDown(KEY_SPACE);
    if (ctx->canvas_dragging && ctx->mouse.left.down_click) {
        ctx->camera_drag_start_offset = camera.offset;
    }
}

PIX_UIState screen_button(PIX_UIContext *ctx, int x, int y, int width, int height) {
    PIX_UIState result = NOTHING;
    Color button_col = LIGHTGRAY;

    Vector2 button_pos = { x, y };
    Vector2 button_dim = { width, height };

    Vector2 mouse_in_button = Vector2Subtract(ctx->mouse.pos, button_pos);

    if (0 <= mouse_in_button.x && mouse_in_button.x <= button_dim.x &&
        0 <= mouse_in_button.y && mouse_in_button.y <= button_dim.y) {
        // the mouse is inside the button
        if (ctx->mouse.left.click) {
            result = BUTTON_CLICKED | BUTTON_HOVERED;
            button_col = RED;
        } else if (ctx->mouse.left.press) {
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

    Vector2 mouse_in_button = Vector2Subtract(ctx->mouse.world_pos, button_pos);

    if (0 <= mouse_in_button.x && mouse_in_button.x <= button_dim.x &&
        0 <= mouse_in_button.y && mouse_in_button.y <= button_dim.y) {
        // the mouse is inside the button
        if (ctx->mouse.left.click) {
            result = BUTTON_CLICKED | BUTTON_HOVERED;
            button_col = RED;
        } else if (ctx->mouse.left.press) {
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

    Vector2 mouse_in_button = Vector2Subtract(ctx->mouse.world_pos, pos);

    if (0 <= mouse_in_button.x && mouse_in_button.x <= dim.x &&
        0 <= mouse_in_button.y && mouse_in_button.y <= dim.y) {
        // the mouse is inside the button
        if (ctx->mouse.left.click) {
            result = BUTTON_CLICKED | BUTTON_HOVERED;
        } else if (ctx->mouse.left.press) {
            result = BUTTON_PRESSED | BUTTON_HOVERED;
        } else {
            result = BUTTON_HOVERED;
        }
    }

    // Vector2 pixel_center_pos = Vector2AddValue(pos, 1);
    // Vector2 pixel_center_dim = Vector2SubtractValue(dim, 2);
    DrawRectangleV(pos, dim, col);
    // DrawRectangleV(pixel_center_pos, pixel_center_dim, col);

    return result;
}

PIX_Canvas *create_canvas(size_t width, size_t height, Color erase_color) {
    PIX_Canvas *canvas = malloc(sizeof(PIX_Canvas));
    if (!canvas) return NULL;

    canvas->layers_count = 1;
    canvas->layers = malloc(canvas->layers_count * sizeof(PIX_Layer));
    canvas->pixels_width = width;
    canvas->pixels_height = height;
    canvas->erase_color = erase_color;

    PIX_Layer *layer = &(canvas->layers[0]);
    strcpy(layer->name, "Default layer");
    layer->pixels = malloc(
            canvas->pixels_width * canvas->pixels_height * sizeof(PIX_Pixel));

    for(size_t pixel_index = 0;
        pixel_index < canvas->pixels_width * canvas->pixels_height;
        ++pixel_index) {
        PIX_Pixel *pixel = &layer->pixels[pixel_index];
        pixel->color = canvas->erase_color;
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
            pixel_index < canvas->pixels_width * canvas->pixels_height;
            ++pixel_index) {

            PIX_Pixel *pixel = &layer->pixels[pixel_index];
            Vector2 pixel_pos = {
                .x = pos.x +
                    (pixel_index % canvas->pixels_width) * (pixel_size.x),
                .y = pos.y +
                    (pixel_index / canvas->pixels_width) * (pixel_size.y)
            };

            if ((pixel_button(ctx, pixel_pos, pixel_size, pixel->color) &
                    BUTTON_PRESSED) && !ctx->canvas_dragging) {
                pixel->color = RED;
                result = CANVAS_DRAW;
            }
            // DrawRectangleV(pixel_pos, pixel_size, pixel->color);
        }
    }

    // vertical pixel lines
    for(int line_index = 0;
        line_index <= canvas->pixels_width;
        ++line_index) {

        int line_x = pos.x + line_index * pixel_size.x;

        DrawLine(line_x, pos.y,
                 line_x, pos.y + canvas->pixels_height * pixel_size.y,
                 DARKGRAY);                // Draw a line
    }

    // horizontal pixel lines
    for(int line_index = 0;
        line_index <= canvas->pixels_height;
        ++line_index) {

        int line_y = pos.y + line_index * pixel_size.y;

        DrawLine(pos.x, line_y,
                 pos.x + canvas->pixels_width * pixel_size.x, line_y,
                 DARKGRAY);                // Draw a line
    }
    return result;
}

void draw_background(int width, int height) {
    Vector2 block_size = { 25, 25 };
    for(int x = 0; x < width; x += block_size.x) {
        for(int y = 0; y < height; y += block_size.y) {
            Color block_color = ((x + y) % 2 == 0) ? RAYWHITE : LIGHTGRAY;
            Vector2 block_pos = { x, y };
            DrawRectangleV(block_pos, block_size, block_color);
        }
    }
}

void erase_canvas(PIX_Canvas *canvas) {
    for(size_t layer_index = 0;
        layer_index < canvas->layers_count;
        ++layer_index) {

        PIX_Layer *layer = &canvas->layers[layer_index];
        for(size_t pixel_index = 0;
            pixel_index < canvas->pixels_width * canvas->pixels_height;
            ++pixel_index) {

            PIX_Pixel *pixel = &layer->pixels[pixel_index];
            pixel->color = canvas->erase_color;
        }
    }
}
