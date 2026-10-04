# Sprite Display

An extensible sprite-sheet animation foundation for the Waveshare
ESP32-S3-Touch-AMOLED-1.75-B/C:

- ESP32-S3 with 8 MB OPI PSRAM and 16 MB flash
- 466x466 CO5300 round AMOLED
- Arduino CLI 1.5.1
- Espressif Arduino core `esp32:esp32@3.3.10`

The included animation is an original four-frame sugar-skull design with
color-cycling floral, eye, and cheek accents. It demonstrates sprite timing,
transparency, placement, scaling, and display transfer without depending on
external artwork.

## Repository layout

```text
firmware/SpriteDisplay/
  SpriteDisplay.ino        Minimal hardware entrypoint
  assets/                  Generated RGB565 sprite data
  src/                     Animation, metadata, and rendering modules
  partitions.csv           Custom 16 MB flash layout
assets/
  placeholder.ppm          Neutral source art retained as a pipeline fixture
tests/                     Hardware-independent native unit tests
tools/
  arduino.sh               Pinned firmware build/upload helper
  convert_sprite.py        Deterministic image-to-RGB565 header converter
  test.sh                  Native test runner
```

## Build and test

Install Arduino CLI 1.5.1 and the pinned ESP32 core:

```bash
arduino-cli config init
arduino-cli config add board_manager.additional_urls \
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
arduino-cli core update-index
arduino-cli core install esp32:esp32@3.3.10
```

Then run:

```bash
bash tools/test.sh
bash tools/arduino.sh build
```

The Arduino helper fetches only the required libraries from the pinned
Waveshare revision into `.deps/`. Firmware artifacts are written to
`build/firmware/`.

## Flash

Build first, then explicitly supply the device port:

```bash
bash tools/arduino.sh upload /dev/cu.usbmodemXXXX
```

The upload command never guesses or scans serial ports.

## Replace the sprite asset

Install the single conversion dependency:

```bash
python3 -m pip install Pillow==11.3.0
```

Arrange equal-sized frames left-to-right and then top-to-bottom in a PNG,
PPM, or other Pillow-supported image. Generate a deterministic RGB565 header:

```bash
python3 tools/convert_sprite.py artwork.png \
  firmware/SpriteDisplay/assets/MySprite.h \
  --symbol my_sprite_pixels
```

Update `firmware/SpriteDisplay/assets/SugarSkullSprite.h` usage in
`SpriteDisplay.ino`, then set the `SpriteSheet` metadata:

- full sheet dimensions
- frame dimensions and count
- frames per row
- frame duration in milliseconds
- loop behavior
- optional RGB565 transparent color key
- top-left placement on the 466x466 display
- integer nearest-neighbor scale

Large source artwork should live outside `firmware/`; only generated RGB565
data needed by the device belongs in the sketch. If an animation grows too
large for compiled flash data, the `SpriteSheet` pixel source is the intended
extension point for an asset-partition-backed reader.

## Visual design extension points

The current sugar skull is generated from compact, repository-owned C++ shape
primitives. Its palette, details, frame rate, composition, background, scale,
touch behavior, and transitions can be changed without altering the tested
timing and metadata contracts.
