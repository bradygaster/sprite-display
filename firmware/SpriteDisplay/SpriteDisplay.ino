#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <esp_heap_caps.h>

#include <algorithm>
#include <cstring>

#include "assets/SugarSkullSprite.h"
#include "src/Config.h"
#include "src/SpriteAnimator.h"
#include "src/SpriteRenderer.h"
#include "src/SpriteSheet.h"

using namespace sprite_display;

namespace {

Arduino_ESP32QSPI displayBus(12, 38, 4, 5, 6, 7);
Arduino_CO5300 display(&displayBus, 39, 0, kDisplayWidth, kDisplayHeight, 6, 0,
                       0, 0);

constexpr size_t kTransferBytes = 16384;
constexpr uint8_t kSpriteScale = 4;
constexpr SpriteSheet kSprite{
    kSugarSkullPixels.data(),
    kSugarSkullSheetWidth,
    kSugarSkullSheetHeight,
    kSugarSkullFrameWidth,
    kSugarSkullFrameHeight,
    kSugarSkullFrameCount,
    kSugarSkullFramesPerRow,
    frameDurationFromFps(3),
    true,
    true,
    0x0000,
    (kDisplayWidth - kSugarSkullFrameWidth * kSpriteScale) / 2,
    (kDisplayHeight - kSugarSkullFrameHeight * kSpriteScale) / 2,
    kSpriteScale,
};

uint16_t* framebuffer = nullptr;
uint8_t* transferBuffer = nullptr;
SpriteRenderer* renderer = nullptr;
SpriteAnimator animator(kSprite);

[[noreturn]] void fatal(const char* message) {
  Serial.printf("FATAL %s\n", message);
  display.fillScreen(0);
  display.setTextColor(0xf81f);
  display.setTextSize(2);
  display.setCursor(115, 220);
  display.print("SPRITE DISPLAY ERROR");
  for (;;) delay(2000);
}

void present() {
  display.startWrite();
  display.writeAddrWindow(0, 0, kDisplayWidth, kDisplayHeight);
  const auto* bytes = reinterpret_cast<const uint8_t*>(framebuffer);
  constexpr size_t frameBytes =
      static_cast<size_t>(kDisplayWidth) * kDisplayHeight * sizeof(uint16_t);
  for (size_t offset = 0; offset < frameBytes; offset += kTransferBytes) {
    const size_t count = std::min(kTransferBytes, frameBytes - offset);
    std::memcpy(transferBuffer, bytes + offset, count);
    displayBus.writeBytes(transferBuffer, count);
  }
  display.endWrite();
}

}  // namespace

void setup() {
  Serial.begin(115200);
  if (!psramFound()) fatal("8 MB OPI PSRAM not detected.");
  if (!display.begin(kSpiFrequency)) fatal("CO5300 initialization failed.");
  display.setBrightness(0);
  display.fillScreen(0);

  const auto validation =
      validateSpriteSheet(kSprite, kDisplayWidth, kDisplayHeight);
  if (validation != SpriteSheetError::None) {
    fatal(spriteSheetErrorMessage(validation));
  }

  constexpr size_t frameBytes =
      static_cast<size_t>(kDisplayWidth) * kDisplayHeight * sizeof(uint16_t);
  framebuffer = static_cast<uint16_t*>(
      heap_caps_malloc(frameBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  transferBuffer = static_cast<uint8_t*>(heap_caps_aligned_alloc(
      16, kTransferBytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL));
  if (!framebuffer || !transferBuffer) fatal("frame buffer allocation failed");

  static SpriteRenderer spriteRenderer(framebuffer, kDisplayWidth,
                                       kDisplayHeight);
  renderer = &spriteRenderer;
  animator.reset(millis());
  renderer->render(kSprite, 0);
  present();
  display.setBrightness(kBrightness);
  Serial.println("READY Sprite Display");
}

void loop() {
  static uint16_t shownFrame = UINT16_MAX;
  const uint32_t now = millis();
  const uint16_t frame = animator.frameAt(now);
  if (frame != shownFrame) {
    if (!renderer->render(kSprite, frame)) fatal("sprite rendering failed");
    present();
    shownFrame = frame;
  }
  delay(1);
}
