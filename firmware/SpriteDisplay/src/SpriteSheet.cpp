#include "SpriteSheet.h"

namespace sprite_display {

SpriteSheetError validateSpriteSheet(const SpriteSheet& sheet,
                                     uint16_t displayWidth,
                                     uint16_t displayHeight) {
  if (!sheet.pixels) return SpriteSheetError::MissingPixels;
  if (!sheet.sheetWidth || !sheet.sheetHeight) {
    return SpriteSheetError::EmptySheet;
  }
  if (!sheet.frameWidth || !sheet.frameHeight) {
    return SpriteSheetError::EmptyFrame;
  }
  if (!sheet.frameCount) return SpriteSheetError::EmptyAnimation;
  if (!sheet.frameDurationMs) return SpriteSheetError::EmptyFrameDuration;
  if (!sheet.scale) return SpriteSheetError::EmptyScale;
  if (!sheet.framesPerRow || sheet.frameWidth > sheet.sheetWidth ||
      sheet.frameHeight > sheet.sheetHeight ||
      static_cast<uint32_t>(sheet.framesPerRow) * sheet.frameWidth >
          sheet.sheetWidth) {
    return SpriteSheetError::InvalidGrid;
  }

  const uint32_t rows = sheet.sheetHeight / sheet.frameHeight;
  const uint32_t capacity = rows * sheet.framesPerRow;
  if (sheet.frameCount > capacity) {
    return SpriteSheetError::FrameCountExceedsSheet;
  }

  const int32_t right = static_cast<int32_t>(sheet.x) +
                        static_cast<int32_t>(sheet.frameWidth) * sheet.scale;
  const int32_t bottom = static_cast<int32_t>(sheet.y) +
                         static_cast<int32_t>(sheet.frameHeight) * sheet.scale;
  if (sheet.x < 0 || sheet.y < 0 || right > displayWidth ||
      bottom > displayHeight) {
    return SpriteSheetError::PlacementOutOfBounds;
  }
  return SpriteSheetError::None;
}

const char* spriteSheetErrorMessage(SpriteSheetError error) {
  switch (error) {
    case SpriteSheetError::None:
      return "none";
    case SpriteSheetError::MissingPixels:
      return "sprite pixels are missing";
    case SpriteSheetError::EmptySheet:
      return "sprite sheet dimensions must be nonzero";
    case SpriteSheetError::EmptyFrame:
      return "frame dimensions must be nonzero";
    case SpriteSheetError::EmptyAnimation:
      return "frame count must be nonzero";
    case SpriteSheetError::EmptyFrameDuration:
      return "frame duration must be nonzero";
    case SpriteSheetError::EmptyScale:
      return "sprite scale must be nonzero";
    case SpriteSheetError::InvalidGrid:
      return "frame grid does not fit the sprite sheet";
    case SpriteSheetError::FrameCountExceedsSheet:
      return "frame count exceeds sprite sheet capacity";
    case SpriteSheetError::PlacementOutOfBounds:
      return "sprite placement exceeds the display";
  }
  return "unknown sprite sheet error";
}

}  // namespace sprite_display
