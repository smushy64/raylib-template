/**
 * @file   main.cpp
 * @brief  Entry point for editor.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   October 13, 2025
*/
#include "rl/common.h"

int main( int argc, char** argv ) {
    InitWindow( RL_WINDOW_WIDTH, RL_WINDOW_HEIGHT, "Editor: " RL_PROJECT_NAME );
    InitAudioDevice();
    SetTargetFPS( RL_FRAMERATE );

    while( !WindowShouldClose() ) {
        BeginDrawing();
        ClearBackground( RAYWHITE );

        DrawText( "Editor", 200, 200, 24, RED );

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

