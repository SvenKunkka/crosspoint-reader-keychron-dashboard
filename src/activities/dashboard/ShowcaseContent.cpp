#include "ShowcaseContent.h"

#include <array>

namespace {
constexpr std::array<ShowcasePageInfo, 10> kPages = {{
    {ShowcasePage::Clock, "Analog Clock", nullptr, false, false},
    {ShowcasePage::TextLogo, "Keychron Wordmark", nullptr, false, false},
    {ShowcasePage::GraphicLogo, "Keychron Graphic Logo", nullptr, false, false},
    {ShowcasePage::KeyboardImage, "Keyboard Image", "/keychron-showcase/images/keyboard.bmp", false, false},
    {ShowcasePage::KeychronEnglish, "Keychron English", "/keychron-showcase/text/keychron_en.txt", true, false},
    {ShowcasePage::KeychronChinese, "Keychron Chinese", "/keychron-showcase/text/keychron_zh.txt", true, true},
    {ShowcasePage::QmkEnglish, "QMK English", "/keychron-showcase/text/qmk_en.txt", true, false},
    {ShowcasePage::QmkChinese, "QMK Chinese", "/keychron-showcase/text/qmk_zh.txt", true, true},
    {ShowcasePage::ZmkEnglish, "ZMK English", "/keychron-showcase/text/zmk_en.txt", true, false},
    {ShowcasePage::ZmkChinese, "ZMK Chinese", "/keychron-showcase/text/zmk_zh.txt", true, true},
}};
}

namespace ShowcaseContent {

const ShowcasePageInfo& pageAt(const size_t index) { return kPages[index % kPages.size()]; }

size_t pageCount() { return kPages.size(); }

size_t indexOf(const ShowcasePage page) {
  for (size_t i = 0; i < kPages.size(); ++i) {
    if (kPages[i].page == page) return i;
  }
  return 0;
}

}  // namespace ShowcaseContent
