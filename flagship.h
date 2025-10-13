#if !defined(FSHIP_H)
#define FSHIP_H
/**
 * @file   flagship.h
 * @brief  Flagship.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   September 29, 2025
*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <inttypes.h>

#undef FSHIP_INL
#if __cplusplus
    #define FSHIP_INL inline
#else
    #define FSHIP_INL static inline
#endif

#undef FSHIP_FMT_FN
#define FSHIP_FMT_FN(FormatIndex, ArgIndex) \
__attribute__((format(printf, FormatIndex, ArgIndex)))

#if !( \
    defined(FSHIP_STDOUT_FMT)    && \
    defined(FSHIP_STDERR_FMT)    && \
    defined(FSHIP_STDOUT_FMT_VA) && \
    defined(FSHIP_STDERR_FMT_VA) && \
    defined(FSHIP_COUNT_FMT)     && \
    defined(FSHIP_COUNT_FMT_VA)  && \
    defined(FSHIP_FMT)           && \
    defined(FSHIP_FMT_VA)           \
) \

#include <stdio.h>

#undef FSHIP_STDOUT_FMT
#undef FSHIP_STDERR_FMT
#undef FSHIP_STDOUT_FMT_VA
#undef FSHIP_STDERR_FMT_VA
#undef FSHIP_COUNT_FMT
#undef FSHIP_COUNT_FMT_VA
#undef FSHIP_FMT
#undef FSHIP_FMT_VA

#define FSHIP_STDOUT_FMT( fmt, ... ) \
    fprintf( stdout, fmt __VA_OPT__(,) __VA_ARGS__ )

#define FSHIP_STDERR_FMT( fmt, ... ) \
    fprintf( stderr, fmt __VA_OPT__(,) __VA_ARGS__ )

#define FSHIP_STDOUT_FMT_VA( fmt, va ) \
    vfprintf( stdout, fmt, va )

#define FSHIP_STDERR_FMT_VA( fmt, va ) \
    vfprintf( stderr, fmt, va )

#define FSHIP_COUNT_FMT( fmt, ... ) \
    snprintf( NULL, 0, fmt __VA_OPT__(,) __VA_ARGS__ )

#define FSHIP_COUNT_FMT_VA( fmt, va ) \
    vsnprintf( NULL, 0, fmt, va )

#define FSHIP_FMT( ptr, size, fmt, ... ) \
    snprintf( ptr, size, fmt __VA_OPT__(,) __VA_ARGS__ )

#define FSHIP_FMT_VA( ptr, size, fmt, va ) \
    vsnprintf( ptr, size, fmt, va )

#endif /* stdout, stderr */

#if !( \
    defined(FSHIP_MEMSET)       && \
    defined(FSHIP_MEMCMP)       && \
    defined(FSHIP_STRLEN)       && \
    defined(FSHIP_STRCMP)       && \
    defined(FSHIP_STRCHR)          \
) \

#undef FSHIP_MEMSET
#undef FSHIP_MEMCMP
#undef FSHIP_STRLEN
#undef FSHIP_STRCMP
#undef FSHIP_STRCHR

#include <string.h>

#define FSHIP_MEMSET( ptr, ch, size ) \
    memset( ptr, ch, size )

#define FSHIP_STRLEN( ptr ) \
    strlen( ptr )

FSHIP_INL
bool __fls_strcmp( const char* a, const char* b ) {
    return strcmp( a, b ) == 0;
}

#define FSHIP_STRCMP( a, b ) \
    __fls_strcmp( a, b )

FSHIP_INL
bool __fls_memcmp( const void* a, const void* b, size_t len ) {
    return memcmp( a, b, len ) == 0;
}

#define FSHIP_MEMCMP( a, b, len ) \
    __fls_memcmp( a, b, len )

#define FSHIP_STRCHR( string, ch ) \
    strchr( string, ch )

#endif /* memset, strlen, memcmp, strtod, strtoll, strchr, strcmp */

#if !( \
    defined(FSHIP_DBL_FROM_STR) && \
    defined(FSHIP_INT_FROM_STR) && \
    defined(FSHIP_ALLOC)        && \
    defined(FSHIP_FREE)            \
) \

#include <stdlib.h>

#undef FSHIP_ALLOC
#undef FSHIP_FREE
#undef FSHIP_DBL_FROM_STR
#undef FSHIP_INT_FROM_STR

FSHIP_INL
bool __fls_dbl_from_str( const char* string, double* out_value ) {
    char* endptr = NULL;

    double value = strtod( string, &endptr );

    const char* end_of_string = string + FSHIP_STRLEN( string );

    if( string != endptr && endptr == end_of_string ) {
        *out_value = value;
        return true;
    }

    return false;
}

#define FSHIP_DBL_FROM_STR( string, out_value ) \
    __fls_dbl_from_str( string, out_value )

FSHIP_INL
bool __fls_int_from_str( const char* string, int64_t* out_value ) {
    char* endptr = NULL;

    int64_t value = strtoll( string, &endptr, 10 );

    const char* end_of_string = string + FSHIP_STRLEN( string );

    if( string != endptr && endptr == end_of_string ) {
        *out_value = value;
        return true;
    }

    return false;
}

#define FSHIP_INT_FROM_STR( string, out_value ) \
    __fls_int_from_str( string, out_value )

FSHIP_INL
void* __fls_alloc( void* ptr, size_t old_size, size_t new_size ) {
    void* result = realloc( ptr, new_size );
    if( !result ) {
        return NULL;
    }

    for( size_t i = old_size; i < (new_size - old_size); ++i ) {
        *((char*)result + i) = 0;
    }

    return result;
}

#define FSHIP_ALLOC( ptr, old_size, new_size ) \
    __fls_alloc( ptr, old_size, new_size )

#define FSHIP_FREE( ptr, size ) \
    free( ptr )

#endif /* alloc, free */

#define FSHIP_COLOR_BLACK   "\033[1;30m"
#define FSHIP_COLOR_WHITE   "\033[1;37m"
#define FSHIP_COLOR_RED     "\033[1;31m"
#define FSHIP_COLOR_GREEN   "\033[1;32m"
#define FSHIP_COLOR_BLUE    "\033[1;34m"
#define FSHIP_COLOR_MAGENTA "\033[1;35m"
#define FSHIP_COLOR_YELLOW  "\033[1;33m"
#define FSHIP_COLOR_CYAN    "\033[1;36m"
#define FSHIP_COLOR_RESET   "\033[1;00m"

/// @brief Flag types.
typedef enum {
    /// @brief Boolean flag.
    /// @details
    /// To set flag:    -flag
    ///
    /// To ignore flag: +flag
    ///
    /// Boolean flags must always be named.
    FSHIP_BOOL,
    /// @brief Integer flag.
    /// @details
    /// To set flag:    -flag:\%d or -flag \%d
    ///
    /// To ignore flag: +flag:\%d or +flag \%d
    ///
    /// Where \%d is any positive/negative whole number.
    ///
    /// Integer flags can be unnamed, provided that there are no
    /// other integer or float unnamed flags in a given context.
    ///
    /// Unnamed integer flags can only be positive without a leading +.
    FSHIP_INT,
    /// @brief Float flag.
    /// @details
    /// To set flag:    -flag:\%f or -flag \%f
    ///
    /// To ignore flag: +flag:\%f or +flag \%f
    ///
    /// Where \%f is any positive/negative number.
    ///
    /// Float flags can be unnamed, provided that there are no
    /// other integer or float unnamed flags in a given context.
    ///
    /// Unnamed float flags can only be positive without a leading +.
    FSHIP_FLT,
    /// @brief String flag.
    /// @details
    /// To set flag:    -flag:\%s or -flag \%s
    ///
    /// To ignore flag: +flag:\%s or +flag \%s
    ///
    /// Where \%s is any string.
    ///
    /// String flags can be unnamed, provided that there are no
    /// other string unnamed flags in a given context.
    ///
    /// Unnamed string flags cannot start with + or -.
    FSHIP_STR,
} FShipType;

struct __FShipArg {
    FShipType type;

    size_t name;
    size_t description;
    size_t note;
    size_t warning;

    struct {
        size_t  len;
        size_t* ptr;
    } aliases;

    bool is_required;
    bool is_terminating;
    bool is_set;
    bool is_found;
    bool is_valid;
    bool has_default;

    union {
        struct {
            bool is_flipped;
            bool is_toggle;

            bool value;
        } boolean;
        struct {
            int64_t min;
            int64_t max;

            int64_t value;
        } integer;
        struct {
            double min;
            double max;

            double value;
        } flt;
        struct {
            struct {
                size_t  len;
                size_t* ptr;
            } valid;

            size_t value;
        } str;
    };
};

struct __FShipMode {
    size_t             cap;
    size_t             len;
    struct __FShipArg* ptr;

    size_t name;
    size_t description;

    bool is_terminating;
};

struct __FShipResult {
    FShipType type;

    const char* name;

    bool is_found;
    bool is_valid;

    union {
        bool        boolean;
        int64_t     integer;
        double      flt;
        const char* str;
    };
};

/// @brief Flagship context.
/// @warning Do not modify context directly.
typedef struct {
    struct {
        size_t cap;
        size_t len;
        char*  ptr;
    } string;

    size_t program_name;
    size_t description;

    struct {
        size_t              cap;
        size_t              len;
        struct __FShipMode* ptr;
    } modes;

    struct {
        size_t cap;
        size_t len;

        struct __FShipResult* ptr;

        size_t      mode_index;
        const char* mode_name;
    } result;

    size_t current_mode;
} FShipContext;

/// @brief Settings for flags.
struct FShipSettings {
    /// @brief Description for flag.
    const char* description;
    /// @brief Note for flag.
    const char* note;
    /// @brief Warning for flag.
    const char* warning;
    /// @brief Default value for flag.
    const char* default_value;

    /// @brief Array of string aliases for flag.
    /// @details
    /// Must always end with a NULL pointer.
    /// Use fls_strings() to create a correct array of strings.
    const char** aliases;

    /// @brief If flag is required.
    /// @details
    /// It's only required for the current mode.
    /// If it's a flag that appears in all modes and is
    /// always required, redefine the flag for each mode.
    bool is_required;
    /// @brief If parsing should stop after this flag is parsed.
    bool is_terminating;

    /// @brief Union of settings for different types.
    union {
        /// @brief Boolean flag settings.
        struct {
            /// @brief If flag should start true and setting it sets it to false.
            bool is_flipped;
            /// @brief If flag should toggle every time it is encountered.
            bool is_toggle;
        } boolean;
        /// @brief Integer flag settings.
        struct {
            /// @brief Minimum range for integer, inclusive.
            /// @note If min == max, range is full int64 range.
            int64_t min;
            /// @brief Maximum range for integer, exclusive.
            /// @note If min == max, range is full int64 range.
            int64_t max;
        } integer;
        /// @brief Float flag settings.
        struct {
            /// @brief Minimum range for float, inclusive.
            /// @note If min == max, range is full double range.
            double min;
            /// @brief Maximum range for float, exclusive.
            /// @note If min == max, range is full double range.
            double max;
        } flt;
        /// @brief String flag settings.
        struct {
            /// @brief List of strings that are valid.
            /// @details
            /// Must always end with a NULL pointer.
            /// Use fls_strings() to create a correct array of strings.
            const char** valid;
        } str;
    };
};

/// @brief Settings for reading flags.
struct FShipReadOut {
    /// @brief If flag name is found at all.
    bool* is_found;
    /// @brief If flag was set by user.
    bool* is_set;
    /// @brief If flag value is valid according to settings.
    bool* is_valid;
    /// @brief If flag type matches with read type.
    bool* is_type_correct;
    /// @brief If flag type does not match but was successfully cast.
    bool* is_type_cast;
};

/// @brief Create a list of strings.
/// @details Appends NULL pointer to end of list.
/// @param ... String literals.
#define fls_strings( ... ) \
(const char*[]){ __VA_ARGS__ __VA_OPT__(,) NULL }

/// @brief Get name of type.
/// @param type Type to get name of.
/// @return Name of type.
FSHIP_INL
const char* fls_type_name( FShipType type );
/// @brief Get format specifier of type.
/// @param type Type to get name of.
/// @return Format specifier of type.
FSHIP_INL
const char* fls_type_fmt( FShipType type );

/// @brief Set name of program. This is usually argv[0].
/// @param[in] ctx          Pointer to the context.
/// @param[in] program_name Program name string.
///                           The context copies strings to its internal buffer.
FSHIP_INL
void fls_set_program_name( FShipContext* ctx, const char* program_name );
FSHIP_INL FSHIP_FMT_FN(2,3)
void fls_set_description( FShipContext* ctx, const char* description, ... );

/// @brief Begin new mode.
/// @details
/// If mode already exists, switches to existing mode.
///
/// Modes are set when the first argument is the name of a given mode.
/// For example:
///
/// ./program mode_name [args...]
///
/// @warning
/// Mode name cannot start with - or +
///
/// @param[in] ctx  Pointer to the context.
/// @param[in] name Name of mode.
///                   The context copies strings to its internal buffer.
FSHIP_INL
void fls_mode_begin( FShipContext* ctx, const char* name );
/// @brief End current mode and return to default mode.
/// @param[in] ctx Pointer to the context.
FSHIP_INL
void fls_mode_end( FShipContext* ctx );
FSHIP_INL FSHIP_FMT_FN(2,3)
void fls_mode_set_description( FShipContext* ctx, const char* description, ... );
/// @brief Set if current mode should terminate parsing immediately.
/// @param[in] ctx Pointer to the context.
FSHIP_INL
void fls_mode_set_terminating( FShipContext* ctx );

/// @brief Add a boolean flag to the current mode.
/// @warning
/// Boolean flags cannot be unnamed.
/// @param[in] ctx  Pointer to the context.
/// @param[in] name Name of flag. This is the primary name and used to search flags when reading.
/// @param     ...  Flag settings. See FShipSettings.
#define fls_add_flag( ctx, name, ... ) \
    __fls_add_flag( ctx, name, (struct FShipSettings){ __VA_ARGS__ } )
/// @brief Add an integer flag to the current mode.
/// @details
/// If name is NULL, creates an unnamed flag.
/// @warning
/// Only one integer/float flag can be unnamed in a given mode.
/// @param[in] ctx  Pointer to the context.
/// @param[in] name Name of flag. This is the primary name and used to search flags when reading.
/// @param     ...  Flag settings. See FShipSettings.
#define fls_add_int( ctx, name, ... ) \
    __fls_add_int( ctx, name, (struct FShipSettings){ __VA_ARGS__ } )
/// @brief Add a float flag to the current mode.
/// @details
/// If name is NULL, creates an unnamed flag.
/// @warning
/// Only one integer/float flag can be unnamed in a given mode.
/// @param[in] ctx  Pointer to the context.
/// @param[in] name Name of flag. This is the primary name and used to search flags when reading.
/// @param     ...  Flag settings. See FShipSettings.
#define fls_add_flt( ctx, name, ... ) \
    __fls_add_flt( ctx, name, (struct FShipSettings){ __VA_ARGS__ } )
/// @brief Add a string flag to the current mode.
/// @details
/// If name is NULL, creates an unnamed flag.
/// @warning
/// Only one string flag can be unnamed in a given mode.
/// @param[in] ctx  Pointer to the context.
/// @param[in] name Name of flag. This is the primary name and used to search flags when reading.
/// @param     ...  Flag settings. See FShipSettings.
#define fls_add_str( ctx, name, ... ) \
    __fls_add_str( ctx, name, (struct FShipSettings){ __VA_ARGS__ } )

/// @brief Print help message for context and mode.
/// @param[in] ctx            Pointer to the context.
/// @param[in] opt_mode       (optional) Name of mode to print help for.
///                             Can be NULL, in which case the default mode's
///                             help message is printed.
/// @param     opt_show_modes (optional) Show all modes and their descriptions.
FSHIP_INL
void fls_help( FShipContext* ctx, const char* opt_mode, bool opt_show_modes );

/// @brief Parse arguments.
/// @details
/// Do not skip first argument, usually name of the program.
/// @param[in]  ctx              Pointer to the context.
/// @param      argc             Number of arguments. Obtained from main function.
/// @param[in]  argv             Pointer to arguments. Obtained from main function.
/// @param[out] opt_out_last_arg (optional) Pointer to write index of last argument read.
/// @return
///     - @c true  : Arguments were parsed successfully.
///     - @c false : Failed to parse arguments.
FSHIP_INL
bool fls_parse( FShipContext* ctx, int argc, char** argv, int* opt_out_last_arg );

/// @brief Query current mode, after parsing.
/// @warning
/// Only call this function after fls_parse()!
/// @param[in] ctx Pointer to the context.
/// @return Name of the current mode. If NULL, current mode is the default mode.
FSHIP_INL
const char* fls_query_mode( FShipContext* ctx );

/// @brief Read boolean flag.
/// @warning
/// Only call this function after fls_parse()!
/// @param[in] ctx  Pointer to the context.
/// @param[in] name Name of flag to read.
/// @param     ...  Read settings. See FShipReadOut.
/// @return Boolean flag result.
#define fls_read_flag( ctx, name, ... ) \
    __fls_read_flag( ctx, name, (struct FShipReadOut){ __VA_ARGS__ } )
/// @brief Read integer flag.
/// @warning
/// Only call this function after fls_parse()!
/// @param[in] ctx  Pointer to the context.
/// @param[in] name Name of flag to read. Can be NULL for unnamed flag.
/// @param     ...  Read settings. See FShipReadOut.
/// @return Integer flag result.
#define fls_read_int( ctx, name, ... ) \
    __fls_read_int( ctx, name, (struct FShipReadOut){ __VA_ARGS__ } )
/// @brief Read float flag.
/// @warning
/// Only call this function after fls_parse()!
/// @param[in] ctx  Pointer to the context.
/// @param[in] name Name of flag to read. Can be NULL for unnamed flag.
/// @param     ...  Read settings. See FShipReadOut.
/// @return Float flag result.
#define fls_read_flt( ctx, name, ... ) \
    __fls_read_flt( ctx, name, (struct FShipReadOut){ __VA_ARGS__ } )
/// @brief Read string flag.
/// @warning
/// Only call this function after fls_parse()!
/// @param[in] ctx  Pointer to the context.
/// @param[in] name Name of flag to read. Can be NULL for unnamed flag.
/// @param     ...  Read settings. See FShipReadOut.
/// @return
/// String flag result.
///
/// Returned string is never NULL.
///
/// Use @c .is_set to check if flag was set.
/// Use @c .is_found to check if flag exists in the context at all.
#define fls_read_str( ctx, name, ... ) \
    __fls_read_str( ctx, name, (struct FShipReadOut){ __VA_ARGS__ } )

/// @brief Free context.
/// @param[in] ctx Pointer to the context.
FSHIP_INL
void fls_free( FShipContext* ctx );

FSHIP_INL
void __fls_add_flag( FShipContext* ctx, const char* name, struct FShipSettings settings );
FSHIP_INL
void __fls_add_int( FShipContext* ctx, const char* name, struct FShipSettings settings );
FSHIP_INL
void __fls_add_flt( FShipContext* ctx, const char* name, struct FShipSettings settings );
FSHIP_INL
void __fls_add_str( FShipContext* ctx, const char* name, struct FShipSettings settings );

FSHIP_INL
bool        __fls_read_flag( FShipContext* ctx, const char* name, struct FShipReadOut out );
FSHIP_INL
int64_t     __fls_read_int( FShipContext* ctx, const char* name, struct FShipReadOut out );
FSHIP_INL
double      __fls_read_flt( FShipContext* ctx, const char* name, struct FShipReadOut out );
FSHIP_INL
const char* __fls_read_str( FShipContext* ctx, const char* name, struct FShipReadOut out );

// NOTE(alicia): implementation -----------------------------------------------

FSHIP_INL
void __fls_warn( const char* fmt, ... ) {
    FSHIP_STDERR_FMT( FSHIP_COLOR_YELLOW );

    va_list va;
    va_start( va, fmt );
    FSHIP_STDERR_FMT_VA( fmt, va );
    va_end( va );

    FSHIP_STDERR_FMT( FSHIP_COLOR_RESET "\n" );
}

FSHIP_INL
void __fls_err( const char* fmt, ... ) {
    FSHIP_STDERR_FMT( FSHIP_COLOR_RED );

    va_list va;
    va_start( va, fmt );
    FSHIP_STDERR_FMT_VA( fmt, va );
    va_end( va );

    FSHIP_STDERR_FMT( FSHIP_COLOR_RESET "\n" );
}

FSHIP_INL
void __fls_str_init( FShipContext* ctx, size_t initial ) {
    // NOTE(alicia): this is so that 0 offsets are always NULL
    ctx->string.len = sizeof(int);
    initial += sizeof(int);

    ctx->string.cap = initial;
    ctx->string.ptr = (char*)FSHIP_ALLOC( NULL, 0, initial );
}

FSHIP_INL
void __fls_str_reserve( FShipContext* ctx, size_t required ) {
    if( !ctx->string.ptr ) {
        __fls_str_init( ctx, required + 16 );
        return;
    }

    if( (ctx->string.cap - ctx->string.len) >= required ) {
        return;
    }

    size_t remaining = ctx->string.cap - ctx->string.len;

    size_t allocate_size = (required - remaining) + 16;

    size_t old_size = ctx->string.cap;
    size_t new_size = ctx->string.cap + allocate_size;

    char* new_ptr = (char*)FSHIP_ALLOC( ctx->string.ptr, old_size, new_size );

    ctx->string.cap = new_size;
    ctx->string.ptr = new_ptr;
}

FSHIP_INL FSHIP_FMT_FN(2,0)
size_t __fls_str_va( FShipContext* ctx, const char* fmt, va_list va ) {
    va_list va2;
    va_copy( va2, va );

    size_t len = FSHIP_COUNT_FMT_VA( fmt, va2 );

    va_end( va2 );

    __fls_str_reserve( ctx, len + 1 );

    size_t result = ctx->string.len;

    FSHIP_FMT_VA(
        ctx->string.ptr + ctx->string.len,
        ctx->string.cap - ctx->string.len,
        fmt, va );

    ctx->string.len += len + 1;

    return result;
}
FSHIP_INL FSHIP_FMT_FN(2,3)
size_t __fls_str( FShipContext* ctx, const char* fmt, ... ) {
    va_list va;
    va_start( va, fmt );

    size_t result = __fls_str_va( ctx, fmt, va );

    va_end( va );
    return result;
}

FSHIP_INL
void fls_set_program_name( FShipContext* ctx, const char* program_name ) {
    ctx->program_name = __fls_str( ctx, "%s", program_name );
}
/// @brief Set description of program.
/// @param[in] ctx         Pointer to the context.
/// @param[in] description Description of program.
///                          This is a format string.
/// @param     ...         Arguments for description format string.
FSHIP_INL
void fls_set_description( FShipContext* ctx, const char* description, ... ) {
    va_list va;
    va_start( va, description );
    ctx->description = __fls_str_va( ctx, description, va );
    va_end( va );
}

FSHIP_INL
void fls_free( FShipContext* ctx ) {
    if( !ctx ) {
        return;
    }

    if( ctx->modes.ptr ) {
        for( size_t i = 0; i < ctx->modes.len; ++i ) {
            struct __FShipMode* mode = ctx->modes.ptr;
            if( mode->ptr ) {
                for( size_t j = 0; j < mode->len; ++j ) {
                    struct __FShipArg* arg = mode->ptr + j;

                    if( arg->aliases.ptr ) {
                        FSHIP_FREE( arg->aliases.ptr, sizeof(size_t) * arg->aliases.len );
                    }

                    if( arg->type == FSHIP_STR ) {
                        if( arg->str.valid.ptr ) {
                            FSHIP_FREE( arg->str.valid.ptr, sizeof(size_t) * arg->str.valid.len );
                        }
                    }
                }

                FSHIP_FREE( mode->ptr, sizeof(struct __FShipArg) * mode->cap );
            }
        }
        FSHIP_FREE( ctx->modes.ptr, sizeof(struct __FShipMode) * ctx->modes.cap );
    }

    FSHIP_FREE( ctx->string.ptr, ctx->string.cap );
    FSHIP_MEMSET( ctx, 0, sizeof(FShipContext) );
}

FSHIP_INL
void __fls_mode_init( FShipContext* ctx ) {
    ctx->modes.cap = 4;
    // NOTE(alicia): this is the default mode.
    ctx->modes.len = 1;
    ctx->modes.ptr =
        (struct __FShipMode*)FSHIP_ALLOC( NULL, 0, sizeof(struct __FShipMode) * ctx->modes.cap );
}

FSHIP_INL
void __fls_mode_reserve( FShipContext* ctx ) {
    if( ctx->modes.cap - ctx->modes.len ) {
        return;
    }

    size_t new_cap = ctx->modes.cap + 4;

    size_t old_size = sizeof(struct __FShipMode) * ctx->modes.cap;
    size_t new_size = sizeof(struct __FShipMode) * new_cap;

    struct __FShipMode* new_ptr =
        (struct __FShipMode*)FSHIP_ALLOC( ctx->modes.ptr, old_size, new_size );

    ctx->modes.cap = new_cap;
    ctx->modes.ptr = new_ptr;
}

FSHIP_INL
size_t __fls_mode_push( FShipContext* ctx, struct __FShipMode* in_mode ) {
    if( !ctx->modes.len ) {
        __fls_mode_init( ctx );
    }

    __fls_mode_reserve( ctx );

    size_t result = ctx->modes.len;

    ctx->modes.ptr[ctx->modes.len++] = *in_mode;

    return result;
}

FSHIP_INL
struct __FShipMode* __fls_search_mode( FShipContext* ctx, const char* name ) {
    if( !name ) {
        return ctx->modes.ptr;
    }

    for( size_t i = 0; i < ctx->modes.len; ++i ) {
        struct __FShipMode* mode = ctx->modes.ptr + i;

        if( !mode->name ) {
            continue;
        }

        const char* mode_name = ctx->string.ptr + mode->name;

        if( FSHIP_STRCMP( mode_name, name ) ) {
            return mode;
        }
    }

    return NULL;
}

FSHIP_INL
void fls_mode_begin( FShipContext* ctx, const char* name ) {
    if( !ctx->modes.ptr ) {
        __fls_mode_init( ctx );
    }

    if( name ) {
        for( size_t i = 0; i < ctx->modes.len; ++i ) {
            struct __FShipMode* current = ctx->modes.ptr + i;

            if( !current->name ) {
                continue;
            }

            const char* current_name = ctx->string.ptr + current->name;

            if( FSHIP_STRCMP( current_name, name ) ) {
                ctx->current_mode = i;
                return;
            }
        }
    } else {
        for( size_t i = 0; i < ctx->modes.len; ++i ) {
            struct __FShipMode* current = ctx->modes.ptr + i;

            if( current->name ) {
                continue;
            }

            ctx->current_mode = i;
            return;
        }
    }

    struct __FShipMode mode = {};

    mode.name = __fls_str( ctx, "%s", name );

    ctx->current_mode = __fls_mode_push( ctx, &mode );
}

FSHIP_INL
void fls_mode_end( FShipContext* ctx ) {
    ctx->current_mode = 0;
}

/// @brief Set description for the current mode.
/// @param[in] ctx         Pointer to the context.
/// @param[in] description Description for the current mode.
///                          This is a format string.
/// @param     ...         Arguments for description format string.
FSHIP_INL
void fls_mode_set_description( FShipContext* ctx, const char* description, ... ) {
    struct __FShipMode* mode = ctx->modes.ptr + ctx->current_mode;

    va_list va;
    va_start( va, description );

    mode->description = __fls_str_va( ctx, description, va );

    va_end( va );
}

FSHIP_INL
void fls_mode_set_terminating( FShipContext* ctx ) {
    struct __FShipMode* mode = ctx->modes.ptr + ctx->current_mode;

    mode->is_terminating = !mode->is_terminating;
}

FSHIP_INL
struct __FShipMode* __fls_get_mode( FShipContext* ctx ) {
    if( !ctx->modes.ptr ) {
        __fls_mode_init( ctx );
    }

    struct __FShipMode* mode = ctx->modes.ptr + ctx->current_mode;

    return mode;
}

FSHIP_INL
void __fls_arg_reserve( struct __FShipMode* mode ) {
    if( mode->cap - mode->len ) {
        return;
    }

    size_t new_cap = mode->cap + 4;

    size_t old_size = sizeof(struct __FShipArg) * mode->cap;
    size_t new_size = sizeof(struct __FShipArg) * new_cap;

    struct __FShipArg* new_ptr =
        (struct __FShipArg*)FSHIP_ALLOC( mode->ptr, old_size, new_size );

    mode->cap = new_cap;
    mode->ptr = new_ptr;
}

FSHIP_INL
void __fls_push_arg( struct __FShipMode* mode, struct __FShipArg* arg ) {
    __fls_arg_reserve( mode );

    mode->ptr[mode->len++] = *arg;
}

FSHIP_INL
struct __FShipArg* __fls_arg_search(
    FShipContext* ctx, struct __FShipMode* mode, const char* name
) {
    #define SEARCH_FLAG        0
    #define SEARCH_UNNAMED_INT 1
    #define SEARCH_UNNAMED_FLT 2
    #define SEARCH_UNNAMED_STR 3

    int search_type = SEARCH_FLAG;

    size_t name_len = FSHIP_STRLEN( name );

    if( name[0] != '-' ) {
        bool has_non_alphanumeric = false;
        bool has_dot              = false;

        for( size_t i = 0; i < name_len; ++i ) {
            switch( name[i] ) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': break;
                case '.': {
                    if( has_dot ) {
                        has_non_alphanumeric = true;
                    } else {
                        has_dot = true;
                    }
                } break;
                default: {
                    has_non_alphanumeric = true;
                } break;
            }

            if( has_non_alphanumeric ) {
                break;
            }
        }

        if( has_non_alphanumeric ) {
            search_type = SEARCH_UNNAMED_STR;
        } else {
            if( has_dot ) {
                search_type = SEARCH_UNNAMED_FLT;
            } else {
                search_type = SEARCH_UNNAMED_INT;
            }
        }
    }

    switch( search_type ) {
        case SEARCH_FLAG: {
            const char* name_start = name + 1;
            name_len--; {
                const char* colon = FSHIP_STRCHR( name_start, ':' );
                if( colon ) {
                    name_len = (size_t)colon - (size_t)name_start;
                }
            }


            for( size_t i = 0; i < mode->len; ++i ) {
                struct __FShipArg* arg = mode->ptr + i;

                if( !arg->name ) {
                    continue;
                }

                const char* current_name     = ctx->string.ptr + arg->name;
                size_t      current_name_len = FSHIP_STRLEN( current_name );

                if( current_name_len == name_len ) {
                    if( FSHIP_MEMCMP( current_name, name_start, name_len ) ) {
                        return arg;
                    }
                }

                for( size_t j = 0; j < arg->aliases.len; ++j ) {
                    current_name     = ctx->string.ptr + arg->aliases.ptr[j];
                    current_name_len = FSHIP_STRLEN( current_name );

                    if( current_name_len == name_len ) {
                        if( FSHIP_MEMCMP( current_name, name_start, name_len ) ) {
                            return arg;
                        }
                    }
                }
            }
        } break;
        case SEARCH_UNNAMED_INT: {
            for( size_t i = 0; i < mode->len; ++i ) {
                struct __FShipArg* arg = mode->ptr + i;

                if( arg->name ) {
                    continue;
                }

                if( arg->type != FSHIP_INT ) {
                    continue;
                }

                return arg;
            }
        } break;
        case SEARCH_UNNAMED_FLT: {
            for( size_t i = 0; i < mode->len; ++i ) {
                struct __FShipArg* arg = mode->ptr + i;

                if( arg->name ) {
                    continue;
                }

                if( arg->type != FSHIP_FLT ) {
                    continue;
                }

                return arg;
            }
        } break;
        case SEARCH_UNNAMED_STR: {
            for( size_t i = 0; i < mode->len; ++i ) {
                struct __FShipArg* arg = mode->ptr + i;

                if( arg->name ) {
                    continue;
                }

                if( arg->type != FSHIP_STR ) {
                    continue;
                }

                return arg;
            }
        } break;
    }

    return NULL;

    #undef SEARCH_FLAG
    #undef SEARCH_UNNAMED_INT
    #undef SEARCH_UNNAMED_FLT
    #undef SEARCH_UNNAMED_STR
}

FSHIP_INL
void __fls_init_arg( FShipContext* ctx, struct __FShipArg* arg, struct FShipSettings* settings ) {
    if( settings->description ) {
        arg->description = __fls_str( ctx, "%s", settings->description );
    }
    if( settings->note ) {
        arg->note = __fls_str( ctx, "%s", settings->note );
    }
    if( settings->warning ) {
        arg->warning = __fls_str( ctx, "%s", settings->warning );
    }
    size_t alias_count = 0; {
        const char** aliases = settings->aliases;
        for( ;; ) {
            if( !aliases || !*aliases ) {
                break;
            }

            aliases++;
            alias_count++;
        }
    }

    if( alias_count ) {
        arg->aliases.len = alias_count;
        arg->aliases.ptr = (size_t*)FSHIP_ALLOC( NULL, 0, sizeof(size_t) * arg->aliases.len );

        size_t at = 0;

        const char** aliases = settings->aliases;
        for( ;; ) {
            if( !*aliases ) {
                break;
            }

            arg->aliases.ptr[at++] = __fls_str( ctx, "%s", *aliases );

            aliases++;
        }
    }

    arg->is_required    = settings->is_required;
    arg->is_terminating = settings->is_terminating;
}

FSHIP_INL
void __fls_add_flag( FShipContext* ctx, const char* name, struct FShipSettings settings ) {
    struct __FShipMode* mode = __fls_get_mode( ctx );

    struct __FShipArg arg = {};
    arg.type = FSHIP_BOOL;

    if( !name ) {
        FSHIP_STDERR_FMT( "Failed to add null boolean flag! Boolean flags require a name!\n" );
        return;
    }

    arg.name = __fls_str( ctx, "%s", name );

    __fls_init_arg( ctx, &arg, &settings );

    if( settings.default_value ) {
        arg.has_default = true;
        if( FSHIP_STRCMP( settings.default_value, "true" ) ) {
            arg.boolean.value = true;
        } else if( FSHIP_STRCMP( settings.default_value, "false" ) ) {
            arg.boolean.value = false;
        }
    }

    arg.boolean.is_flipped = settings.boolean.is_flipped;
    arg.boolean.is_toggle  = settings.boolean.is_toggle;

    __fls_push_arg( mode, &arg );
}
FSHIP_INL
void __fls_add_int( FShipContext* ctx, const char* name, struct FShipSettings settings ) {
    struct __FShipMode* mode = __fls_get_mode( ctx );

    struct __FShipArg arg = {};
    arg.type = FSHIP_INT;

    if( name ) {
        arg.name = __fls_str( ctx, "%s", name );
    } else {
        bool already_has_nameless_number = false;
        for( size_t i = 0; i < mode->len; ++i ) {
            struct __FShipArg* current = mode->ptr + i;

            if(
                !current->name &&
                (current->type == FSHIP_INT || current->type == FSHIP_FLT)
            ) {
                already_has_nameless_number = true;
                break;
            }
        }

        if( already_has_nameless_number ) {
            FSHIP_STDERR_FMT(
                "Failed to add nameless number flag! This mode already has another one!\n" );
            return;
        }
    }

    __fls_init_arg( ctx, &arg, &settings );

    if( settings.default_value ) {
        int64_t value = 0;
        if( FSHIP_INT_FROM_STR( settings.default_value, &value ) ) {
            arg.has_default   = true;
            arg.integer.value = value;
        } else {
            FSHIP_STDERR_FMT(
                "Failed to parse integer '%s' for %s",
                settings.default_value, name ? name : "nameless integer" );
        }
    }

    arg.integer.min = settings.integer.min;
    arg.integer.max = settings.integer.max;

    __fls_push_arg( mode, &arg );
}
FSHIP_INL
void __fls_add_flt( FShipContext* ctx, const char* name, struct FShipSettings settings ) {
    struct __FShipMode* mode = __fls_get_mode( ctx );

    struct __FShipArg arg = {};
    arg.type = FSHIP_FLT;

    if( name ) {
        arg.name = __fls_str( ctx, "%s", name );
    } else {
        bool already_has_nameless_number = false;
        for( size_t i = 0; i < mode->len; ++i ) {
            struct __FShipArg* current = mode->ptr + i;

            if(
                !current->name &&
                (current->type == FSHIP_INT || current->type == FSHIP_FLT)
            ) {
                already_has_nameless_number = true;
                break;
            }
        }

        if( already_has_nameless_number ) {
            FSHIP_STDERR_FMT(
                "Failed to add nameless number flag! This mode already has another one!\n" );
            return;
        }
    }

    __fls_init_arg( ctx, &arg, &settings );

    if( settings.default_value ) {
        double value = 0.0;
        if( FSHIP_DBL_FROM_STR( settings.default_value, &value ) ) {
            arg.has_default = true;
            arg.flt.value   = value;
        } else {
            FSHIP_STDERR_FMT(
                "Failed to parse float '%s' for %s",
                settings.default_value, name ? name : "nameless float" );
        }
    }

    arg.flt.min = settings.flt.min;
    arg.flt.max = settings.flt.max;

    __fls_push_arg( mode, &arg );

}
FSHIP_INL
void __fls_add_str( FShipContext* ctx, const char* name, struct FShipSettings settings ) {
    struct __FShipMode* mode = __fls_get_mode( ctx );

    struct __FShipArg arg = {};
    arg.type = FSHIP_STR;

    if( name ) {
        arg.name = __fls_str( ctx, "%s", name );
    } else {
        bool already_has_nameless_string = false;
        for( size_t i = 0; i < mode->len; ++i ) {
            struct __FShipArg* current = mode->ptr + i;

            if(
                !current->name &&
                current->type == FSHIP_STR
            ) {
                already_has_nameless_string = true;
                break;
            }
        }

        if( already_has_nameless_string ) {
            FSHIP_STDERR_FMT(
                "Failed to add nameless string flag! This mode already has another one!\n" );
            return;
        }
    }

    __fls_init_arg( ctx, &arg, &settings );

    if( settings.default_value ) {
        arg.str.value   = __fls_str( ctx, "%s", settings.default_value );
        arg.has_default = true;
    }

    size_t valid_count = 0;
    const char** valid = settings.str.valid;
    for( ;; ) {
        if( !valid || !*valid ) {
            break;
        }

        valid++;
        valid_count++;
    }

    if( valid_count ) {
        arg.str.valid.len = valid_count;
        arg.str.valid.ptr = (size_t*)FSHIP_ALLOC( NULL, 0, sizeof(size_t) * valid_count );

        size_t at = 0;

        valid = settings.str.valid;
        for( ;; ) {
            if( !*valid ) {
                break;
            }

            arg.str.valid.ptr[at++] = __fls_str( ctx, "%s", *valid );

            valid++;
        }
    }

    __fls_push_arg( mode, &arg );
}

FSHIP_INL
size_t __fls_print_spaces( size_t count ) {
    for( size_t i = 0; i < count; ++i ) {
        putchar( ' ' );
    }
    return count;
}

FSHIP_INL
void fls_help( FShipContext* ctx, const char* opt_mode, bool opt_show_modes ) {
    const char* program_name = "<command>";
    if( ctx->program_name ) {
        program_name = ctx->string.ptr + ctx->program_name;
    }
    const char* overview = NULL;
    if( ctx->description ) {
        overview = ctx->string.ptr + ctx->description;
    }

    // NOTE(alicia): search mode handles NULL pointer
    struct __FShipMode* mode = __fls_search_mode( ctx, opt_mode );

    if( overview ) {
        printf( "OVERVIEW:    %s\n", overview );
    }

    bool is_modal = ctx->modes.len > 1;

    if( is_modal ) {
        if( mode ) {
            if( mode->name ) {
                printf( "USAGE:       %s %s [args...]\n",
                    program_name, ctx->string.ptr + mode->name );
            } else {
                printf( "USAGE:       %s [args...]\n", program_name );
            }
        } else {
            printf( "USAGE:       %s [args...]\n", program_name );
        }
    } else {
        printf( "USAGE:       %s [args...]\n", program_name );
    }

    bool has_to_show_modes = (!mode || !mode->name) || opt_show_modes;

    if( is_modal && has_to_show_modes ) {
        printf( "MODES:       \n" );

        size_t max_mode_len = 0;
        for( size_t i = 0; i < ctx->modes.len; ++i ) {
            struct __FShipMode* current = ctx->modes.ptr + i;

            if( !current->name ) {
                continue;
            }

            size_t mode_len = FSHIP_COUNT_FMT( "  %s", ctx->string.ptr + current->name );

            if( mode_len > max_mode_len ) {
                max_mode_len = mode_len;
            }
        }

        for( size_t i = 0; i < ctx->modes.len; ++i ) {
            struct __FShipMode* current = ctx->modes.ptr + i;

            if( !current->name ) {
                continue;
            }

            size_t mode_len = printf( "  %s", ctx->string.ptr + current->name );
            if( !current->description ) {
                printf( "\n" );
                continue;
            }

            if( mode_len < max_mode_len ) {
                __fls_print_spaces( max_mode_len - mode_len );
            }

            printf( "  %s\n", ctx->string.ptr + current->description );
        }
    }

    if( mode && mode->description ) {
        printf( "DESCRIPTION: \n  %s\n", ctx->string.ptr + mode->description );
    }

    if( !mode || !mode->len ) {
        return;
    }

    printf( "ARGUMENTS:\n" );

    size_t arg_max_len = 0;
    for( size_t i = 0; i < mode->len; ++i ) {
        struct __FShipArg* arg = mode->ptr + i;

        const char* type_name = fls_type_fmt( arg->type );

        size_t arg_len = 0;
        if( arg->name ) {
            const char* arg_name = ctx->string.ptr + arg->name;

            if( arg->type == FSHIP_BOOL ) {
                arg_len = FSHIP_COUNT_FMT( "  -%s", arg_name );
            } else {
                arg_len = FSHIP_COUNT_FMT(
                    "  -%s <%s>, -%s:<%s>",
                    arg_name, type_name, arg_name, type_name );
            }

            if( arg->aliases.len ) {
                // NOTE(alicia): for trailing comma
                arg_len++;
            }

            for( size_t j = 0; j < arg->aliases.len; ++j ) {
                if( !arg->aliases.ptr[j] ) {
                    continue;
                }
                const char* alias = ctx->string.ptr + arg->aliases.ptr[j];

                size_t alias_len = 0;

                if( arg->type == FSHIP_BOOL ) {
                    alias_len = FSHIP_COUNT_FMT( "  -%s", alias );
                } else {
                    alias_len = FSHIP_COUNT_FMT(
                        "  -%s <%s>, -%s:<%s>",
                        alias, type_name, alias, type_name );
                }

                if( (j + 1) < arg->aliases.len ) {
                    // NOTE(alicia): for trailing comma
                    alias_len++;
                }

                if( arg_len < alias_len ) {
                    arg_len = alias_len;
                }
            }
        } else {
            arg_len = FSHIP_COUNT_FMT( "  <%s>", type_name );
        }

        if( arg_len > arg_max_len ) {
            arg_max_len = arg_len;
        }
    }


    for( size_t i = 0; i < mode->len; ++i ) {
        struct __FShipArg* arg = mode->ptr + i;

        const char* type_name = fls_type_fmt( arg->type );

        size_t arg_len = 0;

        if( arg->name ) {
            const char* arg_name = ctx->string.ptr + arg->name;

            if( arg->type == FSHIP_BOOL ) {
                arg_len = printf( "  -%s", arg_name );
            } else {
                arg_len = printf(
                    "  -%s <%s>, -%s:<%s>",
                    arg_name, type_name, arg_name, type_name );
            }

            if( arg->aliases.len ) {
                arg_len++;
                printf( ",\n" );
            }

            for( size_t j = 0; j < arg->aliases.len; ++j ) {
                if( !arg->aliases.ptr[j] ) {
                    continue;
                }
                const char* alias = ctx->string.ptr + arg->aliases.ptr[j];

                size_t alias_len = 0;

                if( arg->type == FSHIP_BOOL ) {
                    alias_len = printf( "  -%s", alias );
                } else {
                    alias_len = printf(
                        "  -%s <%s>, -%s:<%s>",
                        alias, type_name, alias, type_name );
                }

                if( (j + 1) < arg->aliases.len ) {
                    printf( ",\n" );
                    alias_len++;
                }

                arg_len = alias_len;
            }
        } else {
            arg_len = printf( "  <%s>", type_name );
        }

        if( arg_len < arg_max_len ) {
            __fls_print_spaces( arg_max_len - arg_len );
        }

        const char* description = "";
        if( arg->description ) {
            description = ctx->string.ptr + arg->description;
        }
        printf( "  %s\n", description );

        #define TEXT_NOTE        "note"
        #define TEXT_WARNING     "warning"
        #define TEXT_DEFAULT     "default"
        #define TEXT_REQUIRED    "note"
        #define TEXT_VALID       "valid"
        #define TEXT_RANGE       "range"
        #define TEXT_TERMINATING "note"

        const char* note           = arg->note ? ctx->string.ptr + arg->note : NULL;
        const char* warning        = arg->warning ? ctx->string.ptr + arg->warning : NULL;
        bool        has_default    = arg->has_default;
        bool        is_required    = arg->is_required;
        bool        is_terminating = arg->is_terminating;
        bool        has_valid      = false;
        bool        has_range      = false;

        switch( arg->type ) {
            case FSHIP_BOOL: break;
            case FSHIP_INT: {
                if( arg->integer.min != arg->integer.max ) {
                    has_range = true;
                }
            } break;
            case FSHIP_FLT: {
                if( arg->flt.min != arg->flt.max ) {
                    has_range = true;
                }
            } break;
            case FSHIP_STR: {
                if( arg->str.valid.len ) {
                    has_valid = true;
                }
            } break;
        }

        bool has_addendum =
            note    != NULL ||
            warning != NULL ||
            has_default     ||
            is_required     ||
            is_terminating  ||
            has_valid       ||
            has_range;

        size_t max_addendum = 0;
        if( note ) {
            if( max_addendum < sizeof(TEXT_NOTE) ) {
                max_addendum = sizeof(TEXT_NOTE);
            }
        }
        if( warning ) {
            if( max_addendum < sizeof(TEXT_WARNING) ) {
                max_addendum = sizeof(TEXT_WARNING);
            }
        }
        if( has_default ) {
            if( max_addendum < sizeof(TEXT_DEFAULT) ) {
                max_addendum = sizeof(TEXT_DEFAULT);
            }
        }
        if( is_required ) {
            if( max_addendum < sizeof(TEXT_REQUIRED) ) {
                max_addendum = sizeof(TEXT_REQUIRED);
            }
        }
        if( is_terminating ) {
            if( max_addendum < sizeof(TEXT_TERMINATING) ) {
                max_addendum = sizeof(TEXT_TERMINATING);
            }
        }
        if( has_valid ) {
            if( max_addendum < sizeof(TEXT_VALID) ) {
                max_addendum = sizeof(TEXT_VALID);
            }
        }
        if( has_range ) {
            if( max_addendum < sizeof(TEXT_RANGE) ) {
                max_addendum = sizeof(TEXT_RANGE);
            }
        }

        if( warning ) {
            __fls_print_spaces( arg_max_len );
            printf( "    " );
            size_t addendum_size = printf( "%s:", TEXT_WARNING );
            if( addendum_size < max_addendum ) {
                __fls_print_spaces( max_addendum - addendum_size );
            }
            printf( "  %s\n", warning );
        }

        if( note ) {
            __fls_print_spaces( arg_max_len );
            printf( "    " );
            size_t addendum_size = printf( "%s:", TEXT_NOTE );
            if( addendum_size < max_addendum ) {
                __fls_print_spaces( max_addendum - addendum_size );
            }
            printf( "  %s\n", note );
        }
        if( is_required ) {
            if( note ) {
                size_t addendum_size = sizeof(TEXT_NOTE);
                __fls_print_spaces( arg_max_len + 4 + sizeof(TEXT_NOTE) );
                if( addendum_size < max_addendum ) {
                    __fls_print_spaces( max_addendum - addendum_size );
                }
                printf( "  This flag is required.\n" );
            } else {
                __fls_print_spaces( arg_max_len );
                printf( "    " );
                size_t addendum_size = printf( "%s:", TEXT_NOTE );
                if( addendum_size < max_addendum ) {
                    __fls_print_spaces( max_addendum - addendum_size );
                }
                printf( "  This flag is required.\n" );
            }
        }
        if( is_terminating ) {
            if( note || is_required ) {
                size_t addendum_size = sizeof(TEXT_NOTE);
                __fls_print_spaces( arg_max_len + 4 + sizeof(TEXT_NOTE) );
                if( addendum_size < max_addendum ) {
                    __fls_print_spaces( max_addendum - addendum_size );
                }
                printf( "  Once this flag is parsed, stops parsing remaining flags.\n" );
            } else {
                __fls_print_spaces( arg_max_len );
                printf( "    " );
                size_t addendum_size = printf( "%s:", TEXT_NOTE );
                if( addendum_size < max_addendum ) {
                    __fls_print_spaces( max_addendum - addendum_size );
                }
                printf( "  Once this flag is parsed, stops parsing remaining flags.\n" );
            }
        }

        if( has_default ) {
            __fls_print_spaces( arg_max_len );
            printf( "    " );
            size_t addendum_size = printf( "%s:", TEXT_DEFAULT );
            if( addendum_size < max_addendum ) {
                __fls_print_spaces( max_addendum - addendum_size );
            }

            switch( arg->type ) {
                case FSHIP_BOOL : printf( "\n" ); break;
                case FSHIP_INT  : {
                    printf( "  %" PRId64 "\n", arg->integer.value );
                } break;
                case FSHIP_FLT  : {
                    printf( "  %f\n", arg->flt.value );
                } break;
                case FSHIP_STR  : {
                    printf( "  %s\n", ctx->string.ptr + arg->str.value );
                } break;
            }
        }

        switch( arg->type ) {
            case FSHIP_BOOL : break;
            case FSHIP_INT  : {
                if( has_range ) {
                    __fls_print_spaces( arg_max_len );
                    printf( "    " );
                    size_t addendum_size = printf( "%s:", TEXT_RANGE );
                    if( addendum_size < max_addendum ) {
                        __fls_print_spaces( max_addendum - addendum_size );
                    }

                    printf( "  [%" PRId64 ", %" PRId64 "]\n", arg->integer.min, arg->integer.max );
                }
            } break;
            case FSHIP_FLT  : {
                if( has_range ) {
                    __fls_print_spaces( arg_max_len );
                    printf( "    " );
                    size_t addendum_size = printf( "%s:", TEXT_RANGE );
                    if( addendum_size < max_addendum ) {
                        __fls_print_spaces( max_addendum - addendum_size );
                    }

                    printf( "  [%f, %f]\n", arg->flt.min, arg->flt.max );
                }
            } break;
            case FSHIP_STR  : {
                if( has_valid ) {
                    __fls_print_spaces( arg_max_len );
                    printf( "    " );
                    size_t addendum_size = printf( "%s:", TEXT_VALID );
                    if( addendum_size < max_addendum ) {
                        __fls_print_spaces( max_addendum - addendum_size );
                    }

                    printf( "  " );
                    for( size_t i = 0; i < arg->str.valid.len; ++i ) {
                        size_t offset = arg->str.valid.ptr[i];
                        if( !offset ) {
                            continue;
                        }

                        const char* valid = ctx->string.ptr + offset;

                        printf( "%s", valid );
                        if( (i + 1) < arg->str.valid.len ) {
                            printf( ", " );
                        }
                    }

                    printf( "\n" );
                }
            } break;
        }

        #undef TEXT_NOTE     
        #undef TEXT_WARNING  
        #undef TEXT_DEFAULT  
        #undef TEXT_REQUIRED 
        #undef TEXT_VALID    
        #undef TEXT_RANGE    
        #undef TEXT_TERMINATING

        printf( "\n" );
    }
}

FSHIP_INL
void __fls_result_set_mode( FShipContext* ctx, struct __FShipMode* mode ) {
    ctx->result.mode_index =
        (size_t)((char*)mode - (char*)ctx->modes.ptr) / sizeof(struct __FShipMode);
    ctx->result.mode_name =
        ctx->string.ptr + mode->name;
}

FSHIP_INL
void __fls_result_reserve( FShipContext* ctx ) {
    if( ctx->result.cap - ctx->result.len ) {
        return;
    }

    size_t new_cap = ctx->result.cap + 4;

    size_t old_size = sizeof(struct __FShipResult) * ctx->result.cap;
    size_t new_size = sizeof(struct __FShipResult) * new_cap;

    struct __FShipResult* new_ptr =
        (struct __FShipResult*)FSHIP_ALLOC( ctx->result.ptr, old_size, new_size );

    ctx->result.cap = new_cap;
    ctx->result.ptr = new_ptr;
}

FSHIP_INL
void __fls_result_push( FShipContext* ctx, struct __FShipResult* value ) {
    // TODO(alicia): some way to always push flag?
    // how do duplicates get read?
    bool   is_unique = true;
    size_t index     = 0;

    for( size_t i = 0; i < ctx->result.len; ++i ) {
        struct __FShipResult* current = ctx->result.ptr + i;
        index = i;

        if( value->name ) {
            if( current->name ) {
                if( FSHIP_STRCMP( value->name, current->name ) ) {
                    is_unique = false;
                    break;
                }
            } else {
                continue;
            }
        } else {
            if( current->name ) {
                continue;
            } else {
                if( value->type == current->type ) {
                    is_unique = false;
                    break;
                }
            }
        }
    }

    if( is_unique ) {
        __fls_result_reserve( ctx );

        ctx->result.ptr[ctx->result.len++] = *value;
    } else {
        ctx->result.ptr[index] = *value;
    }
}

FSHIP_INL
size_t __fls_mode_required_count( struct __FShipMode* mode ) {
    size_t result = 0;
    for( size_t i = 0; i < mode->len; ++i ) {
        struct __FShipArg* arg = mode->ptr + i;

        if( arg->is_required ) {
            result++;
        }
    }

    return result;
}

FSHIP_INL
bool fls_parse( FShipContext* ctx, int argc, char** argv, int* opt_out_last_arg ) {
    struct __FShipMode* mode = ctx->modes.ptr;

    int index = 0;
    index++;

    if( index == argc ) {
        size_t required_count = __fls_mode_required_count( mode );

        if( opt_out_last_arg ) {
            *opt_out_last_arg = index;
        }

        if( required_count ) {
            __fls_err( "required arguments were not provided!" );
            fls_help( ctx, NULL, false );
            return false;
        } else {
            return true;
        }
    }

    if( !(argv[index][0] == '-' || argv[index][0] == '+') && (ctx->modes.len > 1) ) {
        mode = __fls_search_mode( ctx, argv[index] );
        if( !mode ) {
            if( opt_out_last_arg ) {
                *opt_out_last_arg = index;
            }

            __fls_err( "unrecognized mode: %s", argv[index] );
            fls_help( ctx, NULL, false );
            return false;
        }

        if( !mode->is_terminating ) {
            index++;
        }
    }

    if( !mode ) {
        if( opt_out_last_arg ) {
            *opt_out_last_arg = index;
        }
        return true;
    }

    const char* mode_name = NULL;
    if( mode->name ) {
        mode_name = ctx->string.ptr + mode->name;
    }

    ctx->result.mode_name  = mode_name;
    ctx->result.mode_index =
        (size_t)( (char*)mode - (char*)ctx->modes.ptr ) / sizeof(struct __FShipMode);

    bool success = true;
    size_t required_count   = __fls_mode_required_count( mode );
    size_t required_counter = 0;

    for( ; index < argc; ++index ) {
        if( mode->is_terminating ) {
            break;
        }

        const char* arg_text = argv[index];
        if( arg_text[0] == '+' ) {
            if( (index + 1) < argc ) {
                const char* next = argv[index + 1];
                // NOTE(alicia): skip potential arg payload
                if( !(next[0] == '-' || next[1] == '+') ) {
                    index++;
                }
            }
            continue;
        }

        struct __FShipArg* arg = __fls_arg_search( ctx, mode, arg_text );

        if( !arg ) {
            __fls_err( "unrecognized argument: %s", arg_text );
            success = false;
            continue;
        }

        struct __FShipResult result = {};
        result.type     = arg->type;
        if( arg->name ) {
            result.name = ctx->string.ptr + arg->name;
        } else {
            result.name = NULL;
        }
        result.is_found = true;

        bool is_unnamed = arg->name == 0;
        if( !is_unnamed && arg_text[0] == '-' ) {
            arg_text++;
        }

        const char* payload = NULL;

        if( is_unnamed ) {
            payload = arg_text;
        } else {
            if( (payload = FSHIP_STRCHR( arg_text, ':' )) ) {
                payload++;
            } else if( result.type != FSHIP_BOOL ) {
                if(
                    ((index + 1) < argc) &&
                    !(argv[index + 1][0] == '-' || argv[index + 1][0] == '+')
                ) {
                    index++;
                    payload = argv[index];
                }
            }
        }

        switch( result.type ) {
            case FSHIP_BOOL: {
                if( payload ) {
                    __fls_err( "%s: boolean flags cannot take a payload!", result.name );
                    success = false;
                } else {
                    result.is_valid = true;

                    bool value = arg->boolean.value;

                    if( arg->boolean.is_flipped ) {
                        result.boolean = value;
                    } else {
                        result.boolean = !value;
                    }

                    if( arg->boolean.is_toggle ) {
                        arg->boolean.value = !value;
                    } else {
                        arg->boolean.value = result.boolean;
                    }
                }
            } break;
            case FSHIP_INT: {
                if( payload ) {
                    int64_t value  = 0;

                    if( FSHIP_INT_FROM_STR( payload, &value ) ) {
                        __fls_err( "%s: could not parse integer! '%s'", result.name, payload );
                        success = false;
                    } else {
                        bool out_of_range = false;
                        if( arg->integer.min != arg->integer.max ) {
                            if( value < arg->integer.min ) {
                                out_of_range = true;
                            }
                            if( value >= arg->integer.max ) {
                                out_of_range = true;
                            }
                        }

                        result.integer = value;

                        if( out_of_range ) {
                            __fls_err(
                                "%s: value %" PRId64 " is out of range! [%" PRId64 ", %" PRId64 "]",
                                result.name, value, arg->integer.min, arg->integer.max );
                            success = false;
                        } else {
                            result.is_valid = true;
                        }
                    }
                } else {
                    __fls_err( "%s requires an integer payload!", result.name );
                    success = false;
                }
            } break;
            case FSHIP_FLT: {
                if( payload ) {
                    double value = 0.0;

                    if( FSHIP_DBL_FROM_STR( payload, &value ) ) {
                        __fls_err( "%s: could not parse float! '%s'", result.name, payload );
                        success = false;
                    } else {
                        bool out_of_range = false;
                        if( arg->flt.min != arg->flt.max ) {
                            if( value < arg->flt.min ) {
                                out_of_range = true;
                            }
                            if( value >= arg->flt.max ) {
                                out_of_range = true;
                            }
                        }

                        result.flt = value;

                        if( out_of_range ) {
                            __fls_err(
                                "%s: value %f is out of range! [%f, %f]",
                                result.name, value, arg->flt.min, arg->flt.max );
                            success = false;
                        } else {
                            result.is_valid = true;
                        }
                    }
                } else {
                    __fls_err( "%s requires a float payload!", result.name );
                    success = false;
                }
            } break;
            case FSHIP_STR: {
                const char* value = payload;

                bool is_valid = true;
                if( arg->str.valid.len ) {
                    is_valid = false;
                    for( size_t i = 0; i < arg->str.valid.len; ++i ) {
                        const char* valid_text = ctx->string.ptr + arg->str.valid.ptr[i];

                        if( FSHIP_STRCMP( value, valid_text ) ) {
                            is_valid = true;
                            break;
                        }
                    }
                }

                result.str = value;

                if( is_valid ) {
                    result.is_valid = true;
                } else {
                    __fls_err( "%s: value '%s' is not valid!", result.name, value );
                    success = false;
                }
            } break;
        }

        if( result.is_valid ) {
            if( arg->is_required ) {
                required_counter++;
            }
        }

        __fls_result_push( ctx, &result );

        if( arg->is_terminating ) {
            break;
        }
    }

    if( opt_out_last_arg ) {
        *opt_out_last_arg = index;
    }

    if( required_counter != required_count ) {
        __fls_err( "required arguments were not provided!" );
        success = false;
    }

    if( !success ) {
        fls_help( ctx, mode_name, false );
    }

    return success;
}

FSHIP_INL
const char* fls_query_mode( FShipContext* ctx ) {
    const char* result = NULL;

    if( ctx->result.mode_name ) {
        result = ctx->result.mode_name;
    }

    return result;
}

FSHIP_INL
size_t __fls_type_size( FShipType type ) {
    switch( type ) {
        case FSHIP_BOOL : return sizeof(bool);
        case FSHIP_INT  : return sizeof(int64_t);
        case FSHIP_FLT  : return sizeof(double);
        case FSHIP_STR  : return sizeof(const char*);
    }
    return 0;
}

FSHIP_INL
void __fls_flag_search(
    FShipContext* ctx,
    const char*   name,
    FShipType     type,
    bool*         out_is_found,
    bool*         out_is_set,
    bool*         out_is_valid,
    bool*         out_is_type_correct,
    bool*         out_is_type_cast,
    void*         out_value
) {
    bool is_found, is_set, is_valid, is_type_correct, is_type_cast;
    is_found = is_set = is_valid = is_type_correct = is_type_cast = false;

    struct __FShipMode* mode = ctx->modes.ptr + ctx->result.mode_index;

    for( size_t i = 0; i < ctx->result.len; ++i ) {
        struct __FShipResult* current = ctx->result.ptr + i;

        bool match = false;
        if( name ) {
            if( current->name ) {
                match = FSHIP_STRCMP( name, current->name );
            } else {
                match = false;
            }
        } else {
            match = !current->name;
        }

        if( match ) {
            is_found        = true;
            is_set          = true;
            is_valid        = current->is_valid;
            is_type_correct = type == current->type;

            if( is_type_correct ) {
                memcpy( out_value, &current->integer, __fls_type_size( current->type ) );
            } else {

                switch( current->type ) {
                    case FSHIP_BOOL : {
                        switch( type ) {
                            case FSHIP_BOOL : break;
                            case FSHIP_INT  : {
                                is_type_cast = true;
                                *(int64_t*)out_value = current->boolean ? 1 : 0;
                            } break;
                            case FSHIP_FLT  : {
                                is_type_cast = true;
                                *(double*)out_value = current->boolean ? 1.0 : 0.0;
                            } break;
                            case FSHIP_STR  : {
                                is_type_cast = true;
                                *(const char**)out_value = current->boolean ? "true" : "false";
                            } break;
                        }
                    } break;
                    case FSHIP_INT  : {
                        switch( type ) {
                            case FSHIP_BOOL : {
                                is_type_cast = true;
                                *(bool*)out_value = current->integer ? true : false;
                            } break;
                            case FSHIP_INT  : break;
                            case FSHIP_FLT  : {
                                is_type_cast = true;
                                *(double*)out_value = (double)current->integer;
                            } break;
                            case FSHIP_STR  : {
                                *(const char**)out_value = "integer";
                            } break;
                        }
                    } break;
                    case FSHIP_FLT  : {
                        switch( type ) {
                            case FSHIP_BOOL : {
                                is_type_cast = true;
                                *(bool*)out_value = current->flt ? true : false;
                            } break;
                            case FSHIP_INT  : {
                                is_type_cast = true;
                                *(int64_t*)out_value = (int64_t)current->flt;
                            } break;
                            case FSHIP_FLT  : break;
                            case FSHIP_STR  : {
                                *(const char**)out_value = "float";
                            } break;
                        }
                    } break;
                    case FSHIP_STR  : {
                        switch( type ) {
                            case FSHIP_BOOL : {
                                is_type_cast = true;
                                *(bool*)out_value =
                                    FSHIP_STRCMP( current->str, "true" ) ? true : false;
                            } break;
                            case FSHIP_INT  : {
                                is_type_cast = true;
                                FSHIP_INT_FROM_STR( current->str, (int64_t*)out_value );
                            } break;
                            case FSHIP_FLT  : {
                                is_type_cast = true;
                                FSHIP_DBL_FROM_STR( current->str, (double*)out_value );
                            } break;
                            case FSHIP_STR  : break;
                        }
                    } break;
                }
            }

            *out_is_found        = is_found;
            *out_is_set          = is_set;
            *out_is_valid        = is_valid;
            *out_is_type_correct = is_type_correct;
            *out_is_type_cast    = is_type_cast;

            return;
        }
    }

    is_set = false;

    for( size_t i = 0; i < mode->len; ++i ) {
        struct __FShipArg* arg = mode->ptr + i;

        bool match = false;
        if( name ) {
            if( arg->name ) {
                match = FSHIP_STRCMP( name, ctx->string.ptr + arg->name );
            } else {
                match = false;
            }
        } else {
            match = !arg->name;
        }

        if( match ) {
            is_found        = true;
            is_type_correct = is_valid = type == arg->type;

            if( arg->has_default ) {

                if( is_type_correct ) {
                    switch( type ) {
                        case FSHIP_BOOL: {
                            *(bool*)out_value = arg->boolean.value;
                        } break;
                        case FSHIP_INT: {
                            *(int64_t*)out_value = arg->integer.value;
                        } break;
                        case FSHIP_FLT: {
                            *(double*)out_value = arg->flt.value;
                        } break;
                        case FSHIP_STR: {
                            if( arg->has_default && arg->str.value ) {
                                *(const char**)out_value = ctx->string.ptr + arg->str.value;
                            } else {
                                *(const char**)out_value = NULL;
                            }
                        } break;
                    }
                } else {
                    switch( arg->type ) {
                        case FSHIP_BOOL : {
                            switch( type ) {
                                case FSHIP_BOOL : break;
                                case FSHIP_INT  : {
                                    is_type_cast = true;
                                    *(int64_t*)out_value = arg->boolean.value ? 1 : 0;
                                } break;
                                case FSHIP_FLT  : {
                                    is_type_cast = true;
                                    *(double*)out_value = arg->boolean.value ? 1.0 : 0.0;
                                } break;
                                case FSHIP_STR  : {
                                    is_type_cast = true;
                                    *(const char**)out_value = arg->boolean.value ? "true" : "false";
                                } break;
                            }
                        } break;
                        case FSHIP_INT  : {
                            switch( type ) {
                                case FSHIP_BOOL : {
                                    is_type_cast = true;
                                    *(bool*)out_value = arg->integer.value ? true : false;
                                } break;
                                case FSHIP_INT  : break;
                                case FSHIP_FLT  : {
                                    is_type_cast = true;
                                    *(double*)out_value = (double)arg->integer.value;
                                } break;
                                case FSHIP_STR  : {
                                    *(const char**)out_value = "integer";
                                } break;
                            }
                        } break;
                        case FSHIP_FLT  : {
                            switch( type ) {
                                case FSHIP_BOOL : {
                                    is_type_cast = true;
                                    *(bool*)out_value = arg->flt.value ? true : false;
                                } break;
                                case FSHIP_INT  : {
                                    is_type_cast = true;
                                    *(int64_t*)out_value = (int64_t)arg->flt.value;
                                } break;
                                case FSHIP_FLT  : break;
                                case FSHIP_STR  : {
                                    *(const char**)out_value = "float";
                                } break;
                            }
                        } break;
                        case FSHIP_STR  : {
                            switch( type ) {
                                case FSHIP_BOOL : {
                                    is_type_cast = true;
                                    if( arg->str.value ) {
                                        *(bool*)out_value = FSHIP_STRCMP(
                                            ctx->string.ptr + arg->str.value, "true" );
                                    } else {
                                        *(bool*)out_value = false;
                                    }
                                } break;
                                case FSHIP_INT  : {
                                    is_type_cast = true;
                                    if( arg->str.value ) {
                                        FSHIP_INT_FROM_STR(
                                            ctx->string.ptr + arg->str.value,
                                            (int64_t*)out_value );
                                    } else {
                                        *(int64_t*)out_value = 0;
                                    }
                                } break;
                                case FSHIP_FLT  : {
                                    is_type_cast = true;
                                    if( arg->str.value ) {
                                        FSHIP_DBL_FROM_STR(
                                            ctx->string.ptr + arg->str.value, (double*)out_value );
                                    } else {
                                        *(double*)out_value = 0.0;
                                    }
                                } break;
                                case FSHIP_STR  : break;
                            }
                        } break;
                    }
                }
            } else {
                switch( type ) {
                    case FSHIP_BOOL: {
                        *(bool*)out_value = false;
                    } break;
                    case FSHIP_INT: {
                        *(int64_t*)out_value = 0;
                    } break;
                    case FSHIP_FLT: {
                        *(double*)out_value = 0.0;
                    } break;
                    case FSHIP_STR: {
                        *(const char**)out_value = "";
                    } break;
                }
            }

            *out_is_found        = is_found;
            *out_is_set          = is_set;
            *out_is_valid        = is_valid;
            *out_is_type_correct = is_type_correct;
            *out_is_type_cast    = is_type_cast;
            return;
        }
    }

    *out_is_found        = is_found;
    *out_is_set          = is_set;
    *out_is_valid        = is_valid;
    *out_is_type_correct = is_type_correct;
    *out_is_type_cast    = is_type_cast;
}

FSHIP_INL
bool __fls_read_flag( FShipContext* ctx, const char* name, struct FShipReadOut out ) {
    bool is_found, is_set, is_valid, is_type_correct, is_type_cast;
    is_found = is_set = is_valid = is_type_correct = is_type_cast = false;
    bool value = false;

    __fls_flag_search(
        ctx, name, FSHIP_BOOL,
        &is_found, &is_set, &is_valid, &is_type_correct, &is_type_cast, &value );

    if( out.is_found ) {
        *out.is_found = is_found;
    }
    if( out.is_set ) {
        *out.is_set = is_set;
    }
    if( out.is_valid ) {
        *out.is_valid = is_valid;
    }
    if( out.is_type_correct ) {
        *out.is_type_correct = is_type_correct;
    }
    if( out.is_type_cast ) {
        *out.is_type_cast = is_type_cast;
    }

    return value;
}
FSHIP_INL
int64_t __fls_read_int( FShipContext* ctx, const char* name, struct FShipReadOut out ) {
    bool is_found, is_set, is_valid, is_type_correct, is_type_cast;
    is_found = is_set = is_valid = is_type_correct = is_type_cast = false;
    int64_t value = 0;

    __fls_flag_search(
        ctx, name, FSHIP_INT,
        &is_found, &is_set, &is_valid, &is_type_correct, &is_type_cast, &value );

    if( out.is_found ) {
        *out.is_found = is_found;
    }
    if( out.is_set ) {
        *out.is_set = is_set;
    }
    if( out.is_valid ) {
        *out.is_valid = is_valid;
    }
    if( out.is_type_correct ) {
        *out.is_type_correct = is_type_correct;
    }
    if( out.is_type_cast ) {
        *out.is_type_cast = is_type_cast;
    }

    return value;
}
FSHIP_INL
double __fls_read_flt( FShipContext* ctx, const char* name, struct FShipReadOut out ) {
    bool is_found, is_set, is_valid, is_type_correct, is_type_cast;
    is_found = is_set = is_valid = is_type_correct = is_type_cast;
    double value = 0.0;

    __fls_flag_search(
        ctx, name, FSHIP_FLT,
        &is_found, &is_set, &is_valid, &is_type_correct, &is_type_cast, &value );

    if( out.is_found ) {
        *out.is_found = is_found;
    }
    if( out.is_set ) {
        *out.is_set = is_set;
    }
    if( out.is_valid ) {
        *out.is_valid = is_valid;
    }
    if( out.is_type_correct ) {
        *out.is_type_correct = is_type_correct;
    }
    if( out.is_type_cast ) {
        *out.is_type_cast = is_type_cast;
    }

    return value;
}
FSHIP_INL
const char* __fls_read_str( FShipContext* ctx, const char* name, struct FShipReadOut out ) {
    bool is_found, is_set, is_valid, is_type_correct, is_type_cast;
    is_found = is_set = is_valid = is_type_correct = is_type_cast = false;
    const char* value = "";

    __fls_flag_search(
        ctx, name, FSHIP_STR,
        &is_found, &is_set, &is_valid, &is_type_correct, &is_type_cast, &value );

    if( out.is_found ) {
        *out.is_found = is_found;
    }
    if( out.is_set ) {
        *out.is_set = is_set;
    }
    if( out.is_valid ) {
        *out.is_valid = is_valid;
    }
    if( out.is_type_correct ) {
        *out.is_type_correct = is_type_correct;
    }
    if( out.is_type_cast ) {
        *out.is_type_cast = is_type_cast;
    }

    return value;
}

FSHIP_INL
const char* fls_type_name( FShipType type ) {
    switch( type ) {
        case FSHIP_BOOL : return "bool";
        case FSHIP_INT  : return "int";
        case FSHIP_FLT  : return "float";
        case FSHIP_STR  : return "string";
    }
    return "";
}
FSHIP_INL
const char* fls_type_fmt( FShipType type ) {
    switch( type ) {
        case FSHIP_BOOL : return "%d";
        case FSHIP_INT  : return "%" PRId64;
        case FSHIP_FLT  : return "%f";
        case FSHIP_STR  : return "%s";
    }
    return "";
}

#endif /* header guard */

