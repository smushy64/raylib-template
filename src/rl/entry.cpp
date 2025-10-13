/**
 * @file   entry.cpp
 * @brief  Raylib Template: Entry Point
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   October 09, 2025
*/
#include "rl/entry.h"

bool rl_initialize( int argc, char** argv, void** out_memory ) {
    (void)argc;
    (void)argv;
    (void)out_memory;
    return true;
}
bool rl_update( void* memory ) {
    (void)memory;

    BeginDrawing(); {
        ClearBackground( RAYWHITE );

        DrawText( "Hello, World", 200, 200, 24, RED );

        EndDrawing();
    }

    return true;
}
void rl_shutdown( void* memory ) {
    (void)memory;
}


