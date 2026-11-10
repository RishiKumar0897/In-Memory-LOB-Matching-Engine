#!/usr/bin/env bash
set -euo pipefail

TYPE=${1:-Debug}                     # ./build.sh  or  ./build.sh Release
DIR=build/$(echo "$TYPE" | tr '[:upper:]' '[:lower:]')

cmake -S . -B "$DIR" -G Ninja -DCMAKE_BUILD_TYPE="$TYPE"
cmake --build "$DIR"
ctest --test-dir "$DIR" --output-on-failure
