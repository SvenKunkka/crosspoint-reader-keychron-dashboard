#include "DashboardView.h"

#include <cmath>
#include <cstdio>
#include <ctime>

#include "CrossPointSettings.h"
#include "components/UITheme.h"
#include "fontIds.h"

namespace {
constexpr int kMargin = 32;
constexpr int kHeaderRuleY = 116;
constexpr int kWordmarkFontId = NOTOSANS_18_FONT_ID;
constexpr int kWordmarkTop = 48;
constexpr int kWordmarkTracking = 8;
constexpr char kWordmark[] = "Keychron";
constexpr int kWeatherTop = 190;
constexpr int kAgendaRuleY = 470;
constexpr int kDateTop = 540;
constexpr char kWeekday[] = "Thursday";
constexpr char kDate[] = "July 9, 2026";

void drawRule(GfxRenderer& renderer, const int y, const int thickness) {
  renderer.fillRect(kMargin, y, renderer.getScreenWidth() - 2 * kMargin, thickness, true);
}

void drawWordmark(GfxRenderer& renderer) {
  char one[2] = {0, 0};
  int total = -kWordmarkTracking;
  for (const char* ch = kWordmark; *ch != '\0'; ++ch) {
    one[0] = *ch;
    total += renderer.getTextWidth(kWordmarkFontId, one, EpdFontFamily::BOLD) + kWordmarkTracking;
  }
  int x = (renderer.getScreenWidth() - total) / 2;
  for (const char* ch = kWordmark; *ch != '\0'; ++ch) {
    one[0] = *ch;
    renderer.drawText(kWordmarkFontId, x, kWordmarkTop, one, true, EpdFontFamily::BOLD);
    x += renderer.getTextWidth(kWordmarkFontId, one, EpdFontFamily::BOLD) + kWordmarkTracking;
  }
}

void drawWeatherBand(GfxRenderer& renderer, const WeatherConfig& config, const WeatherData& weather,
                     const DashboardSyncStatus syncStatus) {
  char buf[48];
  int y = kWeatherTop;

  if (!weather.valid) {
    renderer.drawCenteredText(NOTOSANS_18_FONT_ID, y, "--", true, EpdFontFamily::BOLD);
    y += renderer.getLineHeight(NOTOSANS_18_FONT_ID) + 18;
    renderer.drawCenteredText(UI_12_FONT_ID, y, "No weather data - press OK to sync");
    return;
  }

  const char unit = config.useFahrenheit ? 'F' : 'C';
  snprintf(buf, sizeof(buf), "%ld\xC2\xB0%c", lroundf(weather.temperature), unit);
  renderer.drawCenteredText(NOTOSANS_18_FONT_ID, y, buf, true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(NOTOSANS_18_FONT_ID) + 18;

  renderer.drawCenteredText(NOTOSANS_16_FONT_ID, y, WeatherService::describe(weather.weatherCode));
  y += renderer.getLineHeight(NOTOSANS_16_FONT_ID) + 12;

  snprintf(buf, sizeof(buf), "H %ld\xC2\xB0   \xC2\xB7   L %ld\xC2\xB0", lroundf(weather.high), lroundf(weather.low));
  renderer.drawCenteredText(NOTOSANS_14_FONT_ID, y, buf);
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + 10;

  renderer.drawCenteredText(UI_12_FONT_ID, y, config.location);
  y += renderer.getLineHeight(UI_12_FONT_ID) + 8;

  if (syncStatus == DashboardSyncStatus::Synced) {
    if (weather.fetchedAt != 0) {
      const time_t t = static_cast<time_t>(weather.fetchedAt) + (SETTINGS.clockUtcOffsetQ - 48) * 900;
      struct tm tmv;
      gmtime_r(&t, &tmv);
      snprintf(buf, sizeof(buf), "Synced %02d:%02d", tmv.tm_hour, tmv.tm_min);
    } else {
      snprintf(buf, sizeof(buf), "Synced");
    }
  } else if (syncStatus == DashboardSyncStatus::Offline) {
    snprintf(buf, sizeof(buf), "Offline - showing cached data");
  } else {
    snprintf(buf, sizeof(buf), "Cached");
  }
  renderer.drawCenteredText(SMALL_FONT_ID, y, buf);
}
}  // namespace

void DashboardView::render(GfxRenderer& renderer, const WeatherConfig& config, const WeatherData& weather,
                           const DashboardSyncStatus syncStatus, const bool showButtonHints) {
  renderer.clearScreen();
  drawWordmark(renderer);
  drawRule(renderer, kHeaderRuleY, 2);
  drawWeatherBand(renderer, config, weather, syncStatus);
  drawRule(renderer, kAgendaRuleY, 1);

  int y = kDateTop;
  renderer.drawCenteredText(UI_12_FONT_ID, y, kWeekday);
  y += renderer.getLineHeight(UI_12_FONT_ID) + 8;
  renderer.drawCenteredText(NOTOSANS_18_FONT_ID, y, kDate, true, EpdFontFamily::BOLD);

  if (showButtonHints) {
    GUI.drawButtonHints(renderer, "", "Sync", "", "");
  }
}
