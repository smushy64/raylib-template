#if !defined(CB_H)
#define CB_H
/**
 * @file   cb.h
 * @brief  C Build system.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 20, 2025
*/
// IWYU pragma: begin_exports
#define _XOPEN_SOURCE 800
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
// IWYU pragma: end_exports

#define CB_VERSION_MAJOR 0
#define CB_VERSION_MINOR 1
#define CB_VERSION_PATCH 0

#define CB_VERSION_STRING \
    CB_STRINGIFY2(CB_VERSION_MAJOR) "." \
    CB_STRINGIFY2(CB_VERSION_MINOR) "." \
    CB_STRINGIFY2(CB_VERSION_PATCH)

#define CB_VERSION \
    CB_VERSION_CREATE( CB_VERSION_MAJOR, CB_VERSION_MINOR, CB_VERSION_PATCH )

#define CB_VERSION_CREATE( major, minor, patch ) \
    ((unsigned int)((major) << 16u | (minor) << 8u | (patch)))

#define CB_VERSION_READ_MAJOR( version ) \
    ((unsigned short)(((version) & 0xFFFF0000u) >> 16u))
#define CB_VERSION_READ_MINOR( version ) \
    ((unsigned char)(((version) & 0x0000FF00u) >> 8u))
#define CB_VERSION_READ_PATCH( version ) \
    ((unsigned char)((version) & 0x000000FFu))

#define CB_CONCAT( a, b ) a##b
#define CB_CONCAT2( a, b ) CB_CONCAT( a, b )

#define CB_STRINGIFY(x)  #x
#define CB_STRINGIFY2(x) CB_STRINGIFY(x)

#if !defined(CB_FLT_EPSILON)
    #define CB_FLT_EPSILON (1e-6f)
#endif

#if !defined(CB_DBL_EPSILON)
    #define CB_DBL_EPSILON (1e-12f)
#endif

#define CB_MAX_PATH (4096)

#if !defined(CB_LOCAL_SIZE)
    #define CB_LOCAL_SIZE (4096)
#endif

#if !defined(CB_LOCAL_COUNT)
    #define CB_LOCAL_COUNT (8)
#endif

#if CB_LOCAL_SIZE < 256
    #undef CB_LOCAL_SIZE
    #define CB_LOCAL_SIZE (256)
#endif

#if CB_LOCAL_COUNT < 4
    #undef CB_LOCAL_COUNT
    #define CB_LOCAL_COUNT (4)
#endif

#if !defined(CB_THREAD_STACK_SIZE)
    #define CB_THREAD_STACK_SIZE (1 * 1024 * 1024)
#endif

#if CB_THREAD_STACK_SIZE < 4096
    #undef CB_THREAD_STACK_SIZE
    #define CB_THREAD_STACK_SIZE (4096)
#endif

#if !defined(CB_THREAD_COUNT)
    #define CB_THREAD_COUNT (8)
#endif

#if CB_THREAD_COUNT < 4
    #undef CB_THREAD_COUNT
    #define CB_THREAD_COUNT (4)
#endif

#if !defined(CB_JOB_COUNT)
    #define CB_JOB_COUNT (32)
#endif

#if CB_JOB_COUNT < 16
    #undef CB_JOB_COUNT
    #define CB_JOB_COUNT (16)
#endif

#if !defined(CB_DEF)
    #define CB_DEF
#endif

#define CB_FMT(FormatIndex, ArgIndex) \
    __attribute__((format(printf, FormatIndex, ArgIndex)))

#if __cplusplus
    #define CB_INL inline
#else
    #define CB_INL static inline
#endif

#if __cplusplus
    #define CB_TLS thread_local
#else
    #define CB_TLS _Thread_local
#endif

#if !defined(CB_PREFIX_TYPE)
    #define CB_PREFIX_TYPE
#endif

#if !defined(CB_PREFIX_FUNC)
    #define CB_PREFIX_FUNC
#endif

#if !defined(CB_PREFIX_ENUM)
    #define CB_PREFIX_ENUM
#endif

#define _T(x) CB_CONCAT2( CB_PREFIX_TYPE, x )
#define _F(x) CB_CONCAT2( CB_PREFIX_FUNC, x )
#define _E(x) CB_CONCAT2( CB_PREFIX_ENUM, x )

#define CB_COMPILER_UNKNOWN 0
#define CB_COMPILER_GCC     1
#define CB_COMPILER_CLANG   2
#define CB_COMPILER_MSVC    3
#define CB_COMPILER_COUNT   4

#define CB_COMPILER_CURRENT        CB_COMPILER_UNKNOWN
#define CB_COMPILER_GNU_COMPATIBLE 0

#if defined(__GNUC__)
    #undef  CB_COMPILER_GNU_COMPATIBLE
    #define CB_COMPILER_GNU_COMPATIBLE 1

    #if defined(__clang__)
        #undef  CB_COMPILER_CURRENT
        #define CB_COMPILER_CURRENT CB_COMPILER_CLANG
    #else
        #undef  CB_COMPILER_CURRENT
        #define CB_COMPILER_CURRENT CB_COMPILER_GCC
    #endif
#elif defined(_MSC_VER)
    #undef  CB_COMPILER_CURRENT
    #define CB_COMPILER_CURRENT CB_COMPILER_MSVC
#endif

#define CB_OS_UNKNOWN   0
#define CB_OS_GNU_LINUX 1
#define CB_OS_WINDOWS   2
#define CB_OS_MACOS     3
#define CB_OS_COUNT     4

#define CB_OS_CURRENT  CB_OS_UNKNOWN
#define CB_OS_IS_POSIX 0

#if defined(__gnu_linux__)
    #undef  CB_OS_CURRENT
    #define CB_OS_CURRENT CB_OS_GNU_LINUX
    #undef  CB_OS_IS_POSIX
    #define CB_OS_IS_POSIX 1
#elif defined(__MACOS)
    #undef  CB_OS_CURRENT
    #define CB_OS_CURRENT CB_OS_MACOS
    #undef  CB_OS_IS_POSIX
    #define CB_OS_IS_POSIX 1
#elif defined(_WIN32)
    #undef  CB_OS_CURRENT
    #define CB_OS_CURRENT CB_OS_WINDOWS
    #undef  CB_OS_IS_POSIX
    #define CB_OS_IS_POSIX 0
#endif

#define CB_ARCH_UNKNOWN 0
#define CB_ARCH_X86     1
#define CB_ARCH_ARM     2
#define CB_ARCH_COUNT   3

#define CB_ARCH_CURRENT   CB_ARCH_UNKNOWN
#define CB_ARCH_WORD_SIZE 64
#define CB_ARCH_IS_64BIT  1

#if defined(__aarch64__) || defined(_M_ARM64)
    #undef  CB_ARCH_CURRENT
    #define CB_ARCH_CURRENT CB_ARCH_ARM
#elif defined(__arm__) || defined(_M_ARM)
    #undef  CB_ARCH_CURRENT
    #define CB_ARCH_CURRENT CB_ARCH_ARM
    #undef  CB_ARCH_WORD_SIZE
    #define CB_ARCH_WORD_SIZE 32
    #undef  CB_ARCH_IS_64BIT
    #define CB_ARCH_IS_64BIT 0
#elif defined(__X86__) || defined(__x86_64__)
    #undef  CB_ARCH_CURRENT
    #define CB_ARCH_CURRENT CB_ARCH_X86
#elif defined(__i386__) || defined(_M_IX86)
    #undef  CB_ARCH_CURRENT
    #define CB_ARCH_CURRENT CB_ARCH_X86
    #undef  CB_ARCH_WORD_SIZE
    #define CB_ARCH_WORD_SIZE 32
    #undef  CB_ARCH_IS_64BIT
    #define CB_ARCH_IS_64BIT 0
#endif

#define CB_ENDIAN_LITTLE 0
#define CB_ENDIAN_BIG    1
#define CB_ENDIAN_COUNT  2

#define CB_ENDIAN_CURRENT CB_ENDIAN_LITTLE

#define CB_WAIT_INF 0xFFFFFFFF

#define CB_COLOR_BLACK   "\033[1;30m"
#define CB_COLOR_WHITE   "\033[1;37m"
#define CB_COLOR_RED     "\033[1;31m"
#define CB_COLOR_GREEN   "\033[1;32m"
#define CB_COLOR_BLUE    "\033[1;34m"
#define CB_COLOR_MAGENTA "\033[1;35m"
#define CB_COLOR_YELLOW  "\033[1;33m"
#define CB_COLOR_CYAN    "\033[1;36m"
#define CB_COLOR_RESET   "\033[1;00m"

#define CB_CP8_REPLACEMENT_CHARACTER \
    _F(cp8_from_code_units)( 0xEF, 0xBF, 0xBD, 0 )

#define CB_CP16_REPLACEMENT_CHARACTER \
    _F(cp16_from_code_units)( 0xFFFD, 0 )

#define CB_CP32_REPLACEMENT_CHARACTER \
    _F(cp32_from_code_units)( 0xFFFD )

// NOTE(alicia): types --------------------------------------------------------

typedef void* _T(AllocFN)( void* ptr, size_t old_size, size_t new_size, void* params );
typedef void  _T(FreeFN)( void* ptr, size_t size, void* params );

typedef int _T(JobID);
typedef int _T(JobFN)( _T(JobID) id, void* params );

typedef struct {
    volatile int atom;
} _T(Mutex);

typedef struct {
    volatile int atom;
    _T(Mutex)    mtx;
} _T(Semaphore);

typedef struct {
    size_t len;
    char*  ptr;
} _T(ByteSlice);

typedef struct {
    size_t cap;
    union {
        struct {
            size_t len;
            char*  ptr;
        };
        _T(ByteSlice) slice;
    };
} _T(ByteBuf);

typedef _T(ByteSlice) _T(String);
typedef _T(ByteBuf)   _T(StringBuf);

typedef struct {
    size_t       cap;
    size_t       len;
    const char** ptr;
} _T(CStringBuf);

typedef struct {
    union {
        struct {
            size_t       cap;
            size_t       len;
            const char** ptr;
        };
        _T(CStringBuf) deps;
    };
    const char*   target;
    _T(StringBuf) string;
} _T(Deps);

typedef struct {
    uint8_t cp[4];
} _T(CodeUnits8);

typedef struct {
    uint16_t cp[2];
} _T(CodeUnits16);

typedef uint32_t _T(Rune32);
typedef uint16_t _T(Rune16);
typedef uint8_t  _T(Rune8);

typedef struct {
    size_t      len;
    _T(Rune32)* ptr;
} _T(Rune32Set);

typedef enum {
    _E(LOG_INFO),
    _E(LOG_WARN),
    _E(LOG_ERROR),
    _E(LOG_FATAL),
    _E(LOG_NONE),

    _E(LOG_COUNT)
} _T(LogLevel);

typedef union {
    void* opaque_ptr;
    int   opaque_fd;
} _T(Handle);

typedef _T(Handle) _T(File);
typedef _T(Handle) _T(Pipe);
typedef _T(Handle) _T(Process);

typedef intptr_t _T(Time);

typedef uint32_t _T(TimeYear);
typedef uint32_t _T(TimeMonth);
typedef uint32_t _T(TimeDay);
typedef uint32_t _T(TimeHour);
typedef uint32_t _T(TimeMinute);
typedef uint32_t _T(TimeSecond);

typedef struct {
    _T(TimeYear)   year;
    _T(TimeMonth)  month;
    _T(TimeDay)    day;
    _T(TimeHour)   hour;
    _T(TimeMinute) minute;
    _T(TimeSecond) second;
} _T(TimeSplit);

typedef enum {
    _E(FF_READ)     = (1 << 0),
    _E(FF_WRITE)    = (1 << 1),
    _E(FF_EXECUTE)  = (1 << 2),
    _E(FF_CREATE)   = (1 << 4),
    _E(FF_TRUNCATE) = (1 << 5),
    _E(FF_APPEND)   = (1 << 6),
} _T(FileFlags);

typedef enum {
    _E(FS_SET),
    _E(FS_CUR),
    _E(FS_END),
} _T(FileSeek);

typedef enum {
    _E(FT_NULL),
    _E(FT_FILE),
    _E(FT_DIR),
    _E(FT_PIPE),
} _T(FileType);

typedef struct {
    _T(FileType) type;
    size_t       size;
    _T(Time)     time_create;
    _T(Time)     time_modify;
} _T(FileInfo);

typedef enum {
    _E(DWA_CONTINUE),
    _E(DWA_STOP),
    _E(DWA_SKIP),
} _T(DirWalkAction);

typedef struct {
    _T(FileType) file_type;
    _T(Time)     file_time_create;
    _T(Time)     file_time_modify;
    size_t       file_size;
    size_t       path_len;
    size_t       path_name_offset;
    bool         path_has_ext    : 1;
    size_t       path_ext_offset : CB_ARCH_WORD_SIZE - 1;
    intptr_t     level;
} _T(DirWalkInfo);

typedef _T(DirWalkAction) _T(DirWalkFN)( const char* path, _T(DirWalkInfo)* info, void* params );

typedef struct {
    size_t       len;
    const char** ptr;
} _T(Cmd);

typedef struct {
    size_t       len;
    const char** ptr;
} _T(CmdLine);

typedef struct {
    size_t cap;
    union {
        struct {
            size_t       len;
            const char** ptr;
        };
        _T(Cmd) cmd;
    };
    _T(StringBuf) string;
} _T(CmdBuf);

typedef struct {
    size_t len;
    size_t offset;
} _T(StringOffset);

typedef struct {
    _T(StringOffset) key, value;
} _T(EnvKV);

typedef struct {
    size_t        cap;
    size_t        len;
    _T(EnvKV)*    ptr;
    _T(StringBuf) string;
} _T(Env);

typedef enum {
    _E(PSIG_NULL),
    _E(PSIG_KILL),

    _E(PSIG_COUNT)
} _T(ProcessSignal);

typedef struct {
    _T(Process) handle;
    int         result;
} _T(ProcessData);

typedef struct {
    size_t           cap;
    size_t           len;
    _T(ProcessData)* ptr;
} _T(ProcessGroup);

typedef uint32_t _T(CRC32);

typedef struct {
    uint32_t c[4];
} _T(MD5);

typedef struct {
    uint32_t c[5];
} _T(SHA1);

typedef union {
    struct {
        uint32_t lo;
        uint32_t hi;
    };
    uint32_t c[2];
} _T(HashCode);

// NOTE(alicia): internal structures ------------------------------------------

typedef struct {
    const char*       wd;
    _T(Env)*          env;
    _T(Pipe)*         pstdin;
    _T(Pipe)*         pstdout;
    _T(Pipe)*         pstderr;
    _T(ProcessGroup)* group;
    _T(Process)*      proc;
} __CbExecParams;

// NOTE(alicia): functions ----------------------------------------------------

#if CB_COMPILER_CURRENT == CB_COMPILER_MSVC
    /// @brief Insert unreachable statement.
    #define CB_INSERT_UNREACHABLE() __assume(0)

    /// @brief Insert a trap statement. (crash program)
    #define CB_INSERT_TRAP() __debugbreak()

    /// @brief Insert a debugger break statement.
    #define CB_INSERT_DBG_BREAK() __debugbreak()
#else
    /// @brief Insert unreachable statement.
    #define CB_INSERT_UNREACHABLE() __builtin_unreachable()

    /// @brief Insert a trap statement. (crash program)
    #define CB_INSERT_TRAP() __builtin_trap()

    /// @brief Insert a debugger break statement.
    #define CB_INSERT_DBG_BREAK() __builtin_debugtrap()
#endif

#define CB_PANIC( ... ) do {                      \
    CB_FATAL(                                     \
        "Panic! "                                 \
        __FILE__ ":" CB_STRINGIFY2(__LINE__) ": " \
        __VA_ARGS__ );                            \
    CB_INSERT_TRAP();                             \
} while(0)

#define CB_DBG_BREAK( ... ) do {                  \
    CB_FATAL(                                     \
        "Debug Break: "                           \
        __FILE__ ":" CB_STRINGIFY2(__LINE__) ": " \
        __VA_ARGS__ );                            \
    CB_INSERT_DBG_BREAK();                        \
} while(0)

#define CB_ASSERT(Condition, ...) do {                \
    if( !(Condition) ) {                              \
        CB_FATAL(                                     \
            "Assertion Failed! (" #Condition ") "     \
            __FILE__ ":" CB_STRINGIFY2(__LINE__) ": " \
            __VA_ARGS__ );                            \
        CB_INSERT_TRAP();                             \
    }                                                 \
} while(0)

#define CB_UNUSED(...) \
    __cb_unused( 0, __VA_ARGS__ )

#define CB_QUICK_SLICE( T, Name ) \
typedef struct Name##Slice {      \
    size_t len;                   \
    T*     ptr;                   \
} Name##Slice

#define CB_QUICK_BUF( T, Name ) \
typedef struct Name##Buf {      \
    size_t cap;                 \
    size_t len;                 \
    T*     ptr;                 \
} Name##Buf

#define CB_ARRAY_LEN( Array ) \
    (sizeof((Array)) / sizeof((Array)[0]))

#define CB_STRING_FMT ".*s"

#define CB_STRING_ARG( StringStruct ) \
    (int)(StringStruct)->len, (char*)((StringStruct)->ptr)

#define __CB_FOREACH( Slice, CodeSnippet, Value, Index, ... ) do {    \
    __typeof((Slice)->ptr) Value = (Slice)->ptr;                      \
    for( size_t Index = 0; Index < (Slice)->len; ++Index, ++Value ) { \
        CodeSnippet                                                   \
    }                                                                 \
} while(0)

#define CB_FOREACH( Slice, CodeSnippet, ... ) \
    __CB_FOREACH(                             \
        Slice,                                \
        { CodeSnippet },                      \
        __VA_ARGS__ __VA_OPT__(,)             \
        CB_CONCAT2(__ptr_,__LINE__),          \
        CB_CONCAT2(__i_,__LINE__) )

#define __CB_FOREACH_REV( Slice, CodeSnippet, Value, Index, ... ) do { \
    __typeof((Slice)->ptr) Value = (Slice)->ptr + (Slice)->len;        \
    for( size_t Index = (Slice)->len; Value--, Index-- > 0; ) {        \
        CodeSnippet                                                    \
    }                                                                  \
} while(0)

#define CB_FOREACH_REV( Slice, CodeSnippet, ... ) \
    __CB_FOREACH_REV(                             \
        Slice,                                    \
        CodeSnippet,                              \
        __VA_ARGS__ __VA_OPT__(,)                 \
        CB_CONCAT2(__ptr_,__LINE__),              \
        CB_CONCAT2(__i_,__LINE__) )

#define CB_INFO( fmt, ... ) \
    _F(logger_output)( _E(LOG_INFO), "[I] " fmt "\n" __VA_OPT__(,) __VA_ARGS__ )

#define CB_WARN( fmt, ... ) \
    _F(logger_output)( _E(LOG_WARN), "[W] " fmt "\n" __VA_OPT__(,) __VA_ARGS__ )

#define CB_ERROR( fmt, ... ) \
    _F(logger_output)( _E(LOG_ERROR), "[E] " fmt "\n" __VA_OPT__(,) __VA_ARGS__ )

#define CB_FATAL( fmt, ... ) \
    _F(logger_output)( _E(LOG_FATAL), "[F] " fmt "\n" __VA_OPT__(,) __VA_ARGS__ )

#if defined(__cplusplus)
    #define CB_STRUCT( Name, ... ) Name{ __VA_ARGS__ }
#else
    #define CB_STRUCT( Name, ... ) (Name){ __VA_ARGS__ }
#endif

#define CB_STRING( Literal ) \
    CB_STRUCT(_T(String), .len=sizeof((Literal)) - 1, .ptr=(char*)(Literal) )

#define CB_RUNE_SET( ... ) \
    CB_STRUCT( _T(Rune32Set), \
        .len=sizeof((_T(Rune32)[]){ __VA_ARGS__ }) / sizeof(_T(Rune32)), \
        .ptr=(_T(Rune32)[]){ __VA_ARGS__ } )

#define CB_BUF_RESERVE( Buf, Minimum ) do {                                     \
    size_t __remaining__ = (Buf)->cap - (Buf)->len;                             \
    if( __remaining__ < (Minimum) ) {                                           \
        size_t __new_cap__  = (Buf)->cap + (Minimum) + 16;                      \
        size_t __old_size__ = sizeof((Buf)->ptr[0]) * (Buf)->cap;               \
        size_t __new_size__ = sizeof((Buf)->ptr[0]) * __new_cap__;              \
        (Buf)->ptr = _F(mem_realloc)( (Buf)->ptr, __old_size__, __new_size__ ); \
        (Buf)->cap = __new_cap__;                                               \
    }                                                                           \
} while(0)

#define CB_BUF_PUSH( Buf, Item ) do {  \
    CB_BUF_RESERVE( Buf, 1 );          \
    (Buf)->ptr[(Buf)->len++] = (Item); \
} while(0)

#define CB_BUF_APPEND( Buf, Items, Count ) do {                                        \
    CB_BUF_RESERVE( Buf, (Count) );                                                    \
    _F(mem_copy)( (Buf)->ptr + (Buf)->len, (Items), sizeof((Buf)->ptr[0]) * (Count) ); \
    (Buf)->len += (Count);                                                             \
} while(0)

#define CB_BUF_REMOVE( Buf, Index ) do {                        \
    _F(mem_move)(                                               \
        (Buf)->ptr + (Index),                                   \
        (Buf)->ptr + (Index) + 1,                               \
        sizeof((Buf)->ptr[0]) * ((Buf)->cap - ((Index) + 1)) ); \
    (Buf)->len--;                                               \
} while(0)

#define CB_BUF_REMOVE_SWAP( Buf, Index ) do {       \
    (Buf)->ptr[(Index)] = (Buf)->ptr[--(Buf)->len]; \
} while(0)

#define CB_BYTES_FROM_SLICE( Slice ) \
    CB_STRUCT( _T(ByteSlice), sizeof(*((Slice)->ptr)) * (Slice)->len, (char*)(Slice)->ptr )

#define CB_BYTES_FROM_BUF( Buf )                                      \
    CB_STRUCT( _T(ByteBuf), sizeof(*((Array)->ptr)) * (Array)->cap,   \
        sizeof(*((Array)->ptr)) * (Array)->len, (char*)(Array)->ptr )

#define CB_SLICE_NEW( BaseType, SliceType, ... )            \
    CB_STRUCT(                                              \
        SliceType,                                          \
        .len=(CB_ARRAY_LEN(((BaseType[]){ __VA_ARGS__ }))), \
        .ptr=((BaseType[]){ __VA_ARGS__ }) )

#define __CB_SLICE_ADVANCE( Slice, Amount, ... )                                           \
    CB_STRUCT( __typeof(*(Slice)),                                                         \
        (Amount) <= (Slice)->len ? (Slice)->len - (Amount) : 0,                            \
        (Amount) <= (Slice)->len ? (Slice)->ptr + (Amount) : (Slice)->ptr + (Slice)->len )

#define CB_SLICE_ADVANCE( Slice, ... ) \
    __CB_SLICE_ADVANCE( Slice, __VA_ARGS__ __VA_OPT__(,) 1 )

#define CB_SLICE_ALLOC( Slice, Amount ) do {                            \
    (Slice)->ptr = _F(mem_alloc)( (Amount) * sizeof(*((Slice)->ptr)) ); \
    (Slice)->len = (Amount);                                            \
} while(0)

#define CB_SLICE_REALLOC( Slice, NewAmount ) do { \
    (Slice)->ptr = _F(mem_realloc)( (Slice)->ptr, \
        sizeof(*((Slice)->ptr)) * (Slice)->len,   \
        sizeof(*((Slice)->ptr)) * (NewAmount) );  \
    (Slice)->len = (NewAmount);                   \
} while(0)

#define CB_SLICE_FREE( Slice ) do {                                       \
    _F(mem_free)( (Slice)->ptr, sizeof(*((Slice)->ptr)) * (Slice)->len ); \
    (Slice)->ptr = NULL;                                                  \
    (Slice)->len = 0;                                                     \
} while(0)

#define CB_SLICE_REVERSE( Slice ) do {       \
    _F(array_reverse)(                       \
        (Slice)->ptr,                        \
        sizeof(*((Slice)->ptr)),             \
        (Slice)->len,                        \
        (char[sizeof(*((Slice)->ptr))]){} ); \
} while(0)

#define CB_BUF_ALLOC( Buf, Amount ) do {                            \
    (Buf)->ptr = _F(mem_alloc)( (Amount) * sizeof(*((Buf)->ptr)) ); \
    (Buf)->cap = (Amount);                                          \
} while(0)

#define CB_BUF_REALLOC( Buf, NewAmount ) do {  \
    (Buf)->ptr = _F(mem_realloc)( (Buf)->ptr,  \
        sizeof(*((Buf)->ptr)) * (Buf)->cap,    \
        sizeof(*((Buf)->ptr)) * (NewAmount) ); \
    (Buf)->cap = (NewAmount);                  \
} while(0)

#define CB_BUF_FREE( Buf ) do {                                     \
    _F(mem_free)( (Buf)->ptr, sizeof(*((Buf)->ptr)) * (Buf)->cap ); \
    (Buf)->ptr = NULL;                                              \
    (Buf)->len = 0;                                                 \
    (Buf)->cap = 0;                                                 \
} while(0)

#define CB_DIR_CREATE_MANY( ... ) \
    __cb_dir_create_many( __VA_ARGS__ , NULL )

#define CB_EXEC( cmd, ... ) \
    __cb_exec( cmd, CB_STRUCT( __CbExecParams, __VA_ARGS__ ) )

#define CB_CMD( ... ) \
    CB_STRUCT( _T(Cmd), \
        .len=sizeof(((const char*[]){ __VA_ARGS__ })) / sizeof(const char*), \
        .ptr=((const char*[]){ __VA_ARGS__, NULL }) )

#define CB_CMD_APPEND( cmd, ... ) \
    __cb_cmd_append( cmd, __VA_ARGS__, NULL )

#define CB_BUILD_INIT() \
    _F(build_init)( argv[0], __FILE__, argc, (const char**)argv )

#define CB_CL( argc, argv ) \
    CB_STRUCT( _T(CmdLine), .len=(size_t)argc, .ptr=(const char**)argv )

#define CB_CL_NEXT( CommandLine ) \
    CB_SLICE_ADVANCE( CommandLine, 1 )

#define CB_CL_FOREACH( CommandLine, CodeSnippet ) do { \
    while( (CommandLine)->len ) { \
        CodeSnippet \
        *(CommandLine) = CB_CL_NEXT( CommandLine ); \
    } \
} while(0)

#define CB_KILOBYTES( kb ) (kb * 1000ULL)
#define CB_MEGABYTES( mb ) (CB_KILOBYTES(mb) * 1000ULL)
#define CB_GIGABYTES( gb ) (CB_MEGABYTES(gb) * 1000ULL)
#define CB_TERABYTES( tb ) (CB_GIGABYTES(tb) * 1000ULL)

#define CB_KIBIBYTES( kb ) (kb * 1024ULL)
#define CB_MEBIBYTES( mb ) (CB_KIBIBYTES(mb) * 1024ULL)
#define CB_GIBIBYTES( gb ) (CB_MEBIBYTES(gb) * 1024ULL)
#define CB_TEBIBYTES( tb ) (CB_GIBIBYTES(tb) * 1024ULL)

#define CB_STRING_FROM_CSTR( CStr ) \
    CB_STRUCT( _T(String), .len=_F(cstr_len)( (CStr) ), .ptr=(char*)(CStr) )

CB_DEF void _F(build_init)(
    const char*  path_exec,
    const char*  path_src,
    int          argc,
    const char** argv );
CB_DEF void _F(build_rebuild)(
    const char*  path_exec,
    const char*  path_src,
    int          argc,
    const char** argv,
    _T(Cmd)*     opt_cmd,
    bool         should_reload );

CB_DEF bool _F(float_cmp)( float a, float b );
CB_DEF bool _F(double_cmp)( double a, double b );

CB_DEF void  _F(mem_allocator_set)( _T(AllocFN)* alloc, _T(FreeFN)* free, void* params );
CB_DEF void  _F(mem_allocator_reset)(void);
CB_DEF void* _F(mem_alloc)( size_t size );
CB_DEF void* _F(mem_realloc)( void* ptr, size_t old_size, size_t new_size );
CB_DEF void  _F(mem_free)( void* ptr, size_t size );

CB_DEF bool  _F(mem_cmp)( const void* a, const void* b, size_t size );
CB_DEF void* _F(mem_copy)( void* restrict dst, const void* restrict src, size_t size );
CB_DEF void* _F(mem_move)( void* dst, const void* src, size_t size );
CB_DEF void* _F(mem_set)( void* buf, uint8_t byte, size_t size );

CB_DEF _T(ByteBuf) _F(local_buf)(void);
CB_DEF const char* _F(local_fmt_va)( const char* fmt, va_list va ) CB_FMT(1, 0);
CB_DEF const char* _F(local_fmt)( const char* fmt, ... ) CB_FMT(1, 2);
CB_DEF const char* _F(local_path)( _T(String) string );

CB_DEF void _F(array_reverse)( void* base, size_t stride, size_t count, char* swap );

CB_DEF size_t _F(cstr_len)( const char* string );

CB_DEF bool       _F(rune_is_ascii)( _T(Rune32) rune );
CB_DEF _T(Rune32) _F(rune_from_cp8)( _T(CodeUnits8) cp8 );

CB_DEF _T(CodeUnits8) _F(cp8_from_code_units)( uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3 );
CB_DEF size_t         _F(cp8_from_string)( _T(String) string, _T(CodeUnits8)* out_cp8 );

CB_DEF _T(CodeUnits16) _F(cp16_from_code_units)( uint16_t c0, uint16_t c1 );

CB_DEF _T(Rune32) _F(cp32_from_code_units)( uint32_t c0 );

CB_DEF bool _F(string_cmp)( _T(String) a, _T(String) b );

CB_DEF bool _F(string_find)( _T(String) string, _T(Rune32) c, size_t* opt_out_pos );
CB_DEF bool _F(string_find_set)( _T(String) string, _T(Rune32Set) set, size_t* opt_out_pos );
CB_DEF bool _F(string_find_phrase)( _T(String) string, _T(String) phrase, size_t* opt_out_pos );

CB_DEF bool _F(string_find_rev)( _T(String) string, _T(Rune32) c, size_t* opt_out_pos );
CB_DEF bool _F(string_find_set_rev)( _T(String) string, _T(Rune32Set) set, size_t* opt_out_pos );
CB_DEF bool _F(string_find_phrase_rev)( _T(String) string, _T(String) phrase, size_t* opt_out_pos );

CB_DEF _T(String) _F(string_trim_leading_ws)( _T(String) string );
CB_DEF _T(String) _F(string_trim_trailing_ws)( _T(String) string );
CB_DEF _T(String) _F(string_trim_ws)( _T(String) string );

CB_DEF void _F(string_split)(
    _T(String)  string,
    size_t      pos,
    _T(String)* opt_out_left,
    _T(String)* opt_out_right );

CB_DEF size_t     _F(string_unicode_len)( _T(String) string );
CB_DEF _T(String) _F(string_unicode_next)( _T(String) string, _T(Rune32)* out_c );
CB_DEF _T(Rune32) _F(string_unicode_index)( _T(String) string, size_t index );

CB_DEF size_t _F(string_buf_fmt_va)( _T(StringBuf)* buf, const char* fmt, va_list va ) CB_FMT(2, 0);
CB_DEF size_t _F(string_buf_fmt)( _T(StringBuf)* buf, const char* fmt, ... ) CB_FMT(2, 3);

CB_DEF _T(LogLevel) _F(logger_level_set)( _T(LogLevel) level );
CB_DEF bool         _F(logger_level_check)( _T(LogLevel) level );

CB_DEF size_t _F(logger_output_va)( _T(LogLevel) level, const char* fmt, va_list va ) CB_FMT(2, 0);
CB_DEF size_t _F(logger_output)( _T(LogLevel) level, const char* fmt, ... ) CB_FMT(2, 3);

CB_DEF _T(File)* _F(stdout_handle)(void);
CB_DEF _T(File)* _F(stderr_handle)(void);
CB_DEF _T(File)* _F(stdin_handle)(void);

CB_INL bool         _F(path_exists)( const char* path );
CB_DEF bool         _F(path_query_info)( const char* path, _T(FileInfo)* out_info );
CB_INL _T(FileType) _F(path_query_file_type)( const char* path );
CB_INL bool         _F(path_query_time_create)( const char* path, _T(Time)* out_time );
CB_INL bool         _F(path_query_time_modify)( const char* path, _T(Time)* out_time );
CB_DEF size_t       _F(path_canonicalize_buf)( _T(StringBuf)* buf, const char* path );
CB_INL const char*  _F(path_canonicalize)( const char* path );

CB_INL _T(File)     _F(file_null)(void);
CB_INL bool         _F(file_is_null)( _T(File)* file );
CB_DEF bool         _F(file_open)( const char* path, _T(FileFlags) flags, _T(File)* out_file );
CB_DEF void         _F(file_close)( _T(File)* file );
CB_DEF bool         _F(file_remove)( const char* path );
CB_DEF _T(FileInfo) _F(file_query_info)( _T(File)* file );
CB_INL _T(FileType) _F(file_query_type)( _T(File)* file );
CB_INL _T(Time)     _F(file_query_time_create)( _T(File)* file );
CB_INL _T(Time)     _F(file_query_time_modify)( _T(File)* file );
CB_INL size_t       _F(file_query_size)( _T(File)* file );
CB_DEF bool         _F(file_is_executable)( const char* path );
CB_DEF size_t       _F(file_read)( _T(File)* file, void* dst, size_t size );
CB_INL size_t       _F(file_read_entire)( const char* path, _T(ByteBuf)* buf );
CB_DEF size_t       _F(file_write)( _T(File)* file, const void* src, size_t size );
CB_DEF bool         _F(file_copy)( const char* dst, const char* src, bool fail_if_exists );
CB_DEF bool         _F(file_move)( const char* dst, const char* src, bool fail_if_exists );
CB_DEF intptr_t     _F(file_seek)( _T(File)* file, intptr_t offset, _T(FileSeek) type );
CB_DEF size_t       _F(file_write_fmt_va)( _T(File)* file, const char* fmt, va_list va ) CB_FMT(2, 0);
CB_DEF size_t       _F(file_write_fmt)( _T(File*) file, const char* fmt, ... ) CB_FMT(2, 3);
CB_DEF int          _F(file_which_is_newer)( const char* path_a, const char* path_b );
CB_DEF int          _F(file_check_dependencies)( const char* path, size_t len, const char** deps );

CB_DEF bool   _F(dir_create)( const char* path, bool should_fail_if_exists );
CB_DEF bool   _F(dir_remove)( const char* path, bool is_recursive );
CB_DEF bool   _F(dir_copy)( const char* dst, const char* src );
CB_DEF bool   _F(dir_move)( const char* dst, const char* src );
CB_DEF size_t _F(dir_walk)( const char* path, _T(DirWalkFN)* fn, void* params );

CB_DEF const char* _F(dir_cwd_query)(void);
CB_DEF bool        _F(dir_cwd_set)( const char* cwd );

CB_INL _T(Pipe) _F(pipe_null)(void);
CB_INL bool     _F(pipe_is_null)( _T(Pipe)* pipe );
CB_DEF bool     _F(pipe_open)( _T(Pipe)* out_read, _T(Pipe)* out_write );
CB_DEF void     _F(pipe_close)( _T(Pipe)* pipe );
CB_DEF bool     _F(pipe_wait)( _T(Pipe)* pipe, uint32_t wait_msec );
CB_DEF size_t   _F(pipe_query)( _T(Pipe)* pipe );

CB_DEF void _F(cmd_reset)( _T(CmdBuf)* cmd );
CB_DEF void _F(cmd_add)( _T(CmdBuf)* cmd, const char* arg );
CB_DEF void _F(cmd_copy)( _T(CmdBuf)* cmd, _T(Cmd) src );
CB_DEF void _F(cmd_free)( _T(CmdBuf)* cmd );
CB_DEF void _F(cmd_flatten)( _T(Cmd) cmd, _T(StringBuf)* string );

CB_DEF void _F(proc_group_add)( _T(ProcessGroup)* group, _T(Process)* proc );
CB_DEF void _F(proc_group_clean)( _T(ProcessGroup)* group );
CB_DEF void _F(proc_group_free)( _T(ProcessGroup)* group );
CB_DEF bool _F(proc_group_join)( _T(ProcessGroup)* group, uint32_t wait_msec );

CB_INL _T(Process) _F(proc_null)(void);
CB_INL bool        _F(proc_is_null)( _T(Process)* proc );
CB_DEF void        _F(proc_discard)( _T(Process)* proc );
CB_DEF int         _F(proc_wait)( _T(Process)* proc, uint32_t wait_msec );
CB_DEF bool        _F(proc_signal)( _T(Process)* proc, _T(ProcessSignal) sig );
CB_DEF bool        _F(proc_exists)( const char* path );

CB_DEF const char* _F(env_query)( const char* key );
CB_DEF bool        _F(env_set)( const char* key, const char* value );
CB_DEF void        _F(env_copy)( _T(Env)* out_env );
CB_DEF void        _F(env_reset)( _T(Env)* env );
CB_DEF void        _F(env_add)( _T(Env)* env, const char* key, const char* value );
CB_DEF void        _F(env_append)( _T(Env)* dst, _T(Env)* src );
CB_DEF _T(String)  _F(env_get)( _T(Env)* env, const char* key );
CB_DEF void        _F(env_remove)( _T(Env)* env, const char* key );
CB_DEF _T(String)  _F(env_deref)( _T(Env)* env, _T(StringOffset) so );
CB_DEF void        _F(env_free)( _T(Env)* env );

CB_DEF void _F(mt_sleep)( uint32_t wait_msec );
CB_DEF void _F(mt_yield)(void);
CB_DEF bool _F(mt_spinlock)( volatile int* atom, int sentinel, uint32_t wait_msec );
CB_DEF void _F(mt_barrier)(void);
CB_DEF int  _F(mt_add)( volatile int* atom, int addend );
CB_INL int  _F(mt_incr)( volatile int* atom );
CB_INL int  _F(mt_decr)( volatile int* atom );
CB_DEF int  _F(mt_exch)( volatile int* atom, int exch );
CB_DEF int  _F(mt_cmp_exch)( volatile int* atom, int cmp, int exch );

CB_DEF void _F(mtx_reset)( _T(Mutex)* mtx );
CB_DEF bool _F(mtx_lock)( _T(Mutex)* mtx, uint32_t wait_msec );
CB_DEF void _F(mtx_unlock)( _T(Mutex)* mtx );

CB_DEF void _F(semaphore_init)( _T(Semaphore)* sem, int* opt_value );
CB_DEF void _F(semaphore_signal)( _T(Semaphore)* sem );
CB_DEF bool _F(semaphore_wait)( _T(Semaphore)* sem, uint32_t wait_msec );

CB_DEF _T(Time)      _F(time_now)(void);
CB_DEF double        _F(time_diff)( _T(Time) a, _T(Time) b );
CB_DEF double        _F(time_msec)(void);
CB_DEF double        _F(time_sec)(void);
CB_DEF _T(TimeSplit) _F(time_split_from_time)( _T(Time) time );
CB_DEF _T(Time)      _F(time_from_time_split)( _T(TimeSplit)* ts );

CB_DEF _T(HashCode) _F(hash_elf)( _T(ByteSlice) bytes );
CB_DEF _T(HashCode) _F(hash_murmur2)( _T(ByteSlice) bytes );

CB_DEF void   _F(deps_set_target)( _T(Deps)* deps, const char* target );
CB_DEF void   _F(deps_add)( _T(Deps)* deps, const char* dependency );
CB_DEF void   _F(deps_reset)( _T(Deps)* deps );
CB_DEF void   _F(deps_free)( _T(Deps)* deps );
CB_DEF size_t _F(deps_parse_makefile)(
    const char* path, _T(Deps)* out_deps, bool can_replace_target );

// NOTE(alicia): inline functions

CB_INL _T(File) _F(file_null)(void) {
#if CB_OS_CURRENT == CB_OS_WINDOWS
    return CB_STRUCT( _T(File), .opaque_ptr=NULL );
#else
    return CB_STRUCT( _T(File), .opaque_fd=-1 );
#endif
}

CB_INL bool _F(file_is_null)( _T(File)* file ) {
#if CB_OS_CURRENT == CB_OS_WINDOWS
    return file->opaque_ptr == NULL;
#else
    return file->opaque_fd == -1;
#endif
}

CB_INL _T(Pipe) _F(pipe_null)(void) {
#if CB_OS_CURRENT == CB_OS_WINDOWS
    return CB_STRUCT( _T(Pipe), .opaque_ptr=NULL );
#else
    return CB_STRUCT( _T(Pipe), .opaque_fd=-1 );
#endif
}

CB_INL bool _F(pipe_is_null)( _T(Pipe)* pipe ) {
#if CB_OS_CURRENT == CB_OS_WINDOWS
    return pipe->opaque_ptr == NULL;
#else
    return pipe->opaque_fd == -1;
#endif
}

CB_INL _T(Process) _F(proc_null)(void) {
#if CB_OS_CURRENT == CB_OS_WINDOWS
    return CB_STRUCT( _T(Process), .opaque_ptr=NULL );
#else
    return CB_STRUCT( _T(Process), .opaque_fd=-1 );
#endif
}

CB_INL bool _F(proc_is_null)( _T(Process)* p ) {
#if CB_OS_CURRENT == CB_OS_WINDOWS
    return p->opaque_ptr == NULL;
#else
    return p->opaque_fd == -1;
#endif
}

CB_INL _T(String) _F(string_from_log_level)( _T(LogLevel) level ) {
    switch( level ) {
        case _E(LOG_INFO  ): return CB_STRING("LOG_INFO");
        case _E(LOG_WARN  ): return CB_STRING("LOG_WARN");
        case _E(LOG_ERROR ): return CB_STRING("LOG_ERROR");
        case _E(LOG_FATAL ): return CB_STRING("LOG_FATAL");
        case _E(LOG_NONE  ): return CB_STRING("LOG_NONE");
        case _E(LOG_COUNT ): break;
    }
    return CB_STRING("LOG_UNKNOWN");
}
CB_INL bool _F(log_level_from_string)( _T(String) string, _T(LogLevel)* out_level ) {
    for( size_t i = 0; i < (size_t)_E(LOG_COUNT); ++i ) {
        _T(String) current = _F(string_from_log_level)( (_T(LogLevel))i );
        if( _F(string_cmp)( current, string ) ) {
            *out_level = (_T(LogLevel))i;
            return true;
        }
    }
    return false;
}
CB_INL _T(String) _F(string_from_file_seek)( _T(FileSeek) seek ) {
    switch( seek ) {
        case _E(FS_SET): return CB_STRING("SET");
        case _E(FS_CUR): return CB_STRING("CUR");
        case _E(FS_END): return CB_STRING("END");
    }
    return CB_STRING("UNKNOWN");
}
CB_INL bool _F(file_seek_from_string)( _T(String) string, _T(FileSeek)* out_seek ) {
    for( size_t i = 0; i < (size_t)_E(FS_END) + 1; ++i ) {
        _T(String) current = _F(string_from_file_seek)( (_T(FileSeek))i );
        if( _F(string_cmp)( current, string ) ) {
            *out_seek = (_T(FileSeek))i;
            return true;
        }
    }
    return false;
}
CB_INL _T(String) _F(string_from_file_type)( _T(FileType) type ) {
    switch( type ) {
        case _E(FT_NULL): return CB_STRING("null");
        case _E(FT_FILE): return CB_STRING("file");
        case _E(FT_DIR):  return CB_STRING("directory");
        case _E(FT_PIPE): return CB_STRING("pipe");
    }
    return CB_STRING("unknown");
}
CB_INL bool _F(file_type_from_string)( _T(String) string, _T(FileType)* out_type ) {
    for( size_t i = 0; i < (size_t)_E(FT_PIPE) + 1; ++i ) {
        _T(String) current = _F(string_from_file_type)( (_T(FileType))i );
        if( _F(string_cmp)( current, string ) ) {
            *out_type = (_T(FileType))i;
            return true;
        }
    }
    return false;
}
CB_INL _T(String) _F(string_from_dir_walk_action)( _T(DirWalkAction) action ) {
    switch( action ) {
        case _E(DWA_CONTINUE): return CB_STRING("CONTINUE");
        case _E(DWA_STOP):     return CB_STRING("STOP");
        case _E(DWA_SKIP):     return CB_STRING("SKIP");
    }
    return CB_STRING("UNKNOWN");
}
CB_INL bool _F(dir_walk_action_from_string)(
    _T(String) string, _T(DirWalkAction)* out_action
) {
    for( size_t i = 0; i < (size_t)_E(DWA_SKIP) + 1; ++i ) {
        _T(String) current = _F(string_from_dir_walk_action)( (_T(DirWalkAction))i );
        if( _F(string_cmp)( current, string ) ) {
            *out_action = (_T(DirWalkAction))i;
            return true;
        }
    }
    return false;
}
CB_INL _T(String) _F(string_from_process_signal)( _T(ProcessSignal) sig ) {
    switch( sig ) {
        case _E(PSIG_NULL):  return CB_STRING("null");
        case _E(PSIG_KILL):  return CB_STRING("kill");
        case _E(PSIG_COUNT): break; 
    }
    return CB_STRING("unknown");
}
CB_INL bool _F(process_signal_from_string)(
    _T(String) string, _T(ProcessSignal)* out_sig
) {
    for( size_t i = 0; i < (size_t)_E(PSIG_COUNT); ++i ) {
        _T(String) current = _F(string_from_process_signal)( (_T(ProcessSignal))i );
        if( _F(string_cmp)( current, string ) ) {
            *out_sig = (_T(ProcessSignal))i;
            return true;
        }
    }
    return false;
}

CB_INL bool _F(path_exists)( const char* path ) {
    return _F(path_query_file_type)( path ) != _E(FT_NULL);
}
CB_INL _T(FileType) _F(path_query_file_type)( const char* path ) {
    _T(FileInfo) info = {};
    if( _F(path_query_info)( path, &info ) ) {
        return info.type;
    }
    return _E(FT_NULL);
}
CB_INL bool _F(path_query_time_create)( const char* path, _T(Time)* out_time ) {
    _T(FileInfo) info = {};
    if( _F(path_query_info)( path, &info ) ) {
        *out_time = info.time_create;
        return true;
    }
    return false;
}
CB_INL bool _F(path_query_time_modify)( const char* path, _T(Time)* out_time ) {
    _T(FileInfo) info = {};
    if( _F(path_query_info)( path, &info ) ) {
        *out_time = info.time_modify;
        return true;
    }
    return false;
}

CB_INL const char* _F(path_canonicalize)( const char* path ) {
    _T(StringBuf) l = _F(local_buf)(); 
    _F(path_canonicalize_buf)( &l, path );
    return l.ptr;
}

CB_INL size_t _F(file_query_size)( _T(File)* file ) {
    size_t seek   = _F(file_seek)( file, 0, _E(FS_CUR) );
    size_t result = _F(file_seek)( file, 0, _E(FS_END) );
    _F(file_seek)( file, seek, _E(FS_SET) );

    return result;
}

CB_INL _T(FileType) _F(file_query_type)( _T(File)* file ) {
    _T(FileInfo) info = _F(file_query_info)( file );
    return info.type;
}
CB_INL _T(Time) _F(file_query_time_create)( _T(File)* file ) {
    _T(FileInfo) info = _F(file_query_info)( file );
    return info.time_create;
}
CB_INL _T(Time) _F(file_query_time_modify)( _T(File)* file ) {
    _T(FileInfo) info = _F(file_query_info)( file );
    return info.time_modify;
}

CB_INL size_t _F(file_read_entire)( const char* path, _T(ByteBuf)* buf ) {
    _T(File) fp;
    if( !_F(file_open)( path, _E(FF_READ), &fp ) ) {
        return 0;
    }

    size_t fsize = _F(file_query_size)( &fp );

    CB_BUF_RESERVE( buf, fsize );

    size_t read_size = _F(file_read)( &fp, buf->ptr + buf->len, fsize );
    buf->len += read_size;

    _F(file_close)( &fp );
    return read_size;
}

CB_INL int _F(mt_incr)( volatile int* atom ) {
    return _F(mt_add)( atom, 1 );
}
CB_INL int _F(mt_decr)( volatile int* atom ) {
    return _F(mt_add)( atom, -1 );
}

// NOTE(alicia): internal functions

CB_INL void __cb_unused( int v, ... ) { (void)v; }

CB_DEF bool __cb_dir_create_many( const char* first, ... );

CB_DEF void __cb_cmd_append( _T(CmdBuf)* cmd, ... );

CB_DEF int __cb_exec( _T(Cmd) cmd, __CbExecParams params );


#endif /* header guard */













// NOTE(alicia): implementation

#if defined(CB_IMPL) && !defined(__CB_IMPL__H__)
#define __CB_IMPL__H__
/**
 * @file   cb.h
 * @brief  C Build System implementation.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 20, 2025
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <ftw.h>
#include <poll.h>

#if CB_OS_CURRENT == CB_OS_WINDOWS
    #define CB_LOCAL_SIZE_BYTES (CB_LOCAL_SIZE * 2)
#else
    #define CB_LOCAL_SIZE_BYTES (CB_LOCAL_SIZE)
#endif

#define CB_LOCAL_PLATFORM_COUNT (2)

#define _T(x) CB_CONCAT2( CB_PREFIX_TYPE, x )
#define _F(x) CB_CONCAT2( CB_PREFIX_FUNC, x )

void* __cb_default_alloc( void* ptr, size_t old_size, size_t new_size, void* params );
void  __cb_default_free( void* ptr, size_t size, void* params );

struct __CbFtwState {
    _T(DirWalkFN)* fn;
    void*          params;
    int            skip_level;
    size_t         count;
};

CB_TLS
struct __CbState {
    _T(AllocFN)* alloc;
    _T(FreeFN)*  free;
    void*        alloc_params;

    _T(File) h_stdout, h_stderr, h_stdin;

    char     local_buffer[CB_LOCAL_COUNT][CB_LOCAL_SIZE];
    uint32_t local_index;

    char     local_platform_buffer[CB_LOCAL_PLATFORM_COUNT][CB_LOCAL_SIZE_BYTES];
    uint32_t local_platform_index;

    bool cwd_set;
    char cwd[CB_MAX_PATH];

    _T(LogLevel) level;

    struct __CbFtwState ftw;
} __CB_STATE__ = {
    .alloc                 = __cb_default_alloc,
    .free                  = __cb_default_free,
    .alloc_params          = NULL,
    .h_stdout              = CB_STRUCT(_T(File), .opaque_fd = STDOUT_FILENO),
    .h_stderr              = CB_STRUCT(_T(File), .opaque_fd = STDERR_FILENO),
    .h_stdin               = CB_STRUCT(_T(File), .opaque_fd = STDIN_FILENO),
    .local_buffer          = {},
    .local_index           = 0,
    .local_platform_buffer = {},
    .local_platform_index  = 0,
    .cwd_set               = false,
    .cwd                   = {},
    .level                 = _E(LOG_INFO),
    .ftw                   = {},
};

typedef _T(Handle) _T(Thread);

struct CB_JobState {
    bool       is_running;
    int        result;
    _T(JobFN)* fn;
    void*      params;
};

#define cbs  __CB_STATE__
#define cbjq __CB_JOB_QUEUE__

double __cb_msec_from_timespec( struct timespec* ts ) {
    double result = 0.0;
    result += (double)ts->tv_nsec / 1000000.0;
    result += (double)ts->tv_sec * 1000.0;
    return result;
}
void __cb_timespec_from_msec( uint32_t msec, struct timespec* out_timespec ) {
    out_timespec->tv_sec  = msec / 1000;
    out_timespec->tv_nsec = (msec % 1000) * 1000000;
}
void __cb_timespec_from_msec_abs( uint32_t msec, struct timespec* out_timespec ) {
    struct timespec ts;
    _F(mem_set)( &ts, 0, sizeof(ts) );
    __cb_timespec_from_msec( msec, &ts );

    struct timespec current;
    clock_gettime( CLOCK_REALTIME, &current );

    ts.tv_nsec += current.tv_nsec;
    ts.tv_sec  += current.tv_sec;
    if( ts.tv_nsec >= 999999999 ) {
        ts.tv_sec++;
        ts.tv_nsec -= 999999999;
    }
    *out_timespec = ts;
}
void __cb_spinlock( volatile int* atom, int sentinel ) {
    for( ;; ) {
        _F(mt_cmp_exch)( atom, sentinel, sentinel );
        if( *atom == sentinel ) {
            break;
        }

        _F(mt_barrier)();
        _F(mt_yield)();
    }
}
void __cb_semaphore_wait( _T(Semaphore)* sem ) {
    for( ;; ) {
        _F(mtx_lock)( &sem->mtx, CB_WAIT_INF );
        if( sem->atom > 0 ) {
            _F(mt_decr)( &sem->atom );
            _F(mtx_unlock)( &sem->mtx );
            return;
        }
        _F(mtx_unlock)( &sem->mtx );
    }
}

void* __cb_default_alloc( void* ptr, size_t old_size, size_t new_size, void* params ) {
    (void)params;

    if( ptr ) {
        void* result = realloc( ptr, new_size );
        if( !result ) {
            return NULL;
        }
        memset( (char*)result + old_size, 0, new_size - old_size );
        return result;
    } else {
        return calloc( 1, new_size );
    }
}
void __cb_default_free( void* ptr, size_t size, void* params ) {
    (void)params;
    if( !ptr ) {
        return;
    }
    free(ptr);
}

void _F(build_init)(
    const char*  path_exec,
    const char*  path_src,
    int          argc,
    const char** argv
) {
    _T(CmdLine) cl = CB_CL( argc, argv );
    cl = CB_CL_NEXT( &cl );

    if( !_F(path_exists)( __FILE__ ) ) {
        CB_PANIC(
            "build_init: %s not found! build system MUST be run from source code directory!",
            __FILE__ );
    }
    if( !_F(path_exists)( path_src ) ) {
        CB_PANIC(
            "build_init: %s not found! build system MUST be run from source code directory!",
            path_src );
    }

    bool should_rebuild =
        _F(file_check_dependencies)( path_exec, 2, (const char*[]){ path_src, __FILE__ } ) != 0;

    if( !should_rebuild ) {
        const char* old_name = _F(local_fmt)( "%s.old", path_exec );
        if( _F(path_exists)( old_name ) ) {
            _F(file_remove)( old_name );
        }

        return;
    }

    CB_INFO( CB_COLOR_CYAN "changes detected in build source, rebuilding . . ." CB_COLOR_RESET );
    _F(build_rebuild)( path_exec, path_src, cl.len, cl.ptr, NULL, true );
}
_Noreturn
void _F(build_rebuild)(
    const char*  path_exec,
    const char*  path_src,
    int          argc,
    const char** argv,
    _T(Cmd)*     opt_cmd,
    bool         should_reload
) {
    bool is_windows = false;
    bool is_msvc    = false;

#if CB_COMPILER_CURRENT == CB_COMPILER_UNKNOWN
    #if __cplusplus
        _T(String) compiler = CB_STRING("c++");
    #else
        _T(String) compiler = CB_STRING("cc");
    #endif
#elif CB_COMPILER_CURRENT == CB_COMPILER_GCC
    #if __cplusplus
        _T(String) compiler = CB_STRING("g++");
    #else
        _T(String) compiler = CB_STRING("gcc");
    #endif
#elif CB_COMPILER_CURRENT == CB_COMPILER_CLANG
    #if __cplusplus
        _T(String) compiler = CB_STRING("clang++");
    #else
        _T(String) compiler = CB_STRING("clang");
    #endif
#elif CB_COMPILER_CURRENT == CB_COMPILER_MSVC
    _T(String) compiler = CB_STRING("cl");
    is_msvc = true;
#endif

#if CB_OS_CURRENT == CB_OS_WINDOWS
    is_windows = true;
#endif

    double start = _F(time_msec)();

    _T(String) exec = CB_STRING_FROM_CSTR( path_exec );

    _T(CmdBuf)    cmd    = {};
    _T(StringBuf) string = {};

    if( opt_cmd ) {
        _F(cmd_copy)( &cmd, *opt_cmd );
    } else {
        CB_CMD_APPEND( &cmd, compiler.ptr, path_src );

        if( is_msvc ) {
            _F(string_buf_fmt)( &string, "/Fe%" CB_STRING_FMT, CB_STRING_ARG(&exec) );
            CB_BUF_PUSH( &string, 0 );

            CB_CMD_APPEND( &cmd, "/nologo", string.ptr );

            string.len = 0;
        } else {
            CB_CMD_APPEND( &cmd, "-o", exec.ptr );
        }
    }

    _F(cmd_add)( &cmd, "-DCB_BOOTSTRAPPED" );

    _F(cmd_flatten)( cmd.cmd, &string );
    CB_BUF_PUSH( &string, 0 );

    CB_INFO( CB_COLOR_CYAN "rebuilding with command: %s" CB_COLOR_RESET, string.ptr );
    string.len = 0;

    _F(string_buf_fmt)( &string, "%s.old", exec.ptr );
    CB_BUF_PUSH( &string, 0 );

    switch( _F(path_query_file_type)( string.ptr ) ) {
        case _E(FT_NULL): break;
        case _E(FT_FILE): {
            if( !_F(file_remove)( string.ptr ) ) {
                CB_PANIC( "build_rebuild: failed to remove %s!", string.ptr );
            }
        } break;
        case _E(FT_PIPE):
        case _E(FT_DIR): {
            CB_PANIC( "build_rebuild: %s is not a file!", string.ptr );
        } break;
    }

    if( !_F(file_move)( string.ptr, exec.ptr, false ) ) {
        CB_PANIC( "build_rebuild: failed to rename existing executable!" );
    }

    _T(Process) pid = _F(proc_null)();

    if( CB_EXEC( cmd.cmd, .proc=&pid ) != 0 ) {
        CB_PANIC( "build_rebuild: failed to rebuild!" );
    }

    int exit_code = _F(proc_wait)( &pid, CB_WAIT_INF );
    if( exit_code ) {
        if( !_F(file_move)( exec.ptr, string.ptr, false ) ) {
            CB_ERROR( "build_rebuild: failed to rename executable!" );
        }
        CB_PANIC( "build_rebuild: failed to rebuild!" );
    }

    string.len = 0;

    if( is_msvc ) {
        _F(string_buf_fmt)( &string, "%" CB_STRING_FMT, CB_STRING_ARG(&exec) );
        size_t dotpos = 0;
        if( _F(string_find_rev)( string.slice, '.', &dotpos ) ) {
            string.len = dotpos;
        }
        _F(string_buf_fmt)( &string, ".obj" );
        CB_BUF_PUSH( &string, 0 );

        if( _F(path_exists)( string.ptr ) ) {
            _F(file_remove)( string.ptr );
        }
    }

    double end = _F(time_msec)();
    CB_INFO( "build_rebuild: rebuilt in %fmsec", end - start );

    if( !should_reload ) {
        exit(0);
    }

    if( is_windows ) {
        CB_WARN( "build_rebuild: cannot reload program on Windows! please run command again." );
        exit(0);
    }

    CB_INFO( CB_COLOR_CYAN "build_rebuild: reloading . . ." CB_COLOR_RESET );

    _F(cmd_reset)( &cmd );
    _F(cmd_add)( &cmd, exec.ptr );
    for( int i = 0; i < argc; ++i ) {
        _F(cmd_add)( &cmd, argv[i] );
    }

    exit_code = 0;
    if( (exit_code = CB_EXEC( cmd.cmd )) < 0 ) {
        CB_PANIC( "build_rebuild: failed to reload!" );
    }

    exit( exit_code );
}

bool _F(float_cmp)( float a, float b ) {
    float diff = a - b;
    if( diff < 0.0f ) {
        diff = -diff;
    }
    return diff < CB_FLT_EPSILON;
}
bool _F(double_cmp)( double a, double b ) {
    double diff = a - b;
    if( diff < 0.0 ) {
        diff = -diff;
    }
    return diff < CB_DBL_EPSILON;
}

void  _F(mem_allocator_set)( _T(AllocFN)* alloc, _T(FreeFN)* free, void* params ) {
    cbs.alloc        = alloc;
    cbs.free         = free;
    cbs.alloc_params = params;
}
void  _F(mem_allocator_reset)(void) {
    _F(mem_allocator_set)( __cb_default_alloc, __cb_default_free, NULL );
}
void* _F(mem_alloc)( size_t size ) {
    return cbs.alloc( NULL, 0, size, cbs.alloc_params );
}
void* _F(mem_realloc)( void* ptr, size_t old_size, size_t new_size ) {
    return cbs.alloc( ptr, old_size, new_size, cbs.alloc_params );
}
void  _F(mem_free)( void* ptr, size_t size ) {
    cbs.free( ptr, size, cbs.alloc_params );
}

bool _F(mem_cmp)( const void* a, const void* b, size_t size ) {
    return memcmp( a, b, size ) == 0;
}
void* _F(mem_copy)( void* restrict dst, const void* restrict src, size_t size ) {
    return memcpy( dst, src, size );
}
void* _F(mem_move)( void* dst, const void* src, size_t size ) {
    return memmove( dst, src, size );
}
void* _F(mem_set)( void* buf, uint8_t byte, size_t size ) {
    return memset( buf, byte, size );
}

_T(ByteBuf) _F(local_buf)(void) {
    _T(ByteBuf) result = {};

    uint32_t index = (cbs.local_index++) % CB_LOCAL_COUNT;

    result.ptr = _F(mem_set)( cbs.local_buffer[index], 0, CB_LOCAL_SIZE );
    result.cap = CB_LOCAL_SIZE;

    return result;
}
const char* _F(local_fmt_va)( const char* fmt, va_list va ) {
    _T(ByteBuf) buf = _F(local_buf)();

    vsnprintf( buf.ptr, buf.cap, fmt, va );

    return buf.ptr;
}
const char* _F(local_fmt)( const char* fmt, ... ) {
    va_list va;

    va_start( va, fmt );
    const char* result = _F(local_fmt_va)( fmt, va );
    va_end( va );

    return result;
}
const char* _F(local_path)( _T(String) string ) {
    _T(ByteBuf) buf = _F(local_buf)();
    if( string.len > buf.cap ) {
        string.len = buf.cap - 1;
    }
    return _F(mem_copy)( buf.ptr, string.ptr, string.len );
}

void _F(array_reverse)( void* base, size_t stride, size_t count, char* swap ) {
    for( size_t i = 0; i < (count / 2); ++i ) {
        size_t offset0 = i * stride;
        size_t offset1 = (count - 1 - i) * stride;

        _F(mem_copy)( swap, (char*)base + offset0, stride );

        _F(mem_copy)( (char*)base + offset0, (char*)base + offset1, stride );
        _F(mem_copy)( (char*)base + offset1, swap, stride );
    }
}

size_t _F(cstr_len)( const char* string ) {
    return (size_t)strlen( string );
}

bool _F(rune_is_ascii)( _T(Rune32) rune ) {
    return rune <= 0x7f;
}
_T(Rune32) _F(rune_from_cp8)( _T(CodeUnits8) cp8 ) {
    _T(Rune32) result;

    if( cp8.cp[0] < 0x80 ) {

        result = cp8.cp[0];
        
    } else if( (cp8.cp[0] & 0xE0) == 0xC0 ) {


        result = cp8.cp[0] & 0x1F;
        result = (result << 6) | (cp8.cp[1] & 0x3F);

    } else if( (cp8.cp[0] & 0xF0) == 0xE0 ) {

        result = cp8.cp[0] & 0x0F;
        result = (result << 6) | (cp8.cp[1] & 0x3F);
        result = (result << 6) | (cp8.cp[2] & 0x3F);

    } else if( (cp8.cp[0] & 0xF8) ) {

        result = cp8.cp[0] & 0x07;
        result = (result << 6) | (cp8.cp[1] & 0x37);
        result = (result << 6) | (cp8.cp[2] & 0x37);
        result = (result << 6) | (cp8.cp[3] & 0x37);

    } else {

        result = CB_CP32_REPLACEMENT_CHARACTER;

    }
    return result;
}

_T(CodeUnits8) _F(cp8_from_code_units)( uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3 ) {
    _T(CodeUnits8) result;
    result.cp[0] = c0;
    result.cp[1] = c1;
    result.cp[2] = c2;
    result.cp[3] = c3;
    return result;
}
size_t _F(cp8_from_string)( _T(String) string, _T(CodeUnits8)* out_cp8 ) {
    if( !string.len ) {
        return 0;
    }

    const uint8_t* uc = (const uint8_t*)string.ptr;

    if( !(uc[0] & ~0x7F) ) {
        *out_cp8 = _F(cp8_from_code_units)( uc[0], 0, 0, 0 );
        return 1;
    } else if( (uc[0] & 0xE0) == 0xC0 ) {
        if( string.len >= 2 ) {
            *out_cp8 = _F(cp8_from_code_units)( uc[0], uc[1], 0, 0 );
            return 2;
        } else {
            goto unicode_cp8_from_string_error;
        }
    } else if( (uc[0] & 0xF0) == 0xE0 ) {
        if( string.len >= 3 ) {
            *out_cp8 = _F(cp8_from_code_units)( uc[0], uc[1], uc[2], 0 );
            return 3;
        } else {
            goto unicode_cp8_from_string_error;
        }
    } else if( (uc[0] & 0xF8) == 0xF0 ) {
        if( string.len >= 4 ) {
            *out_cp8 = _F(cp8_from_code_units)( uc[0], uc[1], uc[2], uc[3] );
            return 4;
        } else {
            goto unicode_cp8_from_string_error;
        }
    }

unicode_cp8_from_string_error:
    *out_cp8 = CB_CP8_REPLACEMENT_CHARACTER;
    return 1;
}

_T(CodeUnits16) _F(cp16_from_code_units)( uint16_t c0, uint16_t c1 ) {
    _T(CodeUnits16) result;
    result.cp[0] = c0;
    result.cp[1] = c1;
    return result;
}

_T(Rune32) _F(cp32_from_code_units)( uint32_t c0 ) {
    return c0;
}

bool _F(string_cmp)( _T(String) a, _T(String) b ) {
    if( a.len != b.len ) {
        return false;
    }
    return _F(mem_cmp)( a.ptr, b.ptr, a.len );
}

bool __cb_find_ascii( _T(String) string, char c, size_t* opt_out_pos ) {
    CB_FOREACH( &string, {
        if( *v == c ) {
            if( opt_out_pos ) {
                *opt_out_pos = i;
            }
            return true;
        }
    }, v, i );
    
    return false;
}

bool _F(string_find)( _T(String) string, _T(Rune32) c, size_t* opt_out_pos ) {
    if( _F(rune_is_ascii)( c ) ) {
        return __cb_find_ascii( string, c, opt_out_pos );
    }

    _T(String) remaining, substr;
    remaining = substr = string;

    _T(Rune32) at = 0;

    while( (remaining = _F(string_unicode_next)( substr, &at )).len ) {
        if( c == at ) {
            if( opt_out_pos ) {
                *opt_out_pos = (size_t)substr.ptr - (size_t)string.ptr;
            }
            return true;
        }
        substr = remaining;
    }

    return false;
}

_T(FileType) __cb_file_type_from_stat( const struct stat* st ) {
    if( S_ISREG( st->st_mode ) ) {
        return _E(FT_FILE);
    } else if( S_ISDIR( st->st_mode ) ) {
        return _E(FT_DIR);
    } else {
        return _E(FT_NULL);
    }
}

bool __cb_find_set_ascii( _T(String) string, _T(Rune32Set) set, size_t* opt_out_pos ) {
    for( size_t i = 0; i < string.len; ++i ) {
        char c = string.ptr[i];
        for( size_t j = 0; j < set.len; ++j ) {
            if( c == set.ptr[j] ) {
                if( opt_out_pos ) {
                    *opt_out_pos = i;
                }
                return true;
            }
        }
    }
    return false;
}

bool _F(string_find_set)( _T(String) string, _T(Rune32Set) set, size_t* opt_out_pos ) {
    bool is_ascii = true;
    for( size_t i = 0; i < set.len; ++i ) {
        if( !_F(rune_is_ascii)( set.ptr[i] ) ) {
            is_ascii = false;
            break;
        }
    }
    if( is_ascii ) {
        return __cb_find_set_ascii( string, set, opt_out_pos );
    }

    _T(String) remaining, substr;
    remaining = substr = string;

    _T(Rune32) at = 0;

    while( (remaining = _F(string_unicode_next)( substr, &at )).len ) {
        for( size_t i = 0; i < set.len; ++i ) {
            _T(Rune32) c = set.ptr[i];

            if( c == at ) {
                if( opt_out_pos ) {
                    *opt_out_pos = (size_t)substr.ptr - (size_t)string.ptr;
                }
                return true;
            }
        }
        substr = remaining;
    }

    return false;
}
bool _F(string_find_phrase)( _T(String) string, _T(String) phrase, size_t* opt_out_pos ) {
    _T(String) substr = string;
    _T(Rune32) first  = _F(string_unicode_index)( phrase, 0 );
    while( substr.len ) {
        size_t pos = 0;
        if( !_F(string_find)( substr, first, &pos ) ) {
            return false;
        }

        substr = CB_SLICE_ADVANCE( &substr, pos );

        if( substr.len < phrase.len ) {
            return false;
        }

        _T(String) cmp = substr;
        cmp.len = phrase.len;

        substr = CB_SLICE_ADVANCE( &substr );

        if( _F(string_cmp)( cmp, phrase ) ) {
            if( opt_out_pos ) {
                *opt_out_pos = (size_t)cmp.ptr - (size_t)string.ptr;
                return true;
            }
        }
    }

    return false;
}

bool _F(string_find_rev)( _T(String) string, _T(Rune32) c, size_t* opt_out_pos ) {
    // TODO(alicia): UTF-8

    for( size_t i = string.len; i-- > 0; ) {
        if( string.ptr[i] == c ) {
            if( opt_out_pos ) {
                *opt_out_pos = i;
            }
            return true;
        }
    }

    return false;
}
bool _F(string_find_set_rev)( _T(String) string, _T(Rune32Set) set, size_t* opt_out_pos ) {
    // TODO(alicia): UTF-8

    for( size_t i = string.len; i-- > 0; ) {
        for( size_t j = 0; j < set.len; ++j ) {
            char c = set.ptr[j];

            if( string.ptr[i] == c ) {
                if( opt_out_pos ) {
                    *opt_out_pos = i;
                }
                return true;
            }
        }
    }

    return false;
}
bool _F(string_find_phrase_rev)( _T(String) string, _T(String) phrase, size_t* opt_out_pos ) {
    for( size_t i = string.len; i-- > 0; ) {
        _T(String) part;
        part.ptr = string.ptr + i;
        part.len = string.len - i;

        if( part.len >= phrase.len && part.ptr[0] == phrase.ptr[0] ) {
            part.len = phrase.len;

            if( _F(string_cmp)( part, phrase ) ) {
                if( opt_out_pos ) {
                    *opt_out_pos = i;
                }
                return true;
            }
        }
    }

    return false;
}

#define CB_WHITESPACE_SET { ' ', '\t', '\n', '\r' }

_T(String) _F(string_trim_leading_ws)( _T(String) string ) {
    const char ws[] = CB_WHITESPACE_SET;

    size_t offset = 0;
    for( size_t i = 0; i < string.len; ++i ) {
        for( size_t j = 0; j < CB_ARRAY_LEN(ws); ++j ) {
            if( string.ptr[i] != ws[j] ) {
                return CB_SLICE_ADVANCE( &string, i );
            }
        }
    }
    return CB_SLICE_ADVANCE( &string, string.len );
}
_T(String) _F(string_trim_trailing_ws)( _T(String) string ) {
    const char ws[] = CB_WHITESPACE_SET;

    size_t offset = 0;
    for( size_t i = string.len; i-- > 0; ) {
        for( size_t j = 0; j < CB_ARRAY_LEN(ws); ++j ) {
            if( string.ptr[i] != ws[j] ) {
                _T(String) result = string;
                result.len = i + 1;
                return result;
            }
        }
    }

    _T(String) result = string;
    result.len = 0;
    return result;
}
_T(String) _F(string_trim_ws)( _T(String) string ) {
    return _F(string_trim_leading_ws)( _F(string_trim_trailing_ws)( string ) );
}

#undef CB_WHITESPACE_SET

void _F(string_split)(
    _T(String)  string,
    size_t      pos,
    _T(String)* opt_out_left,
    _T(String)* opt_out_right
) {
    if( opt_out_left ) {
        opt_out_left->ptr = string.ptr;
        opt_out_left->len = pos;
    }
    if( opt_out_right ) {
        opt_out_right->ptr = string.ptr + pos;
        opt_out_right->len = string.len - pos;
    }
}

size_t _F(string_unicode_len)( _T(String) string ) {
    size_t res = 0;
    for( size_t i = 0; i < string.len; ++i ) {
        uint8_t byte = ((uint8_t*)string.ptr)[i];
        if( (byte & 0xC0) != 0x80 ) {
            res++;
        }
    }
    return res;
}
_T(String) _F(string_unicode_next)( _T(String) string, _T(Rune32)* out_c ) {
    _T(CodeUnits8) cp8 = {};
    size_t advance     = _F(cp8_from_string)( string, &cp8 );
    *out_c             = _F(rune_from_cp8)( cp8 );
    return CB_SLICE_ADVANCE( &string, advance );
}
_T(Rune32) _F(string_unicode_index)( _T(String) string, size_t index ) {
    size_t counter     = 0;
    size_t byte_offset = 0;
    for( ; byte_offset < string.len; ++byte_offset ) {
        if( counter == index ) {
            break;
        }
        uint8_t byte = *(((uint8_t*)string.ptr) + byte_offset);
        if( (byte & 0xC0) != 0x80 ) {
            counter++;
        }
    }

    _T(CodeUnits8) cp8 = {};
    _F(cp8_from_string)( CB_SLICE_ADVANCE( &string, byte_offset ), &cp8 );
    return _F(rune_from_cp8)( cp8 );
}

size_t _F(string_buf_fmt_va)( _T(StringBuf)* buf, const char* fmt, va_list va ) {
    va_list va2;
    va_copy( va2, va );

    size_t len = (size_t)vsnprintf( NULL, 0, fmt, va2 );

    va_end( va2 );

    CB_BUF_RESERVE( buf, len + 1 );

    size_t result = (size_t)vsnprintf( buf->ptr + buf->len, buf->cap - buf->len, fmt, va );

    buf->len += result;
    return result;
}
size_t _F(string_buf_fmt)( _T(StringBuf)* buf, const char* fmt, ... ) {
    va_list va;
    va_start( va, fmt );
    size_t result = _F(string_buf_fmt_va)( buf, fmt, va );
    va_end( va );
    return result;
}

_T(LogLevel) _F(logger_level_set)( _T(LogLevel) level ) {
    return cbs.level = level;
}
bool _F(logger_level_check)( _T(LogLevel) level ) {
    return (cbs.level < _E(LOG_NONE)) && level >= cbs.level;
}

size_t _F(logger_output_va)( _T(LogLevel) level, const char* fmt, va_list va ) {
    if( !_F(logger_level_check( level )) ) {
        return 0;
    }

    _T(File)*   file  = _F(stdout_handle)();
    const char* color = CB_COLOR_RESET;

    switch( level ) {
        case _E(LOG_COUNT):
        case _E(LOG_NONE):
        case _E(LOG_INFO): {
        } break;
        case _E(LOG_WARN): {
            color = CB_COLOR_YELLOW;
        } break;
        case _E(LOG_ERROR): {
            file  = _F(stderr_handle)();
            color = CB_COLOR_RED;
        } break;
        case _E(LOG_FATAL): {
            file  = _F(stderr_handle)();
            color = CB_COLOR_MAGENTA;
        } break;
    }

    size_t result = 0;
    result += _F(file_write_fmt)( file, "%s", color );
    result += _F(file_write_fmt_va)( file, fmt, va );
    result += _F(file_write_fmt)( file, CB_COLOR_RESET );

    return result;
}
size_t _F(logger_output)( _T(LogLevel) level, const char* fmt, ... ) {
    va_list va;
    va_start( va, fmt );
    size_t result = _F(logger_output_va)( level, fmt, va );
    va_end( va );
    return result;
}

_T(File)* _F(stdout_handle)(void) {
    return &cbs.h_stdout;
}
_T(File)* _F(stderr_handle)(void) {
    return &cbs.h_stderr;
}
_T(File)* _F(stdin_handle)(void) {
    return &cbs.h_stdin;
}

bool _F(path_query_info)( const char* path, _T(FileInfo)* out_info ) {
    _T(FileInfo) fi = {};
    fi.type = _E(FT_NULL);

    struct stat st;
    if( stat( path, &st ) ) {
        *out_info = fi;
        return false;
    }

    fi.size        = st.st_size;
    fi.time_modify = st.st_ctime;
    fi.time_create = st.st_mtime;
    fi.type        = __cb_file_type_from_stat( &st );

    *out_info = fi;

    return true;
}
size_t _F(path_canonicalize_buf)( _T(StringBuf)* buf, const char* path ) {
    char*  allocated = realpath( path, NULL );
    size_t len       = _F(cstr_len)( allocated );

    _T(String) string;
    string.len = len;
    string.ptr = allocated;

    size_t start_len = buf->len;

    CB_BUF_APPEND( buf, string.ptr, string.len );

    free(allocated);
    return buf->len - start_len;
}

bool _F(file_open)( const char* path, _T(FileFlags) flags, _T(File)* out_file ) {
    int    oflag = 0;
    mode_t mode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    // mode_t mode  = S_IRWXU;
    if( ( flags & (_E(FF_READ) | _E(FF_WRITE)) ) == (_E(FF_READ) | _E(FF_WRITE)) ) {
        oflag = O_RDWR;
    } else if( ( flags & (_E(FF_READ)) ) == (_E(FF_READ)) ) {
        oflag = O_RDONLY;
    } else if( ( flags & (_E(FF_WRITE)) ) == (_E(FF_WRITE)) ) {
        oflag = O_WRONLY;
    }

    if( ( flags & (_E(FF_APPEND)) ) == (_E(FF_APPEND)) ) {
        oflag |= O_APPEND;
    }

    if( ( flags & (_E(FF_CREATE)) ) == (_E(FF_CREATE)) ) {
        oflag |= O_CREAT | O_EXCL;

        if( (flags & (_E(FF_EXECUTE))) == (_E(FF_EXECUTE)) ) {
            mode |= S_IXUSR;
        }
    }
    if( ( flags & (_E(FF_TRUNCATE)) ) == (_E(FF_TRUNCATE)) ) {
        oflag |= O_TRUNC;
    }

    int fd = open( path, oflag, mode );

    if( fd < 0 ) {
        CB_ERROR( "file_open: %s", strerror(errno));
        return false;
    }

    out_file->opaque_fd = fd;

    return true;
}
void _F(file_close)( _T(File)* file ) {
    close( file->opaque_fd );
    file->opaque_fd = -1;
}
bool _F(file_remove)( const char* path ) {
    return unlink( path ) == 0;
}
_T(FileInfo) _F(file_query_info)( _T(File)* file ) {
    _T(FileInfo) fi = {};
    fi.type = _E(FT_NULL);

    struct stat st;
    if( fstat( file->opaque_fd, &st ) ) {
        return fi;
    }

    fi.size        = st.st_size;
    fi.time_modify = st.st_ctime;
    fi.time_create = st.st_mtime;
    fi.type        = __cb_file_type_from_stat( &st );

    return fi;
}
size_t _F(file_read)( _T(File)* file, void* dst, size_t size ) {
    return (size_t)read( file->opaque_fd, dst, size );
}
size_t _F(file_write)( _T(File)* file, const void* src, size_t size ) {
    return (size_t)write( file->opaque_fd, src, size );
}
bool _F(file_is_executable)( const char* path ) {
    return access( path, X_OK ) == 0;
}
bool _F(file_copy)( const char* dst, const char* src, bool fail_if_exists ) {
    _T(FileType) ft     = _F(path_query_file_type)( dst );
    bool         exists = ft != _E(FT_NULL);

    if( fail_if_exists && exists ) {
        CB_ERROR(
            "file_copy: file '%s' already exists! type: %s",
            dst, _F(string_from_file_type)(ft).ptr );
        return false;
    }

    char temp[1024] = {};

    _T(File) dstf, srcf;
    _T(FileFlags) dst_flags, src_flags;
    dst_flags = _E(FF_WRITE);
    src_flags = _E(FF_READ);

    if( exists ) {
        dst_flags |= _E(FF_TRUNCATE);
    } else {
        dst_flags |= _E(FF_CREATE);
    }
    if( _F(file_is_executable)( dst ) ) {
        dst_flags |= _E(FF_EXECUTE);
    }

    if( !_F(file_open)( dst, dst_flags, &dstf ) ) {
        return false;
    }
    if( !_F(file_open)( src, src_flags, &srcf ) ) {
        _F(file_close)( &dstf );
        return false;
    }

    size_t rem = _F(file_query_size)( &srcf );
    while( rem ) {
        size_t read_size = rem;
        if( read_size > sizeof(temp) ) {
            read_size = sizeof(temp);
        }

        _F(file_read)( &srcf, temp, read_size );
        _F(file_write)( &dstf, temp, read_size );

        rem -= read_size;
    }

    _F(file_close)( &srcf );
    _F(file_close)( &dstf );
    return true;
}
bool _F(file_move)( const char* dst, const char* src, bool fail_if_exists ) {
    if( !_F(file_copy)( dst, src, fail_if_exists ) ) {
        return false;
    }

    unlink( src );
    return true;
}
intptr_t _F(file_seek)( _T(File)* file, intptr_t offset, _T(FileSeek) type ) {
    return lseek( file->opaque_fd, offset, type );
}
size_t _F(file_write_fmt_va)( _T(File)* file, const char* fmt, va_list va ) {
    int res = vdprintf( file->opaque_fd, fmt, va );
    if( res >= 0 ) {
        return (size_t)res;
    } else {
        return 0;
    }
}
size_t _F(file_write_fmt)( _T(File*) file, const char* fmt, ... ) {
    va_list va;
    va_start( va, fmt );
    size_t res = _F(file_write_fmt_va)( file, fmt, va );
    va_end( va );
    return res;
}
int _F(file_which_is_newer)( const char* path_a, const char* path_b ) {
    _T(Time) time_a, time_b;

    if( !_F(path_query_time_create)( path_a, &time_a ) ) {
        return -1;
    }
    if( !_F(path_query_time_create)( path_b, &time_b ) ) {
        return -2;
    }

    return _F(time_diff)( time_a, time_b ) < 0.0;
}
int _F(file_check_dependencies)( const char* path, size_t len, const char** deps ) {
    _T(Time) t;
    
    if( !_F(path_query_time_create)( path, &t ) ) {
        return -1;
    }

    for( size_t i = 0; i < len; ++i ) {
        _T(Time) other;

        if( !_F(path_query_time_create)( deps[i], &other ) ) {
            int result = (int)i;
            return -(result + 2);
        }

        double diff = _F(time_diff)( t, other );
        if( diff < 0.0 ) {
            return i + 1;
        } else if( diff > 0.0 ) {
            continue;
        } else {
            continue;
        }
    }

    return 0;
}

bool _F(dir_create)( const char* path, bool should_fail_if_exists ) {
    if( should_fail_if_exists ) {
        _T(FileType) ft = _F(path_query_file_type)( path );
        if( ft != _E(FT_NULL) ) {
            CB_ERROR(
                "dir_create: '%s' already exists! type: %s",
                path, _F(string_from_file_type)(ft).ptr );
            return false;
        }
    }

    if( mkdir( path, S_IRWXU ) ) {
        CB_ERROR( "dir_create: %s", strerror( errno ) );
        return false;
    }
    return true;
}

struct __CbDirRemoveRecursiveFiles {
    bool is_successful;
};

_T(DirWalkAction) __cb_dir_remove_recursive_files(
    const char* path, _T(DirWalkInfo)* info, void* params
) {
    struct __CbDirRemoveRecursiveFiles* p = (struct __CbDirRemoveRecursiveFiles*)params;
    if( info->file_type == _E(FT_FILE) ) {
        if( !_F(file_remove)( path ) ) {
            p->is_successful = false;
            return _E(DWA_STOP);
        }
    } else {

        if( !_F(dir_remove)( path, true ) ) {
            return _E(DWA_STOP);
        }

        return _E(DWA_SKIP);
    }
    return _E(DWA_CONTINUE);
}

bool _F(dir_remove)( const char* path, bool is_recursive ) {
    if( is_recursive ) {
        struct __CbDirRemoveRecursiveFiles p = {};
        p.is_successful = true;

        _F(dir_walk)( path, __cb_dir_remove_recursive_files, &p );

        if( !p.is_successful ) {
            return false;
        }

        return _F(dir_remove)( path, false );
    } else {
        if( rmdir( path ) ) {
            CB_ERROR( "dir_remove: '%s' %s", path, strerror(errno) );
            return false;
        } else {
            return true;
        }
    }
}

struct __CbDirCopyState {
    _T(StringBuf) string;
    size_t        base_len;
    bool          is_successful;
};

_T(DirWalkAction) __cb_dir_copy( const char* path, _T(DirWalkInfo)* info, void* params ) {
    struct __CbDirCopyState* state = (struct __CbDirCopyState*)params;

    _T(String) p;
    p.ptr = (char*)path;
    p.len = info->path_len;

    CB_BUF_APPEND( &state->string, p.ptr, p.len + 1 );

    if( info->file_type == _E(FT_FILE) ) {
        state->is_successful = _F(file_copy)( state->string.ptr, path, true );
    } else if( info->file_type == _E(FT_DIR) ) {
        state->is_successful = _F(dir_create)( state->string.ptr, true );
    }

    state->string.len = state->base_len;

    if( !state->is_successful ) {
        return _E(DWA_STOP);
    }

    return _E(DWA_CONTINUE);
}

bool _F(dir_copy)( const char* dst, const char* src ) {
    struct __CbDirCopyState state = {};

    _T(String) dst_path = CB_STRING_FROM_CSTR( dst );

    if( !_F(dir_create)( dst, true ) ) {
        return false;
    }

    CB_BUF_APPEND( &state.string, dst_path.ptr, dst_path.len );
    CB_BUF_PUSH( &state.string, '/' );

    state.base_len = state.string.len;

    _F(dir_walk)( src, __cb_dir_copy, &state );

    CB_BUF_FREE( &state.string );

    return state.is_successful;
}

bool _F(dir_move)( const char* dst, const char* src ) {
    if( !_F(dir_copy)( dst, src ) ) {
        return false;
    }

    return _F(dir_remove)( src, true );
}

int __cb_nftw( const char* fpath, const struct stat* st, int typeflag, struct FTW* ftwbuf ) {
    CB_UNUSED(typeflag);

    _T(String) path = CB_STRING_FROM_CSTR(fpath);

    struct __CbFtwState* state = &cbs.ftw;

    if( !state->count ) {
        state->count++;
        return 0;
    }

    if( state->skip_level >= 0 ) {
        if( ftwbuf->level == state->skip_level ) {
            return 0;
        } else {
            state->skip_level = -1;
        }
    }

    _T(DirWalkInfo) info = {};
    info.file_type        = __cb_file_type_from_stat( st );
    info.file_time_create = st->st_ctime;
    info.file_time_modify = st->st_mtime;
    info.file_size        = st->st_size;
    info.path_len         = path.len;
    info.path_name_offset = ftwbuf->base;

    for( size_t i = path.len; i-- > 0; ) {
        if( path.ptr[i] == '/' ) {
            break;
        }
        if( path.ptr[i] == '.' ) {
            info.path_has_ext    = true;
            info.path_ext_offset = i;
            break;
        }
    }

    info.level = ftwbuf->level;

    state->count++;

    switch( state->fn( fpath, &info, state->params ) ) {
        case _E(DWA_CONTINUE): break;
        case _E(DWA_STOP):     return 1;
        case _E(DWA_SKIP): {
            state->skip_level = ftwbuf->level + 1;
        } break;
    }

    return 0;
}

size_t _F(dir_walk)( const char* path, _T(DirWalkFN)* fn, void* params ) {
    struct __CbFtwState* state = _F(mem_set)( &cbs.ftw, 0, sizeof(cbs.ftw) );

    state->fn         = fn;
    state->params     = params;
    state->skip_level = -1;
    state->count      = 0;

    if( nftw( path, __cb_nftw, 5, 0 ) < 0 ) {
        CB_ERROR( "dir_walk: %s", strerror(errno) );
        return 0;
    }
    return state->count;
}

const char* _F(dir_cwd_query)(void) {
    if( !cbs.cwd_set ) {
        cbs.cwd_set = true;
        getcwd( cbs.cwd, CB_MAX_PATH - 1 );
    }
    return cbs.cwd;
}
bool _F(dir_cwd_set)( const char* cwd ) {
    size_t len = _F(cstr_len)( cwd );
    if( len > (CB_MAX_PATH - 1) ) {
        CB_ERROR(
            "dir_cwd_set: path provided is too long! len: %zu max: %zu",
            len, (size_t)(CB_MAX_PATH - 1) );
        return false;
    }

    if( chdir( cwd ) ) {
        CB_ERROR( "dir_cwd_set: %s", strerror(errno) );
        return false;
    }

    CB_INFO( "chdir: '%s'", cwd );

    cbs.cwd_set = true;
    _F(mem_copy)( cbs.cwd, cwd, len + 1 );
    return true;
}

bool _F(pipe_open)( _T(Pipe)* out_read, _T(Pipe)* out_write ) {
    int fd[2];
    if( pipe( fd ) ) {
        CB_ERROR( "pipe_open: %s", strerror(errno) );
        return false;
    }

    out_read->opaque_fd  = fd[0];
    out_write->opaque_fd = fd[1];
    return true;
}
void _F(pipe_close)( _T(Pipe)* pipe ) {
    close( pipe->opaque_fd );
    pipe->opaque_fd = -1;
}
bool _F(pipe_wait)( _T(Pipe)* pipe, uint32_t wait_msec ) {
    int timeout = wait_msec;
    if( wait_msec == CB_WAIT_INF ) {
        timeout = -1;
    }
    struct pollfd pfd = {};
    pfd.fd     = pipe->opaque_fd;
    pfd.events = POLLIN;
    if( poll( &pfd, 1, timeout ) <= 0 ) {
        return false;
    }
    return true;
}
size_t _F(pipe_query)( _T(Pipe)* pipe ) {
    int result = 0;
    if( ioctl( pipe->opaque_fd, FIONREAD, &result ) == 0 ) {
        return (size_t)result;
    }
    return 0;
}

void _F(cmd_reset)( _T(CmdBuf)* cmd ) {
    cmd->len        = 0;
    cmd->string.len = 0;
}
void _F(cmd_add)( _T(CmdBuf)* cmd, const char* arg ) {
    const char* ptr = NULL;
    if( arg ) {
        size_t arglen = _F(cstr_len)( arg );
        size_t rem    = (cmd->string.cap - cmd->string.len);
        if( rem < (arglen + 1) ) {
            for( size_t i = 0; i < cmd->len; ++i ) {
                size_t offset = (size_t)cmd->ptr[i] - (size_t)cmd->string.ptr;

                cmd->ptr[i] = (const char*)offset;
            }

            size_t old_size = cmd->string.cap;
            size_t new_size = cmd->string.cap + ((arglen + 1) - rem) + 16;

            cmd->string.ptr = _F(mem_realloc)( cmd->string.ptr, old_size, new_size );
            cmd->string.cap = new_size;

            for( size_t i = 0; i < cmd->len; ++i ) {
                size_t offset = (size_t)cmd->ptr[i];

                cmd->ptr[i] = cmd->string.ptr + offset;
            }
        }

        ptr = cmd->string.ptr + cmd->string.len;
        _F(mem_copy)( cmd->string.ptr + cmd->string.len, arg, arglen + 1 );
        cmd->string.len += arglen + 1;
    }

    if( (cmd->cap - cmd->len) < 2 ) {
        CB_BUF_RESERVE( cmd, 8 );
    }
    cmd->ptr[cmd->len++] = ptr;
    cmd->ptr[cmd->len]   = NULL;
}
void _F(cmd_copy)( _T(CmdBuf)* cmd, _T(Cmd) src ) {
    for( size_t i = 0; i < src.len; ++i ) {
        _F(cmd_add)( cmd, src.ptr[i] );
    }
}
void _F(cmd_free)( _T(CmdBuf)* cmd ) {
    CB_BUF_FREE( &cmd->string );
    CB_BUF_FREE( cmd );
}
void _F(cmd_flatten)( _T(Cmd) cmd, _T(StringBuf)* string ) {
    for( size_t i = 0; i < cmd.len; ++i ) {
        if( !cmd.ptr[i] ) {
            break;
        }

        _T(String) arg = CB_STRING_FROM_CSTR( cmd.ptr[i] );
        arg = _F(string_trim_ws)( arg );

        if( !arg.len ) {
            continue;
        }

        if(
            arg.ptr[0] == '"' &&
            arg.ptr[arg.len - 1] == '"'
        ) {
            arg = CB_SLICE_ADVANCE( &arg );
            if( arg.len ) {
                arg.len--;
            }
        }

        if( !arg.len ) {
            continue;
        }

        CB_BUF_APPEND( string, arg.ptr, arg.len );
        if( (i + 1) != cmd.len ) {
            CB_BUF_PUSH( string, ' ' );
        }
    }
}

void _F(proc_group_add)( _T(ProcessGroup)* group, _T(Process)* proc ) {
    _T(ProcessData) pd = {};
    pd.handle = *proc;
    pd.result = -2;

    for( size_t i = 0; i < group->len; ++i ) {
        if( _F(mem_cmp)( &group->ptr[i].handle, &pd.handle, sizeof(pd.handle)) ) {
            return;
        }
    }

    CB_BUF_PUSH( group, pd );
}
void _F(proc_group_clean)( _T(ProcessGroup)* group ) {
    for( size_t i = 0; i < group->len; ++i ) {
        if( group->ptr[i].result >= -1 ) {
            CB_BUF_REMOVE_SWAP( group, i );
            i--;
        }
    }
}
void _F(proc_group_free)( _T(ProcessGroup)* group ) {
    CB_BUF_FREE( group );
}
bool __cb_proc_group_join_inf( _T(ProcessGroup)* group ) {
    for( ;; ) {
        size_t complete_count = 0;

        for( size_t i = 0; i < group->len; ++i ) {
            _T(ProcessData)* pd = group->ptr + i;

            if( pd->result >= -1 ) {
                complete_count++;
                continue;
            }

            pd->result = _F(proc_wait)( &pd->handle, CB_WAIT_INF );

            complete_count++;
        }

        if( complete_count >= group->len ) {
            break;
        }

        _F(mt_yield)();
    }

    return true;
}
bool _F(proc_group_join)( _T(ProcessGroup)* group, uint32_t wait_msec ) {
    if( wait_msec == CB_WAIT_INF ) {
        return __cb_proc_group_join_inf( group );
    }

    double start = _F(time_msec)();

    for( ;; ) {
        size_t complete_count = 0;

        for( size_t i = 0; i < group->len; ++i ) {
            _T(ProcessData)* pd = group->ptr + i;

            if( pd->result >= -1 ) {
                complete_count++;
                continue;
            }

            int wait_result = _F(proc_wait)( &pd->handle, 0 );

            if( wait_result >= -1 ) {
                pd->result = wait_result;
            } else {
                return false;
            }
        }

        if( complete_count >= group->len ) {
            break;
        }

        double elapsed = _F(time_msec)() - start;
        if( (uint32_t)elapsed >= wait_msec ) {
            return false;
        }

        _F(mt_yield)();
    }

    return true;
}

void _F(proc_discard)( _T(Process)* proc ) {
    CB_UNUSED(proc);
}
int __cb_proc_wait( _T(Process)* proc ) {
    int wstatus = 0;
    pid_t value = waitpid( proc->opaque_fd, &wstatus, 0 );
    if( value < 0 ) {
        CB_ERROR( "proc_wait: %s", strerror(errno) );
        return -2;
    }
    if( WIFEXITED( wstatus ) ) {
        return WEXITSTATUS( wstatus );
    } else {
        return -1;
    }
}
int _F(proc_wait)( _T(Process)* proc, uint32_t wait_msec ) {
    if( wait_msec == CB_WAIT_INF ) {
        return __cb_proc_wait( proc );
    }

    double start = _F(time_msec)();
    for( ;; ) {
        double current = _F(time_msec)();
        double elapsed = current - start;
        if( elapsed >= (double)wait_msec ) {
            break;
        }

        int wstatus = 0;
        pid_t value = waitpid( proc->opaque_fd, &wstatus, WNOHANG );

        if( value == 0 ) {
            _F(mt_yield)();
            continue;
        }

        if( value < 0 ) {
            CB_ERROR( "proc_wait: %s", strerror(errno) );
            return -1;
        }

        if( WIFEXITED(wstatus) ) {
            return WEXITSTATUS(wstatus);
        } else {
            return -3;
        }
    }

    return -2;
}
bool _F(proc_signal)( _T(Process)* proc, _T(ProcessSignal) sig ) {
    int signal = 0;
    switch( sig ) {
        case _E(PSIG_KILL): {
            return kill( proc->opaque_fd, SIGKILL ) == 0;
        } break;
        case _E(PSIG_NULL):
        case _E(PSIG_COUNT): break;
    }

    return false;
}
bool _F(proc_exists)( const char* path ) {
    char mini_buf[255 + sizeof("which %s > /dev/null 2>&1")] = {};
    snprintf( mini_buf, sizeof(mini_buf), "which %s > /dev/null 2>&1", path );
    return system( mini_buf ) == 0;
}

const char* _F(env_query)( const char* key ) {
    return getenv( key );
}
bool _F(env_set)( const char* key, const char* value ) {
    if( setenv( key, value, true ) ) {
        CB_ERROR( "env_set: %s", strerror(errno) );
        return false;
    }
    return true;
}

extern char** environ;

void _F(env_copy)( _T(Env)* out_env ) {
    for( char** env = environ; *env; env++ ) {
        _T(String) kv;
        kv.len = _F(cstr_len)( *env );
        kv.ptr = *env;

        _T(String) key;
        _T(String) value;

        size_t eq = 0;
        if( _F(string_find)( kv, '=', &eq ) ) {
            _F(string_split)( kv, eq, &key, &value );
            value = CB_SLICE_ADVANCE( &value );

            const char* k = _F(local_fmt)( "%" CB_STRING_FMT, CB_STRING_ARG(&key) );
            const char* v = _F(local_fmt)( "%" CB_STRING_FMT, CB_STRING_ARG(&value) );

            _F(env_add)( out_env, k, v );
        }

    }
}
void _F(env_append)( _T(Env)* dst, _T(Env)* src ) {
    for( size_t i = 0; i < src->len; ++i ) {
        _T(String) key, value;
        key   = _F(env_deref)( src, src->ptr[i].key );
        value = _F(env_deref)( src, src->ptr[i].value );
        _F(env_add)( dst, key.ptr, value.ptr );
    }
}
void _F(env_reset)( _T(Env)* env ) {
    env->len        = 0;
    env->string.len = 0;
}
_T(StringOffset) __cb_env_new_string( _T(Env)* env, _T(String) src ) {
    _T(StringOffset) result;
    result.len = src.len;

    CB_BUF_RESERVE( &env->string, src.len );

    result.offset = env->string.len;
    CB_BUF_APPEND( &env->string, src.ptr, src.len + 1 );

    return result;
}
void __cb_env_set( _T(Env)* env, size_t index, _T(String) value ) {
    env->ptr[index].value = __cb_env_new_string( env, value );
}
void __cb_env_add( _T(Env)* env, _T(String) key, _T(String) value ) {
    _T(EnvKV) kv = {};
    kv.key   = __cb_env_new_string( env, key );
    kv.value = __cb_env_new_string( env, value );

    CB_BUF_PUSH( env, kv );
}
void _F(env_add)( _T(Env)* env, const char* key_cc, const char* value_cc ) {
    _T(String) key, value;
    key.len = _F(cstr_len)( key_cc );
    key.ptr = (char*)key_cc;

    value.len = _F(cstr_len)( value_cc );
    value.ptr = (char*)value_cc;

    bool   key_found = false;
    size_t key_index = 0;
    for( size_t i = 0; i < env->len; ++i ) {
        _T(String) current = _F(env_deref)( env, env->ptr[i].key );

        if( _F(string_cmp)( key, current ) ) {
            key_found = true;
            key_index = i;
            break;
        }
    }

    if( key_found ) {
        __cb_env_set( env, key_index, value );
    } else {
        __cb_env_add( env, key, value );
    }
}
_T(String) _F(env_get)( _T(Env)* env, const char* key ) {
    _T(String) skey;
    skey.len = _F(cstr_len)( key );
    skey.ptr = (char*)key;

    _T(String) result = {};
    CB_FOREACH( env, {
        _T(String) current = _F(env_deref)( env, kv->key );

        if( _F(string_cmp)( current, skey ) ) {
            result = _F(env_deref)( env, kv->value );
            break;
        }
    }, kv );

    return result;
}
void _F(env_remove)( _T(Env)* env, const char* key ) {
    _T(String) string;
    string.len = _F(cstr_len)( key );
    string.ptr = (char*)key;

    for( size_t i = 0; i < env->len; ++i ) {
        _T(String) current = _F(env_deref)( env, env->ptr[i].key );

        if( _F(string_cmp)( string, current ) ) {
            env->len--;
            if( env->len ) {
                env->ptr[i] = env->ptr[env->len];
            }

            break;
        }
    }
}
_T(String) _F(env_deref)( _T(Env)* env, _T(StringOffset) so ) {
    _T(String) result;
    result.len = so.len;
    result.ptr = env->string.ptr + so.offset;
    return result;
}
void _F(env_free)( _T(Env)* env ) {
    CB_BUF_FREE( &env->string );
    CB_BUF_FREE( env );
    // NOTE(alicia): CB_BUF_FREE will set all fields to zero
}

void _F(mt_sleep)( uint32_t wait_msec ) {
    if( !wait_msec ) {
        return;
    }

    struct timespec ts;
    _F(mem_set)( &ts, 0, sizeof(ts) );
    __cb_timespec_from_msec( wait_msec, &ts );

    struct timespec rem;
    _F(mem_set)( &rem, 0, sizeof(rem) );

    while( clock_nanosleep( CLOCK_REALTIME, 0, &ts, &rem ) ) {
        ts = rem;
    }
}
void _F(mt_yield)(void) {
    sched_yield();
}
bool _F(mt_spinlock)( volatile int* atom, int sentinel, uint32_t wait_msec ) {
    if( wait_msec == CB_WAIT_INF ) {
        __cb_spinlock( atom, sentinel );
        return true;
    }

    double start = _F(time_msec)();
    for( ;; ) {
        double current = _F(time_msec)();
        double elapsed = current - start;
        if( elapsed >= (double)wait_msec ) {
            return false;
        }

        _F(mt_barrier)();

        _F(mt_cmp_exch)( atom, sentinel, sentinel );
        if( *atom == sentinel ) {
            break;
        }

        _F(mt_barrier)();
        _F(mt_yield)();
    }

    return true;
}
void _F(mt_barrier)(void) {
    // TODO(alicia): ARM or other CPUs
    __asm__ volatile ("mfence":::"memory");
}
int _F(mt_add)( volatile int* atom, int addend ) {
    return __sync_fetch_and_add( atom, addend );
}
int _F(mt_exch)( volatile int* atom, int exch ) {
    return __sync_val_compare_and_swap( atom, *atom, exch );
}
int _F(mt_cmp_exch)( volatile int* atom, int cmp, int exch ) {
    return __sync_val_compare_and_swap( atom, cmp, exch );
}

void _F(mtx_reset)( _T(Mutex)* mtx ) {
    _F(mt_exch)( &mtx->atom, 0 );
}
bool _F(mtx_lock)( _T(Mutex)* mtx, uint32_t wait_msec ) {
    if( !_F(mt_spinlock)( &mtx->atom, 0, wait_msec ) ) {
        return false;
    }

    _F(mt_exch)( &mtx->atom, 1 );
    return true;
}
void _F(mtx_unlock)( _T(Mutex)* mtx ) {
    _F(mt_exch)( &mtx->atom, 0 );
}

void _F(semaphore_init)( _T(Semaphore)* sem, int* opt_value ) {
    if( opt_value ) {
        sem->atom = *opt_value;
    } else {
        sem->atom = 0;
    }
    _F(mtx_reset)( &sem->mtx );
}
void _F(semaphore_signal)( _T(Semaphore)* sem ) {
    _F(mtx_lock)( &sem->mtx, CB_WAIT_INF );
    _F(mt_incr)( &sem->atom );
    _F(mtx_unlock)( &sem->mtx );
}
bool _F(semaphore_wait)( _T(Semaphore)* sem, uint32_t wait_msec ) {
    if( wait_msec == CB_WAIT_INF ) {
        __cb_semaphore_wait( sem );
        return true;
    }

    double   start = _F(time_msec)();
    uint32_t rem   = wait_msec;
    for( ;; ) {
        if( !_F(mtx_lock)( &sem->mtx, rem ) ) {
            return false;
        }

        if( sem->atom > 0 ) {
            _F(mt_decr)( &sem->atom );
            _F(mtx_unlock)( &sem->mtx );
            return true;
        }

        _F(mtx_unlock)( &sem->mtx );
        double current = _F(time_msec)();
        double elapsed = current - start;

        if( (uint32_t)elapsed > wait_msec ) {
            return false;
        }

        rem -= (uint32_t)elapsed;
    }
}

_T(Time) _F(time_now)(void) {
    return time(NULL);
}
double _F(time_diff)( _T(Time) a, _T(Time) b ) {
    return difftime( a, b );
}
double _F(time_msec)(void) {
    struct timespec ts;
    clock_gettime( CLOCK_MONOTONIC_RAW, &ts );

    return __cb_msec_from_timespec( &ts );
}
double _F(time_sec)(void) {
    return _F(time_msec)() / 1000.0;
}
_T(TimeSplit) _F(time_split_from_time)( _T(Time) time ) {
    struct tm tm;
    localtime_r( &time, &tm );
    _T(TimeSplit) result;

    result.year   = tm.tm_year + 1900;
    result.month  = tm.tm_mon + 1;
    result.day    = tm.tm_mday;
    result.hour   = tm.tm_hour;
    result.minute = tm.tm_min;
    result.second = tm.tm_sec % 60;

    return result;
}
_T(Time) _F(time_from_time_split)( _T(TimeSplit)* ts ) {
    struct tm tm;
    tm.tm_year = ts->year  - 1900;
    tm.tm_mon  = ts->month - 1;
    tm.tm_mday = ts->day;
    tm.tm_hour = ts->hour;
    tm.tm_min  = ts->minute;
    tm.tm_sec  = ts->second;

    return mktime( &tm );
}

_T(HashCode) _F(hash_elf)( _T(ByteSlice) bytes ) {
    _T(HashCode) result = {};
    uint32_t hi, lo;
    hi = lo = 0;

    for( size_t i = 0; i < bytes.len; ++i ) {
        uint32_t carry = (lo >> 28) & 0xF;
        lo = (lo << 4) + bytes.ptr[i];
        hi = (hi << 4) | carry;

        uint32_t x = hi & 0xF0000000u;
        if( x ) {
            hi ^= x >> 24;
        }
        hi &= ~x;
    }

    result.lo = lo;
    result.hi = hi;

    return result;
}
_T(HashCode) _F(hash_murmur2)( _T(ByteSlice) bytes ) {
    // TODO(alicia): convert to use only uint32_t to maintain compat with 32-bit?

    #define M           0xc6a4a7935bd1e995
    #define R           47
    #define SEED        0xA13F346234624
    #define FALLTHROUGH __attribute__((fallthrough))

    uint64_t h = SEED ^ ( bytes.len * M );

    const uint64_t* data = (const uint64_t*)bytes.ptr;
    const uint64_t* end  = data + (bytes.len / sizeof(uint64_t));

    while( data != end ) {
        uint64_t k = *data++;

        k *= M;
        k ^= k >> R;
        k *= M;

        h ^= k;
        h *= M;
    }

    const uint8_t* data2 = (const uint8_t*)data;

    switch( bytes.len & 7 ) {
        case 7: h ^= (uint64_t)(data2[6]) << 48;
            FALLTHROUGH;
        case 6: h ^= (uint64_t)(data2[5]) << 40;
            FALLTHROUGH;
        case 5: h ^= (uint64_t)(data2[4]) << 32;
            FALLTHROUGH;
        case 4: h ^= (uint64_t)(data2[3]) << 24;
            FALLTHROUGH;
        case 3: h ^= (uint64_t)(data2[2]) << 16;
            FALLTHROUGH;
        case 2: h ^= (uint64_t)(data2[1]) << 8;
            FALLTHROUGH;
        case 1: h ^= (uint64_t)(data2[0]);
            h *= M;
            break;
    }

    h ^= h >> R;
    h *= M;
    h ^= h >> R;

    #undef SEED
    #undef M
    #undef R
    #undef FALLTHROUGH

    _T(HashCode) hash;
    hash.lo = h & 0xFFFFFFFF;
    hash.hi = h >> 32;

    return hash;
}

size_t __cb_deps_add_string( _T(Deps)* deps, const char* cstr ) {
    _T(String) string = CB_STRING_FROM_CSTR( cstr );

    if( (deps->string.cap - deps->string.len) < (string.len + 1) ) {
        bool   has_target    = deps->target != NULL;
        size_t target_offset = 0;
        if( has_target ) {
            target_offset = (size_t)deps->target - (size_t)deps->string.ptr;
        }

        for( size_t i = 0; i < deps->len; ++i ) {
            deps->ptr[i] = (const char*)( (size_t)deps->ptr[i] - (size_t)deps->string.ptr );
        }

        CB_BUF_RESERVE( &deps->string, string.len + 16 );

        for( size_t i = 0; i < deps->len; ++i ) {
            deps->ptr[i] = deps->string.ptr + (size_t)deps->ptr[i];
        }

        if( has_target ) {
            deps->target = deps->string.ptr + target_offset;
        }
    }

    size_t result = deps->string.len;

    CB_BUF_APPEND( &deps->string, string.ptr, string.len + 1 );

    return result;
}
void _F(deps_set_target)( _T(Deps)* deps, const char* target ) {
    size_t offset = __cb_deps_add_string( deps, target );
    deps->target  = deps->string.ptr + offset;
}
void _F(deps_add)( _T(Deps)* deps, const char* dependency ) {
    size_t offset = __cb_deps_add_string( deps, dependency );
    const char* ptr = deps->string.ptr + offset;
    CB_BUF_PUSH( deps, ptr );
}
void _F(deps_reset)( _T(Deps)* deps ) {
    deps->len        = 0;
    deps->string.len = 0;
    deps->target     = NULL;
}
void _F(deps_free)( _T(Deps)* deps ) {
    CB_BUF_FREE( &deps->string );
    CB_BUF_FREE( deps );
    deps->target = NULL;
}
size_t _F(deps_parse_makefile)( const char* path, _T(Deps)* deps, bool can_replace_target ) {
    _T(StringBuf) depfile = {};
    if( !_F(file_read_entire)( path, &depfile ) ) {
        return 0;
    }

    _T(String) contents, target, dependencies;

    contents = depfile.slice;

    size_t dep_count = 0;

    size_t colon = 0;
    if( !_F(string_find)( contents, ':', &colon ) ) {
        goto end;
    }

    _F(string_split)( contents, colon, &target, &dependencies );

    dependencies = CB_SLICE_ADVANCE( &dependencies );

    if( !deps->target || can_replace_target ) {
        _F(deps_set_target)( deps, _F(local_fmt)( "%" CB_STRING_FMT, CB_STRING_ARG(&target) ) );
    }

    while( dependencies.len ) {
        _T(String) dep = dependencies;

        _T(Rune32Set) set = CB_RUNE_SET( ' ', '\\', '\n' );

        size_t end_of_path = 0;
        if( _F(string_find_set)( dep, set, &end_of_path ) ) {
            dep.len = end_of_path;
        }

        dependencies = CB_SLICE_ADVANCE( &dependencies, dep.len + 1 );

        dep = _F(string_trim_ws)( dep );

        if( dep.len ) {
            _F(deps_add)( deps, _F(local_fmt)( "%" CB_STRING_FMT, CB_STRING_ARG(&dep) ) );
            dep_count++;
        }
    }

end:
    CB_BUF_FREE( &depfile );
    return dep_count;
}

bool __cb_dir_create_many( const char* first, ... ) {
    bool result = true;

    va_list va;
    va_start( va, first );

    const char* arg = first;
    while( arg ) {
        _T(FileType) ft = _F(path_query_file_type)( arg );
        switch( ft ) {
            case _E(FT_NULL): {
                CB_INFO( "creating '%s' . . .", arg );
                if( !_F(dir_create)( arg, false ) ) {
                    CB_ERROR( "failed to create '%s'", arg );
                    result = false;
                    goto early_return;
                }
            } break;
            case _E(FT_DIR): {
                CB_INFO( "dir '%s' exists", arg );
            } break;
            case _E(FT_FILE):
            case _E(FT_PIPE): {
                CB_ERROR( "'%s' already exists as a file!", arg );
                result = false;
            } break;
        }

        arg = va_arg( va, const char* );
    }

early_return:
    va_end( va );

    return result;
}
void __cb_cmd_append( _T(CmdBuf)* cmd, ... ) {
    va_list va;
    va_start( va, cmd );

    const char* arg = NULL;

    for( ;; ) {
        arg = va_arg( va, const char* );
        if( !arg ) {
            break;
        }

        _F(cmd_add)( cmd, arg );
    }

    va_end( va );
}
bool __cb_exec_async( _T(Cmd) cmd, __CbExecParams params, _T(Process)* out_pid ) {
    if( !cmd.len || cmd.ptr[cmd.len] ) {
        CB_ERROR( "exec: command requires a NULL string at the end!" );
        return false;
    }

    int fd_stdin  = params.pstdin  ? params.pstdin->opaque_fd  : STDIN_FILENO;
    int fd_stdout = params.pstdout ? params.pstdout->opaque_fd : STDOUT_FILENO;
    int fd_stderr = params.pstderr ? params.pstderr->opaque_fd : STDERR_FILENO;

    if( params.wd ) {
        CB_INFO( "exec: chdir: '%s'", params.wd );
    }

    _T(StringBuf) string = {};

    if( params.env ) {
        CB_FOREACH( params.env, {
            _T(String) key   = _F(env_deref)( params.env, kv->key );
            _T(String) value = _F(env_deref)( params.env, kv->value );

            _F(string_buf_fmt)(
                &string,
                "%" CB_STRING_FMT "=%" CB_STRING_FMT " ",
                CB_STRING_ARG(&key), CB_STRING_ARG(&value) );
        }, kv );
    }

    _F(cmd_flatten)( cmd, &string );
    CB_BUF_PUSH( &string, 0 );

    CB_INFO( "  > %s", string.ptr );

    CB_BUF_FREE( &string );

    pid_t pid = fork();
    if( pid < 0 ) {
        CB_ERROR( "exec: failed to fork!" );
        return false;
    }

    // NOTE(alicia): thread where program was executed
    if( pid ) {
        out_pid->opaque_fd = pid;
        return true;
    }

    // NOTE(alicia): thread where program will run

    if( params.wd ) {
        chdir( params.wd );
    }

    if( dup2( fd_stdin, STDIN_FILENO ) < 0 ) {
        CB_ERROR( "exec: failed to duplicate stdin!" );
    }

    if( dup2( fd_stdout, STDOUT_FILENO ) < 0 ) {
        CB_ERROR( "exec: failed to duplicate stdout!" );
    }

    if( dup2( fd_stderr, STDERR_FILENO ) < 0 ) {
        CB_ERROR( "exec: failed to duplicate stderr!" );
    }

    if( params.env ) {
        CB_FOREACH( params.env, {
            _T(String) key   = _F(env_deref)( params.env, kv->key );
            _T(String) value = _F(env_deref)( params.env, kv->value );

            setenv( key.ptr, value.ptr, true );
        }, kv );
    }

    // NOTE(alicia): execvp will only return if command failed to execute.
    execvp( cmd.ptr[0], (char* const*)(cmd.ptr) );

    CB_FATAL( "exec: failed to execute %s!", cmd.ptr[0] );
    abort();
}
int __cb_exec_sync( _T(Cmd) cmd, __CbExecParams params ) {
    _T(Process) pid;
    if( __cb_exec_async( cmd, params, &pid ) ) {
        return _F(proc_wait)( &pid, CB_WAIT_INF );
    }
    return -2;
}
int __cb_exec( _T(Cmd) cmd, __CbExecParams params ) {
    if( params.proc || params.group ) {
        _T(Process) pid;
        if( __cb_exec_async( cmd, params, &pid ) ) {
            if( params.proc ) {
                *params.proc = pid;
            }
            if( params.group ) {
                _F(proc_group_add)( params.group, &pid );
            }
            return 0;
        }
        return -2;
    } else {
        return __cb_exec_sync( cmd, params );
    }
}

#undef cbs
#undef cbjq
#undef CB_LOCAL_SIZE_BYTES
#undef CB_LOCAL_PLATFORM_COUNT
#endif /* implementation */

