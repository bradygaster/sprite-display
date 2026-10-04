#!/usr/bin/env python3
"""Convert an image sprite sheet to a deterministic RGB565 C++ header."""

from __future__ import annotations

import argparse
from pathlib import Path
import re
import sys


def rgb565(red: int, green: int, blue: int) -> int:
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--symbol", default="sprite_pixels")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", args.symbol):
        raise SystemExit("--symbol must be a valid C++ identifier")
    try:
        from PIL import Image
    except ImportError:
        print(
            "Pillow is required: python3 -m pip install Pillow==11.3.0",
            file=sys.stderr,
        )
        return 1

    with Image.open(args.input) as source:
        image = source.convert("RGB")
        width, height = image.size
        values = [rgb565(*pixel) for pixel in image.getdata()]

    lines = [
        "#pragma once",
        "",
        "#include <cstdint>",
        "",
        "namespace sprite_display {",
        "",
        f"constexpr uint16_t {args.symbol}[] = {{",
    ]
    for offset in range(0, len(values), 8):
        chunk = ", ".join(f"0x{value:04x}" for value in values[offset : offset + 8])
        lines.append(f"    {chunk},")
    lines.extend(
        [
            "};",
            f"constexpr uint16_t {args.symbol}_width = {width};",
            f"constexpr uint16_t {args.symbol}_height = {height};",
            "",
            "}  // namespace sprite_display",
            "",
        ]
    )
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w", encoding="utf-8", newline="\n") as output:
        output.write("\n".join(lines))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
