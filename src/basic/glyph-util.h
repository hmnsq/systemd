/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ Glyph {
        GLYPH_SPACE,
        GLYPH_ವೃಕ್ಷ_VERTICAL,
        GLYPH_ವೃಕ್ಷ_BRANCH,
        GLYPH_ವೃಕ್ಷ_ಬಲ,
        GLYPH_ವೃಕ್ಷ_SPACE,
        GLYPH_ವೃಕ್ಷ_ತುದಿ,
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
        GLYPH_ARROW_ಎಡ,
        GLYPH_ARROW_ಬಲ,
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
        GLYPH_LOCK_AND_ಕೀಲಿ,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_ಕೆಳಗಿನ_BATTERY,
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

bool emoji_enabled(ಶೂನ್ಯ);

ಧ್ರುವ ವರ್ಣ* glyph_full(Glyph code, bool force_utf) _const_;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* glyph(Glyph code) {
        ಪ್ರತ್ಯಾಗಮ glyph_full(code, false);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* optional_glyph(Glyph code) {
        ಪ್ರತ್ಯಾಗಮ emoji_enabled() ? glyph(code) : "";
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* glyph_check_mark(bool b) {
        ಪ್ರತ್ಯಾಗಮ b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* glyph_check_mark_space(bool b) {
        ಪ್ರತ್ಯಾಗಮ b ? glyph(GLYPH_CHECK_MARK) : " ";
}
