/**
 * @file   src/main.cpp
 * @brief  Raylib Project entry point.
 * @author Alicia D. Amarilla (smushyaa@gmail.com)
 * @date   June 27, 2026
*/
#include "raylib.h"
#include <stdio.h> // IWYU pragma: keep

#if PLATFORM == PLATFORM_WASM
    #include <emscripten/emscripten.h>
#endif

void Update(void) {
    BeginDrawing();
    ClearBackground(BLACK);

    int tsz = 24;
    int gut = 10;
    int x, y;
    x = y = 10;
    DrawText(TextFormat("name: %s", PROJECT_NAME), x, y, tsz, WHITE);
    y += tsz + gut;
    DrawText(TextFormat("version: %s", PROJECT_VERSION), x, y, tsz, WHITE);
    y += tsz + gut;
    DrawText(TextFormat("developer: %s", PROJECT_DEVELOPER), x, y, tsz, WHITE);
    y += tsz + gut;
    DrawText(TextFormat("compiler: %s", PROJECT_COMPILER), x, y, tsz, WHITE);
    y += tsz + gut;

    EndDrawing();
}

int main(int argc, char **argv) {
    (void)argc, (void)argv;
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
        TextFormat("%s | version %s", PROJECT_NAME, PROJECT_VERSION));

#if PLATFORM == PLATFORM_WASM
    emscripten_set_main_loop(Update, 0, 1);
#else
    SetTargetFPS(FRAMERATE);

    while(!WindowShouldClose()) {
        Update();
    }
#endif

    CloseWindow();
    return 0;
}


