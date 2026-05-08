/**
 * @file   cb.c
 * @brief  Build system for Raylib Projects.
 * @author Alicia D. Amarilla (smushyaa@gmail.com)
 * @date   May 03, 2026
*/
#include "cb.h"
#include "flagship.h"
#include "ini-parse.h"
#include <time.h>

#define CFG_DEFAULT_SRC "build/config.ini"

#define CFG_DEFAULT_WIDTH     800
#define CFG_DEFAULT_HEIGHT    600
#define CFG_DEFAULT_FRAMERATE 60

enum Error {
    E_NONE,      // no-args
    E_OPT_PARSE, // no-args
    E_FILE_OPEN, // path
    E_DIR_CREATE, // path
    E_TEMPLATE_SRC_MALFORMED, // path of template src
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
            int width;
            int height;
            int framerate;

            bool is_c  : 1;
            bool force : 1;
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
int __error(enum Error ec, const char* function, ...);

int __mkdirs(bool is_dry, const char* log_msg, const char* first, ...);

#define error(ec, ...) \
    __error(ec, __FUNCTION__ __VA_OPT__(,) __VA_ARGS__)

#define mkdirs(is_dry, log_msg, ...) \
    __mkdirs(is_dry, log_msg, __VA_ARGS__, NULL)

int enum_parse(const char* str, const char** list);

int mode_init(struct Opt* opt);
int mode_build(struct Opt* opt);
int mode_run(struct Opt* opt);
int mode_pkg(struct Opt* opt);

int    ARGC;
char** ARGV;

const char* PARENT_DIR = NULL;
StringBuf STR = {0};
StringBuf TMP = {0};

const char* fmt_str_va(const char* fmt, va_list va);
const char* fmt_str(const char* fmt, ...);

FShipContext __fshipcontext__ = {0};
FShipContext* FLS = &__fshipcontext__;
void fls_define(void);

enum TemplatePayloadType {
    TPT_STRING,
    TPT_INT,
    TPT_FLOAT,
};

struct TemplatePayload {
    enum TemplatePayloadType type;
    union {
        const char* s;
        int         i;
        float       f;
    };
};

struct TemplateParam {
    const char*            name;
    struct TemplatePayload payload;
};

struct TemplateParam TEMPLATE_PARAMS[] = {
    {.name="project-name", .payload={.type=TPT_STRING, .s=NULL}},
    {.name="project-width", .payload={.type=TPT_INT, .i=CFG_DEFAULT_WIDTH}},
    {.name="project-height", .payload={.type=TPT_INT, .i=CFG_DEFAULT_HEIGHT}},
    {.name="project-framerate", .payload={.type=TPT_INT, .i=CFG_DEFAULT_FRAMERATE}},
    {.name="compiler", .payload={.type=TPT_STRING, .s="gcc"}},
    {.name="compiler-version", .payload={.type=TPT_STRING, .s="c20"}},
    {.name="date-time", .payload={.type=TPT_STRING, .s=""}},
};

size_t TEMPLATE_PARAM_COUNT = sizeof(TEMPLATE_PARAMS) / sizeof(TEMPLATE_PARAMS[0]);

void template_set(const char* name, enum TemplatePayloadType type, ...);
const char* template_get(const char* name);

enum Target target_normalize(enum Target target);

const char* get_compiler(bool is_c, enum Target target);
const char* get_compiler_version(bool is_c, enum Target target);

bool   DATETIME_SET = false;
size_t DATETIME     = 0;
const char* get_datetime(void);

int main(int argc, char** argv) {
    CB_BUILD_INIT();
    ARGC = argc;
    ARGV = argv;

    CB_BUF_RESERVE(&STR, 4096);
    CB_BUF_RESERVE(&TMP, 256);

    template_set("date-time", TPT_STRING, get_datetime());
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

typedef void __InitWriteFn(void* target, String str);
void __init_write_file(void* target, String str) {
    file_write((File*)target, str.ptr, str.len);
}
void __init_print(void* target, String str) {
    for(size_t i = 0; i < str.len; ++i) {
        putchar(str.ptr[i]);
    }
    fflush(stdout);
}

int generate_from_template(const char* dst, const char* template, bool is_dry, bool overwrite) {
    int err = E_NONE;

    File dst_file = {0};
    if(is_dry) {
        CB_INFO("generating new file at %s . . .", dst);
    } else {
        FileFlags flags = FF_WRITE;
        if(path_exists(dst)) {
            if(!overwrite) {
                return err;
            }
            flags |= FF_TRUNCATE;

            CB_INFO("overwriting generated file at %s . . .", dst);
        } else {
            flags |= FF_CREATE;
            CB_INFO("generating new file at %s . . .", dst);
        }

        if(!file_open(dst, flags, &dst_file)) {
            return error(E_FILE_OPEN, dst);
        }
    }

    File template_file = {0};
    size_t restore_point = TMP.len;

    if(!file_open(template, FF_READ, &template_file)) {
        file_close(&dst_file);
        return error(E_FILE_OPEN, template);
    }

    String t = {0};
    t.len = file_query_size(&template_file);

    CB_BUF_RESERVE(&TMP, t.len);

    t.ptr = TMP.ptr + TMP.len;

    file_read(&template_file, t.ptr, t.len);
    TMP.len += t.len;
    file_close(&template_file);

    __InitWriteFn* write = is_dry ? __init_print : __init_write_file;

    while(t.len) {
        size_t start = 0;
        if(!string_find(t, '`', &start)) {
            write(&dst_file, t);
            t = CB_SLICE_ADVANCE(&t, t.len);
            continue;
        }

        String out = t;
        out.len = start;
        write(&dst_file, out);

        t = CB_SLICE_ADVANCE(&t, start);

        // if ` is the last character.
        // 1 because t is at [start]
        if(1 > t.len) {
            write(&dst_file, t);
            t = CB_SLICE_ADVANCE(&t, t.len);
            continue;
        }

        String arg = t;
        arg.len = 0;
        for(size_t i = 0; i < t.len; ++i) {
            if(t.ptr[i] == '\'') {
                arg.len = i + 1;
                break;
            }
        }

        // there is no arg or malformed
        if(!arg.len) {
            out     = t;
            out.len = 1;
            write(&dst_file, out);
            t = CB_SLICE_ADVANCE(&t);
            continue;
        }

        // now arg is `*content*'

        arg = CB_SLICE_ADVANCE(&arg);
        arg.len--;

        // now arg is just *content*

        const char* arg_value =
            template_get(local_fmt("%" CB_STRING_FMT, CB_STRING_ARG(&arg)));

        out = CB_STRING_FROM_CSTR(arg_value);
        write(&dst_file, out);

        // +2 because we advanced past ` and trimmed '
        t = CB_SLICE_ADVANCE(&t, arg.len + 2);
    }

    TMP.len = restore_point;
    if(!is_dry) {
        file_close(&dst_file);
    }
    return err;
}

struct GenerateSourcesParams {
    __InitWriteFn* write;
    void*          target;
};

DirWalkAction generate_sources_walk(const char* path, DirWalkInfo* info, void* params) {
    struct GenerateSourcesParams* p = (struct GenerateSourcesParams*)params;

    if(strcmp(path, "src/sources.cpp") == 0) {
        return DWA_CONTINUE;
    }

    if(!info->path_has_ext) {
        return DWA_CONTINUE;
    }
    const char* ext = path + info->path_ext_offset;
    if(!(strcmp(ext, ".cpp") == 0 || strcmp(ext, ".c") == 0)) {
        return DWA_CONTINUE;
    }

    String out = {0};
    out.ptr = (char*)local_fmt("#include \"../%s\"\n", path);
    out.len = strlen(out.ptr);

    p->write(p->target, out);
    return DWA_CONTINUE;
}

int generate_sources(bool is_c, bool is_dry) {
    int err = E_NONE;

    if((err = mkdirs(is_dry, "src/ doesn't exist! creating . . .", "src"))) {
        return err;
    }

    const char* dst_path = is_c ? "src/sources.c" : "src/sources.cpp";
    File dst = {0};

    CB_INFO("generating %s . . .", dst_path);

    if(!is_dry) {
        FileFlags flags = FF_WRITE;
        if(path_exists(dst_path)) {
            flags |= FF_TRUNCATE;
        } else {
            flags |= FF_CREATE;
        }

        if(!file_open(dst_path, flags, &dst)) {
            return error(E_FILE_OPEN, dst_path);
        }
    }

    struct GenerateSourcesParams p = {0};
    p.write  = is_dry ? __init_print : __init_write_file;
    p.target = &dst;

    p.write(p.target, CB_STRING("/* automatically generated by build system. do not edit. */\n"));
    dir_walk("src/", generate_sources_walk, &p);

    p.write(p.target, CB_STRING("\n"));

    if(!is_dry) {
        file_close(&dst);
    }
    return err;
}

int mode_init(struct Opt* opt) {
    int err = E_NONE;

    if((err = mkdirs(opt->is_dry, "build/ does not exist! creating . . .", "build"))) {
        return err;
    }

    // check if config file exists.
    //   if it does: only overwrite if -force is used
    //   if it doesn't: create a new config file.
    bool create_config_file = opt->init.force;
    bool config_file_exists = path_exists(CFG_DEFAULT_SRC);
    if(!create_config_file) {
        create_config_file = config_file_exists;
    }

    if(create_config_file) {
        if((err = generate_from_template(
            CFG_DEFAULT_SRC, "templates/config.template", opt->is_dry, config_file_exists
        ))) {
            return err;
        }
    }

    // create required subdirectories:
    //      src/
    //      include/
    //      build/
    if((err = mkdirs(opt->is_dry, "creating subdirectories . . .", "src", "include", "build"))) {
        return err;
    }

    // generate compile_flags for all subdirectories that need it. - always overwrite
    //      src/
    //      include/

    template_set("compiler", TPT_STRING, get_compiler(opt->init.is_c, T_NATIVE));
    template_set("compiler-version", TPT_STRING, get_compiler_version(opt->init.is_c, T_NATIVE));

    if((err = generate_from_template(
        "src/compile_flags.txt", "templates/compile_flags_src.template", opt->is_dry, true
    ))) {
        return err;
    }

    if((err = generate_from_template(
        "include/compile_flags.txt", "templates/compile_flags_include.template", opt->is_dry, true
    ))) {
        return err;
    }

    // generate main.cpp/.c in src/ - overwrite if -force
    if((err = generate_from_template(
        opt->init.is_c ? "src/main.c" : "src/main.cpp", "templates/main.template",
        opt->is_dry, opt->init.force
    ))) {
        return err;
    }

    // generate sources.cpp/.c in src/ - always overwrite
    if((err = generate_sources(opt->init.is_c, opt->is_dry))) {
        return err;
    }

    return err;
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

    if(fls_read_flag(FLS, "dry")) {
        opt->is_dry = true;
    }
    if(fls_read_flag(FLS, "help")) {
        opt->is_help = true;
    }

    bool        is_set = false;
    int         num    = 0;
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

            if(fls_read_flag(FLS, "force")) {
                opt->init.force = true;
            }

            num = fls_read_int(FLS, "window-width", .is_set=&is_set);
            if(is_set) {
                opt->init.width = num;
            }

            num = fls_read_int(FLS, "window-height", .is_set=&is_set);
            if(is_set) {
                opt->init.height = num;
            }

            num = fls_read_int(FLS, "framerate", .is_set=&is_set);
            if(is_set) {
                opt->init.framerate = num;
            }

            template_set("project-name", TPT_STRING, opt->init.project_name);
            template_set("project-width", TPT_INT, opt->init.width);
            template_set("project-height", TPT_INT, opt->init.height);
            template_set("project-framerate", TPT_INT, opt->init.framerate);
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
            opt->init.width        = CFG_DEFAULT_WIDTH;
            opt->init.height       = CFG_DEFAULT_HEIGHT;
            opt->init.framerate    = CFG_DEFAULT_FRAMERATE;
            opt->init.is_c         = false;
            opt->init.force        = false;
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

void template_set(const char* name, enum TemplatePayloadType type, ...) {
    va_list va;
    va_start(va, type);

    for(size_t i = 0; i < TEMPLATE_PARAM_COUNT; ++i) {
        struct TemplateParam* param = TEMPLATE_PARAMS + i;
        if(strcmp(param->name, name) == 0) {
            CB_ASSERT(
                param->payload.type == type,
                "attempted to set template param incorrectly!");

            switch(type) {
                case TPT_STRING: {
                    const char* s = va_arg(va, const char*);
                    param->payload.s = s;
                } break;
                case TPT_INT: {
                    int i = va_arg(va, int);
                    param->payload.i = i;
                } break;
                case TPT_FLOAT: {
                    float f = va_arg(va, double);
                    param->payload.f = f;
                } break;
            }
            break;
        }
    }

    va_end(va);
}
const char* template_get(const char* name) {
    const char* result = NULL;

    for(size_t i = 0; i < TEMPLATE_PARAM_COUNT; ++i) {
        struct TemplateParam* param = TEMPLATE_PARAMS + i;

        if(strcmp(param->name, name) == 0) {
            switch(param->payload.type) {
                case TPT_STRING:
                    result = param->payload.s;
                    break;
                case TPT_INT:
                    result = local_fmt("%d", param->payload.i);
                    break;
                case TPT_FLOAT:
                    result = local_fmt("%f", param->payload.f);
                    break;
            }
            break;
        }
    }

    if(!result) {
        result = "";
        logger_output(LOG_WARN, "\n[W] template param '%s' does not exist!\n", name);
    }
    return result;
}

enum Target target_normalize(enum Target target) {
    switch(target) {
        case T_COUNT:
        case T_NATIVE: {
            switch(CB_OS_CURRENT) {
                case CB_OS_GNU_LINUX: return T_LINUX;
                case CB_OS_WINDOWS:   return T_WINDOWS;
                default: return T_LINUX;
            }
        } break;

        default: return target;
    }
}

const char* get_compiler(bool is_c, enum Target target) {
    target = target_normalize(target);

    switch(target) {
        case T_LINUX   : return is_c ? "gcc" : "g++";
        case T_WINDOWS : return is_c ? "gcc" : "g++";
        case T_WASM    : return is_c ? "clang" : "clang++";

        case T_NATIVE:
        case T_COUNT:
            CB_PANIC("this path should be unreachable!");
    }
}
const char* get_compiler_version(bool is_c, enum Target target) {
    target = target_normalize(target);

    switch(target) {
        case T_LINUX   :
        case T_WINDOWS :
        case T_WASM    : return is_c ? "c20" : "c++20";

        case T_NATIVE:
        case T_COUNT:
            CB_PANIC("this path should be unreachable!");
    }
}

const char* get_datetime(void) {
    if(!DATETIME_SET) {
        time_t timer;
        struct tm* tm;
        timer = time(NULL);
        tm    = localtime(&timer);

        char* buf = STR.ptr + STR.len;

        size_t size = strftime(buf, STR.cap, "%B %d, %Y", tm);

        STR.len += size + 1;
        DATETIME     = buf - STR.ptr;
        DATETIME_SET = true;
    }

    return STR.ptr + DATETIME;
}

int __error(enum Error ec, const char* function, ...) {
    va_list va;
    va_start(va, function);

#define log(msg, ...) \
    CB_ERROR("%s(): " msg, function __VA_OPT__(,) __VA_ARGS__)

    switch(ec) {
        case E_NONE:
            break;
        case E_OPT_PARSE:
            log("failed to parse command line arguments!");
            break;
        case E_FILE_OPEN: {
            const char* path = va_arg(va, const char*);
            log("failed to open %s!", path);
        } break;
        case E_DIR_CREATE: {
            const char* path = va_arg(va, const char*);
            log("failed to create dir %s!", path);
        } break;
        case E_TEMPLATE_SRC_MALFORMED: {
            const char* path = va_arg(va, const char*);
            log("template '%s' is malformed!", path);
        } break;
    }

#undef log
    va_end(va);
    return (int)ec;
}

int __mkdirs(bool is_dry, const char* log_msg, const char* first, ...) {
    va_list va;
    va_start(va, first);

    bool did_log = false;
    const char* path = first;
    while(path) {
        bool directory_exists = path_exists(path);
        if(!directory_exists) {
            if(!did_log) {
                CB_INFO("%s", log_msg);
            }

            if(is_dry) {
                CB_INFO("  > mkdir %s", path);
            } else {
                if(!dir_create(path, false)) {
                    va_end(va);
                    return error(E_DIR_CREATE, path);
                }
            }
        }
        path = va_arg(va, const char*);
    }

    va_end(va);
    return error(E_NONE);
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
        fls_add_int(FLS, "window-width", .description="Set default window width.", .aliases=fls_strings("W", "width"), .default_value=CB_STRINGIFY2(CFG_DEFAULT_WIDTH));
        fls_add_int(FLS, "window-height", .description="Set default window height.", .aliases=fls_strings("H", "height"), .default_value=CB_STRINGIFY2(CFG_DEFAULT_HEIGHT));
        fls_add_int(FLS, "framerate", .description="Set default target framerate.", .aliases=fls_strings("F"), .default_value=CB_STRINGIFY2(CFG_DEFAULT_FRAMERATE));
        fls_add_flag(FLS, "force", .description="Force initialization. Overwrites config if it already exists.", .aliases=fls_strings("f"));
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

const char* fmt_str_va(const char* fmt, va_list va) {
    va_list va2;
    va_copy(va2, va);
    int len = vsnprintf(NULL, 0, fmt, va2);
    va_end(va2);

    CB_BUF_RESERVE(&STR, len + 1);

    char* result = STR.ptr + STR.len;
    vsnprintf(result, len + 1, fmt, va);

    STR.len += len + 1;
    return result;
}
const char* fmt_str(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    const char* result = fmt_str_va(fmt, va);
    va_end(va);
    return result;
}

#define CB_IMPL
#include "cb.h"

