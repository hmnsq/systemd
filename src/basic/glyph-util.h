/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারনির্দেশ গণনা Glyph {
        GLYPH_SPACE,
        GLYPH_বৃক্ষ_VERTICAL,
        GLYPH_বৃক্ষ_BRANCH,
        GLYPH_বৃক্ষ_ডান,
        GLYPH_বৃক্ষ_SPACE,
        GLYPH_বৃক্ষ_শীর্ষ,
        GLYPH_VERTICAL_DOTTED,
        GLYPH_HORIZONTAL_DOTTED,
        GLYPH_HORIZONTAL_FAT,
        GLYPH_TRIANGULAR_BULLET,
        GLYPH_BLACK_CIRCLE,
        GLYPH_WHITE_CIRCLE,
        GLYPH_MULTIPLICATION_SIGN,
        GLYPH_CIRCLE_ARROW,
        GLYPH_BULLET,
        GLYPH_MU,
        GLYPH_CHECK_MARK,
        GLYPH_CROSS_MARK,
        GLYPH_LIGHT_SHADE,
        GLYPH_DARK_SHADE,
        GLYPH_FULL_BLOCK,
        GLYPH_SIGMA,
        GLYPH_ARROW_UP,
        GLYPH_ARROW_DOWN,
        GLYPH_ARROW_বাম,
        GLYPH_ARROW_ডান,
        GLYPH_ELLIPSIS,
        GLYPH_EXTERNAL_LINK,
        _GLYPH_FIRST_EMOJI,
        GLYPH_ECSTATIC_SMILEY = _GLYPH_FIRST_EMOJI,
        GLYPH_HAPPY_SMILEY,
        GLYPH_SLIGHTLY_HAPPY_SMILEY,
        GLYPH_NEUTRAL_SMILEY,
        GLYPH_SLIGHTLY_UNHAPPY_SMILEY,
        GLYPH_UNHAPPY_SMILEY,
        GLYPH_DEPRESSED_SMILEY,
        GLYPH_LOCK_AND_চাবি,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_নিচের_BATTERY,
        GLYPH_WARNING_SIGN,
        GLYPH_COMPUTER_DISK,
        GLYPH_WORLD,
        GLYPH_RED_CIRCLE,
        GLYPH_ORANGE_CIRCLE,
        GLYPH_YELLOW_CIRCLE,
        GLYPH_BLUE_CIRCLE,
        GLYPH_GREEN_CIRCLE,
        GLYPH_SUPERHERO,
        GLYPH_PUMPKIN,
        GLYPH_IDCARD,
        GLYPH_HOME,
        GLYPH_ROCKET,
        GLYPH_BROOM,
        GLYPH_KEYBOARD,
        GLYPH_CLOCK,
        GLYPH_LABEL,
        GLYPH_SHELL,
        _GLYPH_MAX,
        _GLYPH_INVALID = -EINVAL,
} Glyph;

bool emoji_enabled(শূন্য);

ধ্রুব বর্ণ* glyph_full(Glyph code, bool force_utf) _const_;

স্থির অন্তঃস্থ ধ্রুব বর্ণ* glyph(Glyph code) {
        প্রত্যাগম glyph_full(code, false);
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* optional_glyph(Glyph code) {
        প্রত্যাগম emoji_enabled() ? glyph(code) : "";
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* glyph_check_mark(bool b) {
        প্রত্যাগম b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* glyph_check_mark_space(bool b) {
        প্রত্যাগম b ? glyph(GLYPH_CHECK_MARK) : " ";
}
