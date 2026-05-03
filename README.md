# Raylib Template

Template for Raylib 6.0 projects.

## Table of Contents
- [Usage](#usage)
- [Build System](#build-system)
    - [Modes](#modes)
    - [Mode: init](#mode-init)
    - [Mode: build](#mode-build)
    - [Mode: run](#mode-run)
    - [Mode: pkg](#mode-pkg)
    - [Mode: help](#mode-help)
- [Directory Structure](#directory-structure)
- [External Dependencies](#external-dependencies)
- [System Dependencies](#system-dependencies)
    - [System Dependencies: General](#system-dependencies-general)
    - [System Dependencies: Windows](#system-dependencies-windows)
    - [System Dependencies: POSIX](#system-dependencies-posix)
    - [System Dependencies: WASM](#system-dependencies-wasm)


## Usage

- Clone Repository
    ```bash
    git clone https://github.com/smushy64/raylib-template.git
    ```

- Initialize Submodules [^raylib_version]
    ```bash
    git submodule update --init --recursive
    ```

- Run build system for the first time. This will create required files. [^build_args]
    - POSIX
    ```bash
    ./build.sh -name "<project-name>"
    ```
    - Windows
    ```bat
    .\build.bat -name "<project-name>"
    ```

- To recompile and run project, just run the `build.*` script again.

## Build System

POSIX should use: `build.sh`

Windows should use: `build.bat`

### Modes

| Mode    | Description                        |
| ------- | ---------------------------------- |
| `init`  | Initialize a new project.          |
| `build` | Build project.                     |
| `run`   | Build project and run.             |
| `pkg`   | Build in release mode and package. |
| `help`  | Print help.                        |

### Mode: init

Initialize a new project.

| Flag            | Description                                                              |
| --------------- | ------------------------------------------------------------------------ |
| `-c`            | Generate C project instead of C++ project                                |
| `-name <text>`  | Set name of project. By default, parent directory is used.               |
| `-d`            | Dry run. Doesn't initialize but prints all commands it was going to run. |
| `-h`            | Stops parsing flags and prints help for this mode.                       |

### Mode: build

Build project.

| Flag            | Description                                                                      |
| --------------- | -------------------------------------------------------------------------------- |
| `-t <platform>` | Set target platform. Valid: `linux, windows, wasm, native` Default: `native`     |
| `-re <module>`  | Force rebuild of given module. Valid: `all, raylib, vendor_c, vendor_cpp`        |
| `-nre <module>` | Skip rebuild of given module. Valid: `all, raylib, vendor_c, vendor_cpp`         |
| `-v`            | Only build vendor libraries.                                                     |
| `-s`            | Strip debug symbols.                                                             |
| `-o`            | Build with appropriate optimizations.                                            |
| `-cfg <cfg>`    | Ignore config file and use passed in string as config file.                      |
| `-cfgf <path>`  | Change default config file path. Default: `./cfg.ini` Valid: `<path>, 0 (stdin)` |
| `-d`            | Dry run. Doesn't build but prints all the commands it was going to run.          |
| `-h`            | Stops parsing flags and prints help for this mode.                               |

### Mode: run

Build project and run.

| Flag            | Description                                                                      |
| --------------- | -------------------------------------------------------------------------------- |
| `-t <platform>` | Set target platform. Valid: `linux, windows, wasm, native` Default: `native`     |
| `-re <module>`  | Force rebuild of given module. Valid: `all, raylib, vendor_c, vendor_cpp`        |
| `-nre <module>` | Skip rebuild of given module. Valid: `all, raylib, vendor_c, vendor_cpp`         |
| `-s`            | Strip debug symbols.                                                             |
| `-o`            | Build with appropriate optimizations.                                            |
| `-cfg <cfg>`    | Ignore config file and use passed in string as config file.                      |
| `-cfgf <path>`  | Change default config file path. Default: `./cfg.ini` Valid: `<path>, 0 (stdin)` |
| `--`            | Stop parsing flags and pass remaining to project.                                |
| `-d`            | Dry run. Doesn't build & run but prints all the commands it was going to run.    |
| `-h`            | Stops parsing flags and prints help for this mode.                               |

### Mode: pkg

Build in release mode and package.

| Flag            | Description                                                                      |
| --------------- | -------------------------------------------------------------------------------- |
| `-t <platform>` | Set target platform. Valid: `linux, windows, wasm, native` Default: `native`     |
| `-cfg <cfg>`    | Ignore config file and use passed in string as config file.                      |
| `-cfgf <path>`  | Change default config file path. Default: `./cfg.ini` Valid: `<path>, 0 (stdin)` |
| `-d`            | Dry run. Doesn't package but prints all the commands it was going to run.        |
| `-h`            | Stops parsing flags and prints help for this mode.                               |

### Mode: help

Print help.

| Flag    | Description                                                                    |
| ------- | ------------------------------------------------------------------------------ |
| `<opt>` | Name of something to print help for. Valid: `init, build, run, pkg, help, cfg` |

## Directory Structure

```
├── build/ - where builds are created
│   ├── linux/   - GNU/Linux builds.
│   ├── windows/ - Windows builds.
│   ├── wasm/    - WASM builds.
│   └── build/   - Build system files.
├── extern/ - where external dependencies live
│   ├── cb/       - Build system.
│   ├── flagship/ - Flag parsing library for build system & project.
│   └── raylib/   - Raylib
├── template/ - Template files for generating files. Do not edit directly.
├── vendor/ - Where external dependencies should go.
│   ├── include/ - External dependency headers.
│   ├── bin/ - External dependency binaries.
│   └── src/ - External dependency sources. Each should be placed in its own dir.
├── src/ - Your source code, with some starter code already included.
│   ├── main(.cpp|.c)     - Project entry point. Don't edit this directly unless you know what you're doing.
│   ├── sources.cpp       - Collected C++ sources. Automagically generated by build system.
│   ├── sources.c         - Collected C sources. Automagically generated by build system.
│   └── compile_flags.txt - Automagically generated clangd compile_flags.
├── include/ - Your source code header files.
│   └── compile_flags.txt - Automagically generated clangd compile_flags.
└── README.md
```

## External Dependencies

- [Raylib](https://www.raylib.com)                 : Game-development framework.
- [cb](https://github.com/smushy64/cb)             : My own C build system.
- [flagship](https://github.com/smushy64/flagship) : My own flag-parsing C library.

## System Dependencies

### System Dependencies: General

- `zip`      : For packaging Windows and WASM binaries.
- `tar`      : For packaging POSIX binaries.
- `python3`  : For running WASM binaries.
- `xdg-open` : For running WASM binaries.

### System Dependencies: Windows

- [MinGW]() : For building binaries.
    - `gcc`   : For building native and POSIX binaries.
    - `clang` : For building WASM binaries.

### System Dependencies: POSIX

- `gcc`     : For building native binaries.
- [MinGW]() : For building Windows binaries.
- [wine]()  : For running Windows binaries.
- `clang`   : For building WASM binaries.

### System Dependencies: WASM

- `clang` : For building binaries.

[^raylib_version]: Raylib is pinned to version 6.0
[^build_args]:   The flag `-c` can be passed to build system to create a C project instead of C++. See [#Build-System] for more flags.

