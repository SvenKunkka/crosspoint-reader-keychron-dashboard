#!/usr/bin/env python3
from pathlib import Path
import re
import subprocess
from textwrap import dedent

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "sd_resources"
SHOWCASE_OUT = OUT / "keychron-showcase"


def words(text: str) -> int:
    return len(re.findall(r"[A-Za-z0-9]+(?:[-'][A-Za-z0-9]+)?", text))


def cjk_chars(text: str) -> int:
    return len(re.findall(r"[\u3400-\u9fff]", text))


def repeat_to_words(paragraphs, target=1050):
    text = "\n\n".join(p.strip() for p in paragraphs)
    i = 0
    while words(text) < target:
        text += "\n\n" + paragraphs[i % len(paragraphs)].strip()
        i += 1
    return text


def repeat_to_cjk(paragraphs, target=1050):
    text = "\n\n".join(p.strip() for p in paragraphs)
    i = 0
    while cjk_chars(text) < target:
        text += "\n\n" + paragraphs[i % len(paragraphs)].strip()
        i += 1
    return text


keychron_en = repeat_to_words([
    """Keychron is best understood as a keyboard company built around the daily rhythm of people who type, design, code, edit, and create for long stretches of time. The brand describes its work as designing and building high-quality keyboards and mice, and its public catalog shows a broad effort to make mechanical keyboards approachable without removing the depth that enthusiasts expect. Its products span low-profile portable boards, full-size office layouts, compact custom layouts, aluminum Q series boards, Hall Effect gaming models, and software-forward devices that can be tuned for Mac, Windows, and Linux workflows. That range matters because Keychron is not only selling a switch or a case shape. It is selling a path into better input tools: a first mechanical keyboard for a laptop user, a precise aluminum board for a workstation, a wireless board for travel, or a programmable platform for people who want complete control.""",
    """A defining part of the Keychron identity is cross-platform practicality. Many mechanical keyboards historically treated Mac users as an afterthought, but Keychron made Mac legends, Mac layout support, and quick switching between operating systems visible parts of the experience. That does not mean the boards are Mac-only. The same devices are commonly designed to work on Windows as well, with hardware switches, alternate keycaps, and firmware support that reduce friction between operating systems. For a user who moves from a MacBook to a Windows desktop or from a work laptop to a personal machine, that flexibility is not a small detail. It lets the keyboard become a stable tool instead of another device that must be re-learned every time the computer changes.""",
    """Keychron also sits at the intersection of finished consumer products and enthusiast customization. The company sells prebuilt boards that are ready to use, but many of those boards include hot-swappable switches, gasket or acoustic structures, QMK or ZMK firmware, VIA or launcher-based key remapping, and open design or firmware resources. That combination is important. A beginner can buy a keyboard, plug it in, and start typing. A more advanced user can change switches, tune keycaps, edit layers, define macros, and create layouts for specialized work. The best Keychron products therefore function like complete tools that still leave room for ownership.""",
    """The brand's public GitHub presence reinforces this direction. Keychron maintains or publishes firmware and hardware resources, including official QMK firmware for Keychron and Lemokey keyboards and source-available industrial design files for many keyboard and mouse models. This openness has practical value for users, repairers, accessory makers, and firmware developers. It also signals that a keyboard can be more than a sealed appliance. When a device exposes enough information for the community to study, adapt, and extend it, the lifespan of that device is not limited to the factory configuration.""",
    """On the product side, Keychron's reputation has grown because it addresses several layers of keyboard quality at once: layout, firmware, materials, switch feel, keycap durability, acoustic treatment, connectivity, and visual restraint. A good keyboard is not defined only by loud specifications. The height of the front edge, the stability of the case, the weight of the plate, the firmness of the gasket, the texture of the keycaps, the behavior of Bluetooth, and the clarity of the software all shape the final experience. Keychron's strongest boards are valued because they bring many of these pieces together in a form that can live on a desk every day.""",
    """For a brand presentation on an e-paper device, Keychron is a fitting subject because the values of the keyboard and the display overlap. Both reward patience, clarity, and low-distraction design. A keyboard disappears when it is working well; an e-paper panel does the same by showing only the necessary information with a calm visual texture. The Keychron story is therefore not only about mechanical switches. It is about making interaction with computers feel more deliberate, more personal, and less disposable."""
])

keychron_zh = repeat_to_cjk([
    "Keychron 可以被理解为一家围绕长期输入场景而成长起来的键盘品牌。它面向的是每天大量打字、写代码、设计、剪辑、办公和创作的人，而不是只追求短暂参数刺激的用户。品牌公开介绍中强调自己设计并制造高质量键盘和鼠标，产品线覆盖低矮轴、传统机械轴、铝合金客制化结构、霍尔磁轴、无线办公和游戏等多个方向。这种覆盖面说明 Keychron 不是单纯卖一个外壳或一种开关，而是在提供一套进入更好输入体验的路径：新手可以买到开箱即用的机械键盘，进阶用户可以继续调整手感、布局和固件。", 
    "Keychron 的一个重要特点是跨平台体验。过去很多机械键盘默认服务 Windows 用户，Mac 用户往往需要自己适配键位、图标和功能层。Keychron 很早就把 Mac 布局、Mac 图例、系统切换和替换键帽放进产品体验中，同时也保留 Windows 支持。对于在 MacBook、Windows 台式机、办公电脑和个人设备之间切换的人来说，这种兼容性很实际。键盘不再是某一台电脑的附属配件，而是可以长期跟随个人工作流的核心输入工具。", 
    "Keychron 也处在成品消费电子和客制化键盘之间。很多产品可以直接购买使用，但又保留热插拔轴座、可调键映射、QMK 或 ZMK 固件、VIA 或 Keychron Launcher 配置、声学材料和结构调校空间。这样的设计降低了入门门槛，也保留了探索空间。用户可以先把它当成普通键盘使用，之后再逐步更换轴体、键帽、定位板、层设置和宏命令。一个好的键盘品牌不应该强迫所有用户一开始就成为发烧友，而应该允许用户慢慢理解自己真正需要什么。", 
    "Keychron 在开源和公开资料方面也有明显动作。它在 GitHub 上提供 Keychron 和 Lemokey 相关的 QMK 固件资源，也公开了大量键盘和鼠标的工业设计文件，让用户、维修者、配件制作者和开发者能够更深入地理解产品。公开资料并不等于所有东西都完全开放，但它让设备不再是彻底封闭的黑盒。对于键盘这种长期使用的工具来说，可维护、可研究、可扩展本身就是价值。", 
    "从产品体验看，Keychron 的吸引力来自多个层面的组合：布局是否合理，外壳是否稳定，轴体是否顺滑，键帽是否耐用，声音是否克制，蓝牙和 2.4G 是否稳定，固件是否可配置，软件是否清晰。真正好的键盘不是某一个参数特别高，而是每天放在桌面上都不打扰用户。它应该稳定、明确、可靠，让输入动作自然发生。Keychron 的很多产品正是因为在这些维度上取得了平衡，才被大量普通用户和键盘爱好者同时接受。", 
    "把 Keychron 放到墨水屏设备上展示也很合适。墨水屏强调低功耗、低干扰和清晰的信息表达，键盘强调稳定、可控和长期陪伴。两者都不是为了制造过度刺激，而是为了让人与数字系统之间的接触更安静、更准确。Keychron 的品牌故事不只是机械轴和铝合金，它更像是在说明：输入工具可以被认真设计，用户也应该拥有更好的输入体验。"
])

qmk_en = repeat_to_words([
    """QMK, short for Quantum Mechanical Keyboard, is an open source keyboard firmware project and community centered on computer input devices. Its official site describes it as open-source keyboard firmware for Atmel AVR and Arm USB families, while the documentation frames QMK as a community that maintains firmware, configurators, flashing tools, web resources, and reference documentation. That distinction is important. QMK is not merely a binary file that runs on a keyboard. It is a shared vocabulary for layouts, layers, keycodes, macros, encoders, pointing devices, and the many behaviors that turn a matrix of switches into a personal input system.""",
    """The central idea behind QMK is customization at the firmware level. Instead of depending only on software running on a computer, the keyboard itself can understand layers, tap-hold behavior, mod-tap keys, macros, mouse keys, media controls, lighting states, and layout transformations. When a user defines a QMK keymap, the board can carry that logic with it. Plug the keyboard into another computer and the behavior remains the same because the firmware is inside the device. For power users, this is transformative: a compact keyboard can expose many virtual layers, a single key can behave differently when tapped or held, and repetitive commands can become hardware-level shortcuts.""",
    """QMK's compatibility is also a major part of its value. The QMK project states support for thousands of keyboards, and the number continues to grow through community and manufacturer contributions. This broad compatibility means users are not locked into one brand or one layout philosophy. A 40 percent board, a split ergonomic board, a full-size keyboard, a macropad, a MIDI controller, or a pointing-device hybrid can all share the same conceptual firmware environment. The documentation, source tree, and community examples make it possible to learn once and apply the same ideas across many devices.""",
    """For manufacturers such as Keychron, QMK support can make a keyboard more credible to enthusiasts because it signals that the board is not limited to a proprietary configuration tool. A QMK-compatible keyboard can participate in a larger ecosystem of open keymaps, VIA-compatible definitions, source code, and user experimentation. Keychron's public GitHub repository for QMK firmware lists official QMK firmware for Keychron and Lemokey keyboards, including Hall Effect support and online configurator compatibility. That kind of public firmware work helps connect finished products to the wider custom keyboard world.""",
    """QMK is powerful, but it also asks users to understand a few core ideas. A keyboard matrix converts switch positions into electrical signals; firmware scans that matrix and decides what keycodes or behaviors to send; layers allow one physical key to represent different functions; keymaps are the user's structured description of those decisions. Beginners can start with graphical tools such as QMK Configurator, while advanced users can build from source, edit C files, add features, and flash firmware with QMK Toolbox or command-line tools. This ladder of complexity is part of QMK's success: it can be approachable at the beginning and still deep enough for serious customization.""",
    """In the broader history of mechanical keyboards, QMK helped normalize the idea that firmware should be user-controlled. It gave hobbyists, small designers, and larger brands a common foundation. It made it easier for people to share layouts, maintain unusual boards, and treat keyboard behavior as something that can be designed rather than accepted. For a Keychron showcase, QMK represents openness, configurability, and the firmware culture behind many modern custom keyboards."""
])

qmk_zh = repeat_to_cjk([
    "QMK 是 Quantum Mechanical Keyboard 的缩写，是一个围绕计算机输入设备建立的开源键盘固件项目和社区。QMK 官网把它描述为面向 Atmel AVR 和 Arm USB 系列的开源键盘固件，文档则进一步说明 QMK 社区维护固件、配置器、刷写工具、网站和说明文档。也就是说，QMK 不只是一个可以刷进键盘的文件，它更像是一套关于键盘行为的公共语言：矩阵扫描、键码、层、宏、旋钮、鼠标键、组合键、点按和长按行为都可以被清楚定义。", 
    "QMK 的核心价值在于固件级自定义。普通键盘常常依赖电脑上的软件实现特殊功能，而 QMK 让键盘本身理解这些逻辑。用户可以把一个小键盘设计成多层工作台，把一个按键设置成轻按输出字母、长按变成修饰键，也可以让一组按键完成重复命令。由于行为写进键盘固件，换到另一台电脑后依然保持一致。这对程序员、设计师、文字工作者和重度快捷键用户都很有意义。", 
    "兼容性也是 QMK 的重要优势。QMK 官方页面提到它支持数千款键盘，而且兼容范围持续扩大。无论是四十配列、分体人体工学键盘、全尺寸键盘、宏键盘、MIDI 控制器，还是带指点设备的混合输入装置，都可以在 QMK 的概念框架下工作。用户学会层、键码和 keymap 之后，可以把同样的知识迁移到很多不同设备上。", 
    "对 Keychron 这样的品牌来说，QMK 支持意味着产品不只依赖封闭工具。支持 QMK 的键盘可以进入更大的开源生态：用户可以查看固件、分享布局、使用 VIA 或在线配置器，也可以在需要时自己编译。Keychron 的 GitHub 页面列出了 Keychron 和 Lemokey 键盘的官方 QMK 固件，并提到霍尔磁轴支持和在线配置器兼容。这让成品键盘和客制化社区之间形成了连接。", 
    "当然，QMK 的力量也伴随着学习成本。用户需要理解键盘矩阵如何把按键位置变成信号，固件如何扫描矩阵并输出键码，层如何让一个物理按键在不同情境下承担不同功能。新手可以先从图形化配置器开始，进阶用户再进入源码、C 语言 keymap、命令行构建和刷写工具。这样的层级结构很重要，因为它既允许普通用户快速上手，也允许深度用户继续探索。", 
    "从机械键盘的发展看，QMK 推动了一个观念：键盘行为不应该只能由厂商决定，用户也可以设计自己的输入方式。它让个人创作者、小型工作室和大品牌共享同一个技术基础，也让许多特殊配列和实验性设备能够长期维护。放在 Keychron 展示系统中，QMK 代表开放、可配置和现代客制化键盘背后的固件文化。"
])

zmk_en = repeat_to_words([
    """ZMK is a modern open source keyboard firmware built on the Zephyr Project real-time operating system. Its documentation describes it as MIT licensed and designed for power efficiency, flexibility, and broad hardware support, with the ability to support both wired and wireless input devices. The home page emphasizes a modern open source keyboard firmware, powered by Zephyr, with permissive licensing and a wireless-first design. Those phrases point to ZMK's position in the keyboard world: it was shaped for a generation of keyboards where Bluetooth, batteries, split halves, displays, and low-power sleep states are central concerns rather than optional extras.""",
    """The Zephyr foundation matters because it gives ZMK access to a mature embedded operating system with wide architecture support, hardware abstraction, drivers, scheduling, and power-management concepts. Instead of building every subsystem from scratch, ZMK can use Zephyr's infrastructure and focus on keyboard-specific behavior. This is especially helpful for wireless and split keyboards, where timing, radio behavior, battery reporting, peripheral shutdown, and multi-device connectivity all interact. A keyboard firmware for this world must care not only about which key was pressed, but also how long the battery lasts and how quickly the device wakes when the user returns.""",
    """ZMK's feature set reflects that focus. Its documentation lists support for wireless split keyboards, low active power usage, encoders, LED-based lighting, pointing devices, low power sleep states, battery level reporting, USB connectivity, keymaps and layers, a flexible behavior system, hold-taps, tap dances, sticky keys, combos, macros, and mouse keys. In practical terms, this means ZMK can deliver many of the customization ideas users expect from modern keyboard firmware while also paying close attention to wireless use. A compact wireless board can still have layers, combos, and advanced behaviors without giving up the battery-aware design needed for everyday portability.""",
    """ZMK also has a distinctive user setup model. Its installing guide explains that users can manage keyboard configurations, keymaps, and hardware details outside the core ZMK source repository, and many users can rely on GitHub Actions to build firmware rather than installing complicated local toolchains. This model lowers the barrier for wireless keyboard users who may not want to maintain a full embedded development environment. A configuration repository can become the personal source of truth for a user's layout, and cloud builds can produce firmware artifacts for flashing.""",
    """For Keychron, ZMK is relevant because modern wireless keyboards are increasingly expected to combine strong battery life, low latency, multiple connection modes, and serious customization. Keychron's public GitHub organization includes a ZMK fork, and public product coverage has connected Keychron's wireless high-performance keyboard direction with ZMK-based firmware. Whether a user chooses QMK or ZMK depends on hardware and priorities. QMK has deep history and wide wired-keyboard support; ZMK is especially attractive when Bluetooth, split operation, low power, and Zephyr-based hardware support are essential.""",
    """In a Keychron showcase, ZMK represents the wireless future of programmable keyboards. It shows that customization does not have to stop when a device leaves the USB cable. Layers, behaviors, and personal keymaps can coexist with battery reporting, sleep states, and modern microcontrollers. For users who want a clean desk, portable workflows, or split ergonomic layouts, that combination is increasingly important. ZMK's contribution is to make wireless keyboard firmware feel like a serious platform rather than a compromise."""
])

zmk_zh = repeat_to_cjk([
    "ZMK 是一个建立在 Zephyr Project 实时操作系统之上的现代开源键盘固件。官方文档说明它采用 MIT 许可证，目标是提供高效省电、灵活并具有广泛硬件支持的固件，同时能够用于有线和无线输入设备。ZMK 官网强调现代、开源、由 Zephyr 驱动、许可证宽松以及无线优先。这些关键词说明了 ZMK 在键盘世界中的位置：它面向的是蓝牙、电池、分体键盘、显示屏、低功耗睡眠和多设备连接越来越重要的新一代键盘。", 
    "Zephyr 基础很关键，因为它给 ZMK 带来了成熟嵌入式系统的能力，包括硬件抽象、驱动、任务调度、电源管理和多架构支持。ZMK 不需要从零开始实现所有底层能力，而是可以把重点放在键盘行为和用户配置上。对无线键盘和分体键盘来说，这尤其重要，因为按键扫描、蓝牙连接、电池状态、左右手通信、外设关断和唤醒速度都会互相影响。现代无线键盘固件不仅要知道哪个键被按下，还要知道如何省电、如何稳定连接、如何快速恢复。", 
    "ZMK 的功能也体现了这个方向。官方文档列出了无线分体键盘、低活动功耗、旋钮、LED 灯效、指点设备、低功耗睡眠、电池电量报告、USB 连接、键位层、灵活行为系统、hold-tap、tap dance、sticky key、combo、宏和鼠标键等能力。实际使用中，这意味着用户可以获得类似现代可编程键盘的高级功能，同时又不牺牲无线设备所需要的续航和休眠设计。", 
    "ZMK 的用户配置方式也很有特点。安装文档说明，用户可以在核心 ZMK 源码仓库之外管理自己的键盘配置、keymap 和硬件细节，许多用户不需要在本地安装复杂工具链，而是通过 GitHub Actions 自动构建固件。这种方式降低了无线键盘用户的门槛。一个配置仓库可以成为个人布局的长期记录，云端构建则负责输出可刷写固件。", 
    "对 Keychron 来说，ZMK 的意义在于现代无线键盘越来越需要同时满足续航、低延迟、多连接模式和深度自定义。Keychron 的 GitHub 组织中可以看到 ZMK 相关仓库，公开产品报道也把 Keychron 的高性能无线键盘方向与 ZMK 固件联系起来。QMK 和 ZMK 并不是简单替代关系。QMK 历史更长，有线和大量客制化键盘支持非常成熟；ZMK 则在蓝牙、分体、低功耗和 Zephyr 硬件生态方面更有吸引力。", 
    "放在 Keychron 展示系统中，ZMK 代表可编程键盘的无线未来。它说明个性化布局、层、组合键和宏并不一定只能依赖 USB 线。用户可以在无线设备上同时获得电池状态、休眠唤醒、多设备连接和灵活 keymap。对于追求整洁桌面、移动办公、分体人体工学或长续航的人来说，这种组合越来越重要。ZMK 的贡献是让无线键盘固件成为一个严肃平台，而不是对有线固件能力的妥协。"
])


TEXTS = {
    "keychron_en.txt": keychron_en,
    "keychron_zh.txt": keychron_zh,
    "qmk_en.txt": qmk_en,
    "qmk_zh.txt": qmk_zh,
    "zmk_en.txt": zmk_en,
    "zmk_zh.txt": zmk_zh,
}


def write_texts():
    text_dir = SHOWCASE_OUT / "text"
    text_dir.mkdir(parents=True, exist_ok=True)
    for name, text in TEXTS.items():
      (text_dir / name).write_text(text + "\n", encoding="utf-8")


def make_keyboard_bmp():
    image_dir = SHOWCASE_OUT / "images"
    image_dir.mkdir(parents=True, exist_ok=True)
    img = Image.new("1", (420, 260), 1)
    d = ImageDraw.Draw(img)
    d.rounded_rectangle((10, 28, 410, 235), radius=18, outline=0, width=4)
    d.rounded_rectangle((30, 50, 390, 215), radius=10, outline=0, width=2)
    rows = [62, 98, 134, 170]
    widths = [28, 30, 32, 34]
    for r, y in enumerate(rows):
        x = 48 + r * 8
        for i in range(10 - (1 if r == 3 else 0)):
            w = widths[r]
            d.rounded_rectangle((x, y, x + w, y + 22), radius=4, outline=0, width=2)
            x += w + 8
    d.rounded_rectangle((110, 202, 302, 222), radius=5, outline=0, width=2)
    d.text((148, 12), "KEYCHRON", fill=0)
    img.save(image_dir / "keyboard.bmp")


def make_cjk_intervals():
    chars = set()
    for name, text in TEXTS.items():
        if name.endswith("_zh.txt"):
            chars.update(ord(ch) for ch in text)
    # Include common full-width punctuation and ASCII fallback.
    chars.update(range(0x20, 0x7F))
    chars.update(ord(ch) for ch in "，。；：！？、（）《》“”‘’—0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzKeychronQMKZMK")
    chars = sorted(cp for cp in chars if cp >= 0x20)
    ranges = []
    start = prev = chars[0]
    for cp in chars[1:]:
        if cp == prev + 1:
            prev = cp
            continue
        ranges.append((start, prev))
        start = prev = cp
    ranges.append((start, prev))
    return ",".join(f"(0x{a:X}-0x{b:X})" for a, b in ranges)


def make_font():
    fonts_dir = OUT / ".fonts" / "KeychronCJK"
    fonts_dir.mkdir(parents=True, exist_ok=True)
    candidates = [
        Path("/System/Library/Fonts/Supplemental/Arial Unicode.ttf"),
        Path("/Library/Fonts/Arial Unicode.ttf"),
        Path("/System/Library/Fonts/Hiragino Sans GB.ttc"),
        Path("/System/Library/Fonts/STHeiti Medium.ttc"),
    ]
    font_path = next((path for path in candidates if path.exists()), None)
    if font_path is None:
        raise SystemExit("No suitable CJK font found on this Mac")
    intervals = make_cjk_intervals()
    subprocess.run([
        str(ROOT / ".venv-pio" / "bin" / "python"),
        str(ROOT / "lib" / "EpdFont" / "scripts" / "fontconvert_sdcard.py"),
        "--intervals", intervals,
        "--sizes", "14",
        "--style", "regular",
        str(font_path),
        "--output-dir", str(fonts_dir),
        "--name", "KeychronCJK",
    ], cwd=ROOT, check=True)


def write_manifest():
    manifest = ["Keychron Showcase M5 SD resource pack", ""]
    for name, text in TEXTS.items():
        if name.endswith("_en.txt"):
            manifest.append(f"{name}: {words(text)} English words")
        else:
            manifest.append(f"{name}: {cjk_chars(text)} CJK characters")
    manifest.append("")
    manifest.append("Sources summarized: keychron.com, github.com/keychron, qmk.fm, docs.qmk.fm, zmk.dev.")
    (SHOWCASE_OUT / "README.txt").write_text("\n".join(manifest) + "\n", encoding="utf-8")


def main():
    write_texts()
    make_keyboard_bmp()
    make_font()
    write_manifest()
    print(OUT)
    for name, text in TEXTS.items():
        print(name, words(text), "words", cjk_chars(text), "cjk")


if __name__ == "__main__":
    main()
