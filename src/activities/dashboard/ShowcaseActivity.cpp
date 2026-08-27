#include "ShowcaseActivity.h"

#include <Arduino.h>
#include <HalStorage.h>

#include <algorithm>
#include <cstring>

#include "CrossPointSettings.h"
#include "SdCardFontSystem.h"
#include "ShowcaseRenderer.h"
#include "fontIds.h"

namespace {
constexpr size_t kEnglishPageStep = 820;
constexpr size_t kChinesePageStep = 520;

size_t clampToUtf8Boundary(const std::string& text, size_t offset) {
  offset = std::min(offset, text.size());
  while (offset > 0 && offset < text.size() && (static_cast<unsigned char>(text[offset]) & 0xC0) == 0x80) {
    --offset;
  }
  return offset;
}
}  // namespace

void ShowcaseActivity::onEnter() {
  Activity::onEnter();
  ensureShowcaseFont();
  requestUpdate();
}

void ShowcaseActivity::ensureShowcaseFont() {
  sdFontSystem.markRegistryDirty();
  strncpy(SETTINGS.sdFontFamilyName, "KeychronCJK", sizeof(SETTINGS.sdFontFamilyName) - 1);
  SETTINGS.sdFontFamilyName[sizeof(SETTINGS.sdFontFamilyName) - 1] = '\0';
  sdFontSystem.ensureLoaded(renderer);
  showcaseFontId_ = SETTINGS.getReaderFontId();
  if (showcaseFontId_ == 0) {
    showcaseFontId_ = UI_10_FONT_ID;
  }
}

void ShowcaseActivity::loop() {
  const auto& info = ShowcaseContent::pageAt(pageIndex_);

  if (info.page == ShowcasePage::Clock && millis() - lastClockRefreshMs_ >= 60000) {
    lastClockRefreshMs_ = millis();
    requestUpdate();
  }

  if (mappedInput.wasPressed(MappedInputManager::Button::Confirm)) {
    if (info.textPage && currentHasMoreText()) {
      nextTextPage();
    } else {
      nextPage();
    }
    requestUpdate();
    return;
  }

  if (mappedInput.wasPressed(MappedInputManager::Button::Back)) {
    if (info.textPage && textOffset_ > 0) {
      previousTextPage();
    } else {
      previousPage();
    }
    requestUpdate();
    return;
  }

  if (mappedInput.wasPressed(MappedInputManager::Button::NavNext) ||
      mappedInput.wasPressed(MappedInputManager::Button::PageForward)) {
    nextPage();
    requestUpdate();
    return;
  }

  if (mappedInput.wasPressed(MappedInputManager::Button::NavPrevious) ||
      mappedInput.wasPressed(MappedInputManager::Button::PageBack)) {
    previousPage();
    requestUpdate();
    return;
  }
}

void ShowcaseActivity::render(RenderLock&&) {
  const auto& info = ShowcaseContent::pageAt(pageIndex_);
  ensureTextLoaded(info);
  ShowcaseRenderer::renderPage(renderer, info, pageIndex_, ShowcaseContent::pageCount(), loadedText_, textOffset_,
                               info.textPage && textOffset_ > 0, info.textPage && currentHasMoreText(),
                               showcaseFontId_);
  renderer.displayBuffer(HalDisplay::FULL_REFRESH);
}

void ShowcaseActivity::nextPage() {
  pageIndex_ = (pageIndex_ + 1) % ShowcaseContent::pageCount();
  resetTextPaging();
}

void ShowcaseActivity::previousPage() {
  pageIndex_ = pageIndex_ == 0 ? ShowcaseContent::pageCount() - 1 : pageIndex_ - 1;
  resetTextPaging();
}

void ShowcaseActivity::nextTextPage() {
  const auto& info = ShowcaseContent::pageAt(pageIndex_);
  const size_t step = info.chinese ? kChinesePageStep : kEnglishPageStep;
  textOffset_ = clampToUtf8Boundary(loadedText_, textOffset_ + step);
}

void ShowcaseActivity::previousTextPage() {
  const auto& info = ShowcaseContent::pageAt(pageIndex_);
  const size_t step = info.chinese ? kChinesePageStep : kEnglishPageStep;
  textOffset_ = textOffset_ > step ? textOffset_ - step : 0;
  textOffset_ = clampToUtf8Boundary(loadedText_, textOffset_);
}

void ShowcaseActivity::resetTextPaging() {
  textOffset_ = 0;
  loadedPath_.clear();
  loadedText_.clear();
}

void ShowcaseActivity::ensureTextLoaded(const ShowcasePageInfo& info) {
  if (!info.textPage || info.path == nullptr) {
    loadedPath_.clear();
    loadedText_.clear();
    return;
  }

  if (loadedPath_ == info.path && !loadedText_.empty()) {
    return;
  }

  loadedPath_ = info.path;
  loadedText_.clear();
  HalFile file;
  if (!Storage.openFileForRead("SHOW", info.path, file)) {
    return;
  }

  const size_t size = std::min<size_t>(file.size(), 24576);
  loadedText_.resize(size);
  const int bytesRead = file.read(&loadedText_[0], size);
  file.close();
  if (bytesRead <= 0) {
    loadedText_.clear();
    return;
  }
  loadedText_.resize(static_cast<size_t>(bytesRead));
  textOffset_ = clampToUtf8Boundary(loadedText_, textOffset_);
}

bool ShowcaseActivity::currentHasMoreText() const {
  const auto& info = ShowcaseContent::pageAt(pageIndex_);
  if (!info.textPage || loadedText_.empty()) {
    return false;
  }
  const size_t step = info.chinese ? kChinesePageStep : kEnglishPageStep;
  return textOffset_ + step < loadedText_.size();
}
