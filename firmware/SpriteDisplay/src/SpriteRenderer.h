#pragma once

#include <cstdint>

#include "SpriteSheet.h"

namespace sprite_display {

class SpriteRenderer {
 public:
  SpriteRenderer(uint16_t* framebuffer, uint16_t width, uint16_t height);

  bool render(const SpriteSheet& sheet, uint16_t frameIndex,
              uint16_t clearColor = 0);

 private:
  uint16_t* framebuffer_;
  uint16_t width_;
  uint16_t height_;
};

}  // namespace sprite_display
