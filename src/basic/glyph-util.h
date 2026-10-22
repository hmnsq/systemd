/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Glyph {
        GLYPH_SPACE,
        GLYPH_ਰੁੱਖ_VERTICAL,
        GLYPH_ਰੁੱਖ_BRANCH,
        GLYPH_ਰੁੱਖ_ਸੱਜਾ,
        GLYPH_ਰੁੱਖ_SPACE,
        GLYPH_ਰੁੱਖ_ਸਿਖਰ,
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
        GLYPH_ਭਰਿਆ_ਖੰਡ,
        GLYPH_SIGMA,
        GLYPH_ARROW_ਉੱਪਰ,
        GLYPH_ARROW_ਥੱਲੇ,
        GLYPH_ARROW_ਖੱਬਾ,
        GLYPH_ARROW_ਸੱਜਾ,
        GLYPH_ELLIPSIS,
        GLYPH_EXTERNAL_LINK,
        GLYPH_ਪਹਿਲਾ_EMOJI,
        GLYPH_ECSTATIC_SMILEY = GLYPH_ਪਹਿਲਾ_EMOJI,
        GLYPH_HAPPY_SMILEY,
        GLYPH_SLIGHTLY_HAPPY_SMILEY,
        GLYPH_NEUTRAL_SMILEY,
        GLYPH_SLIGHTLY_UNHAPPY_SMILEY,
        GLYPH_UNHAPPY_SMILEY,
        GLYPH_DEPRESSED_SMILEY,
        GLYPH_ਤਾਲਾ_AND_ਕੁੰਜੀ,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_ਨੀਵਾਂ_BATTERY,
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

bool emoji_enabled(ਖਾਲੀ);

ਅਚਲ ਅੱਖਰ* glyph_ਭਰਿਆ(Glyph ਕੋਡ, bool force_utf) _const_;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* glyph(Glyph ਕੋਡ) {
        ਵਾਪਸ glyph_ਭਰਿਆ(ਕੋਡ, false);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* optional_glyph(Glyph ਕੋਡ) {
        ਵਾਪਸ emoji_enabled() ? glyph(ਕੋਡ) : "";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* glyph_check_mark(bool b) {
        ਵਾਪਸ b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* glyph_check_mark_space(bool b) {
        ਵਾਪਸ b ? glyph(GLYPH_CHECK_MARK) : " ";
}
