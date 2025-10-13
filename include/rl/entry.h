#if !defined(RL_ENTRY_H)
#define RL_ENTRY_H
/**
 * @file   entry.h
 * @brief  Raylib Template: Entry Point.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   October 09, 2025
*/
#include "rl/common.h"

bool rl_initialize( int argc, char** argv, void** out_memory );
bool rl_update( void* memory );
void rl_shutdown( void* memory );

#endif /* header guard */
