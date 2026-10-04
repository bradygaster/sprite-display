#pragma once

#include <cstdint>

#include "SpriteSheet.h"

namespace sprite_display {

class SpriteAnimator {
 public:
  explicit SpriteAnimator(const SpriteSheet& sheet);

  void reset(uint32_t nowMs);
  uint16_t frameAt(uint32_t nowMs) const;

 private:
  const SpriteSheet& sheet_;
  uint32_t startedAtMs_ = 0;
};

}  // namespace sprite_display
