#pragma once

#include <GfxRenderer.h>

#include "dashboard/WeatherService.h"

enum class DashboardSyncStatus : uint8_t { None, Synced, Offline };

class DashboardView {
 public:
  static void render(GfxRenderer& renderer, const WeatherConfig& config, const WeatherData& weather,
                     DashboardSyncStatus syncStatus, bool showButtonHints);
};
