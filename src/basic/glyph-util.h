/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Glyph {
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
        GLYPH_ತುಂಬಿದ_ಖಂಡ,
        GLYPH_SIGMA,
        GLYPH_ARROW_ಮೇಲೆ,
        GLYPH_ARROW_ಕೆಳಕ್ಕೆ,
        GLYPH_ARROW_ಎಡ,
        GLYPH_ARROW_ಬಲ,
        GLYPH_ELLIPSIS,
        GLYPH_EXTERNAL_LINK,
        GLYPH_ಮೊದಲ_EMOJI,
        GLYPH_ECSTATIC_SMILEY = GLYPH_ಮೊದಲ_EMOJI,
        GLYPH_HAPPY_SMILEY,
        GLYPH_SLIGHTLY_HAPPY_SMILEY,
        GLYPH_NEUTRAL_SMILEY,
        GLYPH_SLIGHTLY_UNHAPPY_SMILEY,
        GLYPH_UNHAPPY_SMILEY,
        GLYPH_DEPRESSED_SMILEY,
        GLYPH_ಬೀಗ_AND_ಕೀಲಿ,
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

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* glyph_ತುಂಬಿದ(Glyph ಸಂಕೇತ_2, bool force_utf) _const_;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* glyph(Glyph ಸಂಕೇತ_2) {
        ಹಿಂತಿರುಗಿ glyph_ತುಂಬಿದ(ಸಂಕೇತ_2, false);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* optional_glyph(Glyph ಸಂಕೇತ_2) {
        ಹಿಂತಿರುಗಿ emoji_enabled() ? glyph(ಸಂಕೇತ_2) : "";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* glyph_check_mark(bool b) {
        ಹಿಂತಿರುಗಿ b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* glyph_check_mark_space(bool b) {
        ಹಿಂತಿರುಗಿ b ? glyph(GLYPH_CHECK_MARK) : " ";
}
