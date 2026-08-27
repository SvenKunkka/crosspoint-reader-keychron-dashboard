#include "ShowcaseRenderer.h"

#include <Bitmap.h>
#include <GfxRenderer.h>
#include <HalStorage.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <ctime>

#include "CrossPointSettings.h"
#include "components/UITheme.h"
#include "fontIds.h"

namespace {
constexpr int kMargin = 26;
constexpr int kFooterY = 626;
constexpr int kTextChunkBytes = 900;
constexpr float kPi = 3.14159265358979323846f;

void drawHeader(GfxRenderer& renderer, const char* title, const size_t pageIndex, const size_t pageCount) {
  char counter[24];
  snprintf(counter, sizeof(counter), "%u/%u", static_cast<unsigned>(pageIndex + 1), static_cast<unsigned>(pageCount));
  renderer.drawText(UI_10_FONT_ID, kMargin, 20, title, true, EpdFontFamily::BOLD);
  const int counterWidth = renderer.getTextWidth(SMALL_FONT_ID, counter);
  renderer.drawText(SMALL_FONT_ID, renderer.getScreenWidth() - kMargin - counterWidth, 24, counter);
  renderer.drawLine(kMargin, 54, renderer.getScreenWidth() - kMargin, 54, 2, true);
}

void drawHints(GfxRenderer& renderer, const bool textPage, const bool hasPreviousText, const bool hasMoreText) {
  const char* back = textPage && hasPreviousText ? "Page-" : "Prev";
  const char* ok = textPage ? (hasMoreText ? "Page+" : "Next") : "Next";
  GUI.drawButtonHints(renderer, back, ok, "Prev", "Next");
}

time_t localTimeNow() {
  time_t t = time(nullptr);
  if (t < 100000) {
    t = 1783569600;  // 2026-07-09 12:00:00 UTC fallback when RTC is unset.
  }
  return t + (SETTINGS.clockUtcOffsetQ - 48) * 900;
}

void drawAnalogClock(GfxRenderer& renderer) {
  const int cx = renderer.getScreenWidth() / 2;
  const int cy = 330;
  const int r = 178;

  for (int minute = 0; minute < 60; ++minute) {
    const float angle = (minute / 60.0f) * 2.0f * kPi - kPi / 2.0f;
    const int outerX = cx + static_cast<int>(std::cos(angle) * r);
    const int outerY = cy + static_cast<int>(std::sin(angle) * r);
    const int tickLength = minute % 5 == 0 ? 18 : 8;
    const int innerX = cx + static_cast<int>(std::cos(angle) * (r - tickLength));
    const int innerY = cy + static_cast<int>(std::sin(angle) * (r - tickLength));
    renderer.drawLine(innerX, innerY, outerX, outerY, minute % 5 == 0 ? 3 : 1, true);
  }

  renderer.drawCenteredText(NOTOSANS_18_FONT_ID, cy - r - 44, "Keychron", true, EpdFontFamily::BOLD);

  const time_t local = localTimeNow();
  struct tm tmv;
  gmtime_r(&local, &tmv);
  const float minuteAngle = (tmv.tm_min / 60.0f) * 2.0f * kPi - kPi / 2.0f;
  const float hourAngle = ((tmv.tm_hour % 12 + tmv.tm_min / 60.0f) / 12.0f) * 2.0f * kPi - kPi / 2.0f;

  renderer.drawLine(cx, cy, cx + static_cast<int>(std::cos(hourAngle) * 92),
                    cy + static_cast<int>(std::sin(hourAngle) * 92), 8, true);
  renderer.drawLine(cx, cy, cx + static_cast<int>(std::cos(minuteAngle) * 140),
                    cy + static_cast<int>(std::sin(minuteAngle) * 140), 4, true);
  renderer.fillRoundedRect(cx - 7, cy - 7, 14, 14, 7, Color::Black);

  char timeBuf[16];
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", tmv.tm_hour, tmv.tm_min);
  renderer.drawCenteredText(UI_12_FONT_ID, cy + r + 26, timeBuf, true, EpdFontFamily::BOLD);
  renderer.drawCenteredText(SMALL_FONT_ID, cy + r + 52, "refreshes once per minute");
}

void drawTextLogo(GfxRenderer& renderer) {
  renderer.drawCenteredText(NOTOSANS_18_FONT_ID, 210, "K e y c h r o n", true, EpdFontFamily::BOLD);
  renderer.drawLine(82, 272, renderer.getScreenWidth() - 82, 272, 4, true);
  renderer.drawCenteredText(NOTOSANS_16_FONT_ID, 330, "Mechanical keyboards");
  renderer.drawCenteredText(UI_12_FONT_ID, 370, "for Mac, Windows, and custom workflows");
  renderer.drawLine(118, 442, renderer.getScreenWidth() - 118, 442, 2, true);
}

void drawGraphicLogo(GfxRenderer& renderer) {
  const int cx = renderer.getScreenWidth() / 2;
  const int top = 150;
  renderer.drawRoundedRect(cx - 112, top, 224, 224, 5, 26, true);
  renderer.fillRect(cx - 64, top + 42, 26, 140, true);
  int x1[] = {cx - 28, cx + 72, cx + 42, cx - 42};
  int y1[] = {top + 112, top + 42, top + 80, top + 144};
  renderer.fillPolygon(x1, y1, 4, true);
  int x2[] = {cx - 22, cx + 82, cx + 50, cx - 38};
  int y2[] = {top + 118, top + 182, top + 190, top + 144};
  renderer.fillPolygon(x2, y2, 4, true);
  renderer.drawCenteredText(NOTOSANS_18_FONT_ID, top + 262, "KEYCHRON", true, EpdFontFamily::BOLD);
  renderer.drawCenteredText(UI_12_FONT_ID, top + 306, "open, programmable, durable");
}

void drawKeyboardImage(GfxRenderer& renderer) {
  HalFile file;
  if (Storage.openFileForRead("SHOW", "/keychron-showcase/images/keyboard.bmp", file)) {
    Bitmap bitmap(file, true);
    if (bitmap.parseHeaders() == BmpReaderError::Ok) {
      const int maxWidth = renderer.getScreenWidth() - 60;
      const int maxHeight = 430;
      const int x = (renderer.getScreenWidth() - std::min(bitmap.getWidth(), maxWidth)) / 2;
      renderer.drawBitmap(bitmap, x, 128, maxWidth, maxHeight, 0, 0);
      file.close();
      return;
    }
    file.close();
  }
  renderer.drawCenteredText(UI_12_FONT_ID, 300, "Missing /keychron-showcase/images/keyboard.bmp", true,
                            EpdFontFamily::BOLD);
}

std::string safeTextWindow(const std::string& text, const size_t offset) {
  if (offset >= text.size()) return {};
  size_t end = std::min(text.size(), offset + kTextChunkBytes);
  while (end > offset && (static_cast<unsigned char>(text[end]) & 0xC0) == 0x80) {
    --end;
  }
  return text.substr(offset, end - offset);
}

void drawTextPage(GfxRenderer& renderer, const ShowcasePageInfo& info, const std::string& text, const size_t textOffset,
                  const int textFontId) {
  const int fontId = textFontId != 0 ? textFontId : UI_10_FONT_ID;
  const int contentX = kMargin;
  const int contentY = 82;
  const int contentWidth = renderer.getScreenWidth() - 2 * kMargin;
  const int maxLines = (kFooterY - contentY - 10) / std::max(1, renderer.getLineHeight(fontId));
  const std::string window = safeTextWindow(text, textOffset);

  if (window.empty()) {
    renderer.drawCenteredText(UI_12_FONT_ID, 300, "Copy SD resource files to read this page", true,
                              EpdFontFamily::BOLD);
    renderer.drawCenteredText(SMALL_FONT_ID, 334, info.path ? info.path : "");
    return;
  }

  const auto lines = renderer.wrappedText(fontId, window.c_str(), contentWidth, maxLines);
  int y = contentY;
  for (const auto& line : lines) {
    renderer.drawText(fontId, contentX, y, line.c_str());
    y += renderer.getLineHeight(fontId);
  }
}
}  // namespace

namespace ShowcaseRenderer {

void renderPage(GfxRenderer& renderer, const ShowcasePageInfo& info, const size_t pageIndex, const size_t pageCount,
                const std::string& text, const size_t textOffset, const bool hasPreviousText, const bool hasMoreText,
                const int textFontId) {
  renderer.clearScreen();
  drawHeader(renderer, info.title, pageIndex, pageCount);

  switch (info.page) {
    case ShowcasePage::Clock:
      drawAnalogClock(renderer);
      break;
    case ShowcasePage::TextLogo:
      drawTextLogo(renderer);
      break;
    case ShowcasePage::GraphicLogo:
      drawGraphicLogo(renderer);
      break;
    case ShowcasePage::KeyboardImage:
      drawKeyboardImage(renderer);
      break;
    case ShowcasePage::KeychronEnglish:
    case ShowcasePage::KeychronChinese:
    case ShowcasePage::QmkEnglish:
    case ShowcasePage::QmkChinese:
    case ShowcasePage::ZmkEnglish:
    case ShowcasePage::ZmkChinese:
      drawTextPage(renderer, info, text, textOffset, textFontId);
      break;
  }

  drawHints(renderer, info.textPage, hasPreviousText, hasMoreText);
}

void renderSleep(GfxRenderer& renderer) {
  renderer.clearScreen();
  renderer.drawCenteredText(NOTOSANS_18_FONT_ID, 230, "Keychron", true, EpdFontFamily::BOLD);
  renderer.drawLine(86, 294, renderer.getScreenWidth() - 86, 294, 3, true);
  renderer.drawCenteredText(UI_12_FONT_ID, 352, "Programmable keyboards");
  renderer.drawCenteredText(SMALL_FONT_ID, 392, "sleep display");
}

}  // namespace ShowcaseRenderer
