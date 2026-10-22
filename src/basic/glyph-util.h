/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع تعداد Glyph {
        GLYPH_SPACE,
        GLYPH_وڻ_VERTICAL,
        GLYPH_وڻ_BRANCH,
        GLYPH_وڻ_ساڄو,
        GLYPH_وڻ_SPACE,
        GLYPH_وڻ_چوٽي,
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
        GLYPH_ڀريل_بلاڪ,
        GLYPH_SIGMA,
        GLYPH_ARROW_مٿي,
        GLYPH_ARROW_هيٺ_ڏانهن,
        GLYPH_ARROW_کاٻو,
        GLYPH_ARROW_ساڄو,
        GLYPH_ELLIPSIS,
        GLYPH_EXTERNAL_LINK,
        GLYPH_پهريون_EMOJI,
        GLYPH_ECSTATIC_SMILEY = GLYPH_پهريون_EMOJI,
        GLYPH_HAPPY_SMILEY,
        GLYPH_SLIGHTLY_HAPPY_SMILEY,
        GLYPH_NEUTRAL_SMILEY,
        GLYPH_SLIGHTLY_UNHAPPY_SMILEY,
        GLYPH_UNHAPPY_SMILEY,
        GLYPH_DEPRESSED_SMILEY,
        GLYPH_تالو_AND_ڪنجي,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_هيٺيون_BATTERY,
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

bool emoji_enabled(فراغ);

ثابت حرف* glyph_ڀريل(Glyph ڪوڊ, bool force_utf) _const_;

ساكن مضمن ثابت حرف* glyph(Glyph ڪوڊ) {
        أرجع glyph_ڀريل(ڪوڊ, false);
}

ساكن مضمن ثابت حرف* optional_glyph(Glyph ڪوڊ) {
        أرجع emoji_enabled() ? glyph(ڪوڊ) : "";
}

ساكن مضمن ثابت حرف* glyph_check_mark(bool b) {
        أرجع b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

ساكن مضمن ثابت حرف* glyph_check_mark_space(bool b) {
        أرجع b ? glyph(GLYPH_CHECK_MARK) : " ";
}
