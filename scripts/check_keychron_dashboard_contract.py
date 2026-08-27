#!/usr/bin/env python3
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        print(f"FAIL: {message}")
        sys.exit(1)


showcase_cpp_path = ROOT / "src/activities/dashboard/ShowcaseActivity.cpp"
showcase_renderer_cpp_path = ROOT / "src/activities/dashboard/ShowcaseRenderer.cpp"
showcase_content_cpp_path = ROOT / "src/activities/dashboard/ShowcaseContent.cpp"
sleep_cpp_path = ROOT / "src/activities/boot_sleep/SleepActivity.cpp"
activity_manager_h_path = ROOT / "src/activities/ActivityManager.h"
activity_manager_cpp_path = ROOT / "src/activities/ActivityManager.cpp"
resource_dir = ROOT / "sd_resources/keychron-showcase"
font_file = ROOT / "sd_resources/.fonts/KeychronCJK/KeychronCJK_14.cpfont"

require(showcase_cpp_path.exists(), "ShowcaseActivity.cpp must exist")
require(showcase_renderer_cpp_path.exists(), "ShowcaseRenderer.cpp must exist")
require(showcase_content_cpp_path.exists(), "ShowcaseContent.cpp must exist")

showcase_cpp = read("src/activities/dashboard/ShowcaseActivity.cpp")
showcase_renderer_cpp = read("src/activities/dashboard/ShowcaseRenderer.cpp")
showcase_content_cpp = read("src/activities/dashboard/ShowcaseContent.cpp")
sleep_cpp = read("src/activities/boot_sleep/SleepActivity.cpp")
activity_manager_h = read("src/activities/ActivityManager.h")
activity_manager_cpp = read("src/activities/ActivityManager.cpp")

require("ShowcasePage::Clock" in showcase_content_cpp, "clock page must be defined")
require("ShowcasePage::TextLogo" in showcase_content_cpp, "text logo page must be defined")
require("ShowcasePage::GraphicLogo" in showcase_content_cpp, "graphic logo page must be defined")
require("ShowcasePage::KeyboardImage" in showcase_content_cpp, "keyboard image page must be defined")
require("ShowcasePage::KeychronEnglish" in showcase_content_cpp, "Keychron English page must be defined")
require("ShowcasePage::KeychronChinese" in showcase_content_cpp, "Keychron Chinese page must be defined")
require("ShowcasePage::QmkEnglish" in showcase_content_cpp, "QMK English page must be defined")
require("ShowcasePage::QmkChinese" in showcase_content_cpp, "QMK Chinese page must be defined")
require("ShowcasePage::ZmkEnglish" in showcase_content_cpp, "ZMK English page must be defined")
require("ShowcasePage::ZmkChinese" in showcase_content_cpp, "ZMK Chinese page must be defined")
require("millis()" in showcase_cpp and "60000" in showcase_cpp, "clock must refresh on minute cadence")
require("requestSync(/*manual=*/false)" not in showcase_cpp, "showcase must not auto-start WiFi on entry")
require("onGoHome()" not in showcase_cpp, "showcase Back must not return to stock home")
require("ShowcaseRenderer::renderSleep" in sleep_cpp, "sleep screen must reuse showcase renderer")
require("goToShowcase" in activity_manager_h, "ActivityManager must expose showcase routing")
require("goToShowcase();" in activity_manager_cpp, "default home route must enter showcase")
require("HomeActivity" in activity_manager_cpp, "stock home must remain available for explicit internal routes")
require('"Prev"' in showcase_renderer_cpp, "button hints must keep previous-page action")
require('"Next"' in showcase_renderer_cpp, "button hints must keep next-page action")
require('"Page+"' in showcase_renderer_cpp, "button hints must keep next text-page action")
require('"Page-"' in showcase_renderer_cpp, "button hints must keep previous text-page action")
require(resource_dir.exists(), "sd_resources/keychron-showcase must exist")
require((resource_dir / "text").exists(), "SD text resource directory must exist")
require((resource_dir / "images" / "keyboard.bmp").exists(), "keyboard BMP must exist")
require(font_file.exists(), "KeychronCJK SD font must exist at SD card root .fonts directory")

print("keychron showcase contract ok")
