#include "SpriteRenderer.h"

#include <algorithm>
#include <cstddef>

namespace sprite_display {

SpriteRenderer::SpriteRenderer(uint16_t* framebuffer, uint16_t width,
                               uint16_t height)
    : framebuffer_(framebuffer), width_(width), height_(height) {}

bool SpriteRenderer::render(const SpriteSheet& sheet, uint16_t frameIndex,
                            uint16_t clearColor) {
  if (!framebuffer_ ||
      validateSpriteSheet(sheet, width_, height_) != SpriteSheetError::None ||
      frameIndex >= sheet.frameCount) {
    return false;
  }

  std::fill(framebuffer_,
            framebuffer_ + static_cast<size_t>(width_) * height_, clearColor);
  const uint16_t frameColumn = frameIndex % sheet.framesPerRow;
  const uint16_t frameRow = frameIndex / sheet.framesPerRow;
  const uint16_t sourceX = frameColumn * sheet.frameWidth;
  const uint16_t sourceY = frameRow * sheet.frameHeight;

  for (uint16_t y = 0; y < sheet.frameHeight; ++y) {
    for (uint16_t x = 0; x < sheet.frameWidth; ++x) {
      const uint16_t pixel =
          sheet.pixels[static_cast<size_t>(sourceY + y) * sheet.sheetWidth +
                       sourceX + x];
      if (sheet.hasTransparentKey && pixel == sheet.transparentKey) continue;
      const uint16_t destinationX = sheet.x + x * sheet.scale;
      const uint16_t destinationY = sheet.y + y * sheet.scale;
      for (uint8_t scaledY = 0; scaledY < sheet.scale; ++scaledY) {
        std::fill_n(
            framebuffer_ +
                static_cast<size_t>(destinationY + scaledY) * width_ +
                destinationX,
            sheet.scale, pixel);
      }
    }
  }
  return true;
}

}  // namespace sprite_display
