/**
 * @file   cb.c
 * @brief  Build system.
 * @author Alicia D. Amarilla (smushyaa@gmail.com)
 * @date   June 22, 2026
*/
#include "extern/cb/cb.h"
#include "extern/flagship/flagship.h"
#include "extern/ini-parser/ini-parser.h"

#include <ctype.h>

typedef FlagshipStreamFn StreamFn;

enum Error {
    E_NONE,            //
    E_PARSE_ARGS,      // 
    E_FILE_CREATE,     // const char *path
    E_FILE_OPEN,       // const char *path
    E_MISSING_PROC,    // const char *proc
    E_MAKE_DIR,        // const char *path
    E_WRONG_FILE_TYPE, // const char *path, FileType expected_type, FileType found_type
    E_EXEC_ASYNC,      // const char *name
    E_PROC_FAIL,       // const char *name, int exit_code
    E_NO_RUN_SUPPORT,  // int os_current, enum Target target
    E_FILE_MOVE,       // const char *dst, const char *src
    E_UNIMPLEMENTED,   // const char *msg

    E_COUNT
};

enum Mode {
    M_HELP,
    M_BUILD,
    M_RUN,
    M_PKG,

    M_COUNT,
    M_BEGIN = M_HELP
};
const char *MODE_NAMES[] = {
    "help",
    "build",
    "run",
    "pkg",
};
static_assert(
    CB_ARRAY_LEN(MODE_NAMES) == M_COUNT,
    "number of modes does not match number of mode names!");

enum Target {
    T_NATIVE,
    T_LINUX,
    T_WINDOWS,
    T_MACOS,
    T_WASM,

    T_COUNT,
    T_BEGIN   = T_NATIVE,
    T_DEFAULT = T_NATIVE,
};
const char *TARGET_NAMES[] = {
    "native",
    "linux",
    "windows",
    "macos",
    "wasm"
};
static_assert(
    CB_ARRAY_LEN(TARGET_NAMES) == T_COUNT,
    "number of targets does not match number of target names!");
const char *OS_NAMES[] = {
    "unknown",
    "linux",
    "windows",
    "macos"
};
static_assert(
    CB_ARRAY_LEN(OS_NAMES) == CB_OS_COUNT,
    "number of os does not match number of os names!");


enum EnableOpt {
    ENABLE_WAYLAND,

    ENABLE_OPT_COUNT,
    ENABLE_OPT_BEGIN = ENABLE_WAYLAND
};
const char *ENABLE_OPT_NAMES[] = {
    "wayland"
};
static_assert(
    CB_ARRAY_LEN(ENABLE_OPT_NAMES) == ENABLE_OPT_COUNT,
    "number of enable options does not match number of enable option names!");

enum RebuildTarget {
    REBUILD_ALL,
    REBUILD_RAYLIB,
    REBUILD_PROJECT,

    REBUILD_COUNT,
    REBUILD_BEGIN = REBUILD_ALL
};
const char *REBUILD_NAMES[] = {
    "all",
    "raylib",
    "project",
};
static_assert(
    CB_ARRAY_LEN(REBUILD_NAMES) == REBUILD_COUNT,
    "number of rebuild targets does not match number of rebuild target names!");

enum RebuildOpt {
    REBUILD_OPT_AUTO  = 0b00,
    REBUILD_OPT_FORCE = 0b01,
    REBUILD_OPT_SKIP  = 0b10,

    REBUILD_OPT_COUNT,
    REBUILD_OPT_BEGIN = REBUILD_OPT_AUTO,
    REBUILD_OPT_CLEAR_MASK = 0b11
};
const char *REBUILD_OPT_NAMES[] = {
    "auto",
    "force",
    "skip"
};
static_assert(
    CB_ARRAY_LEN(REBUILD_OPT_NAMES) == REBUILD_OPT_COUNT,
    "number of rebuild opts does not match number of rebuild opt names!");

struct Option {
    enum Mode mode;
    union {
        struct OptionHelp {
            enum Mode mode;
        } help;
        struct OptionBuild {
            enum Target target;
            bool strip_symbols  : 1;
            bool optimized      : 1;
            uint16_t enable     : (sizeof(uint16_t) * 8) - 2;
            uint16_t rebuild    : sizeof(uint16_t) * 8;
        } build;
        struct OptionRun {
            struct OptionBuild build;
            CmdLine passthrough;
        } run;
        struct OptionPkg {
            struct OptionBuild build;
        } pkg;
    };
};

#define TOTAL_MEMORY_MIN ((size_t)CB_MEBIBYTES(16))

struct FlagshipContext  FLS;
struct IniParserContext INI;

struct FlagResultBuf {
    size_t cap;
    size_t len;
    struct FlagshipResult *ptr;
} FRB;

StringBuf STR, TMP;
CmdBuf    CMD;
ProcessGroup PG;

size_t PROJECT_DISPLAY_NAME = 0;
size_t PROGRAM_NAME         = 0;
size_t VERSION              = 0;
size_t DEVELOPER            = 0;
size_t BUILD_DIR            = 0;
size_t BUILD_OBJ_DIR        = 0;

size_t MINGW_C   = 0;
size_t MINGW_CPP = 0;
size_t MINGW_AR  = 0;

bool RESOURCES_CHECKED = false;
bool RESOURCES_EXISTS  = false;

int error(int ec, ...);

size_t file_stream(void *target, size_t n, const void *bytes);

int ini_define(struct IniParserContext *ini);
int ini_load(struct IniParserContext *ini);

int flagship_define(struct FlagshipContext *fls, const char *proc);

int __proc_check(const char *first, ...);
int __makedir(const char *first, ...);

#define proc_check(...) __proc_check( __VA_ARGS__ , NULL )
#define makedir(...)    __makedir( __VA_ARGS__ , NULL )

int mode_build(struct OptionBuild *opt);
int mode_run(struct OptionRun *opt);
int mode_pkg(struct OptionPkg *opt);

const char *project_display_name(void);
const char *program_name(enum Target target);
const char *version(void);
const char *developer(void);
size_t total_memory(void);
const char *memory_fmt(size_t bytes);

bool check_resources(void);

void collect_enable_flags(struct Option *opt);
void collect_rebuild_flags(struct Option *opt);

enum RebuildOpt read_rebuild(struct OptionBuild *opt, enum RebuildTarget target);
bool read_enable(struct OptionBuild *opt, enum EnableOpt e);

enum Target target_normalize(enum Target target);
const char *target_name(enum Target target);

const char *compiler_c(enum Target target);
const char *compiler_cpp(enum Target target);
const char *archiver(enum Target target);

const char *build_dir(enum Target target);
const char *build_obj_dir(enum Target target);

const char *program_ext(enum Target target);

int main(int argc, char **argv) {
    CB_BUILD_INIT();

    CB_BUF_RESERVE(&STR, 256);
    STR.len = 1;

    int ec = E_NONE;

    struct IniParserContext *ini = &INI;
    struct FlagshipContext  *fls = &FLS;

    ini_parser_begin(ini);

    // define defaults
    if((ec = ini_define(ini))) {
        return ec;
    }
    // load existing or create new project.ini
    if((ec = ini_load(ini))) {
        return ec;
    }

    flagship_begin(fls);

    if((ec = flagship_define(fls, argv[0]))) {
        return ec;
    }

    if(!flagship_parse(fls, argc, argv, true)) {
        return ec = error(E_PARSE_ARGS);
    }

    struct Option opt;
    memset(&opt, 0, sizeof(opt));

    flagship_get_last_mode(fls, (int *)&opt.mode, NULL);

    struct FlagshipResult flag_result;

    switch(opt.mode) {
        case M_HELP: {
            if(flagship_search(fls, MODE_NAMES[opt.mode], NULL, &flag_result)) {
                opt.help.mode = flag_result.t_enum;
            }
        } break;
        case M_RUN:
        case M_PKG:
        case M_BUILD: {
            if(flagship_search(fls, MODE_NAMES[opt.mode], "target", &flag_result)) {
                opt.build.target = flag_result.t_enum;
            }
            if(flagship_search(fls, MODE_NAMES[opt.mode], "optimzed", &flag_result)) {
                opt.build.optimized = true;
            }
            if(flagship_search(fls, MODE_NAMES[opt.mode], "strip-symbols", &flag_result)) {
                opt.build.strip_symbols = true;
            }
            collect_enable_flags(&opt);
            collect_rebuild_flags(&opt);

            opt.build.target = target_normalize(opt.build.target);

            switch(opt.mode) {
                case M_RUN: {
                    if(flagship_search(fls, MODE_NAMES[opt.mode], "-", &flag_result)) {
                        int end = flagship_end_position(fls);
                        end++;

                        opt.run.passthrough.len = argc - end;
                        opt.run.passthrough.ptr = (const char **)(argv + end);
                    }
                } break;
                case M_PKG: {
                    // NOTE(alicia): setting default build options
                    opt.build.optimized     = true;
                    opt.build.strip_symbols = true;
                    for(enum RebuildTarget rt = REBUILD_BEGIN + 1; rt < REBUILD_COUNT; ++rt) {
                        opt.build.rebuild |= (REBUILD_OPT_FORCE << ((uint16_t)(rt - 1) * 2));
                    }

                    // TODO(alicia): additional package flags
                } break;
                case M_HELP:
                case M_BUILD:
                case M_COUNT:
                    break;
            }
        } break;
        case M_COUNT:
          break;
    }

    if(flagship_search(fls, MODE_NAMES[opt.mode], "help", &flag_result)) {
        flagship_help_print(fls, MODE_NAMES[opt.mode], opt.mode == M_HELP);
    } else {
        switch(opt.mode) {
            case M_HELP:
                flagship_help_print(fls, MODE_NAMES[opt.help.mode], opt.help.mode == M_HELP);
                break;
            case M_BUILD:
                ec = mode_build(&opt.build);
                break;
            case M_RUN:
                ec = mode_run(&opt.run);
                break;
            case M_PKG:
                ec = mode_pkg(&opt.pkg);
                break;
            case M_COUNT:
                break;
        }
    }

    ini_parser_end(ini);
    flagship_end(fls);
    return ec;
}

int mode_build_raylib(struct OptionBuild *opt) {
    int ec = E_NONE;

    enum RebuildOpt rebuild = read_rebuild(opt, REBUILD_RAYLIB);

    FileType ft = path_query_file_type(local_fmt("%s/libraylib.a", build_obj_dir(opt->target)));
    switch(ft) {
        case FT_NULL:
        case FT_FILE:
            break;
        case FT_DIR:
        case FT_PIPE:
            return error(E_WRONG_FILE_TYPE, local_fmt("%s/libraylib.a",
                build_obj_dir(opt->target)), FT_FILE, ft);
    }

    switch(rebuild) {
        case REBUILD_OPT_AUTO: switch(ft) {
            case FT_NULL:
                break;
            case FT_FILE:
                return ec;

            // NOTE(alicia): unreachable
            case FT_DIR:
            case FT_PIPE:
                break;
        } break;
        case REBUILD_OPT_FORCE:
            // we rebuild anyway
            break;
        case REBUILD_OPT_SKIP:
            // only rebuild if it doesn't exist, required dependency
            if(ft != FT_NULL) {
                return ec;
            }
            break;
        case REBUILD_OPT_COUNT:
            break;
    }

    // TODO(alicia): check for forced rebuild

    CB_INFO("building libraylib.a for %s . . .", target_name(opt->target));

    // check for programs
    if((ec = proc_check(compiler_c(opt->target), archiver(opt->target)))) {
        return ec;
    }

    // reset proc group
    proc_group_clean(&PG);

    CStringBuf sources;
    memset(&sources, 0, sizeof(sources));

    CB_BUF_PUSH(&sources, "rcore");
    CB_BUF_PUSH(&sources, "rshapes");
    CB_BUF_PUSH(&sources, "rtextures");
    CB_BUF_PUSH(&sources, "rtext");
    CB_BUF_PUSH(&sources, "rmodels");
    CB_BUF_PUSH(&sources, "raudio");

    switch(opt->target) {
        case T_WINDOWS:
        case T_LINUX:
        case T_MACOS:
            CB_BUF_PUSH(&sources, "rglfw");
            break;
        case T_WASM:
            break;

        case T_NATIVE:
        case T_COUNT:
            break;
    }

    Cmd other_args = CB_CMD("");
    switch(opt->target) {
        case T_LINUX:
            other_args = CB_CMD(
                "-D_DEFAULT_SOURCE",
                "-DPLATFORM_DESKTOP_GLFW", "-DGRAPHICS_API_OPENGL_33",
                "-Wno-missing-braces", "-Werror=pointer-arith",
                "-fno-strict-aliasing", "-std=c99",
                "-fPIC", "-Werror=implicit-function-declaration",
                "-D_GLFW_X11",  "-Iextern/raylib/src",
                "-Iextern/raylib/src/external/glfw/include" );
            break;
        case T_WINDOWS:
            other_args = CB_CMD(
                "-DPLATFORM_DESKTOP_WIN32", "-DGRAPHICS_API_OPENGL_33",
                "-Wno-missing-braces", "-Werror=pointer-arith",
                "-fno-strict-aliasing", "-std=c99",
                "-DUNICODE", "-Werror=implicit-function-declaration",
                "-Iextern/raylib/src",
                "-Iextern/raylib/src/external/glfw/include" );
            break;
        case T_MACOS:
            other_args = CB_CMD(
                "-DPLATFORM_DESKTOP_GLFW", "-DGRAPHICS_API_OPENGL_33",
                "-Wno-missing-braces", "-Werror=pointer-arith",
                "-fno-strict-aliasing", "-std=c99",
                "-fPIC", "-Werror=implicit-function-declaration",
                 "-Iextern/raylib/src",
                "-Iextern/raylib/src/external/glfw/include" );
            break;
        case T_WASM:
            other_args = CB_CMD(
                "-D_DEFAULT_SOURCE",
                "-DPLATFORM_WEB", "-DGRAPHICS_API_OPENGL_ES2",
                "-Wno-missing-braces", "-Wno-deprecated-pragma",
                "-Wno-tautological-compare", "-Werror=pointer-arith",
                "-fno-strict-aliasing", "-std=gnu99",
                "-fPIC", "-Werror=implicit-function-declaration",
                "-Iextern/raylib/src" );
            break;

        case T_NATIVE:
        case T_COUNT:
            break;
    }

    bool force_rebuild = false;

    for(size_t i = 0; i < sources.len; ++i) {
        const char *name = sources.ptr[i];

        switch((ft = path_query_file_type(
            local_fmt("%s/%s.o", build_obj_dir(opt->target), name))
        )) {
            case FT_NULL:
                break;
            case FT_FILE:
                if(rebuild == REBUILD_OPT_FORCE) {
                    break;
                } else {
                    continue;
                }
            case FT_DIR:
            case FT_PIPE:
                CB_BUF_FREE(&sources);
                return error(E_WRONG_FILE_TYPE, local_fmt("%s/%s.o",
                    build_obj_dir(opt->target), name), FT_FILE, ft);
        }

        cmd_reset(&CMD);
        CB_CMD_APPEND(&CMD, compiler_c(opt->target));
        CB_CMD_APPEND(&CMD, local_fmt("extern/raylib/src/%s.c", name), "-c", "-o");
        CB_CMD_APPEND(&CMD, local_fmt("%s/%s.o", build_obj_dir(opt->target), name));

        switch(opt->target) {
            case T_WASM:
                cmd_add(&CMD, "-Os");
                break;
            case T_NATIVE:
            case T_LINUX:
            case T_WINDOWS:
            case T_MACOS:
            case T_COUNT:
                if(opt->optimized) {
                    cmd_add(&CMD, "-O1");
                } else {
                    cmd_add(&CMD, "-O0");
                }
                break;
        }

        if(!opt->strip_symbols) {
            cmd_add(&CMD, "-D_DEBUG");
        }
        switch(opt->target) {
            case T_WASM:
                break;
            case T_NATIVE:
            case T_LINUX:
            case T_WINDOWS:
            case T_MACOS:
            case T_COUNT:
                if(!opt->strip_symbols) {
                    cmd_add(&CMD, "-g");
                }
                break;
        }

        // TODO(alicia): check which platforms require this
        if(strcmp(name, "rglfw")) {
            cmd_add(&CMD, "-D_GNU_SOURCE");
        }

        cmd_copy(&CMD, other_args);

        if(CB_EXEC(CMD.cmd, .group=&PG)) {
            ec = error(E_EXEC_ASYNC, local_fmt("%s %s", compiler_c(opt->target), name));
            break;
        }
    }

    if(ec) {
        goto cleanup;
    }

    proc_group_join(&PG, CB_WAIT_INF);

    for(size_t i = 0; i < PG.len; ++i) {
        ProcessData *proc = PG.ptr + i;

        if(proc->exit_code) {
            ec = error(E_PROC_FAIL,
                local_fmt("%s %s", compiler_c(opt->target), sources.ptr[i]),
                proc->exit_code);
        }
    }

    if(ec) {
        goto cleanup;
    }

    // archive into libraylib.a
    cmd_reset(&CMD);

    CB_CMD_APPEND(&CMD, archiver(opt->target), "rcs",
        local_fmt("%s/libraylib.a", build_obj_dir(opt->target)));

    for(size_t i = 0; i < sources.len; ++i) {
        cmd_add(&CMD, local_fmt("%s/%s.o", build_obj_dir(opt->target), sources.ptr[i]));
    }

    int exit_code = 0;
    if((exit_code = CB_EXEC(CMD.cmd))) {
        ec = error(E_PROC_FAIL, local_fmt("%s libraylib.a", archiver(opt->target)), exit_code);
    }

cleanup:
    CB_BUF_FREE(&sources);
    return ec;
}

struct GenerateSourcesParams {
    File *f;
    int  *ec;
};

DirWalkAction generate_sources_walk(const char *path, DirWalkInfo *info, void *in_params) {
    struct GenerateSourcesParams *params = (struct GenerateSourcesParams *)in_params;

    // skip non-cpp files
    if(strcmp(path + info->path_ext_offset, ".cpp")) {
        return DWA_CONTINUE;
    }

    // skip self
    if(strcmp(path, "src/sources.cpp") == 0) {
        return DWA_CONTINUE;
    }

    file_write_fmt(params->f, "#include \"../%s\"\n", path);

    return DWA_CONTINUE;
}

int mode_build_sources_file(struct OptionBuild *opt) {
    int ec = E_NONE;

    const char *path = "src/sources.cpp";

    FileType ft;
    switch(read_rebuild(opt, REBUILD_PROJECT)) {
        case REBUILD_OPT_AUTO:
        case REBUILD_OPT_FORCE:
            break;
        case REBUILD_OPT_SKIP: {
            // check if target exists
            // if it does not, generate sources
            const char *path = local_fmt("%s/%s%s",
                build_dir(opt->target), program_name(opt->target), program_ext(opt->target));

            switch((ft = path_query_file_type(path))) {
                case FT_NULL:
                    break;
                case FT_FILE:
                    return ec;
                case FT_DIR:
                case FT_PIPE:
                    return error(E_WRONG_FILE_TYPE, path, FT_FILE, ft);
            }
        } break;
        case REBUILD_OPT_COUNT:
            break;
    }

    switch((ft = path_query_file_type(path))) {
        case FT_NULL:
        case FT_FILE:
            break;

        case FT_DIR:
        case FT_PIPE:
            return error(E_WRONG_FILE_TYPE, path, FT_FILE, ft);
    }

    FileFlags flags = FF_WRITE;
    switch(ft) {
        case FT_NULL:
            flags |= FF_CREATE;
            break;
        case FT_FILE:
            flags |= FF_TRUNCATE;
            break;
        case FT_DIR:
        case FT_PIPE:
            break;
    }

    CB_INFO("generating sources . . .");

    File f;
    if(!file_open(path, flags, &f)) {
        return error(E_FILE_OPEN, path);
    }

    file_write_fmt(&f, "/* generated sources */\n");

    struct GenerateSourcesParams params;
    params.f  = &f;
    params.ec = &ec;

    dir_walk("src", generate_sources_walk, &params);

    CB_INFO("sources generated at %s", path);
    file_close(&f);
    return ec;
}

int mode_build_project(struct OptionBuild *opt) {
    int ec = E_NONE;

    FileType ft;
    switch(read_rebuild(opt, REBUILD_PROJECT)) {
        case REBUILD_OPT_AUTO:
        case REBUILD_OPT_FORCE:
            break;
        case REBUILD_OPT_SKIP: {
            // check if project exists. if it does, exit
            const char *path = local_fmt("%s/%s%s",
                build_dir(opt->target), program_name(opt->target), program_ext(opt->target));

            switch((ft = path_query_file_type(path))) {
                case FT_NULL:
                    break;
                case FT_FILE:
                    return ec;
                case FT_DIR:
                case FT_PIPE:
                    return error(E_WRONG_FILE_TYPE, path, FT_FILE, ft);
            }
        } break;
        case REBUILD_OPT_COUNT:
            break;
    }

    CB_INFO("compiling %s for %s . . .", project_display_name(), target_name(opt->target));

    // check for programs
    if((ec = proc_check(compiler_cpp(opt->target)))) {
        return ec;
    }

    cmd_reset(&CMD);
    CB_CMD_APPEND(&CMD,
        compiler_cpp(opt->target),
        "src/sources.cpp", local_fmt("%s/libraylib.a", build_obj_dir(opt->target)));

    switch(opt->target) {
        case T_NATIVE:
        case T_LINUX:
        case T_WINDOWS:
        case T_MACOS:
        case T_COUNT:
            CB_CMD_APPEND(&CMD, "-o", local_fmt("%s/%s%s",
                build_dir(opt->target), program_name(opt->target), program_ext(opt->target)));
            break;
        case T_WASM:
            CB_CMD_APPEND(&CMD, "-o",
                local_fmt("%s/index.html", build_dir(opt->target)));
            break;
    }

    CB_CMD_APPEND(&CMD,
        "-Iextern/raylib/src", "-Iinclude", "-Iextern/cb",
        "-Iextern/flagship", "-Iextern/ini-parser");

    CB_CMD_APPEND(&CMD, "-Wall");

    switch(opt->target) {
        case T_LINUX:
            if(!opt->strip_symbols) {
                cmd_add(&CMD, "-ggdb");
            }
            if(opt->optimized) {
                cmd_add(&CMD, "-O2");
            } else {
                cmd_add(&CMD, "-O0");
            }

            CB_CMD_APPEND(&CMD, "-lGL", "-lm", "-lpthread", "-ldl", "-lrt", "-lX11");
            break;

        case T_WINDOWS:
            if(!opt->strip_symbols) {
                if(CB_OS_CURRENT == CB_OS_WINDOWS) {
                    CB_CMD_APPEND(&CMD,
                        "-gcodeview", "-fuse-ld=lld",
                        local_fmt("-Wl,-pdb:%s/%s.pdb",
                            build_dir(opt->target), program_name(opt->target)));
                } else {
                    // NOTE(alicia): no .pdb on linux :( as far as I know
                    cmd_add(&CMD, "-g");
                }
            }
            if(opt->optimized) {
                cmd_add(&CMD, "-O2");
            } else {
                cmd_add(&CMD, "-O0");
            }

            CB_CMD_APPEND(&CMD,
                "-static-libgcc", "-lgdi32", "-lwinmm", "-lshcore", "-lopengl32");
            break;

        // TODO(alicia): macos!
        case T_MACOS:
            if(!opt->strip_symbols) {
                cmd_add(&CMD, "-g");
            }
            if(opt->optimized) {
                cmd_add(&CMD, "-O2");
            } else {
                cmd_add(&CMD, "-O0");
            }

            break;
        case T_WASM: {
            if(!opt->strip_symbols) {
                CB_CMD_APPEND(&CMD, "--profiling", "-s", "ASSERTIONS=1");
            }

            // NOTE(alicia): always optimize for space on wasm builds
            cmd_add(&CMD, "-Os");

            size_t mem = total_memory();
            CB_CMD_APPEND(&CMD, "-s", local_fmt("TOTAL_MEMORY=%zu", mem));

            CB_CMD_APPEND(&CMD,
                local_fmt("-L%s", build_obj_dir(opt->target)),
                "-lraylib",
                "-s", "USE_GLFW=3",
                "--shell-file", "extern/raylib/src/minshell.html",
                "-s", "FORCE_FILESYSTEM=1");

            if(check_resources()) {
                CB_CMD_APPEND(&CMD, "--preload-file", "resources");
            }
        } break;

        case T_NATIVE:
        case T_COUNT:
            break;
    }

    cmd_add(&CMD, local_fmt("-DPROJECT_NAME=\"%s\"", project_display_name()));
    cmd_add(&CMD, local_fmt("-DPROJECT_VERSION=\"%s\"", version()));
    cmd_add(&CMD, local_fmt("-DPROJECT_DEVELOPER=\"%s\"", developer()));
    cmd_add(&CMD, local_fmt("-DPROJECT_COMPILER=\"%s\"", compiler_cpp(opt->target)));

    ini_parser_begin_section(&INI, "project");

    int v;

    v = ini_parser_read_integer(&INI, "window-width");
    if(v < 100) {
        v = 100;
    }
    cmd_add(&CMD, local_fmt("-DWINDOW_WIDTH=%d",v));

    v = ini_parser_read_integer(&INI, "window-height");
    if(v < 100) {
        v = 100;
    }
    cmd_add(&CMD, local_fmt("-DWINDOW_HEIGHT=%d", v));

    v = ini_parser_read_integer(&INI, "framerate");
    if(v < 20) {
        v = 20;
    }
    cmd_add(&CMD, local_fmt("-DFRAMERATE=%d", v));

    ini_parser_end_section(&INI);

    for(enum Target t = T_BEGIN; t < T_COUNT; ++t) {
        if(t == T_NATIVE) {
            continue;
        }

        ByteBuf *buf = local_buf();

        string_buf_fmt(buf, "-DPLATFORM_");
        const char *tname = target_name(t);
        while(*tname) {
            CB_BUF_PUSH(buf, toupper(*tname));
            tname++;
        }
        string_buf_fmt(buf, "=%i", t);
        CB_BUF_PUSH(buf, 0);

        cmd_add(&CMD, buf->ptr);
    }

    cmd_add(&CMD, local_fmt("-DPLATFORM=%i", opt->target));

    int exit_code = 0;
    if((exit_code = CB_EXEC(CMD.cmd))) {
        ec = error(E_PROC_FAIL, local_fmt("%s %s", CMD.cmd.ptr[0], CMD.cmd.ptr[1]), exit_code);
    }

    CB_INFO("built %s at %s/%s%s",
        project_display_name(), build_dir(opt->target),
        opt->target == T_WASM ? "index" : program_name(opt->target), program_ext(opt->target));

    return ec;
}

int mode_build(struct OptionBuild *opt) {
    int ec = E_NONE;

    CB_INFO("building . . .");

    // check for directories
    if((ec = makedir("build", build_dir(opt->target), build_obj_dir(opt->target)))) {
        return ec;
    }

    // check if it's even possible to cross-compile
    switch(CB_OS_CURRENT) {
        case CB_OS_GNU_LINUX: switch(opt->target) {
            case T_NATIVE:
            case T_LINUX:
            case T_WINDOWS:
            case T_WASM:
            case T_COUNT:
                break;
            case T_MACOS:
                return error(E_UNIMPLEMENTED, "cannot cross-compile to macos from linux!");
        } break;
        case CB_OS_WINDOWS: switch(opt->target) {
            case T_NATIVE:
            case T_LINUX:
            case T_WINDOWS:
            case T_WASM:
            case T_COUNT:
                break;
            case T_MACOS:
                return error(E_UNIMPLEMENTED, "cannot cross-compile to macos from windows!");
        } break;
        case CB_OS_MACOS:
            // TODO(alicia): 
            return error(E_UNIMPLEMENTED, "no macos support yet!");
    }

    // build dependencies
    if((ec = mode_build_raylib(opt))) {
        return ec;
    }

    // generate sources.cpp
    if((ec = mode_build_sources_file(opt))) {
        return ec;
    }

    // build project
    if((ec = mode_build_project(opt))) {
        return ec;
    }

    return ec;
}
int mode_run(struct OptionRun *opt) {
    int ec = E_NONE;

    // build
    if((ec = mode_build(&opt->build))) {
        return ec;
    }

    CB_INFO("running %s . . .", project_display_name());

    cmd_reset(&CMD);

    int exit_code = 0;
    switch(CB_OS_CURRENT) {
        case CB_OS_GNU_LINUX: switch(opt->build.target) {
            case T_LINUX:
                cmd_add(&CMD,
                local_fmt("%s/%s%s",
                    build_dir(opt->build.target), program_name(opt->build.target),
                    program_ext(opt->build.target)));

                for(size_t i = 0; i < opt->passthrough.len; ++i) {
                    cmd_add(&CMD, opt->passthrough.ptr[i]);
                }

                if((exit_code = CB_EXEC(CMD.cmd))) {
                    return error(E_PROC_FAIL, CMD.cmd.ptr[0], exit_code);
                }
                break;
            case T_WINDOWS:
                if((ec = proc_check("wine"))) {
                    return ec;
                }
                cmd_add(&CMD, "wine");

                cmd_add(&CMD,
                local_fmt("%s/%s%s",
                    build_dir(opt->build.target), program_name(opt->build.target),
                    program_ext(opt->build.target)));

                for(size_t i = 0; i < opt->passthrough.len; ++i) {
                    cmd_add(&CMD, opt->passthrough.ptr[i]);
                }

                if((exit_code = CB_EXEC(CMD.cmd))) {
                    return error(E_PROC_FAIL, CMD.cmd.ptr[0], exit_code);
                }
                break;
            case T_MACOS:
                return error(E_NO_RUN_SUPPORT, CB_OS_CURRENT, opt->build.target);
            case T_WASM:
                if((ec = proc_check("python", "xdg-open"))) {
                    return ec;
                }

                dir_cwd_set(build_dir(opt->build.target));

                CB_INFO("launching local server");
                CB_INFO("to close server, kill this process (Ctrl+C or some other signal)");
                CB_CMD_APPEND(&CMD, "python", "-m", "http.server");
                Process python_server;
                if(CB_EXEC(CMD.cmd, .group=&PG, .proc=&python_server)) {
                    return error(E_EXEC_ASYNC, "python local server");
                }

                // wait for python server to spin up
                mt_sleep(30);

                cmd_reset(&CMD);
                CB_CMD_APPEND(&CMD, "xdg-open", "localhost:8000/index.html");

                if((exit_code = CB_EXEC(CMD.cmd))) {
                    ec = error(E_PROC_FAIL, "xdg-open", exit_code);
                    proc_signal(&python_server, PSIG_KILL);
                }

                proc_group_join(&PG, CB_WAIT_INF);
                break;

            case T_NATIVE:
            case T_COUNT:
                break;
        } break;
        case CB_OS_WINDOWS:
        case CB_OS_MACOS:
        default:
            break;
    }

    return ec;
}
int mode_pkg(struct OptionPkg *opt) {
    int ec = E_NONE;
    if((ec = mode_build(&opt->build))) {
        return ec;
    }

    CB_INFO("packaging project . . .");

    if((ec = makedir("bin"))) {
        return ec;
    }

    int exit_code = 0;
    cmd_reset(&CMD);
    switch(opt->build.target) {
        case T_LINUX: {
            // uses tar
            if((ec = proc_check("tar"))) {
                return ec;
            }

            CB_CMD_APPEND(&CMD, "tar", "-cJf",
                local_fmt("bin/%s-linux-x86_64.tar.xz", program_name(opt->build.target)));

            CB_CMD_APPEND(&CMD, local_fmt("--transform=s,^,%s-linux-x86_64/,",
                program_name(opt->build.target)));

            CB_CMD_APPEND(&CMD, "-C",
                build_dir(opt->build.target), program_name(opt->build.target));

            if(check_resources()) {
                CB_CMD_APPEND(&CMD, "-C", "../../", "resources");
            }

            if((exit_code = CB_EXEC(CMD.cmd))) {
                return error(E_PROC_FAIL, "tar", exit_code);
            }

            CB_INFO("project packaged at %s",
                local_fmt("bin/%s-linux-x86_64.tar.xz", program_name(opt->build.target)));
        } break;
        case T_WINDOWS: {
            // uses zip
            if((ec = proc_check("zip"))) {
                return ec;
            }

            if(check_resources()) {
                CB_CMD_APPEND(&CMD, "zip", "-r", "resources.zip", "resources");

                if((exit_code = CB_EXEC(CMD.cmd))) {
                    return error(E_PROC_FAIL, "zip resources", exit_code);
                }

                const char *dst = local_fmt("%s/resources.zip", build_dir(opt->build.target));
                const char *src = "resources.zip";
                if(!file_move(dst, src, false)) {
                    return error(E_FILE_MOVE, dst, src);
                }

                cmd_reset(&CMD);
            }

            CB_CMD_APPEND(&CMD, "zip", "resources.zip",
                local_fmt("%s%s", program_name(opt->build.target), program_ext(opt->build.target)));

            if((exit_code = CB_EXEC(CMD.cmd, .wd=build_dir(opt->build.target)))) {
                return error(E_PROC_FAIL, "zip resources", exit_code);
            }

            const char *dst = local_fmt("bin/%s-windows-x64.zip", program_name(opt->build.target));
            const char *src = local_fmt("%s/resources.zip", build_dir(opt->build.target));

            if(!file_move(dst, src, false)) {
                return error(E_FILE_MOVE, dst, src);
            }

            CB_INFO("project packaged at %s", dst);
        } break;
        case T_MACOS: {
            // uses zip
            if((ec = proc_check("zip"))) {
                return ec;
            }
            // TODO(alicia): 
            return error(E_UNIMPLEMENTED, "package macos");
        } break;
        case T_WASM: {
            // uses zip
            if((ec = proc_check("zip"))) {
                return ec;
            }

            // NOTE(alicia): we use T_LINUX in order to get actual program name, not index
            CB_CMD_APPEND(&CMD, "zip", local_fmt("%s-wasm.zip", program_name(T_LINUX)));

            CB_CMD_APPEND(&CMD, "index.js", "index.html", "index.wasm");

            // check if there's a data file
            if(path_exists(local_fmt("%s/index.data", build_dir(opt->build.target)))) {
                cmd_add(&CMD, "index.data");
            }

            if((exit_code = CB_EXEC(CMD.cmd, .wd=build_dir(opt->build.target)))) {
                return error(E_PROC_FAIL, "zip wasm", exit_code);
            }

            // NOTE(alicia): we use T_LINUX in order to get actual program name, not index
            const char *dst = local_fmt("bin/%s-wasm.zip", program_name(T_LINUX));
            const char *src = local_fmt("%s/%s-wasm.zip",
                build_dir(opt->build.target), program_name(T_LINUX));

            if(!file_move(dst, src, false)) {
                return error(E_FILE_MOVE, dst, src);
            }

            CB_INFO("project packaged at %s", dst);
        } break;

        case T_NATIVE:
        case T_COUNT:
            break;
    }

    return ec;
}

int flagship_define(struct FlagshipContext *fls, const char *proc) {
    int ec = E_NONE;

    flagship_name(fls, "%s", proc);
    flagship_description(fls, "build system for raylib project");

    flagship_begin_mode(fls); {
        flagship_name(fls, "%s", MODE_NAMES[M_HELP]);
        flagship_description(fls, "print help and exit");

        flagship_begin_flag(fls, FLAGSHIP_TYPE_ENUM); {
            flagship_description(fls, "mode to print help for");
            for(int m = M_BEGIN; m < M_COUNT; ++m) {
                flagship_enum_variant(fls, "%s", MODE_NAMES[m]);
            }

            flagship_is_terminating(fls, true);
            flagship_end_flag(fls);
        }

        flagship_end_mode(fls);
    }

    flagship_begin_mode(fls); {
        flagship_name(fls, "%s", MODE_NAMES[M_BUILD]);
        flagship_description(fls, "build project");

        flagship_begin_flag(fls, FLAGSHIP_TYPE_ENUM); {
            flagship_name(fls, "target");
            flagship_name(fls, "t");

            flagship_description(fls, "set target platform");

            for(enum Target t = T_BEGIN; t < T_COUNT; ++t) {
                flagship_enum_variant(fls, "%s", TARGET_NAMES[t]);
            }

            flagship_default(fls, "%s", TARGET_NAMES[target_normalize(T_NATIVE)]);

            flagship_end_flag(fls);
        }

        flagship_begin_flag(fls, FLAGSHIP_TYPE_BOOL); {
            flagship_name(fls, "optimized");
            flagship_name(fls, "o");

            flagship_description(fls, "build with optimizations turned on");

            flagship_end_flag(fls);
        }

        flagship_begin_flag(fls, FLAGSHIP_TYPE_BOOL); {
            flagship_name(fls, "strip-symbols");
            flagship_name(fls, "s");

            flagship_description(fls, "strip debug symbols");

            flagship_end_flag(fls);
        }

        flagship_begin_flag(fls, FLAGSHIP_TYPE_ENUM); {
            flagship_name(fls, "rebuild");
            flagship_name(fls, "r");

            flagship_description(fls, "force rebuilding given target");

            for(enum RebuildTarget r = REBUILD_BEGIN; r < REBUILD_COUNT; ++r) {
                flagship_enum_variant(fls, "%s", REBUILD_NAMES[r]);
            }

            flagship_is_repeatable(fls, true);
            flagship_end_flag(fls);
        }

        flagship_begin_flag(fls, FLAGSHIP_TYPE_ENUM); {
            flagship_name(fls, "skip-rebuild");
            flagship_name(fls, "sr");

            flagship_description(fls, "skip rebuilding given target");

            for(enum RebuildTarget r = REBUILD_BEGIN; r < REBUILD_COUNT; ++r) {
                flagship_enum_variant(fls, "%s", REBUILD_NAMES[r]);
            }

            flagship_is_repeatable(fls, true);
            flagship_end_flag(fls);
        }

        flagship_begin_flag(fls, FLAGSHIP_TYPE_ENUM); {
            flagship_name(fls, "enable");
            flagship_description(fls, "enable an option");
            flagship_note(fls, "some of these options are only valid for a specific target");

            for(enum EnableOpt e = ENABLE_OPT_BEGIN; e < ENABLE_OPT_COUNT; ++e) {
                flagship_enum_variant(fls, "%s", ENABLE_OPT_NAMES[e]);
            }

            flagship_is_repeatable(fls, true);
            flagship_end_flag(fls);
        }

        flagship_begin_flag(fls, FLAGSHIP_TYPE_ENUM); {
            flagship_name(fls, "disable");
            flagship_description(fls, "disable an option");
            flagship_note(fls, "some of these options are only valid for a specific target");

            for(enum EnableOpt e = ENABLE_OPT_BEGIN; e < ENABLE_OPT_COUNT; ++e) {
                flagship_enum_variant(fls, "%s", ENABLE_OPT_NAMES[e]);
            }

            flagship_is_repeatable(fls, true);
            flagship_end_flag(fls);
        }

        flagship_begin_flag(fls, FLAGSHIP_TYPE_BOOL); {
            flagship_name(fls, "help");
            flagship_name(fls, "h");

            flagship_description(fls, "print help and exit");

            flagship_is_terminating(fls, true);
            flagship_end_flag(fls);
        }

        flagship_end_mode(fls);
    }

    flagship_begin_mode(fls); {
        flagship_name(fls, "%s", MODE_NAMES[M_RUN]);
        flagship_description(fls, "build and run project");

        flagship_copy(fls, MODE_NAMES[M_BUILD], "target");
        flagship_copy(fls, MODE_NAMES[M_BUILD], "optimized");
        flagship_copy(fls, MODE_NAMES[M_BUILD], "strip-symbols");
        flagship_copy(fls, MODE_NAMES[M_BUILD], "enable");
        flagship_copy(fls, MODE_NAMES[M_BUILD], "disable");
        flagship_copy(fls, MODE_NAMES[M_BUILD], "rebuild");
        flagship_copy(fls, MODE_NAMES[M_BUILD], "skip-rebuild");

        flagship_begin_flag(fls, FLAGSHIP_TYPE_BOOL); {
            flagship_name(fls, "-");
            flagship_description(fls, "pass remaining arguments to project process");
            flagship_is_terminating(fls, true);

            flagship_end_flag(fls);
        }

        flagship_copy(fls, MODE_NAMES[M_BUILD], "help");

        flagship_end_mode(fls);
    }

    flagship_begin_mode(fls); {
        flagship_name(fls, "%s", MODE_NAMES[M_PKG]);
        flagship_description(fls, "build project and package for distribution");

        flagship_copy(fls, MODE_NAMES[M_BUILD], "target");
        flagship_copy(fls, MODE_NAMES[M_BUILD], "enable");
        flagship_copy(fls, MODE_NAMES[M_BUILD], "disable");

        flagship_copy(fls, MODE_NAMES[M_BUILD], "help");

        flagship_end_mode(fls);
    }

    return ec;
}

const char *project_display_name(void) {
    if(!PROJECT_DISPLAY_NAME) {
        ini_parser_begin_section(&INI, "project");
        const char *dp = ini_parser_read_string(&INI, "display-name");
        ini_parser_end_section(&INI);

        size_t dp_len = strlen(dp);

        PROJECT_DISPLAY_NAME = STR.len;
        CB_BUF_RESERVE(&STR, dp_len + 1);
        CB_BUF_APPEND(&STR, dp, dp_len + 1);
    }
    return STR.ptr + PROJECT_DISPLAY_NAME;
}
const char *program_name(enum Target target) {
    if(target == T_WASM) {
        return "index";
    }

    if(!PROGRAM_NAME) {
        ini_parser_begin_section(&INI, "project");
        const char *pn = ini_parser_read_string(&INI, "program-name");
        ini_parser_end_section(&INI);

        size_t pn_len = strlen(pn);

        PROGRAM_NAME = STR.len;

        if(!pn_len) {
            CB_BUF_PUSH(&STR, '_');
        }

        switch(*pn) {
            case '0' ... '9':
                CB_BUF_PUSH(&STR, '_');
                break;
            default:
                break;
        }

        for(size_t i = 0; i < pn_len; ++i) {
            String str;
            memset(&str, 0, sizeof(str));
            switch(pn[i]) {
                case '!':
                    str = CB_STRING("_bang_");
                    break;
                case '@':
                    str = CB_STRING("_at_");
                    break;
                case '#':
                    str = CB_STRING("_hash_");
                    break;
                case '$':
                    str = CB_STRING("_dollar_");
                    break;
                case '%':
                    str = CB_STRING("_percent_");
                    break;
                case '^':
                    str = CB_STRING("_hat_");
                    break;
                case '&':
                    str = CB_STRING("_and_");
                    break;
                case '*':
                    str = CB_STRING("_star_");
                    break;
                case '(':
                    str = CB_STRING("_paren_open_");
                    break;
                case ')':
                    str = CB_STRING("_parent_close_");
                    break;
                case '+':
                    str = CB_STRING("_plus_");
                    break;
                case '=':
                    str = CB_STRING("_eq_");
                    break;
                case '[':
                    str = CB_STRING("_bracket_open_");
                    break;
                case ']':
                    str = CB_STRING("_bracket_close_");
                    break;
                case '{':
                    str = CB_STRING("_curly_open_");
                    break;
                case '}':
                    str = CB_STRING("_curly_close_");
                    break;
                case ';':
                    str = CB_STRING("_semicolon_");
                    break;
                case ':':
                    str = CB_STRING("_colon_");
                    break;
                case '\'':
                    str = CB_STRING("_quote_");
                    break;
                case '\"':
                    str = CB_STRING("_double_quote_");
                    break;
                case '<':
                    str = CB_STRING("_lt_");
                    break;
                case '>':
                    str = CB_STRING("_gt_");
                    break;
                case ',':
                    str = CB_STRING("_comma_");
                    break;
                case '.':
                    str = CB_STRING("_dot_");
                    break;
                case '?':
                    str = CB_STRING("_question_");
                    break;
                case '/':
                    str = CB_STRING("_slash_");
                    break;
                case '\\':
                    str = CB_STRING("_backslash_");
                    break;
                case '|':
                    str = CB_STRING("_pipe_");
                    break;
                case '`':
                    str = CB_STRING("_backtick_");
                    break;
                case '~':
                    str = CB_STRING("_tilde_");
                    break;
                case '0' ... '9':
                case 'A' ... 'Z':
                case 'a' ... 'z':
                    CB_BUF_PUSH(&STR, pn[i]);
                    break;
                case '\t':
                case '\r':
                case '\n':
                case ' ':
                default:
                    CB_BUF_PUSH(&STR, '_');
                    break;
            }
            if(str.len) {
                CB_BUF_APPEND(&STR, str.ptr, str.len);
            }
        }

        CB_BUF_PUSH(&STR, 0);
    }
    return STR.ptr + PROGRAM_NAME;
}
const char *version(void) {
    if(!VERSION) {
        ini_parser_begin_section(&INI, "project");
        const char *v = ini_parser_read_string(&INI, "version");
        ini_parser_end_section(&INI);

        size_t v_len = strlen(v);

        VERSION = STR.len;
        CB_BUF_APPEND(&STR, v, v_len + 1);
    }
    return STR.ptr + VERSION;
}
const char *developer(void) {
    if(!DEVELOPER) {
        ini_parser_begin_section(&INI, "project");
        const char *dev = ini_parser_read_string(&INI, "developer-name");
        ini_parser_end_section(&INI);

        size_t dev_len = strlen(dev);

        DEVELOPER = STR.len;
        CB_BUF_APPEND(&STR, dev, dev_len + 1);
    }
    return STR.ptr + DEVELOPER;
}
size_t total_memory(void) {
    ini_parser_begin_section(&INI, "build");
    size_t result = ini_parser_read_integer(&INI, "total-memory");
    ini_parser_end_section(&INI);

    if(result < TOTAL_MEMORY_MIN) {
        result = TOTAL_MEMORY_MIN;
    }

    return result;
}
const char *memory_fmt(size_t bytes) {
    double amount = bytes;
    const char *units = "bytes";

    if(amount >= 1024) {
        amount /= 1024;
        units = "KiB";

        if(amount >= 1024) {
            amount /= 1024;
            units = "MiB";

            if(amount >= 1024) {
                amount /= 1024;
                units = "GiB";

                if(amount >= 1024) {
                    amount /= 1024;
                    units = "TiB";
                }
            }
        }
    }

    return local_fmt("%.2f%s", amount, units);
}
DirWalkAction check_resources_walk(const char *path, DirWalkInfo *info, void *params) {
    int *counter = (int *)params;
    *counter += info->file_type == FT_FILE ? 1 : 0;
    return DWA_CONTINUE;
}
bool check_resources(void) {
    if(!RESOURCES_CHECKED) {
        int counter = 0;
        switch(path_query_file_type("resources")) {
            case FT_NULL:
            case FT_FILE:
            case FT_PIPE:
                RESOURCES_EXISTS = false;
                break;
            case FT_DIR:
                if(dir_walk("resources", check_resources_walk, &counter)) {
                    RESOURCES_EXISTS = counter != 0;
                } else {
                    RESOURCES_EXISTS = false;
                }
                break;
        }

        RESOURCES_CHECKED = true;
    }
    return RESOURCES_EXISTS;
}
void collect_enable_flags(struct Option *opt) {
    FRB.len = 0;

    unsigned int count;
    struct FlagshipResult *ptr;

    if(flagship_search_repeatable(&FLS, MODE_NAMES[opt->mode], "enable", &count, &ptr) && count) {
        CB_BUF_APPEND(&FRB, ptr, count);
    }
    if(flagship_search_repeatable(&FLS, MODE_NAMES[opt->mode], "disable", &count, &ptr) && count) {
        CB_BUF_APPEND(&FRB, ptr, count);
    }

    if(!FRB.len) {
        // no flags
        return;
    }

    flagship_sort(FRB.len, FRB.ptr);

    for(size_t i = 0; i < FRB.len; ++i) {
        struct FlagshipResult *r = FRB.ptr + i;

        const char *name = flagship_result_name(&FLS, r);

        if(strcmp(name, "enable") == 0) {
            opt->build.enable |= (1 << (uint16_t)r->t_enum);
        } else {
            opt->build.enable &= ~(1 << (uint16_t)r->t_enum);
        }
    }
}
void collect_rebuild_flags(struct Option *opt) {
    FRB.len = 0;

    unsigned int count;
    struct FlagshipResult *ptr;

    if(flagship_search_repeatable(&FLS, MODE_NAMES[opt->mode], "rebuild", &count, &ptr) && count) {
        CB_BUF_APPEND(&FRB, ptr, count);
    }
    if(flagship_search_repeatable(&FLS, MODE_NAMES[opt->mode], "skip-rebuild", &count, &ptr) && count) {
        CB_BUF_APPEND(&FRB, ptr, count);
    }

    if(!FRB.len) {
        // no flags
        return;
    }

    flagship_sort(FRB.len, FRB.ptr);

    for(size_t i = 0; i < FRB.len; ++i) {
        struct FlagshipResult *r = FRB.ptr + i;

        const char *name = flagship_result_name(&FLS, r);

        if(strcmp(name, "rebuild") == 0) {
            if(r->t_enum == REBUILD_ALL) {
                for(enum RebuildTarget t = REBUILD_BEGIN + 1; t < REBUILD_COUNT; ++t) {
                    opt->build.rebuild &=
                        ~(REBUILD_OPT_CLEAR_MASK << ((uint16_t)(t - 1) * 2));
                    opt->build.rebuild |=
                         (REBUILD_OPT_FORCE      << ((uint16_t)(t - 1) * 2));
                }
            } else {
                opt->build.rebuild &= ~(REBUILD_OPT_CLEAR_MASK << ((uint16_t)(r->t_enum - 1) * 2));
                opt->build.rebuild |=  (REBUILD_OPT_FORCE      << ((uint16_t)(r->t_enum - 1) * 2));
            }
        } else {
            if(r->t_enum == REBUILD_ALL) {
                for(enum RebuildTarget t = REBUILD_BEGIN + 1; t < REBUILD_COUNT; ++t) {
                    opt->build.rebuild &=
                        ~(REBUILD_OPT_CLEAR_MASK << ((uint16_t)(t - 1) * 2));
                    opt->build.rebuild |=
                         (REBUILD_OPT_SKIP       << ((uint16_t)(t - 1) * 2));
                }
            } else {
                opt->build.rebuild &= ~(REBUILD_OPT_CLEAR_MASK << ((uint16_t)(r->t_enum - 1) * 2));
                opt->build.rebuild |=  (REBUILD_OPT_SKIP       << ((uint16_t)(r->t_enum - 1) * 2));
            }
        }
    }
}
enum RebuildOpt read_rebuild(struct OptionBuild *opt, enum RebuildTarget target) {
    CB_ASSERT(target != REBUILD_ALL, "cannot read rebuild flag %s!", REBUILD_NAMES[target]);
    return (enum RebuildOpt)((opt->rebuild >> ((uint16_t)(target - 1) * 2)) & REBUILD_OPT_CLEAR_MASK);
}
bool read_enable(struct OptionBuild *opt, enum EnableOpt e) {
    return (opt->enable & (1 << e)) == (1 << e);
}

enum Target target_normalize(enum Target target) {
    if(target >= T_COUNT) {
        return target_normalize(T_NATIVE);
    }

    if(target) {
        return target;
    }

    switch(CB_OS_CURRENT) {
        case CB_OS_WINDOWS:
            return T_WINDOWS;
        case CB_OS_MACOS:
            return T_MACOS;

        case CB_OS_GNU_LINUX:
        default:
            return T_LINUX;
    }
}
const char *target_name(enum Target target) {
    return TARGET_NAMES[target];
}

#define attempt(Offset, String) do {\
    if(proc_exists(String)) { \
        Offset = STR.len; \
        size_t len = sizeof(String); \
        CB_BUF_APPEND(&STR, String, len); \
        return STR.ptr + Offset; \
    } \
} while(0)
const char *compiler_c_linux_mingw(void) {
    if(!MINGW_C) {
        attempt(MINGW_C, "x86_64-w64-mingw32-gcc-win32");
        attempt(MINGW_C, "x86_64-w64-mingw32-gcc");

        const char *query = env_query("MINGW_C");
        if(query) {
            MINGW_C = STR.len;
            size_t len = strlen(query);
            CB_BUF_APPEND(&STR, query, len);
        } else {
            // failed all other attemps, womp-womp
            return "x86_64-w64-mingw32-gcc";
        }
    }
    return STR.ptr + MINGW_C;
}
const char *compiler_cpp_linux_mingw(void) {
    if(!MINGW_CPP) {
        attempt(MINGW_CPP, "x86_64-w64-mingw32-g++-win32");
        attempt(MINGW_CPP, "x86_64-w64-mingw32-g++");

        const char *query = env_query("MINGW_CPP");
        if(query) {
            MINGW_CPP = STR.len;
            size_t len = strlen(query);
            CB_BUF_APPEND(&STR, query, len);
        } else {
            // failed all other attemps, womp-womp
            return "x86_64-w64-mingw32-g++";
        }
    }
    return STR.ptr + MINGW_CPP;
}
const char *archiver_linux_mingw(void) {
    if(!MINGW_AR) {
        attempt(MINGW_AR, "x86_64-w64-mingw32-gcc-ar-win32");
        attempt(MINGW_AR, "x86_64-w64-mingw32-gcc-ar");

        const char *query = env_query("MINGW_AR");
        if(query) {
            MINGW_AR = STR.len;
            size_t len = strlen(query);
            CB_BUF_APPEND(&STR, query, len);
        } else {
            // failed all other attemps, womp-womp
            return "x86_64-w64-mingw32-gcc-ar";
        }
    }
    return STR.ptr + MINGW_AR;
}
#undef attempt

const char *compiler_c(enum Target target) {
    const char *result = "cc";
    switch(CB_OS_CURRENT) {
        case CB_OS_MACOS: switch(target) {
            case T_NATIVE:
            case T_COUNT:
            case T_WINDOWS:
                result = "clang";
                break;
            case T_LINUX:
                result = "clang";
                break;
            case T_MACOS:
                result = "clang";
                break;
            case T_WASM:
                result = "emcc";
                break;
        } break;
        case CB_OS_WINDOWS: switch(target) {
            case T_NATIVE:
            case T_COUNT:
            case T_WINDOWS:
                result = "clang";
                break;
            case T_LINUX:
                result = "gcc";
                break;
            case T_MACOS:
                result = "clang";
                break;
            case T_WASM:
                result = "emcc";
                break;
        } break;
        case CB_OS_GNU_LINUX:
        default: switch(target) {
            case T_LINUX:
            case T_NATIVE:
            case T_COUNT:
                result = "gcc";
                break;
            case T_WINDOWS:
                result = compiler_c_linux_mingw();
                break;
            case T_MACOS:
                result = "clang";
                break;
            case T_WASM:
                result = "emcc";
                break;
        } break;
    }
    return result;
}
const char *compiler_cpp(enum Target target) {
    const char *result = "c++";
    switch(CB_OS_CURRENT) {
        case CB_OS_MACOS: switch(target) {
            case T_NATIVE:
            case T_COUNT:
            case T_WINDOWS:
                result = "clang++";
                break;
            case T_LINUX:
                result = "clang++";
                break;
            case T_MACOS:
                result = "clang++";
                break;
            case T_WASM:
                result = "em++";
                break;
        } break;
        case CB_OS_WINDOWS: switch(target) {
            case T_NATIVE:
            case T_COUNT:
            case T_WINDOWS:
                result = "clang++";
                break;
            case T_LINUX:
                result = "g++";
                break;
            case T_MACOS:
                result = "clang++";
                break;
            case T_WASM:
                result = "em++";
                break;
        } break;
        case CB_OS_GNU_LINUX:
        default: switch(target) {
            case T_LINUX:
            case T_NATIVE:
            case T_COUNT:
                result = "g++";
                break;
            case T_WINDOWS:
                result = compiler_cpp_linux_mingw();
                break;
            case T_MACOS:
                result = "clang++";
                break;
            case T_WASM:
                result = "em++";
                break;
        } break;
    }
    return result;
}
const char *archiver(enum Target target) {
    const char *result = "ar";
    switch(CB_OS_CURRENT) {
        case CB_OS_MACOS: switch(target) {
            case T_NATIVE:
            case T_COUNT:
            case T_WINDOWS:
            case T_LINUX:
            case T_MACOS:
                break;
            case T_WASM:
                result = "emar";
                break;
        } break;
        case CB_OS_WINDOWS: switch(target) {
            case T_NATIVE:
            case T_COUNT:
            case T_WINDOWS:
            case T_LINUX:
            case T_MACOS:
                break;
            case T_WASM:
                result = "emar";
                break;
        } break;
        case CB_OS_GNU_LINUX:
        default: switch(target) {
            case T_LINUX:
            case T_NATIVE:
            case T_COUNT:
            case T_MACOS:
                break;
            case T_WINDOWS:
                result = archiver_linux_mingw();
                break;
            case T_WASM:
                result = "emar";
                break;
        } break;
    }
    return result;
}

const char *build_dir(enum Target target) {
    if(!BUILD_DIR) {
        const char *text = local_fmt("build/%s", TARGET_NAMES[target]);
        size_t text_len  = strlen(text);

        BUILD_DIR = STR.len;
        CB_BUF_APPEND(&STR, text, text_len + 1);
    }
    return STR.ptr + BUILD_DIR;
}
const char *build_obj_dir(enum Target target) {
    if(!BUILD_OBJ_DIR) {
        const char *text = local_fmt("build/%s/obj", TARGET_NAMES[target]);
        size_t text_len  = strlen(text);

        BUILD_OBJ_DIR = STR.len;
        CB_BUF_APPEND(&STR, text, text_len + 1);
    }
    return STR.ptr + BUILD_OBJ_DIR;
}

const char *program_ext(enum Target target) {
    switch(target) {
        case T_WINDOWS:
            return ".exe";
        case T_WASM:
            return ".html";
        case T_MACOS:

        case T_NATIVE:
        case T_LINUX:
        case T_COUNT:
            break;
    }
    return "";
}

int ini_define(struct IniParserContext *ini) {
    int ec = E_NONE;

    ini_parser_begin_section(ini, "project"); {
        ini_parser_comment(ini, "project settings");

        ini_parser_begin_field(ini, "display-name"); {
            ini_parser_comment(ini, "name of project for displaying, optional");
            ini_parser_value(ini, "Raylib Project");
            ini_parser_end_field(ini);
        }

        ini_parser_begin_field(ini, "program-name"); {
            ini_parser_comment(ini, "name of project program");
            ini_parser_value(ini, "raylib-project");
            ini_parser_end_field(ini);
        }

        ini_parser_begin_field(ini, "version"); {
            ini_parser_comment(ini, "project version, optional");
            ini_parser_value(ini, "0.0.0");

            ini_parser_end_field(ini);
        }

        ini_parser_begin_field(ini, "developer-name"); {
            ini_parser_comment(ini, "name of developer, optional");
            ini_parser_value(ini, "unknown");
            ini_parser_end_field(ini);
        }

        ini_parser_begin_field(ini, "window-width"); {
            ini_parser_comment(ini, "initial width of window, minimum: 100");
            ini_parser_value(ini, "800");
            ini_parser_end_field(ini);
        }

        ini_parser_begin_field(ini, "window-height"); {
            ini_parser_comment(ini, "initial height of window, minimum: 100");
            ini_parser_value(ini, "600");
            ini_parser_end_field(ini);
        }

        ini_parser_begin_field(ini, "framerate"); {
            ini_parser_comment(ini, "minimum: 20");
            ini_parser_value(ini, "60");
            ini_parser_end_field(ini);
        }

        ini_parser_end_section(ini);
    }

    ini_parser_begin_section(ini, "build"); {
        ini_parser_comment(ini, "build settings");

        ini_parser_begin_field(ini, "total-memory"); {
            ini_parser_comment(ini,
                "memory for wasm build, minimum: %s\n"
                "value must be written in bytes", memory_fmt(TOTAL_MEMORY_MIN));
            ini_parser_value(ini, "%zu", TOTAL_MEMORY_MIN);
            ini_parser_end_field(ini);
        }

        ini_parser_end_section(ini);
    }

    return ec;
}

int ini_load(struct IniParserContext *ini) {
    int ec = E_NONE;
    const char *path = "config.ini";

    if(path_exists(path)) {
        CB_INFO("loading %s . . .", path);
        if(!ini_parser_deserialize_file_path(ini, path)) {
            return error(E_FILE_OPEN, path);
        }
    } else {
        CB_INFO("creating %s . . .", path);
        File project_ini;
        if(!file_open(path, FF_CREATE | FF_WRITE, &project_ini)) {
            return error(E_FILE_CREATE, path);
        }

        ini_parser_serialize_stream(ini, file_stream, &project_ini);

        file_close(&project_ini);
    }

    return ec;
}

int error(int ec, ...) {
    va_list va;
    va_start(va, ec);
    switch((enum Error)ec) {
        case E_PARSE_ARGS:
            break;
        case E_FILE_CREATE: {
            const char *path = va_arg(va, const char *);
            CB_ERROR("failed to create file: %s", path);
        } break;
        case E_FILE_OPEN: {
            const char *path = va_arg(va, const char *);
            CB_ERROR("failed to open file: %s", path);
        } break;
        case E_MISSING_PROC: {
            const char *proc = va_arg(va, const char *);
            CB_ERROR("required program %s is missing!", proc);
        } break;
        case E_MAKE_DIR: {
            const char *path = va_arg(va, const char *);
            CB_ERROR("failed to make dir: %s", path);
        } break;
        case E_WRONG_FILE_TYPE: {
            const char *path = va_arg(va, const char *);
            FileType    eft  = va_arg(va, FileType);
            FileType    fft  = va_arg(va, FileType);
            CB_ERROR(
                "expected %s to be a %s but it's a %s!",
                path, string_from_file_type(eft).ptr, string_from_file_type(fft).ptr);
        } break;
        case E_EXEC_ASYNC: {
            const char *name = va_arg(va, const char *);
            CB_ERROR("failed to add process %s to async group!", name);
        } break;
        case E_PROC_FAIL: {
            const char *name      = va_arg(va, const char *);
            int         exit_code = va_arg(va, int);
            CB_ERROR("child process %s exited with code %i", name, exit_code);
        } break;
        case E_NO_RUN_SUPPORT: {
            int os             = va_arg(va, int);
            enum Target target = va_arg(va, enum Target);
            CB_ERROR(
                "the current os %s does not have run support for target %s!",
                OS_NAMES[os], TARGET_NAMES[target]);
        } break;
        case E_FILE_MOVE: {
            const char *dst = va_arg(va, const char *);
            const char *src = va_arg(va, const char *);
            CB_ERROR("failed to move %s to %s!", src, dst);
        } break;
        case E_UNIMPLEMENTED: {
            const char *msg = va_arg(va, const char *);
            CB_ERROR("feature unimplemented: %s", msg);
        } break;

        case E_NONE:
        case E_COUNT:
            break;
    }
    va_end(va);
    return ec;
}

int __proc_check(const char *first, ...) {
    int success = E_NONE;
    va_list va;
    va_start(va, first);
    const char *p = first;
    while(p) {
        if(!proc_exists(p)) {
            success = error(E_MISSING_PROC, p);
        }
        p = va_arg(va, const char *);
    }
    va_end(va);

    return success;
}
int __makedir(const char *first, ...) {
    int success = E_NONE;
    va_list va, va2;
    va_start(va, first);
    va_copy(va2, va);

    bool list_began_printing = false;

    const char *p = first;
    while(p) {
        switch(path_query_file_type(p)) {
            case FT_NULL:
                break;

            case FT_FILE:
            case FT_PIPE:
            case FT_DIR:
                p = va_arg(va2, const char *);
                continue;
        }
        if(!list_began_printing) {
            logger_output(LOG_INFO, "[I]   > mkdir ");
            list_began_printing = true;
        }
        logger_output(LOG_INFO, "%s ", p);
        p = va_arg(va2, const char *);
    }
    if(list_began_printing) {
        logger_output(LOG_INFO, "\n");
    }
    va_end(va2);

    p = first;
    while(p && (success == E_NONE)) {
        switch(path_query_file_type(p)) {
            case FT_FILE:
            case FT_PIPE:
                success = error(E_MAKE_DIR, p);
                continue;

            case FT_NULL:  break;
            case FT_DIR:
                p = va_arg(va, const char *);
                continue;
        }
        if(!dir_create(p, false)) {
            success = error(E_MAKE_DIR, p);
        }
        p = va_arg(va, const char *);
    }
    va_end(va);

    return success;
}

size_t file_stream(void *target, size_t n, const void *bytes) {
    File *file = (File *)target;

    return file_write(file, bytes, n);
}

#define CB_IMPL
#include "extern/cb/cb.h"

