#pragma once

#include <cstddef>
#include <cstdint>

namespace sprite_display {

struct SpriteSheet {
  const uint16_t* pixels = nullptr;
  uint16_t sheetWidth = 0;
  uint16_t sheetHeight = 0;
  uint16_t frameWidth = 0;
  uint16_t frameHeight = 0;
  uint16_t frameCount = 0;
  uint16_t framesPerRow = 0;
  uint32_t frameDurationMs = 0;
  bool loop = true;
  bool hasTransparentKey = false;
  uint16_t transparentKey = 0;
  int16_t x = 0;
  int16_t y = 0;
};

enum class SpriteSheetError {
  None,
  MissingPixels,
  EmptySheet,
  EmptyFrame,
  EmptyAnimation,
  EmptyFrameDuration,
  InvalidGrid,
  FrameCountExceedsSheet,
  PlacementOutOfBounds,
};

SpriteSheetError validateSpriteSheet(const SpriteSheet& sheet,
                                     uint16_t displayWidth,
                                     uint16_t displayHeight);
const char* spriteSheetErrorMessage(SpriteSheetError error);
constexpr uint32_t frameDurationFromFps(uint16_t fps) {
  return fps == 0 ? 0 : (1000U + fps / 2U) / fps;
}

}  // namespace sprite_display
