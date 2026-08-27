#include "DashboardActivity.h"

#include <WiFi.h>

#include "activities/network/WifiSelectionActivity.h"
#include "components/UITheme.h"

void DashboardActivity::onEnter() {
  Activity::onEnter();
  WeatherService::loadConfig(config);
  if (WeatherService::loadCache(weather)) {
    syncStatus = DashboardSyncStatus::None;
  }
  requestUpdate();
}

void DashboardActivity::onExit() {
  Activity::onExit();
  teardownWifi();
}

void DashboardActivity::teardownWifi() const {
  if (wifiWasUsed && WiFi.getMode() != WIFI_MODE_NULL) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }
}

void DashboardActivity::requestSync(const bool manual) {
  if (WiFi.status() == WL_CONNECTED) {
    pendingSync = true;
    pendingSyncIsManual = manual;
    return;
  }

  wifiWasUsed = true;
  startActivityForResult(std::make_unique<WifiSelectionActivity>(renderer, mappedInput, false),
                         [this, manual](const ActivityResult& result) {
                           if (result.isCancelled || WiFi.status() != WL_CONNECTED) {
                             syncStatus = DashboardSyncStatus::Offline;
                             teardownWifi();
                             requestUpdate();
                             return;
                           }
                           pendingSync = true;
                           pendingSyncIsManual = manual;
                         });
}

void DashboardActivity::doSync(const bool manual) {
  bool popupShown = false;
  if (manual) {
    RenderLock lock;
    GUI.drawPopup(renderer, "Updating weather...");
    popupShown = true;
  }

  WeatherData fresh = weather;
  const bool ok = WeatherService::fetch(config, fresh);
  teardownWifi();

  if (!ok) {
    syncStatus = DashboardSyncStatus::Offline;
    requestUpdate();
    return;
  }

  syncStatus = DashboardSyncStatus::Synced;
  const bool changed = !fresh.sameDisplay(weather);
  weather = fresh;

  if (changed || popupShown) {
    requestUpdate();
  }
}

void DashboardActivity::loop() {
  if (pendingSync) {
    pendingSync = false;
    doSync(pendingSyncIsManual);
    return;
  }
  if (mappedInput.wasPressed(MappedInputManager::Button::Confirm)) {
    requestSync(/*manual=*/true);
  }
}

void DashboardActivity::render(RenderLock&&) {
  DashboardView::render(renderer, config, weather, syncStatus, /*showButtonHints=*/true);
  renderer.displayBuffer(HalDisplay::FULL_REFRESH);
}
