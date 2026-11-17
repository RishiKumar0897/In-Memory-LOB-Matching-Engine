#!/usr/bin/env bash
# Configure, build and test. Presets are defined in CMakePresets.json.
set -euo pipefail # stop on any error, unset variable, or failed pipe

# always run from the project root
cd "$(dirname "$0")"

PRESET=${1:-debug}   # ./build.sh  |  ./build.sh release  |  ./build.sh release-apple-silicon

cmake --preset "$PRESET"
cmake --build --preset "$PRESET"
ctest --preset "$PRESET"
