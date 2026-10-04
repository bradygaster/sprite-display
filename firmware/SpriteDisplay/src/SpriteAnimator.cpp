#include "SpriteAnimator.h"

namespace sprite_display {

SpriteAnimator::SpriteAnimator(const SpriteSheet& sheet) : sheet_(sheet) {}

void SpriteAnimator::reset(uint32_t nowMs) { startedAtMs_ = nowMs; }

uint16_t SpriteAnimator::frameAt(uint32_t nowMs) const {
  if (!sheet_.frameCount || !sheet_.frameDurationMs) return 0;
  const uint32_t elapsed = nowMs - startedAtMs_;
  const uint32_t frame = elapsed / sheet_.frameDurationMs;
  if (sheet_.loop) return static_cast<uint16_t>(frame % sheet_.frameCount);
  return static_cast<uint16_t>(
      frame < sheet_.frameCount ? frame : sheet_.frameCount - 1);
}

}  // namespace sprite_display
