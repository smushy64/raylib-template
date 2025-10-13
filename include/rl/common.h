#if !defined(RL_COMMON_H)
#define RL_COMMON_H
/**
 * @file   common.h
 * @brief  Raylib Template: Common.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   October 09, 2025
*/
// IWYU pragma: begin_exports
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include <stdio.h>
#include <stdlib.h>
// IWYU pragma: end_exports

#if !defined(RL_PROJECT_NAME)
    #define RL_PROJECT_NAME "Raylib Project"
#endif

#if !defined(RL_WINDOW_WIDTH)
    #define RL_WINDOW_WIDTH 800
#endif

#if !defined(RL_WINDOW_HEIGHT)
    #define RL_WINDOW_HEIGHT 600
#endif

#if !defined(RL_FRAMERATE)
    #define RL_FRAMERATE 60
#endif

#if RL_FRAMERATE < 30
    #undef RL_FRAMERATE
    #define RL_FRAMERATE 30
#endif

extern bool SHOULD_QUIT;

#endif /* header guard */
