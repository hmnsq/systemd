/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை Glyph {
        GLYPH_SPACE,
        GLYPH_மரம்_VERTICAL,
        GLYPH_மரம்_BRANCH,
        GLYPH_மரம்_வலது,
        GLYPH_மரம்_SPACE,
        GLYPH_மரம்_உச்சி,
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
        GLYPH_ARROW_இடது,
        GLYPH_ARROW_வலது,
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
        GLYPH_LOCK_AND_சாவி,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_கீழ்_BATTERY,
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

bool emoji_enabled(வெறுமை);

மாறிலி எழுத்து* glyph_full(Glyph code, bool force_utf) _const_;

நிலையான உள்வரி மாறிலி எழுத்து* glyph(Glyph code) {
        திரும்பு glyph_full(code, false);
}

நிலையான உள்வரி மாறிலி எழுத்து* optional_glyph(Glyph code) {
        திரும்பு emoji_enabled() ? glyph(code) : "";
}

நிலையான உள்வரி மாறிலி எழுத்து* glyph_check_mark(bool b) {
        திரும்பு b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

நிலையான உள்வரி மாறிலி எழுத்து* glyph_check_mark_space(bool b) {
        திரும்பு b ? glyph(GLYPH_CHECK_MARK) : " ";
}
