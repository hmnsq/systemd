/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା Glyph {
        GLYPH_SPACE,
        GLYPH_ବୃକ୍ଷ_VERTICAL,
        GLYPH_ବୃକ୍ଷ_BRANCH,
        GLYPH_ବୃକ୍ଷ_ଡାହାଣ,
        GLYPH_ବୃକ୍ଷ_SPACE,
        GLYPH_ବୃକ୍ଷ_ଶୀର୍ଷ,
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
        GLYPH_ARROW_ବାମ,
        GLYPH_ARROW_ଡାହାଣ,
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
        GLYPH_LOCK_AND_ଚାବି,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_ତଳ_BATTERY,
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

bool emoji_enabled(ଶୂନ୍ୟ);

ଅଚଳ ଅକ୍ଷର* glyph_full(Glyph code, bool force_utf) _const_;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* glyph(Glyph code) {
        ଫେରନ୍ତୁ glyph_full(code, false);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* optional_glyph(Glyph code) {
        ଫେରନ୍ତୁ emoji_enabled() ? glyph(code) : "";
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* glyph_check_mark(bool b) {
        ଫେରନ୍ତୁ b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* glyph_check_mark_space(bool b) {
        ଫେରନ୍ତୁ b ? glyph(GLYPH_CHECK_MARK) : " ";
}
