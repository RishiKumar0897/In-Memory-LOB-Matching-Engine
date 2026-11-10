#!/usr/bin/env bash
set -euo pipefail

# always run from the project root
cd "$(dirname "$0")"

PRESET=${1:-debug}   # ./build.sh  |  ./build.sh release  |  ./build.sh release-apple-silicon

cmake --preset "$PRESET"
cmake --build --preset "$PRESET"
ctest --preset "$PRESET"
