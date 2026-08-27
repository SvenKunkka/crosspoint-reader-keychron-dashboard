#pragma once

#include <cstdint>

struct WeatherConfig {
  float latitude = 22.5431f;
  float longitude = 114.0579f;
  char location[28] = "Shenzhen";
  bool useFahrenheit = false;
};

struct WeatherData {
  float temperature = 0.0f;
  float high = 0.0f;
  float low = 0.0f;
  int weatherCode = -1;
  uint32_t fetchedAt = 0;
  bool valid = false;

  bool sameDisplay(const WeatherData& other) const;
};

class WeatherService {
 public:
  static void loadConfig(WeatherConfig& config);
  static bool loadCache(WeatherData& out);
  static bool fetch(const WeatherConfig& config, WeatherData& out);
  static const char* describe(int weatherCode);
};
