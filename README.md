# Raylib Template

Template for Raylib-based projects.

## Usage

- fork this repository
- submodule official raylib repository
- compile cb.c with either gcc or clang
- run cb with the following modes:
```bash
# to compile project:
./cb build

# to compile and run project:
./cb run

# to package project for release on itch.io:
./cb pkg

# to compile and run 'editor' program:
./cb ed

# to list arguments for any of the previous modes:
./cb help <mode>
# where <mode> is build, run, pkg or ed
```

## Notes

- cb.c currently does not work on Windows so
cross-compilation with WSL and MinGW is required

## External Libraries Used:

- [raylib](https://www.raylib.com/)                    : Quick and easy framework for game development.
- [cb]()                                               : Build system library, written by me. Based on my previous project, cbuild.
- [flagship](https://github.com/smushy64/flagship)     : Flag parsing library, written by me.
- [ini-parser](https://github.com/smushy64/ini-parser) : .ini file parsing library, written by me.

