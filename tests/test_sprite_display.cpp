#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "../firmware/SpriteDisplay/src/SpriteAnimator.h"
#include "../firmware/SpriteDisplay/src/SpriteRenderer.h"
#include "../firmware/SpriteDisplay/src/SpriteSheet.h"
#include "../firmware/SpriteDisplay/assets/SugarSkullSprite.h"

namespace {

int failures = 0;

void expect(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    ++failures;
  }
}

sprite_display::SpriteSheet validSheet(const uint16_t* pixels) {
  return {
      pixels, 4, 4, 2, 2, 4, 2, 100, true, true, 0, 1, 1,
  };
}

void testAnimationTiming() {
  constexpr uint16_t pixels[16] = {};
  auto sheet = validSheet(pixels);
  sprite_display::SpriteAnimator animator(sheet);
  animator.reset(1000);
  expect(animator.frameAt(1000) == 0, "animation starts at frame zero");
  expect(animator.frameAt(1099) == 0, "frame remains until duration elapses");
  expect(animator.frameAt(1100) == 1, "animation advances on boundary");
  expect(animator.frameAt(1400) == 0, "looping animation wraps");

  sheet.loop = false;
  sprite_display::SpriteAnimator nonLooping(sheet);
  nonLooping.reset(50);
  expect(nonLooping.frameAt(1000) == 3, "non-looping animation clamps");

  animator.reset(UINT32_MAX - 49);
  expect(animator.frameAt(50) == 1, "millisecond wraparound is handled");
  expect(sprite_display::frameDurationFromFps(25) == 40,
         "FPS converts to frame duration");
  expect(sprite_display::frameDurationFromFps(0) == 0,
         "zero FPS is rejected as zero duration");
}

void testValidation() {
  constexpr uint16_t pixels[16] = {};
  auto sheet = validSheet(pixels);
  using sprite_display::SpriteSheetError;
  expect(sprite_display::validateSpriteSheet(sheet, 8, 8) ==
             SpriteSheetError::None,
         "valid sheet passes validation");

  auto invalid = sheet;
  invalid.pixels = nullptr;
  expect(sprite_display::validateSpriteSheet(invalid, 8, 8) ==
             SpriteSheetError::MissingPixels,
         "missing pixels are rejected");
  invalid = sheet;
  invalid.framesPerRow = 3;
  expect(sprite_display::validateSpriteSheet(invalid, 8, 8) ==
             SpriteSheetError::InvalidGrid,
         "oversized frame grid is rejected");
  invalid = sheet;
  invalid.frameCount = 5;
  expect(sprite_display::validateSpriteSheet(invalid, 8, 8) ==
             SpriteSheetError::FrameCountExceedsSheet,
         "excess frame count is rejected");
  invalid = sheet;
  invalid.scale = 0;
  expect(sprite_display::validateSpriteSheet(invalid, 8, 8) ==
             SpriteSheetError::EmptyScale,
         "zero scale is rejected");
  invalid = sheet;
  invalid.scale = 4;
  expect(sprite_display::validateSpriteSheet(invalid, 8, 8) ==
             SpriteSheetError::PlacementOutOfBounds,
         "scaled placement is bounds checked");
  invalid = sheet;
  invalid.x = 7;
  expect(sprite_display::validateSpriteSheet(invalid, 8, 8) ==
             SpriteSheetError::PlacementOutOfBounds,
         "out-of-bounds placement is rejected");
}

void testRendering() {
  constexpr uint16_t pixels[] = {
      0, 1, 2, 3,
      4, 5, 6, 7,
      8, 9, 10, 11,
      12, 13, 14, 15,
  };
  auto sheet = validSheet(pixels);
  std::vector<uint16_t> framebuffer(36, 99);
  sprite_display::SpriteRenderer renderer(framebuffer.data(), 6, 6);
  expect(renderer.render(sheet, 1, 9), "valid frame renders");
  expect(framebuffer[1 * 6 + 1] == 2, "frame source column is selected");
  expect(framebuffer[1 * 6 + 2] == 3, "frame row is copied");
  expect(framebuffer[2 * 6 + 1] == 6, "second source row is copied");
  expect(framebuffer[0] == 9, "background is cleared");
  expect(renderer.render(sheet, 2, 9), "second sheet row renders");
  expect(framebuffer[1 * 6 + 1] == 8, "frame source row is selected");
  sheet.transparentKey = 2;
  expect(renderer.render(sheet, 1, 9), "transparent frame renders");
  expect(framebuffer[1 * 6 + 1] == 9, "transparent key preserves background");
  expect(!renderer.render(sheet, 4), "out-of-range frame is rejected");

  sheet = validSheet(pixels);
  sheet.scale = 2;
  expect(renderer.render(sheet, 1, 9), "scaled frame renders");
  expect(framebuffer[1 * 6 + 1] == 2 && framebuffer[1 * 6 + 2] == 2 &&
             framebuffer[2 * 6 + 1] == 2 && framebuffer[2 * 6 + 2] == 2,
         "nearest-neighbor scale expands each source pixel");
  expect(framebuffer[1 * 6 + 3] == 3 && framebuffer[2 * 6 + 4] == 3,
         "scaled neighboring pixels remain distinct");
}

void testSugarSkullAsset() {
  using namespace sprite_display;
  expect(kSugarSkullPixels.size() ==
             static_cast<size_t>(kSugarSkullSheetWidth) *
                 kSugarSkullSheetHeight,
         "sugar skull sheet dimensions match its generated pixels");
  size_t visiblePixels = 0;
  for (const uint16_t pixel : kSugarSkullPixels) {
    if (pixel != 0) ++visiblePixels;
  }
  expect(visiblePixels > 6000, "sugar skull frames contain visible artwork");
}

}  // namespace

int main() {
  testAnimationTiming();
  testValidation();
  testRendering();
  testSugarSkullAsset();
  if (failures) return EXIT_FAILURE;
  std::cout << "All sprite display tests passed.\n";
  return EXIT_SUCCESS;
}
