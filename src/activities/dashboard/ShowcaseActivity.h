#pragma once

#include <string>

#include "ShowcaseContent.h"
#include "activities/Activity.h"

class ShowcaseActivity final : public Activity {
 public:
  ShowcaseActivity(GfxRenderer& renderer, MappedInputManager& mappedInput)
      : Activity("Showcase", renderer, mappedInput) {}

  void onEnter() override;
  void loop() override;
  void render(RenderLock&&) override;

 private:
  size_t pageIndex_ = 0;
  size_t textOffset_ = 0;
  unsigned long lastClockRefreshMs_ = 0;
  std::string loadedPath_;
  std::string loadedText_;
  int showcaseFontId_ = 0;

  void nextPage();
  void previousPage();
  void nextTextPage();
  void previousTextPage();
  void resetTextPaging();
  void ensureTextLoaded(const ShowcasePageInfo& info);
  void ensureShowcaseFont();
  bool currentHasMoreText() const;
};
