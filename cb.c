/**
 * @file   cb.c
 * @brief  Build system for Raylib template project.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   October 06, 2025
*/

// NOTE(alicia): 
// For non-Void Linux users,
// set MINGW_GCC, MINGW_G++ and MINGW_AR
// environment variables in
// order to cross-compile to Windows.

// NOTE(alicia): 
// For cross-compilation to WASM,
// make sure EMSDK is set or
// emcc, em++ and emar are set in PATH.

#include "cb.h"
#include "flagship.h"
#include "ini-parse.h"

#define CFG_PATH                       "cb.cfg"
#define CFG_DEFAULT_PROJECT_NAME       "Raylib Template"
#define CFG_DEFAULT_PROJECT_FILE_NAME  "raylib-template"
#define CFG_DEFAULT_WINDOW_WIDTH       800
#define CFG_DEFAULT_WINDOW_HEIGHT      600
#define CFG_DEFAULT_FRAMERATE          60
#define CFG_MINIMUM_FRAMERATE          24

enum Error {
    /* none */
    E_NONE,
    /* none */
    E_PARSE_ARGS,
    /* %s - proc name */
    /* %d - exit code */
    E_SUBPROC,
    /* %s - directory */
    /* %s - reason */
    E_DIR_CREATE,
    /* %s - proc name */
    E_SUBPROC_CREATE,
    /* none */
    E_SUBPROC_GROUP_FAIL,
    /* none */
    E_PROJECT_SOURCES_NOT_EXIST,
    /* none */
    E_RAYLIB_MISSING,
    /* %s - dst path */
    /* %s - src path */
    E_MOVE_FILE,
    /* %s - name of process */
    E_MISSING_PROC,
    /* none */
    E_EDITOR_SINGLE_SOURCE,
    /* none */
    E_EDITOR_NO_SOURCE,
};

typedef enum {
    M_HELP,
    M_BUILD,
    M_RUN,
    M_PKG,
    M_ED,

    M_COUNT
} Mode;
const char* MODE_NAMES[] = {
    "help",
    "build",
    "run",
    "pkg",
    "ed",
    NULL
};
const char* string_from_mode( Mode mode );
Mode mode_from_string( const char* string );

typedef enum {
    T_NATIVE,
    T_WINDOWS,
    T_GNU_LINUX,
    T_WASM,

    T_COUNT
} Target;
const char* TARGET_NAMES[] = {
    "native",
    "windows",
    "gnu-linux",
    "wasm",
    NULL
};
const char* string_from_target( Target target );
Target target_from_string( const char* string );

typedef struct {
    Target target;
    bool   rebuild_vendor_c;
    bool   rebuild_vendor_cpp;
    bool   rebuild_raylib;
    bool   release;
    bool   prefer_cpp;
    bool   no_rebuild;
} Opt;

typedef struct {
    const char* project_file_name;
    const char* project_name;
    int window_width;
    int window_height;
    int framerate;
} Cfg;

Cfg CFG = {
    .project_file_name = CFG_DEFAULT_PROJECT_FILE_NAME,
    .project_name      = CFG_DEFAULT_PROJECT_NAME,
    .window_width      = CFG_DEFAULT_WINDOW_WIDTH,
    .window_height     = CFG_DEFAULT_WINDOW_HEIGHT,
    .framerate         = CFG_DEFAULT_FRAMERATE
};

IniCtx CFG_CTX = {0};

FShipContext FLS = {0};
CmdBuf CMD = {0};
ProcessGroup PROC = {0};

StringBuf STRING = {0};

int err_va( int code, va_list va );
int err( int code, ... );

int __dir_create_many_( const char* first, ... );
#define dir_create_many(...) \
    __dir_create_many_( __VA_ARGS__ __VA_OPT__(,) NULL )

void fls_define(void);

void opt_init( Mode mode, Opt* out_opt );

int mode_build( Opt* opt );
int mode_run( Opt* opt );
int mode_pkg( Opt* opt );
int mode_ed( Opt* opt );

int         PROC_ARG_COUNT = 0;
char**      PROC_ARG_PTR   = NULL;
const char* PROC_NAME      = NULL;
int         PROC_LAST_ARG  = 0;

size_t TARGET_PATH    = 0;
size_t TARGET_DIR     = 0;
size_t TARGET_OBJ_DIR = 0;

size_t COMPILER_C   = 0;
size_t COMPILER_CPP = 0;
size_t ARCHIVER     = 0;

void __init_string(void);

const char* get_project_file_name(void);
const char* get_project_name(void);
int get_window_width(void);
int get_window_height(void);
int get_framerate(void);

const char* get_target_path( Target target );
const char* get_target_dir( Target target );
const char* get_target_obj_dir( Target target );

const char* get_compiler_c( Target target );
const char* get_compiler_cpp( Target target );
const char* get_archiver( Target target );

void parse_cfg(void);

int main( int argc, char** argv ) {
    CB_BUILD_INIT();
    PROC_ARG_COUNT = argc;
    PROC_ARG_PTR   = argv;
    PROC_NAME      = PROC_ARG_PTR[0];

    parse_cfg();
    fls_define();

    if( argc <= 1 ) {
        fls_help( &FLS, "help", true );
        fls_free( &FLS );
        return 0;
    }

    if( !fls_parse( &FLS, PROC_ARG_COUNT, PROC_ARG_PTR, &PROC_LAST_ARG ) ) {
        return E_PARSE_ARGS;
    }

    Mode mode = mode_from_string( fls_query_mode( &FLS ) );

    int error = E_NONE;

    switch( mode ) {
        case M_HELP: {
            bool is_set = false;
            const char* help_mode = fls_read_str( &FLS, NULL, .is_set=&is_set );

            if( is_set ) {
                fls_help( &FLS, help_mode, false );
            } else {
                fls_help( &FLS, "help", true );
            }
        } break;

        case M_BUILD:
        case M_RUN:
        case M_PKG:
        case M_ED: {
            Opt opt = {0};
            opt_init( mode, &opt );

            switch( mode ) {
                case M_BUILD: {
                    error = mode_build( &opt );
                } break;
                case M_RUN: {
                    error = mode_run( &opt );
                } break;
                case M_PKG: {
                    error = mode_pkg( &opt );
                } break;
                case M_ED: {
                    error = mode_ed( &opt );
                } break;

                case M_HELP:
                case M_COUNT: break;
            }
        } break;

        case M_COUNT:
            break;
    }

    fls_free( &FLS );
    return error;
}

void __init_string(void) {
    if( STRING.ptr ) {
        return;
    }

    CB_BUF_RESERVE( &STRING, 256 );

    const char* whitespace = "    ";
    CB_BUF_APPEND( &STRING, whitespace, 4 );
}

// TODO(alicia): 
// read these values from cb.cfg!

const char* get_project_file_name(void) {
    return CFG.project_file_name;
}
const char* get_project_name(void) {
    return CFG.project_name;
}
int get_window_width(void) {
    return CFG.window_width;
}
int get_window_height(void) {
    return CFG.window_height;
}
int get_framerate(void) {
    return CFG.framerate;
}

const char* get_target_path( Target target ) {
    if( !TARGET_PATH ) {
        __init_string();

        const char* dir = get_target_dir( target );
        const char* ext = "";
        switch( target ) {
            case T_WINDOWS: {
                ext = ".exe";
            } break;
            case T_GNU_LINUX: break;
            case T_WASM: {
                ext = ".html";
            } break;

            // NOTE(alicia): T_NATIVE and T_COUNT should never be an option
            case T_NATIVE:
            case T_COUNT:
                break;
        }

        TARGET_PATH = STRING.len;

        string_buf_fmt( &STRING, "%s/%s%s", dir, get_project_file_name(), ext );
        CB_BUF_PUSH( &STRING, 0 );
    }

    return STRING.ptr + TARGET_PATH;
}

const char* get_target_dir( Target target ) {
    if( !TARGET_DIR ) {
        __init_string();

        const char* path = "./build";
        switch( target ) {
            case T_WINDOWS: {
                path = "./build/windows";
            } break;
            case T_GNU_LINUX: {
                path = "./build/gnu-linux";
            } break;
            case T_WASM: {
                path = "./build/wasm";
            } break;

            // NOTE(alicia): T_NATIVE and T_COUNT should never be an option
            case T_NATIVE:
            case T_COUNT:
                break;
        }

        TARGET_DIR = STRING.len;

        string_buf_fmt( &STRING, "%s", path );
        CB_BUF_PUSH( &STRING, 0 );
    }

    return STRING.ptr + TARGET_DIR;
}

const char* get_target_obj_dir( Target target ) {
    if( !TARGET_OBJ_DIR ) {
        __init_string();

        const char* dir = get_target_dir( target );

        TARGET_OBJ_DIR = STRING.len;

        string_buf_fmt( &STRING, "%s/obj", dir );
        CB_BUF_PUSH( &STRING, 0 );
    }

    return STRING.ptr + TARGET_OBJ_DIR;
}

const char* get_compiler_c( Target target ) {
    if( !COMPILER_C ) {
        __init_string();

        const char* path      = NULL;
        bool        allocated = false;
        switch( target ) {
            case T_WINDOWS: {
                path = "x86_64-w64-mingw32-gcc";

                const char* env = env_query( "MINGW_GCC" );
                if( env ) {
                    path = env;
                }
            } break;
            case T_GNU_LINUX: {
                path = "gcc";
            } break;
            case T_WASM: {
                path = "emcc";

                const char* env = env_query( "EMSDK" );
                if( env ) {
                    size_t offset = STRING.len;
                    string_buf_fmt( &STRING, "%s/upstream/emscripten/emcc", env );
                    CB_BUF_PUSH( &STRING, 0 );

                    path      = STRING.ptr + offset;
                    allocated = true;

                    COMPILER_C = offset;
                }
            } break;

            case T_NATIVE:
            case T_COUNT:
                break;
        }

        if( !path || !proc_exists( path ) ) {
            CB_PANIC( "could not find C compiler for target %s!", string_from_target( target ) );
        }

        if( !allocated ) {
            COMPILER_C = STRING.len;

            string_buf_fmt( &STRING, "%s", path );
            CB_BUF_PUSH( &STRING, 0 );
        }
    }

    return STRING.ptr + COMPILER_C;
}
const char* get_compiler_cpp( Target target ) {
    if( !COMPILER_CPP ) {
        __init_string();

        const char* path      = NULL;
        bool        allocated = false;
        switch( target ) {
            case T_WINDOWS: {
                path = "x86_64-w64-mingw32-g++";

                const char* env = env_query( "MINGW_G++" );
                if( env ) {
                    path = env;
                }
            } break;
            case T_GNU_LINUX: {
                path = "g++";
            } break;
            case T_WASM: {
                path = "em++";

                const char* env = env_query( "EMSDK" );
                if( env ) {
                    size_t offset = STRING.len;
                    string_buf_fmt( &STRING, "%s/upstream/emscripten/em++", env );
                    CB_BUF_PUSH( &STRING, 0 );

                    path      = STRING.ptr + offset;
                    allocated = true;

                    COMPILER_CPP = offset;
                }
            } break;

            case T_NATIVE:
            case T_COUNT:
                break;
        }

        if( !path || !proc_exists( path ) ) {
            CB_PANIC( "could not find C++ compiler for target %s!", string_from_target( target ) );
        }

        if( !allocated ) {
            COMPILER_CPP = STRING.len;

            string_buf_fmt( &STRING, "%s", path );
            CB_BUF_PUSH( &STRING, 0 );
        }
    }

    return STRING.ptr + COMPILER_CPP;
}
const char* get_archiver( Target target ) {
    if( !ARCHIVER ) {
        __init_string();

        const char* path      = NULL;
        bool        allocated = false;
        switch( target ) {
            case T_WINDOWS: {
                path = "x86_64-w64-mingw32-ar";

                const char* env = env_query( "MINGW_AR" );
                if( env ) {
                    path = env;
                }
            } break;
            case T_GNU_LINUX: {
                path = "ar";
            } break;
            case T_WASM: {
                path = "emar";

                const char* env = env_query( "EMSDK" );
                if( env ) {
                    size_t offset = STRING.len;
                    string_buf_fmt( &STRING, "%s/upstream/emscripten/emar", env );
                    CB_BUF_PUSH( &STRING, 0 );

                    path      = STRING.ptr + offset;
                    allocated = true;

                    ARCHIVER = offset;
                }
            } break;

            case T_NATIVE:
            case T_COUNT:
                break;
        }

        if( !path || !proc_exists( path ) ) {
            CB_PANIC( "could not find archiver for target %s!", string_from_target( target ) );
        }

        if( !allocated ) {
            ARCHIVER = STRING.len;

            string_buf_fmt( &STRING, "%s", path );
            CB_BUF_PUSH( &STRING, 0 );
        }
    }

    return STRING.ptr + ARCHIVER;
}

int build_vendor_c( Opt* opt, const char* override_obj_dir ) {
    int error = E_NONE;

    const char* src = "vendor/src/sources.c";
    if( !path_exists( src ) ) {
        return error;
    }

    time_t src_time = 0;
    path_query_time_create( src, &src_time );

    const char* target_dir = get_target_dir( opt->target );
    const char* obj_dir    = get_target_obj_dir( opt->target );

    if( override_obj_dir ) {
        obj_dir = override_obj_dir;
    } else {
        if( (error = dir_create_many( "build", target_dir, obj_dir ) ) ) {
            return error;
        }
    }

    const char* compiler = get_compiler_c( opt->target );

    const char* dst = local_fmt( "%s/vendor-c.o", obj_dir ); {
        time_t dst_time = 0;

        bool rebuild = opt->rebuild_vendor_c;

        if( !rebuild ) {
            rebuild =
                !path_query_time_create( dst, &dst_time ) ||
                time_diff( dst_time, src_time ) < 0.0;
        }

        if( !rebuild ) {
            return error;
        }
    }

    cmd_reset( &CMD );
    cmd_add( &CMD, compiler );
    cmd_add( &CMD, src );

    if( path_exists( "vendor/include" ) ) {
        cmd_add( &CMD, "-Ivendor/include" );
    }
    if( path_exists( "include" ) ) {
        cmd_add( &CMD, "-Iinclude" );
    }
    cmd_add( &CMD, "-Iraylib/src" );

    CB_CMD_APPEND( &CMD, "-c", "-o", dst );

    switch( opt->target ) {
        case T_WINDOWS: {
            if( opt->release ) {
                CB_CMD_APPEND( &CMD, "-O2" );
            } else {
                CB_CMD_APPEND( &CMD, "-O0", "-g" );
            }
        } break;
        case T_GNU_LINUX: {
            if( opt->release ) {
                CB_CMD_APPEND( &CMD, "-O2" );
            } else {
                CB_CMD_APPEND( &CMD, "-O0", "-ggdb" );
            }
        } break;
        case T_WASM: {
            if( opt->release ) {
                CB_CMD_APPEND( &CMD, "-O2" );
            } else {
                CB_CMD_APPEND( &CMD, "-O0", "-g" );
            }
        } break;

        case T_NATIVE:
        case T_COUNT:
            break;
    }

    CB_INFO( "building C vendor libraries . . ." );
    int exit_code = 0;
    if( (exit_code = CB_EXEC( CMD.cmd )) ) {
        return err( E_SUBPROC, local_fmt( "%s vendor C", compiler ), exit_code );
    }

    return error;
}
int build_vendor_cpp( Opt* opt, const char* override_obj_dir ) {
    int error = E_NONE;

    const char* src = "vendor/src/sources.cpp";
    if( !path_exists( src ) ) {
        return error;
    }

    time_t src_time = 0;
    path_query_time_create( src, &src_time );

    const char* target_dir = get_target_dir( opt->target );
    const char* obj_dir    = get_target_obj_dir( opt->target );

    if( override_obj_dir ) {
        obj_dir = override_obj_dir;
    } else {
        if( (error = dir_create_many( "build", target_dir, obj_dir ) ) ) {
            return error;
        }
    }

    const char* compiler = get_compiler_cpp( opt->target );

    const char* dst = local_fmt( "%s/vendor-c++.o", obj_dir ); {
        time_t dst_time = 0;

        bool rebuild = opt->rebuild_vendor_c;

        if( !rebuild ) {
            rebuild =
                !path_query_time_create( dst, &dst_time ) ||
                time_diff( dst_time, src_time ) < 0.0;
        }

        if( !rebuild ) {
            return error;
        }
    }

    cmd_reset( &CMD );
    cmd_add( &CMD, compiler );
    cmd_add( &CMD, src );

    if( path_exists( "vendor/include" ) ) {
        cmd_add( &CMD, "-Ivendor/include" );
    }
    if( path_exists( "include" ) ) {
        cmd_add( &CMD, "-Iinclude" );
    }
    cmd_add( &CMD, "-Iraylib/src" );

    CB_CMD_APPEND( &CMD, "-c", "-o", dst );

    switch( opt->target ) {
        case T_WINDOWS: {
            if( opt->release ) {
                CB_CMD_APPEND( &CMD, "-O2" );
            } else {
                CB_CMD_APPEND( &CMD, "-O0", "-g" );
            }
        } break;
        case T_GNU_LINUX: {
            if( opt->release ) {
                CB_CMD_APPEND( &CMD, "-O2" );
            } else {
                CB_CMD_APPEND( &CMD, "-O0", "-ggdb" );
            }
        } break;
        case T_WASM: {
            if( opt->release ) {
                CB_CMD_APPEND( &CMD, "-O2" );
            } else {
                CB_CMD_APPEND( &CMD, "-O0", "-g" );
            }
        } break;

        case T_NATIVE:
        case T_COUNT:
            break;
    }

    CB_INFO( "building C++ vendor libraries . . ." );
    int exit_code = 0;
    if( (exit_code = CB_EXEC( CMD.cmd )) ) {
        return err( E_SUBPROC, local_fmt( "%s vendor C++", compiler ), exit_code );
    }

    return error;
}
int raylib_push( const char* obj_dir, const char* path, Opt* opt ) {
    int error = E_NONE;

    const char* src = local_fmt("raylib/src/%s.c", path);
    const char* dst = local_fmt("%s/%s.o", obj_dir, path);

    if( path_exists( dst ) ) {
        if( !opt->rebuild_raylib ) {
            return error;
        }
    }

    cmd_reset( &CMD );

    const char* compiler = get_compiler_c( opt->target );

    switch( opt->target ) {
        case T_WINDOWS: {
            CB_CMD_APPEND( &CMD, compiler, src );
            CB_CMD_APPEND( &CMD, "-c", "-o", dst );

            CB_CMD_APPEND(
                &CMD,
                "-DPLATFORM_DESKTOP_GLFW", "-DGRAPHICS_API_OPENGL_33",
                "-Wno-missing-braces", "-Werror=pointer-arith",
                "-fno-strict-aliasing", "-std=c99",
                "-Werror=implicit-function-declaration",
                "-Iraylib/src", "-Iraylib/src/external/glfw/include",
                "-static-libgcc" );

            if( opt->release ) {
                cmd_add( &CMD, "-O1" );
            } else {
                CB_CMD_APPEND( &CMD, "-O0", "-g" );
            }
        } break;
        case T_GNU_LINUX: {
            CB_CMD_APPEND( &CMD, compiler, src );
            CB_CMD_APPEND( &CMD, "-c", "-o", dst );

            CB_CMD_APPEND( &CMD, "-std=c99" );
            CB_CMD_APPEND( &CMD, "-Iraylib/src", "-Iraylib/src/external/glfw/include" );
            CB_CMD_APPEND( &CMD, "-D_GNU_SOURCE", "-DPLATFORM_DESKTOP_GLFW", "-D_GLFW_X11" );
            CB_CMD_APPEND( &CMD, "-DGRAPHICS_API_OPENGL_33", "-Wno-missing-braces" );
            CB_CMD_APPEND( &CMD, "-Werror=pointer-arith", "-fno-strict-aliasing" );
            CB_CMD_APPEND( &CMD, "-Werror=implicit-function-declaration", "-Wno-macro-redefined" );

            if( opt->release ) {
                cmd_add( &CMD, "-O1" );
            } else {
                cmd_add( &CMD, "-O0" );
                cmd_add( &CMD, "-ggdb" );
            }
        } break;
        case T_WASM: {
            CB_CMD_APPEND( &CMD, compiler, src );
            CB_CMD_APPEND( &CMD, "-c", "-o", dst );

            CB_CMD_APPEND(
                &CMD, "-std=gnu99", "-Wall", "-DPLATFORM_WEB", "-DGRAPHICS_API_OPENGL_ES2" );

            // NOTE(alicia): no debug version
            cmd_add( &CMD, "-Os" );
        } break;

        case T_NATIVE:
        case T_COUNT:
            break;
    }

    if( CB_EXEC( CMD.cmd, .group=&PROC ) ) {
        return err( E_SUBPROC_CREATE, local_fmt( "%s %s", compiler, path ) );
    }

    return error;
}
int build_raylib( Opt* opt, const char* override_obj_dir ) {
    int error = E_NONE;

    if( !path_exists( "raylib" ) ) {
        return error = err( E_RAYLIB_MISSING );
    }

    const char* target_dir = get_target_dir( opt->target );
    const char* obj_dir    = get_target_obj_dir( opt->target );

    if( override_obj_dir ) {
        obj_dir = override_obj_dir;
    } else {
        if( (error = dir_create_many( "build", target_dir, obj_dir )) ) {
            return error;
        }
    }

    if( path_exists( local_fmt( "%s/libraylib.a", obj_dir ) ) ) {
        if( !opt->rebuild_raylib ) {
            return error;
        }
    }

    if( (error = raylib_push( obj_dir, "rcore", opt )) ) {
        return error;
    }

    if( (error = raylib_push( obj_dir, "rshapes", opt )) ) {
        return error;
    }

    if( (error = raylib_push( obj_dir, "rtextures", opt )) ) {
        return error;
    }

    if( (error = raylib_push( obj_dir, "rtext", opt )) ) {
        return error;
    }

    if( (error = raylib_push( obj_dir, "utils", opt )) ) {
        return error;
    }

    if( opt->target != T_WASM ) {
        if( (error = raylib_push( obj_dir, "rglfw", opt )) ) {
            return error;
        }
    }

    if( (error = raylib_push( obj_dir, "rmodels", opt )) ) {
        return error;
    }

    if( (error = raylib_push( obj_dir, "raudio", opt )) ) {
        return error;
    }

    if( PROC.len ) {
        if( !proc_group_join( &PROC, CB_WAIT_INF ) ) {
            return err( E_SUBPROC_GROUP_FAIL );
        }
    }

    const char* archiver = get_archiver( opt->target );

    const char* dst = local_fmt( "%s/libraylib.a", obj_dir );

    cmd_reset( &CMD );
    CB_CMD_APPEND( &CMD, archiver, "rcs", dst );
    cmd_add( &CMD, local_fmt( "%s/rcore.o", obj_dir ) );
    cmd_add( &CMD, local_fmt( "%s/rshapes.o", obj_dir ) );
    cmd_add( &CMD, local_fmt( "%s/rtextures.o", obj_dir ) );
    cmd_add( &CMD, local_fmt( "%s/rtext.o", obj_dir ) );
    cmd_add( &CMD, local_fmt( "%s/utils.o", obj_dir ) );

    if( opt->target != T_WASM ) {
        cmd_add( &CMD, local_fmt( "%s/rglfw.o", obj_dir ) );
    }

    cmd_add( &CMD, local_fmt( "%s/rmodels.o", obj_dir ) );
    cmd_add( &CMD, local_fmt( "%s/raudio.o", obj_dir ) );

    int exit_code = 0;
    if( (exit_code = CB_EXEC( CMD.cmd )) ) {
        return err( E_SUBPROC, local_fmt( "%s raylib", archiver ), exit_code );
    }

    return error;
}
int build_project( Opt* opt ) {
    int error = E_NONE;

    cmd_reset( &CMD );

    bool c_source_exists   = path_exists( "src/sources.c" );
    bool cpp_source_exists = path_exists( "src/sources.cpp" );

    const char* compiler = NULL;
    const char* src      = NULL;

    if( c_source_exists && cpp_source_exists ) {
        if( opt->prefer_cpp ) {
            src      = "src/sources.cpp";
            compiler = get_compiler_cpp( opt->target );
        } else {
            src      = "src/sources.c";
            compiler = get_compiler_c( opt->target );
        }
    } else if( c_source_exists ) {
        src      = "src/sources.c";
        compiler = get_compiler_c( opt->target );
    } else if( cpp_source_exists ) {
        src      = "src/sources.cpp";
        compiler = get_compiler_cpp( opt->target );
    } else {
        return err( E_PROJECT_SOURCES_NOT_EXIST );
    }

    CB_CMD_APPEND( &CMD, compiler, src );

    const char* target_path = get_target_path( opt->target );
    const char* target_dir  = get_target_dir( opt->target );
    const char* obj_dir     = get_target_obj_dir( opt->target );

    const char* temp = local_fmt( "%s/vendor-c.o", obj_dir );
    if( path_exists( temp ) ) {
        cmd_add( &CMD, temp );
    }

    temp = local_fmt( "%s/vendor-c++.o", obj_dir );
    if( path_exists( temp ) ) {
        cmd_add( &CMD, temp );
    }

    cmd_add( &CMD, local_fmt( "%s/libraylib.a", obj_dir ) );
    cmd_add( &CMD, "-Iraylib/src" );
    cmd_add( &CMD, "-Iinclude" );

    if( path_exists( "vendor/include" ) ) {
        cmd_add( &CMD, "-Ivendor/include" );
    }

    CB_CMD_APPEND( &CMD, "-o", target_path );

    CB_CMD_APPEND( &CMD, local_fmt("-DRL_PROJECT_NAME=\"%s\"", get_project_name() ) );
    CB_CMD_APPEND( &CMD, local_fmt(    "-DRL_WINDOW_WIDTH=%d", get_window_width() ) );
    CB_CMD_APPEND( &CMD, local_fmt(   "-DRL_WINDOW_HEIGHT=%d", get_window_height() ) );
    CB_CMD_APPEND( &CMD, local_fmt(       "-DRL_FRAMERATE=%d", get_framerate() ) );

    CB_CMD_APPEND( &CMD, "-Wall", "-Wextra", "-Werror=vla", "-Wno-missing-field-initializers" );

    switch( opt->target ) {
        case T_WINDOWS: {
            if( opt->release ) {
                CB_CMD_APPEND( &CMD, "-O2", "-mwindows" );
            } else {
                CB_CMD_APPEND( &CMD, "-O0", "-g", "-fuse-ld=lld", "-Wl,/debug" );
                cmd_add( &CMD, "-DRL_IS_DEBUG" );
            }

            CB_CMD_APPEND(
                &CMD,
                "-lkernel32",
                "-lgdi32",
                "-lwinmm",
                "-lopengl32",
                "-lshell32",
                "-Wno-class-memaccess",
                "-Wno-strict-aliasing",
                "-DRL_PLATFORM_WINDOWS" );
        } break;
        case T_GNU_LINUX: {
            if( opt->release ) {
                cmd_add( &CMD, "-O2" );
            } else {
                cmd_add( &CMD, "-O0" );
                cmd_add( &CMD, "-ggdb" );
                cmd_add( &CMD, "-DRL_IS_DEBUG" );
            }

            CB_CMD_APPEND(
                &CMD,
                "-lGL",
                "-lX11",
                "-lXrandr",
                "-lXinerama",
                "-lXi",
                "-lXcursor",
                "-lm",
                "-pthread",
                "-ldl",
                "-lrt",
                "-DRL_PLATFORM_LINUX" );
        } break;
        case T_WASM: {
            temp = local_fmt( "-L%s", obj_dir );

            CB_CMD_APPEND(
                &CMD,
                "-Os",
                "-Wall",
                "-DRL_PLATFORM_WEB",
                temp,
                "-s",
                "USE_GLFW=3",
                "--shell-file",
                "raylib/src/minshell.html" );

            if( !opt->release ) {
                cmd_add( &CMD, "-sASSERTIONS" );
                cmd_add( &CMD, "-DRL_IS_DEBUG" );
            }

            // TOTAL_MEMORY = 1073741824 (1 GiB)
            // STACK_SIZE   = 1048576    (1 MiB)

            cmd_add( &CMD, "-sTOTAL_MEMORY=1073741824" );
            cmd_add( &CMD, "-sSTACK_SIZE=1048576" );

            if( path_exists( "resources" ) ) {
                CB_CMD_APPEND( &CMD, "--preload-file", "resources" );
            }
        } break;

        case T_NATIVE:
        case T_COUNT:
            break;
    }

    int exit_code = 0;
    if( (exit_code = CB_EXEC( CMD.cmd )) ) {
        error = err( E_SUBPROC, local_fmt( "%s project", compiler ), exit_code );
    }

    if( opt->target == T_WASM ) {
        if( path_exists( target_path ) ) {
            const char* index_html = local_fmt( "%s/index.html", target_dir );

            if( !file_move( index_html, target_path, false ) ) {
                return err( E_MOVE_FILE, index_html, target_path );
            }
        }
    }

    return error;
}

int mode_build( Opt* opt ) {
    int error = E_NONE;

    double start = time_msec();
    if( !opt->no_rebuild ) {
        CB_INFO( "building project . . ." );
    }

    if( (error = build_raylib( opt, NULL )) ) {
        return error;
    }
    if( (error = build_vendor_c( opt, NULL )) ) {
        return error;
    }
    if( (error = build_vendor_cpp( opt, NULL )) ) {
        return error;
    }

    if( !opt->no_rebuild ) {
        if( (error = build_project( opt )) ) {
            return error;
        }

        double end = time_msec();
        CB_INFO( "build completed in %fmsec.", end - start );
    }


    return error;
}
int mode_run( Opt* opt ) {
    int error = E_NONE;

    if( (error = mode_build( opt )) ) {
        return error;
    }

    cmd_reset( &CMD );

    const char* target_path = get_target_path( opt->target );

    switch( opt->target ) {
        case T_WINDOWS: {
            if( !proc_exists( "wine" ) ) {
                return error = err( E_MISSING_PROC, "wine" );
            }

            CB_CMD_APPEND( &CMD, "wine", target_path );

            for( int i = PROC_LAST_ARG + 1; i < PROC_ARG_COUNT; ++i ) {
                cmd_add( &CMD, PROC_ARG_PTR[i] );
            }

            CB_INFO( "running project for target: %s . . .", string_from_target( opt->target ) );

            int exit_code = 0;
            if( ( exit_code = CB_EXEC( CMD.cmd ) ) ) {
                error = err( E_SUBPROC, target_path, exit_code );
            }
        } break;
        case T_GNU_LINUX: {
            CB_CMD_APPEND( &CMD, target_path );

            for( int i = PROC_LAST_ARG + 1; i < PROC_ARG_COUNT; ++i ) {
                cmd_add( &CMD, PROC_ARG_PTR[i] );
            }

            CB_INFO( "running project for target: %s . . .", string_from_target( opt->target ) );

            int exit_code = 0;
            if( ( exit_code = CB_EXEC( CMD.cmd ) ) ) {
                error = err( E_SUBPROC, target_path, exit_code );
            }
        } break;
        case T_WASM: {
            if( !proc_exists( "python3" ) ) {
                return error = err( E_MISSING_PROC, "python3" );
            }

            CB_CMD_APPEND( &CMD, "python3", "-m", "http.server" );

            CB_INFO( "spawning python server for wasm target . . ." );
            Process pid;
            int exit_code = 0;
            if( (exit_code = CB_EXEC( CMD.cmd, .proc=&pid )) ) {
                return error = err( E_SUBPROC, "python3 server", exit_code );
            }

            CB_INFO( "to stop server, Ctrl+C" );

            CB_INFO( "opening webpage . . ." );
            mt_sleep( 5000 );

            CB_INFO( "if server does not respond, refresh the page until it does." );

            cmd_reset( &CMD );
            CB_CMD_APPEND( &CMD, "xdg-open", "http://localhost:8000/build/wasm/index.html" );

            if( (exit_code = CB_EXEC( CMD.cmd )) ) {
                proc_signal( &pid, PSIG_KILL );
                mt_sleep( 10 );

                return error = err( E_SUBPROC, "xdg-open wasm", exit_code );
            }

            proc_wait( &pid, CB_WAIT_INF );
        } break;

        // NOTE(alicia): T_NATIVE and T_COUNT should never be an option at this point.
        case T_NATIVE:
        case T_COUNT: break;
    }

    return error;
}
int mode_pkg( Opt* opt ) {
    int error = E_NONE;

    if( (error = mode_build( opt )) ) {
        return error;
    }

    cmd_reset( &CMD );

    if( (error = dir_create_many( "bin" )) ) {
        return error;
    }

    switch( opt->target ) {
        case T_WINDOWS: {
            if( !proc_exists( "zip" ) ) {
                return error = err( E_MISSING_PROC, "zip" );
            }

            CB_INFO( "compressing and archiving for windows x64 . . ." );

            if( path_exists( "resources" ) ) {
                CB_CMD_APPEND( &CMD, "zip", "-r", "resources.zip", "resources" );

                int exit_code = 0;
                if( (exit_code = CB_EXEC( CMD.cmd )) ) {
                    return error = err( E_SUBPROC, "zip resources", exit_code );
                }

                if( !file_move( "build/windows/resources.zip", "resources.zip", false ) ) {
                    return error =
                        err( E_MOVE_FILE, "build/windows/resources.zip", "resources.zip" );
                }

                cmd_reset( &CMD );
            }

            CB_CMD_APPEND( &CMD, "zip", "resources.zip", local_fmt( "%s.exe", get_project_file_name() ) );

            int exit_code = 0;
            if( (exit_code = CB_EXEC( CMD.cmd, .wd="build/windows" )) ) {
                if( path_exists( "build/windows/resources.zip" ) ) {
                    file_remove( "build/windows/resources.zip" );
                }
                return error = err( E_SUBPROC, "zip executable", exit_code );
            }

            const char* dst = local_fmt( "bin/%s-windows-x64.zip", get_project_file_name() );
            const char* src = "build/windows/resources.zip";

            if( !file_move( dst, src, false ) ) {
                if( path_exists( src ) ) {
                    file_remove( src );
                }

                return err( E_MOVE_FILE, dst, src );
            }

            CB_INFO( "archived project at %s", dst );
        } break;
        case T_GNU_LINUX: {
            if( !proc_exists( "tar" ) ) {
                return error = err( E_MISSING_PROC, "tar" );
            }

            CB_INFO( "compressing and archiving for linux x86_64 . . ." );

            CB_CMD_APPEND(
                &CMD,
                "tar", "-cJf",
                local_fmt( "bin/%s-linux-x86_64.tar.xz", get_project_file_name() ) );

            CB_CMD_APPEND( &CMD, "-C", "build/gnu-linux", get_project_file_name() );

            if( path_exists( "resources" ) ) {
                CB_CMD_APPEND( &CMD, "-C", "../../", "resources" );
            }

            CB_CMD_APPEND( &CMD, "--transform=s,^,%s-linux-x86_64/,", get_project_file_name() );

            int exit_code = 0;
            if( (exit_code = CB_EXEC( CMD.cmd )) ) {
                return err( E_SUBPROC, local_fmt( "tar %s (linux)", get_project_file_name() ), exit_code );
            }

            CB_INFO( "archived project at bin/%s-linux-x86_64.tar.xz", get_project_file_name() );
        } break;
        case T_WASM: {
            if( !proc_exists( "zip" ) ) {
                return error = err( E_MISSING_PROC, "zip" );
            }

            CB_INFO( "compressing and archiving for wasm . . ." );

            CB_CMD_APPEND(
                &CMD,
                "zip",
                "files.zip",
                "index.html",
                local_fmt( "%s.js", get_project_file_name() ),
                local_fmt( "%s.wasm", get_project_file_name() )
            );

            if( path_exists( local_fmt( "build/wasm/%s.data", get_project_file_name() ) ) ) {
                cmd_add( &CMD, local_fmt( "%s.data", get_project_file_name() ) );
            }

            int exit_code = 0;
            if( (exit_code = CB_EXEC( CMD.cmd, .wd="build/wasm" )) ) {
                return error = err( E_SUBPROC, "zip wasm resources", exit_code );
            }

            const char* dst = local_fmt( "bin/%s-wasm.zip", get_project_file_name() );
            const char* src = "build/wasm/files.zip";
            if( !file_move( dst, src, false ) ) {
                if( path_exists( src ) ) {
                    file_remove( src );
                }
                return error = err( E_MOVE_FILE, dst, src );
            }

            CB_INFO( "archived project at %s", dst );
        } break;

        case T_NATIVE:
        case T_COUNT:
            break;
    }

    return error;
}
int build_editor( Opt* opt ) {
    int error = E_NONE;

    const char* target_dir = "build/editor";
    const char* obj_dir    = "build/editor/obj";

    if( (error = dir_create_many( "build", target_dir, obj_dir )) ) {
        return error;
    }

    if( (error = build_vendor_c( opt, obj_dir )) ) {
        return error;
    }

    if( (error = build_vendor_cpp( opt, obj_dir )) ) {
        return error;
    }

    if( (error = build_raylib( opt, obj_dir )) ) {
        return error;
    }

    cmd_reset( &CMD );

    bool c_source_exists   = path_exists( "editor/src/sources.c" );
    bool cpp_source_exists = path_exists( "editor/src/sources.cpp" );

    const char* compiler = NULL;
    const char* src      = NULL;

    if( c_source_exists && cpp_source_exists ) {
        return err( E_EDITOR_SINGLE_SOURCE );
    } else if( c_source_exists ) {
        src      = "editor/src/sources.c";
        compiler = get_compiler_c( opt->target );
    } else if( cpp_source_exists ) {
        src      = "editor/src/sources.cpp";
        compiler = get_compiler_cpp( opt->target );
    } else {
        return err( E_EDITOR_NO_SOURCE );
    }

    CB_CMD_APPEND( &CMD, compiler, src );

    const char* temp = NULL;

    temp = local_fmt( "%s/vendor-c.o", obj_dir );
    if( path_exists( temp ) ) {
        cmd_add( &CMD, temp );
    }

    temp = local_fmt( "%s/vendor-c++.o", obj_dir );
    if( path_exists( temp ) ) {
        cmd_add( &CMD, temp );
    }

    temp = local_fmt( "%s/libraylib.a", obj_dir );
    cmd_add( &CMD, temp );
    cmd_add( &CMD, "-Iraylib/src" );
    cmd_add( &CMD, "-Iinclude" );

    if( path_exists( "vendor/include" ) ) {
        cmd_add( &CMD, "-Ivendor/include" );
    }

    const char* dst = local_fmt( "%s/editor", target_dir );
    CB_CMD_APPEND( &CMD, "-o", dst );

    CB_CMD_APPEND( &CMD, "-Wall", "-Wextra", "-Werror=vla", "-Wno-missing-field-initializers" );

    switch( opt->target ) {
        case T_WINDOWS: {
            CB_CMD_APPEND( &CMD, "-O2", "-mwindows" );

            CB_CMD_APPEND(
                &CMD,
                "-lkernel32",
                "-lgdi32",
                "-lwinmm",
                "-lopengl32",
                "-lshell32",
                "-Wno-class-memaccess",
                "-Wno-strict-aliasing",
                "-DRL_PLATFORM_WINDOWS" );
        } break;
        case T_GNU_LINUX: {
            cmd_add( &CMD, "-O2" );

            CB_CMD_APPEND(
                &CMD,
                "-lGL",
                "-lX11",
                "-lXrandr",
                "-lXinerama",
                "-lXi",
                "-lXcursor",
                "-lm",
                "-pthread",
                "-ldl",
                "-lrt",
                "-DRL_PLATFORM_LINUX" );
        } break;
        // NOTE(alicia): target WASM can never be native.
        case T_WASM: break;

        case T_NATIVE:
        case T_COUNT:
            break;
    }

    CB_INFO( "building editor . . ." );

    int exit_code = 0;
    if( (exit_code = CB_EXEC( CMD.cmd )) ) {
        return error = err( E_SUBPROC, local_fmt( "%s editor", compiler ), exit_code );
    }

    cmd_reset( &CMD );

    CB_CMD_APPEND( &CMD, dst );

    for( int i = PROC_LAST_ARG + 1; i < PROC_ARG_COUNT; ++i ) {
        cmd_add( &CMD, PROC_ARG_PTR[i] );
    }

    CB_INFO( "running editor . . ." );

    exit_code = 0;
    if( (exit_code = CB_EXEC( CMD.cmd )) ) {
        return err( E_SUBPROC, dst, exit_code );
    }

    return error;
}
int mode_ed( Opt* opt ) {
    int error = E_NONE;

    if( (error = build_editor( opt )) ) {
        return error;
    }

    return error;
}

int err_va( int code, va_list va ) {

    switch( (enum Error)code ) {
        case E_NONE:
        case E_PARSE_ARGS: break;
        case E_SUBPROC: {
            const char* proc_name = va_arg( va, const char* );
            int         exit_code = va_arg( va, int );

            if( !exit_code ) {
                return E_NONE;
            }

            CB_ERROR( "sub-process '%s' failed with exit code %d!", proc_name, exit_code );
        } break;
        case E_DIR_CREATE: {
            const char* path   = va_arg( va, const char* );
            const char* reason = va_arg( va, const char* );

            CB_ERROR( "failed to create '%s'! reason: %s", path, reason );
        } break;
        case E_SUBPROC_CREATE: {
            const char* proc_name = va_arg( va, const char* );

            CB_ERROR( "failed to start sub-process '%s'!", proc_name );
        } break;
        case E_SUBPROC_GROUP_FAIL: {
            for( size_t i = 0; i < PROC.len; ++i ) {
                if( PROC.ptr[i].result > 0 ) {
                    CB_ERROR( "proc %zu failed with exit code %d!", i, PROC.ptr[i].result );
                }
            }
        } break;
        case E_PROJECT_SOURCES_NOT_EXIST: {
            CB_ERROR( "src/sources.c or src/sources.cpp not found!" );
        } break;
        case E_RAYLIB_MISSING: {
            CB_ERROR( "raylib was not found! use: git submodule add https://github.com/raysan5/raylib.git" );
        } break;
        case E_MOVE_FILE: {
            const char* dst = va_arg( va, const char* );
            const char* src = va_arg( va, const char* );
            CB_ERROR( "failed to move %s to %s!", src, dst );
        } break;
        case E_MISSING_PROC: {
            const char* proc_name = va_arg( va, const char* );
            CB_ERROR( "failed to find required process: %s", proc_name );
        } break;
        case E_EDITOR_SINGLE_SOURCE: {
            CB_ERROR(
                "found editor/src/sources.c AND editor/src/sources.cpp! "
                "editor should only have one source!" );
        } break;
        case E_EDITOR_NO_SOURCE: {
            CB_ERROR(
                "no source files found for editor! "
                "requires either editor/src/sources.c or editor/src/sources.cpp!" );
        } break;
    }

    return code;
}
int err( int code, ... ) {
    va_list va;
    va_start( va, code );

    int retval = err_va( code, va );

    va_end( va );
    return retval;
}

void opt_init( Mode mode, Opt* opt ) {
    switch( mode ) {
        case M_BUILD:
        case M_RUN: {
            opt->target = target_from_string( fls_read_str( &FLS, "target" ) );

            opt->release = fls_read_flag( &FLS, "release" );

            if( opt->release || fls_read_flag( &FLS, "rebuild" ) ) {
                opt->rebuild_vendor_c   = true;
                opt->rebuild_vendor_cpp = true;
                opt->rebuild_raylib     = true;
            } else {
                opt->rebuild_vendor_c   = fls_read_flag( &FLS, "rebuild-vendor-C" );
                opt->rebuild_vendor_cpp = fls_read_flag( &FLS, "rebuild-vendor-C++" );
                opt->rebuild_raylib     = fls_read_flag( &FLS, "rebuild-raylib" );
            }

            opt->prefer_cpp = fls_read_flag( &FLS, "c++" );

            if( mode == M_RUN ) {
                opt->no_rebuild = fls_read_flag( &FLS, "no-rebuild" );

                if( opt->no_rebuild ) {
                    const char* target_path = get_target_path( opt->target );

                    if( !path_exists( target_path ) )  {
                        opt->no_rebuild = false;
                    }
                }
            }
        } break;
        case M_PKG: {
            opt->target = target_from_string( fls_read_str( &FLS, "target" ) );

            opt->no_rebuild = fls_read_flag( &FLS, "no-rebuild" );

            if( opt->no_rebuild ) {
                opt->release            =
                opt->rebuild_vendor_c   =
                opt->rebuild_vendor_cpp =
                opt->rebuild_raylib     = false;
            } else {
                opt->release            =
                opt->rebuild_vendor_c   =
                opt->rebuild_vendor_cpp =
                opt->rebuild_raylib     = true;
            }

            if( opt->no_rebuild ) {
                const char* target_path = get_target_path( opt->target );

                if( !path_exists( target_path ) )  {
                    opt->no_rebuild = false;
                }
            }
        } break;
        case M_ED: {
            opt->rebuild_raylib = false;
            opt->release        = true;
            opt->target         = target_from_string( "native" );
        } break;

        case M_HELP:
        case M_COUNT:
            break;
    }
}

void fls_define(void) {
    fls_set_program_name( &FLS, PROC_NAME );
    fls_set_description( &FLS, "Raylib project: %s", get_project_name() );

    fls_mode_begin( &FLS, "help" ); {
        fls_mode_set_description( &FLS, "Print help message and then exit." );

        fls_add_str(
            &FLS, NULL,
            .description="Name of mode to print arguments for.",
            .is_terminating=true,
            .str={ .valid=MODE_NAMES }
        );

        fls_mode_end( &FLS );
    }

    fls_mode_begin( &FLS, "build" ); {
        fls_mode_set_description( &FLS, "Build project." );

        fls_add_str(
            &FLS, "target",
            .aliases=fls_strings("t"),
            .description="Set compilation target.",
            .note="Windows requires MinGW GCC/G++, "
                  "GNU-Linux requires either clang/clang++ or "
                  "GCC/G++ and WASM requires EMCC/EMC++",
            .default_value=TARGET_NAMES[0],
            .str={ .valid=TARGET_NAMES },
        );
        fls_add_flag(
            &FLS, "c++",
            .description="Prefer src/sources.cpp over src/sources.c if both exist."
        );
        fls_add_flag(
            &FLS, "rebuild-vendor-c",
            .description="Rebuild C vendor libraries. (./vendor/sources.c)"
        );
        fls_add_flag(
            &FLS, "rebuild-vendor-c++",
            .description="Rebuild C++ vendor libraries. (./vendor/sources.cpp)"
        );
        fls_add_flag(
            &FLS, "rebuild-raylib",
            .description="Rebuild raylib dependencies."
        );
        fls_add_flag(
            &FLS, "rebuild",
            .description="Rebuild all dependencies.",
        );
        fls_add_flag(
            &FLS, "release",
            .description="Build project with debug symbols stripped and all optimizations enabled.",
            .note="This flag also rebuilds all dependencies.",
        );

        fls_mode_end( &FLS );
    }

    fls_mode_begin( &FLS, "run" ); {
        fls_mode_set_description( &FLS, "Build and then run project." );

        fls_add_str(
            &FLS, "target",
            .aliases=fls_strings("t"),
            .description="Set compilation target.",
            .note="Windows requires MinGW GCC/G++, "
                  "GNU-Linux requires either clang/clang++ or "
                  "GCC/G++ and WASM requires EMCC/EMC++",
            .default_value=TARGET_NAMES[0],
            .str={ .valid=TARGET_NAMES },
        );
        fls_add_flag(
            &FLS, "c++",
            .description="Prefer src/sources.cpp over src/sources.c if both exist."
        );
        fls_add_flag(
            &FLS, "rebuild-vendor-c",
            .description="Rebuild C vendor libraries. (./vendor/sources.c)"
        );
        fls_add_flag(
            &FLS, "rebuild-vendor-c++",
            .description="Rebuild C++ vendor libraries. (./vendor/sources.cpp)"
        );
        fls_add_flag(
            &FLS, "rebuild-raylib",
            .description="Rebuild raylib dependencies."
        );
        fls_add_flag(
            &FLS, "rebuild",
            .description="Rebuild all dependencies.",
        );
        fls_add_flag(
            &FLS, "release",
            .description="Build project with debug symbols stripped and all optimizations enabled.",
            .note="This flag also rebuilds all dependencies.",
        );
        fls_add_flag(
            &FLS, "no-rebuild",
            .description="Skip rebuilding project.",
            .note="Will build project if it doesn't exist regardless."
        );
        fls_add_flag(
            &FLS, "-",
            .description="Remaining arguments are passed to project.",
            .is_terminating=true
        );

        fls_mode_end( &FLS );
    }

    fls_mode_begin( &FLS, "pkg" ); {
        fls_mode_set_description( &FLS,
            "Build in release mode and archive "
            "compressed assets for itch.io distribution." );

        fls_add_str(
            &FLS, "target",
            .aliases=fls_strings("t"),
            .description="Set compilation target.",
            .warning="GNU-Linux requires tar, Windows/WASM require zip",
            .note="Windows requires MinGW GCC/G++, "
                  "GNU-Linux requires either clang/clang++ or "
                  "GCC/G++ and WASM requires EMCC/EMC++",
            .default_value=TARGET_NAMES[0],
            .str={ .valid=TARGET_NAMES },
        );
        fls_add_flag(
            &FLS, "no-rebuild",
            .description="Skip rebuilding project.",
            .note="Will build project if it doesn't exist regardless."
        );

        fls_mode_end( &FLS );
    }

    fls_mode_begin( &FLS, "ed" ); {
        fls_mode_set_description( &FLS, "Build game editor and run it. (if available) This mode passes all remaining arguments to editor." );
        fls_mode_set_terminating( &FLS );

        fls_mode_end( &FLS );
    }
}

const char* string_from_mode( Mode mode ) {
    if( mode >= M_COUNT ) {
        return "help";
    }
    return MODE_NAMES[mode];
}
Mode mode_from_string( const char* string ) {
    for( size_t i = 0; i < (size_t)M_COUNT; ++i ) {
        if( strcmp( MODE_NAMES[i], string ) == 0 ) {
            return (Mode)i;
        }
    }
    return M_HELP;
}

const char* string_from_target( Target target ) {
    if( target >= T_COUNT ) {
        return TARGET_NAMES[0];
    }
    return TARGET_NAMES[target];
}
Target target_from_string( const char* string ) {
    Target native_target;
#if CB_OS_CURRENT == CB_OS_WINDOWS
    native_target = T_WINDOWS;
#else
    native_target = T_GNU_LINUX;
#endif

    if( strcmp( string, "native" ) == 0 ) {
        return native_target;
    }

    for( size_t i = 1; i < (size_t)T_COUNT; ++i ) {
        if( strcmp( TARGET_NAMES[i], string ) == 0 ) {
            return (Target)i;
        }
    }

    return native_target;
}

int __dir_create_many_( const char* first, ... ) {
    const char* current = first;

    va_list va;
    va_start( va, first );
    for( ;; ) {
        if( !current ) {
            break;
        }

        FileType type = path_query_file_type( current );

        switch( type ) {
            case FT_DIR: break;
            case FT_NULL: {
                if( !dir_create( current, false ) ) {
                    va_end( va );
                    return err( E_DIR_CREATE, current, "failed to create directory!" );
                }
                CB_INFO( "mkdir: %s", current );
            } break;
            case FT_FILE:
            case FT_PIPE: {
                va_end( va );
                return err( E_DIR_CREATE, current, "file already exists at this path!" );
            } break;
        }

        current = va_arg( va, const char* );
    }
    va_end( va );

    return E_NONE;
}

void parse_cfg(void) {
    memset( &CFG_CTX, 0, sizeof(CFG_CTX) );

    ini_open( CFG_PATH, &CFG_CTX );

    CFG.project_name = ini_read(
        &CFG_CTX, "project", "name",
        .default_value=CFG_DEFAULT_PROJECT_NAME,
        .comment="human readable project name."
    );

    CFG.project_file_name = ini_read(
        &CFG_CTX, "project", "file-name",
        .default_value=CFG_DEFAULT_PROJECT_FILE_NAME,
        .comment="name of executable. should be a valid file name."
    );

    CFG.window_width = (int) ini_read_number(
        &CFG_CTX, "project", "window-width",
        .default_value=CB_STRINGIFY2( CFG_DEFAULT_WINDOW_WIDTH ),
        .comment="default window width for project."
    );

    CFG.window_height = (int) ini_read_number(
        &CFG_CTX, "project", "window-height",
        .default_value=CB_STRINGIFY2( CFG_DEFAULT_WINDOW_HEIGHT ),
        .comment="default window height for project."
    );

    CFG.framerate = (int) ini_read_number(
        &CFG_CTX, "project", "framerate",
        .default_value=CB_STRINGIFY2( CFG_DEFAULT_FRAMERATE ),
        .comment="default framerate for project. applies only to desktop targets."
    );

    if( CFG.window_width <= 1 ) {
        CFG.window_width = CFG_DEFAULT_WINDOW_WIDTH;
        ini_write( &CFG_CTX, "project", "window-width", "%d", CFG.window_width );
    }

    if( CFG.window_height <= 1 ) {
        CFG.window_height = CFG_DEFAULT_WINDOW_HEIGHT;
        ini_write( &CFG_CTX, "project", "window-height", "%d", CFG.window_height );
    }

    if( CFG.framerate < CFG_MINIMUM_FRAMERATE ) {
        CFG.framerate = CFG_MINIMUM_FRAMERATE;
        ini_write( &CFG_CTX, "project", "framerate", "%d", CFG.framerate );
    }

    ini_set_section_comment( &CFG_CTX, "project", "settings for raylib project" );

    ini_serialize_to_file( &CFG_CTX, CFG_PATH, true );
}

#define CB_IMPL
#include "cb.h"

