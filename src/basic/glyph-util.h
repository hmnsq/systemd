/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع تعداد Glyph {
        GLYPH_SPACE,
        GLYPH_ونه_VERTICAL,
        GLYPH_ونه_BRANCH,
        GLYPH_ونه_ښی,
        GLYPH_ونه_SPACE,
        GLYPH_ونه_سر,
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
        GLYPH_ډک_بلاک,
        GLYPH_SIGMA,
        GLYPH_ARROW_پورته,
        GLYPH_ARROW_ښکته,
        GLYPH_ARROW_کیڼ,
        GLYPH_ARROW_ښی,
        GLYPH_ELLIPSIS,
        GLYPH_EXTERNAL_LINK,
        GLYPH_لومړی_EMOJI,
        GLYPH_ECSTATIC_SMILEY = GLYPH_لومړی_EMOJI,
        GLYPH_HAPPY_SMILEY,
        GLYPH_SLIGHTLY_HAPPY_SMILEY,
        GLYPH_NEUTRAL_SMILEY,
        GLYPH_SLIGHTLY_UNHAPPY_SMILEY,
        GLYPH_UNHAPPY_SMILEY,
        GLYPH_DEPRESSED_SMILEY,
        GLYPH_قلف_AND_کلی,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_ټیټ_BATTERY,
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

ثابت حرف* glyph_ډک(Glyph کوډ, bool force_utf) _const_;

ساكن مضمن ثابت حرف* glyph(Glyph کوډ) {
        أرجع glyph_ډک(کوډ, false);
}

ساكن مضمن ثابت حرف* optional_glyph(Glyph کوډ) {
        أرجع emoji_enabled() ? glyph(کوډ) : "";
}

ساكن مضمن ثابت حرف* glyph_check_mark(bool b) {
        أرجع b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

ساكن مضمن ثابت حرف* glyph_check_mark_space(bool b) {
        أرجع b ? glyph(GLYPH_CHECK_MARK) : " ";
}
