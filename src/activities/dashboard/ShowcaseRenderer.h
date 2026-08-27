#pragma once

#include <string>

#include "ShowcaseContent.h"

class GfxRenderer;

namespace ShowcaseRenderer {
void renderPage(GfxRenderer& renderer, const ShowcasePageInfo& info, size_t pageIndex, size_t pageCount,
                const std::string& text, size_t textOffset, bool hasPreviousText, bool hasMoreText, int textFontId);
void renderSleep(GfxRenderer& renderer);
}  // namespace ShowcaseRenderer
