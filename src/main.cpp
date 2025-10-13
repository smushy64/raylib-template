/**
 * @file   main.c
 * @brief  Entry point for raylib projects.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   October 07, 2025
*/
#include "rl/common.h"
#include "rl/entry.h"

#if defined(RL_PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

bool SHOULD_QUIT    = false;
bool is_first_frame = true;

int    ARG_COUNT = 0;
char** ARG_PTR   = NULL;

void* GAME_MEMORY = NULL;

void __update_loop__(void);

int main( int argc, char** argv ) {
    ARG_COUNT = argc;
    ARG_PTR   = argv;

#if !defined(RL_IS_DEBUG)
    SetTraceLogLevel( LOG_NONE );
#endif

    InitWindow( RL_WINDOW_WIDTH, RL_WINDOW_HEIGHT, RL_PROJECT_NAME );
    InitAudioDevice();

#if defined(RL_PLATFORM_WEB)
    emscripten_set_main_loop( __update_loop__, 0, 1 );
#else
    SetTargetFPS( RL_FRAMERATE );

    while( !WindowShouldClose() ) {
        __update_loop__();
        if( SHOULD_QUIT ) {
            break;
        }
    }
#endif

    rl_shutdown( GAME_MEMORY );
    CloseWindow();
    return 0;
}

void __update_loop__(void) {
    if( is_first_frame ) {
        if( !rl_initialize( ARG_COUNT, ARG_PTR, &GAME_MEMORY ) ) {
            SHOULD_QUIT = true;
            return;
        }
        is_first_frame = false;
    }

    if( !rl_update( GAME_MEMORY ) ) {
        SHOULD_QUIT = true;
        return;
    }
}

