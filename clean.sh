#!/usr/bin/env bash

set -euo pipefail

#always run from the project root
cd "$(dirname "$0")"
TYPE=${1:-all}                    # ./clean.sh  or  ./clean.sh all

case "$TYPE" in
    all) rm -rf build ;;
    debug|release) rm -rf build/"$TYPE" ;;
    *) echo "usage: ./clean.sh [all|debug|release]" >&2; exit 1 ;;
    esac
echo "cleaned: $TYPE"

