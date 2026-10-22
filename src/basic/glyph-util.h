/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన Glyph {
        GLYPH_SPACE,
        GLYPH_వృక్షం_VERTICAL,
        GLYPH_వృక్షం_BRANCH,
        GLYPH_వృక్షం_కుడి,
        GLYPH_వృక్షం_SPACE,
        GLYPH_వృక్షం_శిఖరం,
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
        GLYPH_నిండిన_ఖండం,
        GLYPH_SIGMA,
        GLYPH_ARROW_పైకి,
        GLYPH_ARROW_కిందికి,
        GLYPH_ARROW_ఎడమ,
        GLYPH_ARROW_కుడి,
        GLYPH_ELLIPSIS,
        GLYPH_EXTERNAL_LINK,
        GLYPH_మొదటి_EMOJI,
        GLYPH_ECSTATIC_SMILEY = GLYPH_మొదటి_EMOJI,
        GLYPH_HAPPY_SMILEY,
        GLYPH_SLIGHTLY_HAPPY_SMILEY,
        GLYPH_NEUTRAL_SMILEY,
        GLYPH_SLIGHTLY_UNHAPPY_SMILEY,
        GLYPH_UNHAPPY_SMILEY,
        GLYPH_DEPRESSED_SMILEY,
        GLYPH_తాళం_AND_కీలకం,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_దిగువ_BATTERY,
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

bool emoji_enabled(శూన్య);

స్థిరరాశి అక్షరం* glyph_నిండిన(Glyph సంకేతం_2, bool force_utf) _const_;

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* glyph(Glyph సంకేతం_2) {
        తిరిగి glyph_నిండిన(సంకేతం_2, false);
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* optional_glyph(Glyph సంకేతం_2) {
        తిరిగి emoji_enabled() ? glyph(సంకేతం_2) : "";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* glyph_check_mark(bool b) {
        తిరిగి b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* glyph_check_mark_space(bool b) {
        తిరిగి b ? glyph(GLYPH_CHECK_MARK) : " ";
}
