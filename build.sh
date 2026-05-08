#!/usr/bin/env bash

# Bootstrapper for Raylib Template Build System.

PS4=' > '
: "${BUILD_DIR:=build}"

log() {
    is_error=${2:-0}

    if [ "$is_error" -eq 1 ]; then
        echo "BOOTSTRAPPER ERROR: $1" >&2
    else
        echo "BOOTSTRAPPER: $1"
    fi
}

traced() {
    set -x
    "$@"
    { set +x; } 2>/dev/null

    local rc=$?
    return $rc
}

logged() {
    local msg="$1"
    shift

    log "$msg"

    traced "$@"
    return $?
}

submodules_loaded=0

if [ -d "${BUILD_DIR}" ]; then
    if [ -f "${BUILD_DIR}/submodules_loaded" ]; then
        submodules_loaded=1
    fi
else
    logged "Build system directory not found, creating . . ." mkdir "${BUILD_DIR}"
fi

if [ "$submodules_loaded" -eq 0 ]; then
    is_submodule_loaded() {
        local dir="$1"
        local __resultvar="$2"

        if [ -d "$dir" ] && [ "$(ls -A "$dir")" ]; then
            eval "$__resultvar=1"
        else
            eval "$__resultvar=0"
        fi
    }

    is_submodule_loaded "extern/raylib" raylib_loaded
    is_submodule_loaded "extern/cb" cb_loaded
    is_submodule_loaded "extern/flagship" flagship_loaded
    is_submodule_loaded "extern/ini-parser" ini_parser_loaded

    if [ "$raylib_loaded" -eq 0 ] || [ "$cb_loaded" -eq 0 ] || [ "$flagship_loaded" -eq 0 ] || [ "$ini_parser_loaded" -eq 0 ]; then
        read -r -p "BOOTSTRAPPER: One or more submodules has not been initialized, would you like to load them? [Y/n] " response

        if [[ -z "$response" || "$response" =~ ^[Yy]$ ]]; then
            logged "Initializing submodules . . ." git submodule update --init --recursive
        else
            log "One or more required submodules has not been initialized!" 1
            exit 1
        fi
    fi

    touch "${BUILD_DIR}/submodules_loaded"
fi


if ! [ -f "build.bin" ]; then
    required_headers=("cb.h" "flagship.h" "ini-parse.h")
    required_header_src=("cb" "flagship" "ini-parser")
    required_headers_available=(1, 1, 1)
    are_all_required_headers_available=1

    for i in "${!required_headers[@]}"; do
        if ! [ -f "${required_headers[$i]}" ]; then
            required_headers_available[$i]=0
            are_all_required_headers_available=0
        fi
    done

    if [ "$are_all_required_headers_available" -eq 0 ]; then
        log "Copying required files for build system . . ."
        for i in "${!required_headers[@]}"; do
            if [ "${required_headers_available[$i]}" -eq 0 ]; then
                traced cp "extern/${required_header_src[$i]}/${required_headers[$i]}" .
            fi
        done
    fi

    logged "Building build system . . . " gcc cb.c -o build.bin
fi

./build.bin "$@"

