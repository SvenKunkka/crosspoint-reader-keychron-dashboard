#pragma once

#include "activities/Activity.h"
#include "activities/dashboard/DashboardView.h"
#include "dashboard/WeatherService.h"

class DashboardActivity final : public Activity {
 public:
  DashboardActivity(GfxRenderer& renderer, MappedInputManager& mappedInput)
      : Activity("Dashboard", renderer, mappedInput) {}

  void onEnter() override;
  void onExit() override;
  void loop() override;
  void render(RenderLock&&) override;

 private:
  WeatherConfig config;
  WeatherData weather;
  bool pendingSync = false;
  bool pendingSyncIsManual = false;
  bool wifiWasUsed = false;
  DashboardSyncStatus syncStatus = DashboardSyncStatus::None;

  void requestSync(bool manual);
  void doSync(bool manual);
  void teardownWifi() const;
};
