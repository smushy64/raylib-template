#!/usr/bin/env bash

# Resets build system.
# Does not affect project files except for generated sources.

read -r -p "Are you sure you want to clean all build system files? [Y/n] " response

if ! [[ -z "$response" || "$response" =~ ^[Yy]$ ]]; then
    exit 0
fi

PS4=' > '

traced() {
    set -x
    "$@"
    { set +x; } 2>/dev/null

    local rc=$?
    return $rc
}

echo "Deleting all build system headers . . ."
traced rm *.h -f

echo "Deleting build system . . ."
traced rm build{.bin,.bin.old} -f

echo "Deleting build directory . . ."
traced rm build/ -rf

echo "Deleting generated files . . ."
traced rm src/main.{c,cpp} src/sources.{c,cpp} {src,include}/compile_flags.txt -f

echo "All done."

