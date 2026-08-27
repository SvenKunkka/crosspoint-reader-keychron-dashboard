#include "WeatherService.h"

#include <ArduinoJson.h>
#include <HalStorage.h>
#include <Logging.h>

#include <cmath>
#include <cstdio>
#include <ctime>
#include <string>

#include "network/HttpDownloader.h"

namespace {
constexpr char TAG[] = "WTHR";
constexpr char kConfigPath[] = "/dashboard/config.json";
constexpr char kCachePath[] = "/dashboard/weather.json";
constexpr uint32_t kMinPlausibleEpoch = 1577836800;
}  // namespace

bool WeatherData::sameDisplay(const WeatherData& other) const {
  return valid == other.valid && weatherCode == other.weatherCode &&
         lroundf(temperature) == lroundf(other.temperature) && lroundf(high) == lroundf(other.high) &&
         lroundf(low) == lroundf(other.low);
}

void WeatherService::loadConfig(WeatherConfig& config) {
  const String content = Storage.readFile(kConfigPath);
  if (!content.isEmpty()) {
    JsonDocument doc;
    if (deserializeJson(doc, content) == DeserializationError::Ok) {
      config.latitude = doc["latitude"] | config.latitude;
      config.longitude = doc["longitude"] | config.longitude;
      config.useFahrenheit = doc["fahrenheit"] | config.useFahrenheit;
      const char* loc = doc["location"] | config.location;
      snprintf(config.location, sizeof(config.location), "%s", loc);
      return;
    }
    LOG_ERR(TAG, "config.json parse failed, using defaults");
  }

  Storage.ensureDirectoryExists("/dashboard");
  JsonDocument doc;
  doc["latitude"] = config.latitude;
  doc["longitude"] = config.longitude;
  doc["location"] = config.location;
  doc["fahrenheit"] = config.useFahrenheit;
  String out;
  serializeJsonPretty(doc, out);
  Storage.writeFile(kConfigPath, out);
}

bool WeatherService::loadCache(WeatherData& out) {
  const String content = Storage.readFile(kCachePath);
  if (content.isEmpty()) return false;

  JsonDocument doc;
  if (deserializeJson(doc, content) != DeserializationError::Ok) {
    LOG_ERR(TAG, "weather cache parse failed");
    return false;
  }

  out.temperature = doc["t"] | 0.0f;
  out.high = doc["hi"] | 0.0f;
  out.low = doc["lo"] | 0.0f;
  out.weatherCode = doc["code"] | -1;
  out.fetchedAt = doc["at"] | 0;
  out.valid = out.weatherCode >= 0;
  return out.valid;
}

static bool saveCache(const WeatherData& data) {
  Storage.ensureDirectoryExists("/dashboard");
  JsonDocument doc;
  doc["t"] = data.temperature;
  doc["hi"] = data.high;
  doc["lo"] = data.low;
  doc["code"] = data.weatherCode;
  doc["at"] = data.fetchedAt;
  String out;
  serializeJson(doc, out);
  return Storage.writeFile(kCachePath, out);
}

bool WeatherService::fetch(const WeatherConfig& config, WeatherData& out) {
  char url[256];
  snprintf(url, sizeof(url),
           "https://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f"
           "&current=temperature_2m,weather_code"
           "&daily=temperature_2m_max,temperature_2m_min"
           "&timezone=auto&forecast_days=1%s",
           static_cast<double>(config.latitude), static_cast<double>(config.longitude),
           config.useFahrenheit ? "&temperature_unit=fahrenheit" : "");

  std::string body;
  if (!HttpDownloader::fetchUrl(url, body) || body.empty()) {
    LOG_ERR(TAG, "fetch failed");
    return false;
  }

  JsonDocument filter;
  filter["current"]["temperature_2m"] = true;
  filter["current"]["weather_code"] = true;
  filter["daily"]["temperature_2m_max"] = true;
  filter["daily"]["temperature_2m_min"] = true;

  JsonDocument doc;
  const auto err = deserializeJson(doc, body, DeserializationOption::Filter(filter));
  if (err != DeserializationError::Ok) {
    LOG_ERR(TAG, "JSON parse failed: %s", err.c_str());
    return false;
  }
  if (doc["current"]["temperature_2m"].isNull()) {
    LOG_ERR(TAG, "response missing current temperature");
    return false;
  }

  WeatherData fresh;
  fresh.temperature = doc["current"]["temperature_2m"].as<float>();
  fresh.weatherCode = doc["current"]["weather_code"] | 0;
  fresh.high = doc["daily"]["temperature_2m_max"][0] | fresh.temperature;
  fresh.low = doc["daily"]["temperature_2m_min"][0] | fresh.temperature;
  const uint32_t now = static_cast<uint32_t>(time(nullptr));
  fresh.fetchedAt = now >= kMinPlausibleEpoch ? now : 0;
  fresh.valid = true;

  out = fresh;
  saveCache(fresh);
  return true;
}

const char* WeatherService::describe(const int weatherCode) {
  switch (weatherCode) {
    case 0: return "Clear sky";
    case 1: return "Mostly clear";
    case 2: return "Partly cloudy";
    case 3: return "Overcast";
    case 45: case 48: return "Fog";
    case 51: case 53: case 55: return "Drizzle";
    case 56: case 57: return "Freezing drizzle";
    case 61: return "Light rain";
    case 63: return "Rain";
    case 65: return "Heavy rain";
    case 66: case 67: return "Freezing rain";
    case 71: return "Light snow";
    case 73: return "Snow";
    case 75: return "Heavy snow";
    case 77: return "Snow grains";
    case 80: case 81: return "Rain showers";
    case 82: return "Heavy showers";
    case 85: case 86: return "Snow showers";
    case 95: return "Thunderstorm";
    case 96: case 99: return "Thunderstorm, hail";
    default: return "Unknown";
  }
}
