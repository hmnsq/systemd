/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા ગણના Glyph {
        GLYPH_SPACE,
        GLYPH_વૃક્ષ_VERTICAL,
        GLYPH_વૃક્ષ_BRANCH,
        GLYPH_વૃક્ષ_જમણું,
        GLYPH_વૃક્ષ_SPACE,
        GLYPH_વૃક્ષ_ટોચ,
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
        GLYPH_ARROW_ડાબું,
        GLYPH_ARROW_જમણું,
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
        GLYPH_LOCK_AND_ચાવી,
        GLYPH_TOUCH,
        GLYPH_RECYCLING,
        GLYPH_DOWNLOAD,
        GLYPH_SPARKLES,
        GLYPH_નીચલું_BATTERY,
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

bool emoji_enabled(શૂન્ય);

અચલ અક્ષર* glyph_full(Glyph code, bool force_utf) _const_;

સ્થિર અંતઃરેખ અચલ અક્ષર* glyph(Glyph code) {
        પાછા glyph_full(code, false);
}

સ્થિર અંતઃરેખ અચલ અક્ષર* optional_glyph(Glyph code) {
        પાછા emoji_enabled() ? glyph(code) : "";
}

સ્થિર અંતઃરેખ અચલ અક્ષર* glyph_check_mark(bool b) {
        પાછા b ? glyph(GLYPH_CHECK_MARK) : glyph(GLYPH_CROSS_MARK);
}

સ્થિર અંતઃરેખ અચલ અક્ષર* glyph_check_mark_space(bool b) {
        પાછા b ? glyph(GLYPH_CHECK_MARK) : " ";
}
