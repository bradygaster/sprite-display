#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
mkdir -p "$ROOT/build"
clang++ -std=c++17 -O2 -Wall -Wextra -Werror \
  "$ROOT/tests/test_sprite_display.cpp" \
  "$ROOT/firmware/SpriteDisplay/src/SpriteAnimator.cpp" \
  "$ROOT/firmware/SpriteDisplay/src/SpriteRenderer.cpp" \
  "$ROOT/firmware/SpriteDisplay/src/SpriteSheet.cpp" \
  -o "$ROOT/build/test-sprite-display"
"$ROOT/build/test-sprite-display"
