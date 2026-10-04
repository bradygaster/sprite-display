#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sprite_display {

constexpr uint16_t kSugarSkullFrameWidth = 64;
constexpr uint16_t kSugarSkullFrameHeight = 64;
constexpr uint16_t kSugarSkullFrameCount = 4;
constexpr uint16_t kSugarSkullFramesPerRow = 4;
constexpr uint16_t kSugarSkullSheetWidth =
    kSugarSkullFrameWidth * kSugarSkullFramesPerRow;
constexpr uint16_t kSugarSkullSheetHeight = kSugarSkullFrameHeight;

namespace sugar_skull_detail {

using Pixels =
    std::array<uint16_t, kSugarSkullSheetWidth * kSugarSkullSheetHeight>;

constexpr void setPixel(Pixels& pixels, int frame, int x, int y,
                        uint16_t color) {
  if (frame < 0 || frame >= kSugarSkullFrameCount || x < 0 ||
      x >= kSugarSkullFrameWidth || y < 0 || y >= kSugarSkullFrameHeight) {
    return;
  }
  pixels[static_cast<size_t>(y) * kSugarSkullSheetWidth +
         frame * kSugarSkullFrameWidth + x] = color;
}

constexpr void fillCircle(Pixels& pixels, int frame, int centerX, int centerY,
                          int radius, uint16_t color) {
  for (int y = centerY - radius; y <= centerY + radius; ++y) {
    for (int x = centerX - radius; x <= centerX + radius; ++x) {
      const int dx = x - centerX;
      const int dy = y - centerY;
      if (dx * dx + dy * dy <= radius * radius) {
        setPixel(pixels, frame, x, y, color);
      }
    }
  }
}

constexpr void fillEllipse(Pixels& pixels, int frame, int centerX, int centerY,
                           int radiusX, int radiusY, uint16_t color) {
  const int limit = radiusX * radiusX * radiusY * radiusY;
  for (int y = centerY - radiusY; y <= centerY + radiusY; ++y) {
    for (int x = centerX - radiusX; x <= centerX + radiusX; ++x) {
      const int dx = x - centerX;
      const int dy = y - centerY;
      if (dx * dx * radiusY * radiusY + dy * dy * radiusX * radiusX <=
          limit) {
        setPixel(pixels, frame, x, y, color);
      }
    }
  }
}

constexpr void fillDiamond(Pixels& pixels, int frame, int centerX, int centerY,
                           int radius, uint16_t color) {
  for (int y = -radius; y <= radius; ++y) {
    const int halfWidth = radius - (y < 0 ? -y : y);
    for (int x = -halfWidth; x <= halfWidth; ++x) {
      setPixel(pixels, frame, centerX + x, centerY + y, color);
    }
  }
}

constexpr Pixels makeSugarSkullPixels() {
  constexpr uint16_t kBlack = 0x0000;
  constexpr uint16_t kBone = 0xff9c;
  constexpr uint16_t kShadow = 0xc618;
  constexpr uint16_t kMagenta = 0xf81f;
  constexpr uint16_t kCyan = 0x07ff;
  constexpr uint16_t kOrange = 0xfd20;
  constexpr uint16_t kGreen = 0x07e0;
  constexpr uint16_t kYellow = 0xffe0;
  constexpr uint16_t kPurple = 0x8010;
  constexpr uint16_t accents[kSugarSkullFrameCount][3] = {
      {kMagenta, kCyan, kOrange},
      {kCyan, kOrange, kGreen},
      {kOrange, kGreen, kPurple},
      {kGreen, kPurple, kMagenta},
  };

  Pixels pixels{};
  for (int frame = 0; frame < kSugarSkullFrameCount; ++frame) {
    const uint16_t primary = accents[frame][0];
    const uint16_t secondary = accents[frame][1];
    const uint16_t tertiary = accents[frame][2];

    fillEllipse(pixels, frame, 32, 27, 23, 24, kBone);
    for (int y = 31; y <= 54; ++y) {
      const int halfWidth = 21 - (y - 31) / 4;
      for (int x = 32 - halfWidth; x <= 32 + halfWidth; ++x) {
        setPixel(pixels, frame, x, y, kBone);
      }
    }

    fillCircle(pixels, frame, 22, 28, 10, primary);
    fillCircle(pixels, frame, 42, 28, 10, secondary);
    fillCircle(pixels, frame, 22, 28, 6, kBlack);
    fillCircle(pixels, frame, 42, 28, 6, kBlack);
    fillCircle(pixels, frame, 20 + (frame & 1), 26, 2, kBone);
    fillCircle(pixels, frame, 40 + ((frame + 1) & 1), 26, 2, kBone);

    fillDiamond(pixels, frame, 32, 39, 5, kBlack);
    setPixel(pixels, frame, 29, 41, kBone);
    setPixel(pixels, frame, 35, 41, kBone);

    for (int x = 20; x <= 44; ++x) setPixel(pixels, frame, x, 48, kBlack);
    for (int x = 24; x <= 40; x += 4) {
      for (int y = 48; y <= 54; ++y) setPixel(pixels, frame, x, y, kBlack);
    }
    for (int x = 21; x <= 43; ++x) setPixel(pixels, frame, x, 55, kShadow);

    fillCircle(pixels, frame, 32, 12, 4, tertiary);
    fillCircle(pixels, frame, 32, 6, 3, primary);
    fillCircle(pixels, frame, 32, 18, 3, secondary);
    fillCircle(pixels, frame, 26, 12, 3, secondary);
    fillCircle(pixels, frame, 38, 12, 3, primary);
    fillCircle(pixels, frame, 32, 12, 2, kYellow);

    fillDiamond(pixels, frame, 12, 39, 3, secondary);
    fillDiamond(pixels, frame, 52, 39, 3, primary);
    fillDiamond(pixels, frame, 16, 45, 2, tertiary);
    fillDiamond(pixels, frame, 48, 45, 2, tertiary);

    for (int offset = 0; offset < 3; ++offset) {
      setPixel(pixels, frame, 8 + offset * 3, 20 - offset, primary);
      setPixel(pixels, frame, 56 - offset * 3, 20 - offset, secondary);
    }
  }
  return pixels;
}

}  // namespace sugar_skull_detail

constexpr auto kSugarSkullPixels = sugar_skull_detail::makeSugarSkullPixels();

}  // namespace sprite_display
