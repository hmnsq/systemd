/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

/* Limits the use of ANSI colors to a subset. */
प्रकारनिर्देश गणना Color_बहुलकम् {
        COLOR_OFF,        /* No colors, monochrome output. */
        COLOR_16,         /* Only the base 16 colors. */
        COLOR_256,        /* Only 256 colors. */
        COLOR_24BIT,      /* For truecolor or 24bit color support, no restriction. */
        COLOR_बहुलकम्_FIXED_MAX,

        /* The "AUTO" modes are as the above, but subject to suitable settings for the environment variables
         * TERM and NO_COLOR. */
        COLOR_AUTO_16 = COLOR_बहुलकम्_FIXED_MAX,
        COLOR_AUTO_256,
        COLOR_AUTO_24BIT,

        /* Same as default (unset), except that $NO_COLOR is ignored/overridden */
        COLOR_TRUE,

        COLOR_बहुलकम्_MAX,
        COLOR_बहुलकम्_INVALID = -EINVAL,
} Color_बहुलकम्;

DECLARE_STRING_सारणी_LOOKUP(color_बहुलकम्, Color_बहुलकम्);

Color_बहुलकम् get_color_बहुलकम्(शून्य);
स्थिर अन्तःस्थ bool colors_enabled(शून्य) {
        /* Returns true if colors are considered supported on our stdout. */
        प्रत्यागम get_color_बहुलकम्() != COLOR_OFF;
}

Color_बहुलकम् parse_systemd_colors(शून्य);

bool underline_enabled(शून्य);

शून्य reset_ansi_feature_caches(शून्य);

bool looks_like_ansi_color_संहिता(ध्रुव वर्ण *str);

/* Regular colors */
#परिभाषय ANSI_BLACK   "\x1B[0;30m" /* Some type of grey usually. */
#परिभाषय ANSI_RED     "\x1B[0;31m"
#परिभाषय ANSI_GREEN   "\x1B[0;32m"
#परिभाषय ANSI_YELLOW  "\x1B[0;33m"
#परिभाषय ANSI_BLUE    "\x1B[0;34m"
#परिभाषय ANSI_MAGENTA "\x1B[0;35m"
#परिभाषय ANSI_CYAN    "\x1B[0;36m"
#परिभाषय ANSI_WHITE   "\x1B[0;37m" /* This is actually rendered as light grey, legible even on a white
                                   * background. See ANSI_HIGHLIGHT_WHITE for real white. */

#परिभाषय ANSI_BRIGHT_BLACK   "\x1B[0;90m"
#परिभाषय ANSI_BRIGHT_RED     "\x1B[0;91m"
#परिभाषय ANSI_BRIGHT_GREEN   "\x1B[0;92m"
#परिभाषय ANSI_BRIGHT_YELLOW  "\x1B[0;93m"
#परिभाषय ANSI_BRIGHT_BLUE    "\x1B[0;94m"
#परिभाषय ANSI_BRIGHT_MAGENTA "\x1B[0;95m"
#परिभाषय ANSI_BRIGHT_CYAN    "\x1B[0;96m"
#परिभाषय ANSI_BRIGHT_WHITE   "\x1B[0;97m"

#परिभाषय ANSI_GREY    "\x1B[0;38:5:245m"

/* Bold/highlighted */
#परिभाषय ANSI_HIGHLIGHT_BLACK    "\x1B[0;1;30m"
#परिभाषय ANSI_HIGHLIGHT_RED      "\x1B[0;1;31m"
#परिभाषय ANSI_HIGHLIGHT_GREEN    "\x1B[0;1;32m"
#परिभाषय _ANSI_HIGHLIGHT_YELLOW  "\x1B[0;1;33m" /* This yellow is currently not displayed well by some terminals */
#परिभाषय ANSI_HIGHLIGHT_BLUE     "\x1B[0;1;34m"
#परिभाषय ANSI_HIGHLIGHT_MAGENTA  "\x1B[0;1;35m"
#परिभाषय ANSI_HIGHLIGHT_CYAN     "\x1B[0;1;36m"
#परिभाषय ANSI_HIGHLIGHT_WHITE    "\x1B[0;1;37m"
#परिभाषय ANSI_HIGHLIGHT_YELLOW4  "\x1B[0;1;38:5:100m"
#परिभाषय ANSI_HIGHLIGHT_KHAKI3   "\x1B[0;1;38:5:185m"
#परिभाषय ANSI_HIGHLIGHT_GREY     "\x1B[0;1;38:5:245m"

#परिभाषय ANSI_HIGHLIGHT_YELLOW   ANSI_HIGHLIGHT_KHAKI3 /* Replacement yellow that is more legible */

/* Underlined */
#परिभाषय ANSI_GREY_UNDERLINE              "\x1B[0;4;38:5:245m"
#परिभाषय ANSI_BRIGHT_BLACK_UNDERLINE      "\x1B[0;4;90m"
#परिभाषय ANSI_HIGHLIGHT_RED_UNDERLINE     "\x1B[0;1;4;31m"
#परिभाषय ANSI_HIGHLIGHT_GREEN_UNDERLINE   "\x1B[0;1;4;32m"
#परिभाषय ANSI_HIGHLIGHT_YELLOW_UNDERLINE  "\x1B[0;1;4;38:5:185m"
#परिभाषय ANSI_HIGHLIGHT_BLUE_UNDERLINE    "\x1B[0;1;4;34m"
#परिभाषय ANSI_HIGHLIGHT_MAGENTA_UNDERLINE "\x1B[0;1;4;35m"
#परिभाषय ANSI_HIGHLIGHT_GREY_UNDERLINE    "\x1B[0;1;4;38:5:245m"

/* Other ANSI codes */
#परिभाषय ANSI_UNDERLINE "\x1B[0;4m"
#परिभाषय ANSI_सङ्कलय_UNDERLINE "\x1B[4m"
#परिभाषय ANSI_सङ्कलय_UNDERLINE_GREY ANSI_सङ्कलय_UNDERLINE "\x1B[58:5:245m"
#परिभाषय ANSI_ITALICS "\x1B[0;3m"
#परिभाषय ANSI_सङ्कलय_ITALICS "\x1B[3m"
#परिभाषय ANSI_HIGHLIGHT "\x1B[0;1;39m"
#परिभाषय ANSI_HIGHLIGHT_UNDERLINE "\x1B[0;1;4m"

/* Fallback colors: 256 → 16 */
#परिभाषय ANSI_HIGHLIGHT_GREY_FALLBACK             "\x1B[0;1;90m"
#परिभाषय ANSI_HIGHLIGHT_GREY_FALLBACK_UNDERLINE   "\x1B[0;1;4;90m"
#परिभाषय ANSI_HIGHLIGHT_YELLOW_FALLBACK           "\x1B[0;1;33m"
#परिभाषय ANSI_HIGHLIGHT_YELLOW_FALLBACK_UNDERLINE "\x1B[0;1;4;33m"

/* Background colors */
#परिभाषय ANSI_BACKGROUND_BLUE "\x1B[44m"

/* Reset/clear ANSI styles */
#परिभाषय ANSI_प्रसामान्यम् "\x1B[0m"

#परिभाषय DEFINE_ANSI_FUNC(name, NAME)                            \
        स्थिर अन्तःस्थ ध्रुव वर्ण* ansi_##name(शून्य) {           \
                प्रत्यागम colors_enabled() ? ANSI_##NAME : "";     \
        }

/* NB: in 256 mode we always emit the fallback color first, in order to deal with terminals with
 * incomplete 256 color support (most notably Linux console, which a) lacks support for ":"
 * subcommand separator and b) skips over the whole CSI-m sequence if it sees an "invalid" command).
 * In 24-bit mode we don't bother with this however, under the assumption that $COLORTERM and friends
 * reflect the correct status. */

#परिभाषय DEFINE_ANSI_FUNC_256(name, NAME, FALLBACK)                              \
        स्थिर अन्तःस्थ ध्रुव वर्ण* ansi_##name(शून्य) {                           \
                विकल्प (get_color_बहुलकम्()) {                                     \
                        पक्ष COLOR_OFF: प्रत्यागम "";                              \
                        पक्ष COLOR_16: प्रत्यागम ANSI_##FALLBACK;                  \
                        पक्ष COLOR_256: प्रत्यागम ANSI_##FALLBACK ANSI_##NAME;     \
                        स्वभाव: प्रत्यागम ANSI_##NAME;                            \
                }                                                               \
        }

DEFINE_ANSI_FUNC(प्रसामान्यम्,            प्रसामान्यम्_2);
DEFINE_ANSI_FUNC(highlight,         HIGHLIGHT);
DEFINE_ANSI_FUNC(black,             BLACK);
DEFINE_ANSI_FUNC(red,               RED);
DEFINE_ANSI_FUNC(green,             GREEN);
DEFINE_ANSI_FUNC(yellow,            YELLOW);
DEFINE_ANSI_FUNC(blue,              BLUE);
DEFINE_ANSI_FUNC(magenta,           MAGENTA);
DEFINE_ANSI_FUNC(cyan,              CYAN);
DEFINE_ANSI_FUNC(white,             WHITE);
DEFINE_ANSI_FUNC_256(grey,          GREY, BRIGHT_BLACK);

DEFINE_ANSI_FUNC(bright_black,      BRIGHT_BLACK);
DEFINE_ANSI_FUNC(bright_red,        BRIGHT_RED);
DEFINE_ANSI_FUNC(bright_green,      BRIGHT_GREEN);
DEFINE_ANSI_FUNC(bright_yellow,     BRIGHT_YELLOW);
DEFINE_ANSI_FUNC(bright_blue,       BRIGHT_BLUE);
DEFINE_ANSI_FUNC(bright_magenta,    BRIGHT_MAGENTA);
DEFINE_ANSI_FUNC(bright_cyan,       BRIGHT_CYAN);
DEFINE_ANSI_FUNC(bright_white,      BRIGHT_WHITE);

DEFINE_ANSI_FUNC(highlight_black,       HIGHLIGHT_BLACK);
DEFINE_ANSI_FUNC(highlight_red,         HIGHLIGHT_RED);
DEFINE_ANSI_FUNC(highlight_green,       HIGHLIGHT_GREEN);
DEFINE_ANSI_FUNC_256(highlight_yellow,  HIGHLIGHT_YELLOW, HIGHLIGHT_YELLOW_FALLBACK);
DEFINE_ANSI_FUNC_256(highlight_yellow4, HIGHLIGHT_YELLOW4, HIGHLIGHT_YELLOW_FALLBACK);
DEFINE_ANSI_FUNC(highlight_blue,        HIGHLIGHT_BLUE);
DEFINE_ANSI_FUNC(highlight_magenta,     HIGHLIGHT_MAGENTA);
DEFINE_ANSI_FUNC(highlight_cyan,        HIGHLIGHT_CYAN);
DEFINE_ANSI_FUNC_256(highlight_grey,    HIGHLIGHT_GREY, HIGHLIGHT_GREY_FALLBACK);
DEFINE_ANSI_FUNC(highlight_white,       HIGHLIGHT_WHITE);

स्थिर अन्तःस्थ ध्रुव वर्ण* _ansi_highlight_yellow(शून्य) {
        प्रत्यागम colors_enabled() ? _ANSI_HIGHLIGHT_YELLOW : "";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* ansi_highlight_green_red(bool b) {
        प्रत्यागम b ? ansi_highlight_green() : ansi_highlight_red();
}

स्थिर अन्तःस्थ ध्रुव वर्ण* ansi_underline(शून्य) {
        प्रत्यागम underline_enabled() ? ANSI_UNDERLINE : "";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* ansi_सङ्कलय_underline(शून्य) {
        प्रत्यागम underline_enabled() ? ANSI_सङ्कलय_UNDERLINE : "";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* ansi_सङ्कलय_underline_grey(शून्य) {
        प्रत्यागम underline_enabled() ?
                (colors_enabled() ? ANSI_सङ्कलय_UNDERLINE_GREY : ANSI_सङ्कलय_UNDERLINE) : "";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* ansi_italics(शून्य) {
        /* We hook italics also into the underline checks, close enough */
        प्रत्यागम underline_enabled() ? ANSI_ITALICS : "";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* ansi_सङ्कलय_italics(शून्य) {
        प्रत्यागम underline_enabled() ? ANSI_सङ्कलय_ITALICS : "";
}

#परिभाषय DEFINE_ANSI_FUNC_UNDERLINE(name, NAME)                          \
        स्थिर अन्तःस्थ ध्रुव वर्ण* ansi_##name##_underline(शून्य) {       \
                प्रत्यागम underline_enabled() ? ANSI_##NAME##_UNDERLINE :  \
                        ansi_##name();                                  \
        }

#परिभाषय DEFINE_ANSI_FUNC_UNDERLINE_256(name, NAME, FALLBACK)                                            \
        स्थिर अन्तःस्थ ध्रुव वर्ण* ansi_##name##_underline(शून्य) {                                       \
                यदि (!underline_enabled())                                                               \
                        प्रत्यागम ansi_##name();                                                           \
                                                                                                        \
                विकल्प (get_color_बहुलकम्()) {                                                             \
                        पक्ष COLOR_OFF: प्रत्यागम "";                                                      \
                        पक्ष COLOR_16: प्रत्यागम ANSI_##FALLBACK##_UNDERLINE;                              \
                        पक्ष COLOR_256: प्रत्यागम ANSI_##FALLBACK##_UNDERLINE ANSI_##NAME##_UNDERLINE;     \
                        स्वभाव: प्रत्यागम ANSI_##NAME##_UNDERLINE;                                        \
                }                                                                                       \
        }

DEFINE_ANSI_FUNC_UNDERLINE(highlight,             HIGHLIGHT);
DEFINE_ANSI_FUNC_UNDERLINE(highlight_red,         HIGHLIGHT_RED);
DEFINE_ANSI_FUNC_UNDERLINE(highlight_green,       HIGHLIGHT_GREEN);
DEFINE_ANSI_FUNC_UNDERLINE_256(highlight_yellow,  HIGHLIGHT_YELLOW, HIGHLIGHT_YELLOW_FALLBACK);
DEFINE_ANSI_FUNC_UNDERLINE(highlight_blue,        HIGHLIGHT_BLUE);
DEFINE_ANSI_FUNC_UNDERLINE(highlight_magenta,     HIGHLIGHT_MAGENTA);
DEFINE_ANSI_FUNC_UNDERLINE_256(grey,              GREY, BRIGHT_BLACK);
DEFINE_ANSI_FUNC_UNDERLINE_256(highlight_grey,    HIGHLIGHT_GREY, HIGHLIGHT_GREY_FALLBACK);
