/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف شمار Glyph {
        GLYPH_SPACE,
        GLYPH_درخت_VERTICAL,
        GLYPH_درخت_BRANCH,
        GLYPH_درخت_دایاں,
        GLYPH_درخت_SPACE,
        GLYPH_درخت_چوٹی,
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
        GLYPH_بھرا_بلاک,
        GLYPH_SIGMA,
        GLYPH_ARROW_اوپر,
        GLYPH_ARROW_نیچے_کو,
        GLYPH_ARROW_بایاں,
        GLYPH_ARROW_دایاں,
        GLYPH_ELLIPSIS,
        GLYPH_EXTERNAL_LINK,
        GLYPH_پہلا_EMOJI,
        GLYPH_ECSTATIC_SMILEY = GLYPH_پہلا_EMOJI,
        GLYPH_HAPPY_SMILEY,
        GLYPH_SLIGHTLY_HAPPY_SMILEY,
        GLYPH_NEUTRAL_SMILEY,
        GLYPH_SLIGHTLY_UNHAPPY_SMILEY,
        GLYPH_UNHAPPY_SMILEY,
        GLYPH_DEPRESSED_SMILEY,
        GLYPH_تالا_AND_کلید,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_کم_BATTERY,
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

bool emoji_enabled(خالی);

ثابت حرف* glyph_بھرا(Glyph رمز, bool force_utf) _const_;

ساکن اندرونی ثابت حرف* glyph(Glyph رمز) {
        واپس glyph_بھرا(رمز, false);
}

ساکن اندرونی ثابت حرف* optional_glyph(Glyph رمز) {
        واپس emoji_enabled() ? glyph(رمز) : "";
}

ساکن اندرونی ثابت حرف* glyph_check_mark(bool b) {
        واپس b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

ساکن اندرونی ثابت حرف* glyph_check_mark_space(bool b) {
        واپس b ? glyph(GLYPH_CHECK_MARK) : " ";
}
