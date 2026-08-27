#pragma once

#include <cstddef>

enum class ShowcasePage {
  Clock,
  TextLogo,
  GraphicLogo,
  KeyboardImage,
  KeychronEnglish,
  KeychronChinese,
  QmkEnglish,
  QmkChinese,
  ZmkEnglish,
  ZmkChinese,
};

struct ShowcasePageInfo {
  ShowcasePage page;
  const char* title;
  const char* path;
  bool textPage;
  bool chinese;
};

namespace ShowcaseContent {
const ShowcasePageInfo& pageAt(size_t index);
size_t pageCount();
size_t indexOf(ShowcasePage page);
}  // namespace ShowcaseContent
