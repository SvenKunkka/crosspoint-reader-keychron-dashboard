# Keychron Showcase M5 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build an X4 firmware and SD resource pack that turns the device into a Keychron showcase with clock, logo, image, brand, QMK, and ZMK pages.

**Architecture:** Replace the single dashboard with a `ShowcaseActivity` that owns page navigation, minute-based clock refresh, SD text loading, and rendering. Keep CrossPoint recovery and lower-level pages available, but make the showcase the default home and sleep screen. Store long text and CJK font resources on the SD card to avoid app partition bloat.

**Tech Stack:** CrossPoint Reader firmware, PlatformIO/pioarduino, ESP32-C3, CrossPoint `GfxRenderer`, `HalStorage`, `SdCardFontSystem`, SD `.txt`/`.cpfont` resources.

---

### Task 1: Add Contract Checks

**Files:**
- Modify: `scripts/check_keychron_dashboard_contract.py`

- [ ] Update the check script so M5 requires a showcase activity, no stock home default, no auto WiFi, page count markers, SD text resources, and sleep-screen reuse.
- [ ] Run the check and confirm it fails before implementation.

### Task 2: Generate SD Resource Pack

**Files:**
- Create: `tools/build_m5_resources.py`
- Create output: `<firmware-workspace>/m5-sd-card/keychron-showcase/text/*.txt`
- Create output: `<firmware-workspace>/m5-sd-card/keychron-showcase/images/keyboard.bmp`
- Create output: `<firmware-workspace>/m5-sd-card/keychron-showcase/fonts/*.cpfont`

- [ ] Generate English and Chinese long-form text for Keychron, QMK, and ZMK based on official sources gathered in the current session.
- [ ] Ensure each required text is at least 1000 words for English or 1000 CJK characters for Chinese.
- [ ] Generate a monochrome keyboard BMP for the image page.
- [ ] Generate or copy CJK SD font resources when available.

### Task 3: Implement Showcase Pages

**Files:**
- Create: `src/activities/dashboard/ShowcaseActivity.{h,cpp}`
- Create: `src/activities/dashboard/ShowcaseContent.{h,cpp}`
- Create: `src/activities/dashboard/ShowcaseRenderer.{h,cpp}`
- Modify: `src/activities/ActivityManager.{h,cpp}`
- Modify: `src/activities/boot_sleep/SleepActivity.{h,cpp}`

- [ ] Add page enum and navigation behavior: Up/Down changes pages, OK/Back pages text within long articles.
- [ ] Add analog clock page with no second hand and minute-based refresh.
- [ ] Add text logo, graphic logo, keyboard image, Keychron EN/ZH, QMK EN/ZH, ZMK EN/ZH pages.
- [ ] Render sleep screen using the current showcase page style without button hints or WiFi.

### Task 4: Build and Package

**Files:**
- Output: `<firmware-workspace>/m5-sd-flash/update.bin`
- Output: `<firmware-workspace>/keychron-showcase-m5.bin`

- [ ] Run contract checks.
- [ ] Build firmware with `pio run -e default`.
- [ ] Copy firmware outputs without overwriting older M3/M4 files.
- [ ] Verify SHA256, size, and ESP32 image header.
