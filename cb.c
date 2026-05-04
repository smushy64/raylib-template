/**
 * @file   cb.c
 * @brief  Build system for Raylib Projects.
 * @author Alicia D. Amarilla (smushyaa@gmail.com)
 * @date   May 03, 2026
*/
#include "cb.h"
#include "flagship.h"
#include "ini-parse.h"

#define CFG_DEFAULT_SRC "config.ini"

#define CFG_DEFAULT_TEXT                              \
"# configuration for raylib-template build system\n"  \
"[project]\n"                                         \
"name=\"Raylib Template\"\n"                          \
"width=800\n"                                         \
"height=600\n"                                        \
"framerate=60\n"

enum Error {
    E_NONE,      // no-args
    E_OPT_PARSE, // no-args
};

enum Mode {
    M_HELP,
    M_INIT,
    M_BUILD,
    M_RUN,
    M_PKG,

    M_COUNT,
    M_DEFAULT = M_HELP
};
#define __MODE_NAMES \
    "help",          \
    "init",          \
    "build",         \
    "run",           \
    "pkg"            \

const char* MODE_NAMES[M_COUNT + 1]   = {__MODE_NAMES, NULL};
const char* HELP_TARGETS[M_COUNT + 2] = {__MODE_NAMES, "cfg", NULL};

enum Target {
    T_NATIVE,
    T_LINUX,
    T_WINDOWS,
    T_WASM,

    T_COUNT,
    T_DEFAULT = T_NATIVE
};
#define __TARGET_NAMES \
    "native",          \
    "linux",           \
    "windows",         \
    "wasm"

const char* TARGET_NAMES[T_COUNT + 1] = {__TARGET_NAMES, NULL};
const char* TARGET_FULL_NAMES[T_COUNT + 1] = {
    "",
    "GNU/Linux",
    "Windows",
    "Web WASM",
    NULL
};
const char* TARGET_DEFAULT_C_COMPILER[T_COUNT + 1] = {
    "",
    "gcc",
    "gcc",
    "clang",
    NULL
};
const char* TARGET_DEFAULT_CPP_COMPILER[T_COUNT + 1] = {
    "",
    "g++",
    "g++",
    "clang++",
    NULL
};

struct Opt {
    enum Mode mode;

    union {
        struct ModeHelp {
            enum Mode mode;
        } help;
        struct ModeInit {
            const char* project_name;

            bool is_c : 1;
        } init;
        struct ModeBuild {
            const char* cfg_src; // if "0", read from stdin. if starts w/ "src:", treat as source file.
            enum Target target;
            bool rebuild_project    : 1;
            bool rebuild_raylib     : 1;
            bool rebuild_vendor     : 1;
            bool vendor_only        : 1;
            bool strip_symbols      : 1;
            bool is_optimized       : 1;
        } build;
        struct ModeRun {
            struct ModeBuild build;
            int    forward_flags_start;
        } run;
        struct ModePkg {
            struct ModeBuild build;
            bool no_resources : 1;
        } pkg;
    };
    bool is_dry  : 1;
    bool is_help : 1;
};
void opt_init(struct Opt* opt, enum Mode mode);
bool opt_parse(struct Opt* opt);

const char* get_parent_directory(void);
int error(enum Error ec, ...);

int enum_parse(const char* str, const char** list);

int mode_init(struct Opt* opt);
int mode_build(struct Opt* opt);
int mode_run(struct Opt* opt);
int mode_pkg(struct Opt* opt);

int    ARGC;
char** ARGV;

const char* PARENT_DIR = NULL;
StringBuf STR = {0};

FShipContext __fshipcontext__ = {0};
FShipContext* FLS = &__fshipcontext__;
void fls_define(void);

int main(int argc, char** argv) {
    CB_BUILD_INIT();
    ARGC = argc;
    ARGV = argv;

    CB_BUF_RESERVE(&STR, 4096);

    fls_define();

    struct Opt opt = {0};
    if(!opt_parse(&opt)) {
        return error(E_OPT_PARSE);
    }

    if(opt.mode != M_HELP && opt.is_help) {
        fls_help(FLS, MODE_NAMES[opt.mode], false);
        return error(E_NONE);
    }

    switch(opt.mode) {
        case M_HELP: {
            fls_help(FLS, MODE_NAMES[opt.help.mode], opt.help.mode == M_HELP);
        } break;
        case M_INIT  : return mode_init(&opt);
        case M_BUILD : return mode_build(&opt);
        case M_RUN   : return mode_run(&opt);
        case M_PKG   : return mode_pkg(&opt);
        case M_COUNT : break;
    }

    return error(E_NONE);
}

int mode_init(struct Opt* opt) {
    (void)opt;
    return error(E_NONE);
}
int mode_build(struct Opt* opt) {
    (void)opt;
    return error(E_NONE);
}
int mode_run(struct Opt* opt) {
    (void)opt;
    return error(E_NONE);
}
int mode_pkg(struct Opt* opt) {
    (void)opt;
    return error(E_NONE);
}

bool opt_parse(struct Opt* opt) {
    int last_arg = 0;
    if(!fls_parse(FLS, ARGC, ARGV, &last_arg)) {
        return false;
    }

    const char* m = fls_query_mode(FLS);
    if(!m) {
        opt->mode      =
        opt->help.mode = M_HELP;
        return true;
    }

    opt_init(opt, enum_parse(m, MODE_NAMES));

    opt->is_dry  = fls_read_flag(FLS, "dry");
    opt->is_help = fls_read_flag(FLS, "help");

    bool        is_set = false;
    const char* str    = NULL;
    switch(opt->mode) {
        case M_HELP: {
            opt->help.mode = enum_parse(fls_read_str(FLS, NULL), MODE_NAMES);
        } break;
        case M_INIT: {
            str = fls_read_str(FLS, NULL, .is_set=&is_set);
            if(is_set) {
                opt->init.project_name = str;
            }

            if(fls_read_flag(FLS, "c-proj")) {
                opt->init.is_c = true;
            }
        } break;
        case M_BUILD: {
            opt->build.target = enum_parse(fls_read_str(FLS, "target"), TARGET_NAMES);
            if(fls_read_flag(FLS, "rebuild")) {
                opt->build.rebuild_project =
                    opt->build.rebuild_raylib  =
                    opt->build.rebuild_vendor  = true;
            }
            if(fls_read_flag(FLS, "rebuild.project")) {
                opt->build.rebuild_project = true;
            }
            if(fls_read_flag(FLS, "rebuild.raylib")) {
                opt->build.rebuild_raylib = true;
            }
            if(fls_read_flag(FLS, "rebuild.vendor")) {
                opt->build.rebuild_vendor = true;
            }

            if(fls_read_flag(FLS, "no-rebuild")) {
                opt->build.rebuild_project =
                    opt->build.rebuild_raylib  =
                    opt->build.rebuild_vendor  = false;
            }
            if(fls_read_flag(FLS, "no-rebuild.project")) {
                opt->build.rebuild_project = false;
            }
            if(fls_read_flag(FLS, "no-rebuild.raylib")) {
                opt->build.rebuild_raylib = false;
            }
            if(fls_read_flag(FLS, "no-rebuild.vendor")) {
                opt->build.rebuild_vendor = false;
            }

            if(fls_read_flag(FLS, "vendor-only")) {
                opt->build.vendor_only = true;
            }
            if(fls_read_flag(FLS, "strip-symbols")) {
                opt->build.strip_symbols = true;
            }
            if(fls_read_flag(FLS, "optimized")) {
                opt->build.is_optimized = true;
            }

            str = fls_read_str(FLS, "config", .is_set=&is_set);
            if(is_set) {
                opt->build.cfg_src = STR.ptr + STR.len;
                string_buf_fmt(&STR, "str:%s", str);
                STR.len++; // append null terminator
            }

            str = fls_read_str(FLS, "config-path", .is_set=&is_set);
            if(is_set) {
                opt->build.cfg_src = str;
            }
        } break;
        case M_RUN: {
            opt->build.target = enum_parse(fls_read_str(FLS, "target"), TARGET_NAMES);
            if(fls_read_flag(FLS, "rebuild")) {
                opt->build.rebuild_project =
                    opt->build.rebuild_raylib  =
                    opt->build.rebuild_vendor  = true;
            }
            if(fls_read_flag(FLS, "rebuild.project")) {
                opt->build.rebuild_project = true;
            }
            if(fls_read_flag(FLS, "rebuild.raylib")) {
                opt->build.rebuild_raylib = true;
            }
            if(fls_read_flag(FLS, "rebuild.vendor")) {
                opt->build.rebuild_vendor = true;
            }

            if(fls_read_flag(FLS, "no-rebuild")) {
                opt->build.rebuild_project =
                    opt->build.rebuild_raylib  =
                    opt->build.rebuild_vendor  = false;
            }
            if(fls_read_flag(FLS, "no-rebuild.project")) {
                opt->build.rebuild_project = false;
            }
            if(fls_read_flag(FLS, "no-rebuild.raylib")) {
                opt->build.rebuild_raylib = false;
            }
            if(fls_read_flag(FLS, "no-rebuild.vendor")) {
                opt->build.rebuild_vendor = false;
            }

            if(fls_read_flag(FLS, "strip-symbols")) {
                opt->build.strip_symbols = true;
            }
            if(fls_read_flag(FLS, "optimized")) {
                opt->build.is_optimized = true;
            }

            str = fls_read_str(FLS, "config", .is_set=&is_set);
            if(is_set) {
                opt->build.cfg_src = STR.ptr + STR.len;
                string_buf_fmt(&STR, "str:%s", str);
                STR.len++; // append null terminator
            }

            str = fls_read_str(FLS, "config-path", .is_set=&is_set);
            if(is_set) {
                opt->build.cfg_src = str;
            }

            fls_read_flag(FLS, "-", .is_set=&is_set);
            if(is_set) {
                opt->run.forward_flags_start = last_arg + 1;
            }
        } break;
        case M_PKG: {
            opt->build.target = enum_parse(fls_read_str(FLS, "target"), TARGET_NAMES);
            str = fls_read_str(FLS, "config", .is_set=&is_set);
            if(is_set) {
                opt->build.cfg_src = STR.ptr + STR.len;
                string_buf_fmt(&STR, "str:%s", str);
                STR.len++; // append null terminator
            }

            str = fls_read_str(FLS, "config-path", .is_set=&is_set);
            if(is_set) {
                opt->build.cfg_src = str;
            }

            if(fls_read_flag(FLS, "no-resources")) {
                opt->pkg.no_resources = true;
            }
        } break;

        default: return false;
    }

    return true;
}
void opt_init(struct Opt* opt, enum Mode mode) {
    opt->mode = mode;
    switch(opt->mode) {
        case M_HELP:
            opt->help.mode = M_HELP;
            break;
        case M_INIT:
            opt->init.project_name = get_parent_directory();
            opt->init.is_c         = false;
            break;
        case M_BUILD:
            opt->build.cfg_src         = CFG_DEFAULT_SRC;
            opt->build.target          = T_DEFAULT;
            opt->build.rebuild_project = true;
            opt->build.rebuild_raylib     =
                opt->build.rebuild_vendor = false;
            opt->build.vendor_only   = false;
            opt->build.strip_symbols = false;
            opt->build.is_optimized  = false;
            break;
        case M_RUN:
            opt->build.cfg_src         = CFG_DEFAULT_SRC;
            opt->build.target          = T_DEFAULT;
            opt->build.rebuild_project = true;
            opt->build.rebuild_raylib     =
                opt->build.rebuild_vendor = false;
            opt->build.vendor_only   = false;
            opt->build.strip_symbols = false;
            opt->build.is_optimized  = false;
            opt->run.forward_flags_start   = ARGC;
            break;
        case M_PKG:
            opt->build.cfg_src         = CFG_DEFAULT_SRC;
            opt->build.target          = T_DEFAULT;
            opt->build.rebuild_project = true;
            opt->build.rebuild_raylib  =
                opt->build.rebuild_vendor = true;
            opt->build.vendor_only   = false;
            opt->build.strip_symbols = true;
            opt->build.is_optimized  = true;
            opt->pkg.no_resources = false;
            break;

        case M_COUNT:
            break;
    }
    opt->is_dry  = false;
    opt->is_help = false;
}

int error(enum Error ec, ...) {
    va_list va;
    va_start(va, ec);

    switch(ec) {

        case E_NONE:
        case E_OPT_PARSE:
            break;
    }

    va_end(va);

    return (int)ec;
}

const char* get_parent_directory(void) {
    if(!PARENT_DIR) {
        const char* cwd = dir_cwd_query();
        int from = 0, to = strlen(cwd);

        for(int i = to; i-- > 0;) {
            if(i != to && (cwd[i] == '/' || cwd[i] == '\\')) {
                from = i + 1;
                break;
            }
        }

        PARENT_DIR = memcpy(STR.ptr + STR.len, cwd + from, to - from);
        STR.len   += (to - from) + 1;
    }
    return PARENT_DIR;
}

int enum_parse(const char* str, const char** list) {
    int i = 0;
    while(*list) {
        if(strcmp(str, *list) == 0) {
            return i;
        }
        i++; list++;
    }
    return -1;
}

void fls_define(void) {
#if CB_OS_CURRENT == CB_OS_WINDOWS
    fls_set_program_name(FLS, "./build.bat");
#else
    fls_set_program_name(FLS, "./build.sh");
#endif

    fls_set_description(FLS, "Build system for Raylib project.");

    fls_mode_begin(FLS, "init"); {
        fls_mode_set_description(FLS, "Initialize a new project.");
        fls_add_str(FLS, NULL, .description="Set name of project.", .default_value="<parent-directory>");
        fls_add_flag(FLS, "c-proj", .description="Generate a C project instead of a C++ project.", .aliases=fls_strings("c"));
        fls_add_flag(FLS, "dry", .description="Dry run. Only prints commands, does not run them.", .aliases=fls_strings("d"));
        fls_add_flag(FLS, "help", .description="Print help for this mode.", .aliases=fls_strings("h"), .is_terminating=true);
        fls_mode_end(FLS);
    }
    fls_mode_begin(FLS, "build"); {
        fls_mode_set_description(FLS, "Build project.");
        fls_add_str(FLS, "target", .description="Set target platform.", .aliases=fls_strings("t"), .default_value=TARGET_NAMES[T_DEFAULT], .str={.valid=TARGET_NAMES});
        fls_add_flag(FLS, "rebuild", .description="Force a rebuild of all dependencies.", .aliases=fls_strings("re"));
        fls_add_flag(FLS, "rebuild.project", .description="Force a rebuild of project.", .aliases=fls_strings("re.project"));
        fls_add_flag(FLS, "rebuild.raylib", .description="Force a rebuild of raylib.", .aliases=fls_strings("re.raylib"));
        fls_add_flag(FLS, "rebuild.vendor", .description="Force a rebuild of vendor libraries.", .aliases=fls_strings("re.vendor"));
        fls_add_flag(FLS, "no-rebuild", .description="Skip rebuild of all dependencies.", .aliases=fls_strings("nre"));
        fls_add_flag(FLS, "no-rebuild.project", .description="Skip rebuild of project.", .aliases=fls_strings("nre.project"));
        fls_add_flag(FLS, "no-rebuild.raylib", .description="Skip rebuild of raylib.", .aliases=fls_strings("nre.raylib"));
        fls_add_flag(FLS, "no-rebuild.vendor", .description="Skip rebuild of vendor libraries.", .aliases=fls_strings("nre.vendor"));
        fls_add_flag(FLS, "vendor-only", .description="Only build/rebuild vendor libraries.", .aliases=fls_strings("v"));
        fls_add_flag(FLS, "strip-symbols", .description="Strip debug symbols.", .aliases=fls_strings("s"));
        fls_add_flag(FLS, "optimized", .description="Build with optimizations turned on.", .aliases=fls_strings("o"));
        fls_add_str(FLS, "config", .description="Ignore build system config file and use string as config file.", .aliases=fls_strings("cfg"));
        fls_add_str(FLS, "config-path", .description="Change config file path.", .aliases=fls_strings("cfgp"), .default_value=CFG_DEFAULT_SRC);
        fls_add_flag(FLS, "dry", .description="Dry run. Only prints commands, does not run them.", .aliases=fls_strings("d"));
        fls_add_flag(FLS, "help", .description="Print help for this mode.", .aliases=fls_strings("h"), .is_terminating=true);
        fls_mode_end(FLS);
    }
    fls_mode_begin(FLS, "run"); {
        fls_mode_set_description(FLS, "Build project and run.");
        fls_add_str(FLS, "target", .description="Set target platform.", .aliases=fls_strings("t"), .default_value=TARGET_NAMES[T_DEFAULT], .str={.valid=TARGET_NAMES});
        fls_add_flag(FLS, "rebuild", .description="Force a rebuild of all dependencies.", .aliases=fls_strings("re"));
        fls_add_flag(FLS, "rebuild.project", .description="Force a rebuild of project.", .aliases=fls_strings("re.project"));
        fls_add_flag(FLS, "rebuild.raylib", .description="Force a rebuild of raylib.", .aliases=fls_strings("re.raylib"));
        fls_add_flag(FLS, "rebuild.vendor", .description="Force a rebuild of vendor libraries.", .aliases=fls_strings("re.vendor"));
        fls_add_flag(FLS, "no-rebuild", .description="Skip rebuild of all dependencies.", .aliases=fls_strings("nre"));
        fls_add_flag(FLS, "no-rebuild.project", .description="Skip rebuild of project.", .aliases=fls_strings("nre.project"));
        fls_add_flag(FLS, "no-rebuild.raylib", .description="Skip rebuild of raylib.", .aliases=fls_strings("nre.raylib"));
        fls_add_flag(FLS, "no-rebuild.vendor", .description="Skip rebuild of vendor libraries.", .aliases=fls_strings("nre.vendor"));
        fls_add_flag(FLS, "strip-symbols", .description="Strip debug symbols.", .aliases=fls_strings("s"));
        fls_add_flag(FLS, "optimized", .description="Build with optimizations turned on.", .aliases=fls_strings("o"));
        fls_add_str(FLS, "config", .description="Ignore build system config file and use string as config file.", .aliases=fls_strings("cfg"));
        fls_add_str(FLS, "config-path", .description="Change config file path.", .aliases=fls_strings("cfgp"), .default_value=CFG_DEFAULT_SRC);
        fls_add_flag(FLS, "-", .description="Stop parsing flags and pass remaining to project.", .is_terminating=true);
        fls_add_flag(FLS, "dry", .description="Dry run. Only prints commands, does not run them.", .aliases=fls_strings("d"));
        fls_add_flag(FLS, "help", .description="Print help for this mode.", .aliases=fls_strings("h"), .is_terminating=true);
        fls_mode_end(FLS);
    }
    fls_mode_begin(FLS, "pkg"); {
        fls_mode_set_description(FLS, "Build in release mode and package.");
        fls_add_str(FLS, "target", .description="Set target platform.", .aliases=fls_strings("t"), .default_value=TARGET_NAMES[T_DEFAULT], .str={.valid=TARGET_NAMES});
        fls_add_str(FLS, "config", .description="Ignore build system config file and use string as config file.", .aliases=fls_strings("cfg"));
        fls_add_str(FLS, "config-path", .description="Change config file path.", .aliases=fls_strings("cfgp"), .default_value=CFG_DEFAULT_SRC);
        fls_add_flag(FLS, "no-resources", .description="Don't package optional resources/ directory.", .aliases=fls_strings("nr"));
        fls_add_flag(FLS, "dry", .description="Dry run. Only prints commands, does not run them.", .aliases=fls_strings("d"));
        fls_add_flag(FLS, "help", .description="Print help for this mode.", .aliases=fls_strings("h"), .is_terminating=true);
        fls_mode_end(FLS);
    }
    fls_mode_begin(FLS, "help"); {
        fls_mode_set_description(FLS, "Print help and exit.");
        fls_add_str(FLS, NULL, .description="Name of mode or concept to print help for.", .str={.valid=HELP_TARGETS});
        fls_add_flag(FLS, "help", .description="Print help for this mode.", .aliases=fls_strings("h"), .is_terminating=true);
        fls_mode_end(FLS);
    }
}

#define CB_IMPL
#include "cb.h"

