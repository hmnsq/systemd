/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ Glyph {
        GLYPH_SPACE,
        GLYPH_വൃക്ഷം_VERTICAL,
        GLYPH_വൃക്ഷം_BRANCH,
        GLYPH_വൃക്ഷം_വലത്,
        GLYPH_വൃക്ഷം_SPACE,
        GLYPH_വൃക്ഷം_മുകൾ,
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
        GLYPH_ARROW_ഇടത്,
        GLYPH_ARROW_വലത്,
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
        GLYPH_LOCK_AND_താക്കോൽ,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_താഴെയുള്ള_BATTERY,
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

bool emoji_enabled(ശൂന്യം);

സ്ഥിരം അക്ഷരം* glyph_full(Glyph code, bool force_utf) _const_;

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* glyph(Glyph code) {
        തിരികെ glyph_full(code, false);
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* optional_glyph(Glyph code) {
        തിരികെ emoji_enabled() ? glyph(code) : "";
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* glyph_check_mark(bool b) {
        തിരികെ b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* glyph_check_mark_space(bool b) {
        തിരികെ b ? glyph(GLYPH_CHECK_MARK) : " ";
}
