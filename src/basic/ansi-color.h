/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Limits the use of ANSI colors to a subset. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Color_ಬಹುಲಕ {
        COLOR_OFF,        /* No colors, monochrome output. */
        COLOR_16,         /* Only the base 16 colors. */
        COLOR_256,        /* Only 256 colors. */
        COLOR_24_ಬಿಟ್,      /* For truecolor or 24bit color support, no restriction. */
        COLOR_ಬಹುಲಕ_FIXED_MAX,

        /* The "AUTO" modes are as the above, but subject to suitable settings for the environment variables
         * TERM and NO_COLOR. */
        COLOR_AUTO_16 = COLOR_ಬಹುಲಕ_FIXED_MAX,
        COLOR_AUTO_256,
        COLOR_AUTO_24_ಬಿಟ್,

        /* Same as default (unset), except that $NO_COLOR is ignored/overridden */
        COLOR_TRUE,

        COLOR_ಬಹುಲಕ_MAX,
        COLOR_ಬಹುಲಕ_INVALID = -EINVAL,
} Color_ಬಹುಲಕ;

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(color_ಬಹುಲಕ, Color_ಬಹುಲಕ);

Color_ಬಹುಲಕ get_color_ಬಹುಲಕ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool colors_enabled(ಶೂನ್ಯ) {
        /* Returns true if colors are considered supported on our stdout. */
        ಹಿಂತಿರುಗಿ get_color_ಬಹುಲಕ() != COLOR_OFF;
}

Color_ಬಹುಲಕ parse_systemd_colors(ಶೂನ್ಯ);

bool underline_enabled(ಶೂನ್ಯ);

ಶೂನ್ಯ reset_ansi_feature_caches(ಶೂನ್ಯ);

bool looks_like_ansi_color_ಸಂಕೇತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str);

/* Regular colors */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BLACK   "\x1B[0;30m" /* Some type of grey usually. */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_RED     "\x1B[0;31m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_GREEN   "\x1B[0;32m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_YELLOW  "\x1B[0;33m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BLUE    "\x1B[0;34m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_MAGENTA "\x1B[0;35m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_CYAN    "\x1B[0;36m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_WHITE   "\x1B[0;37m" /* This is actually rendered as light grey, legible even on a white
                                   * background. See ANSI_HIGHLIGHT_WHITE for real white. */

#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BRIGHT_BLACK   "\x1B[0;90m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BRIGHT_RED     "\x1B[0;91m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BRIGHT_GREEN   "\x1B[0;92m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BRIGHT_YELLOW  "\x1B[0;93m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BRIGHT_BLUE    "\x1B[0;94m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BRIGHT_MAGENTA "\x1B[0;95m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BRIGHT_CYAN    "\x1B[0;96m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BRIGHT_WHITE   "\x1B[0;97m"

#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_GREY    "\x1B[0;38:5:245m"

/* Bold/highlighted */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_BLACK    "\x1B[0;1;30m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_RED      "\x1B[0;1;31m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_GREEN    "\x1B[0;1;32m"
#ವ್ಯಾಖ್ಯಾನಿಸು _ANSI_HIGHLIGHT_YELLOW  "\x1B[0;1;33m" /* This yellow is currently not displayed well by some terminals */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_BLUE     "\x1B[0;1;34m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_MAGENTA  "\x1B[0;1;35m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_CYAN     "\x1B[0;1;36m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_WHITE    "\x1B[0;1;37m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_YELLOW4  "\x1B[0;1;38:5:100m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_KHAKI3   "\x1B[0;1;38:5:185m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_GREY     "\x1B[0;1;38:5:245m"

#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_YELLOW   ANSI_HIGHLIGHT_KHAKI3 /* Replacement yellow that is more legible */

/* Underlined */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_GREY_UNDERLINE              "\x1B[0;4;38:5:245m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BRIGHT_BLACK_UNDERLINE      "\x1B[0;4;90m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_RED_UNDERLINE     "\x1B[0;1;4;31m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_GREEN_UNDERLINE   "\x1B[0;1;4;32m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_YELLOW_UNDERLINE  "\x1B[0;1;4;38:5:185m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_BLUE_UNDERLINE    "\x1B[0;1;4;34m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_MAGENTA_UNDERLINE "\x1B[0;1;4;35m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_GREY_UNDERLINE    "\x1B[0;1;4;38:5:245m"

/* Other ANSI codes */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_UNDERLINE "\x1B[0;4m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ಕೂಡಿಸು_UNDERLINE "\x1B[4m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ಕೂಡಿಸು_UNDERLINE_GREY ANSI_ಕೂಡಿಸು_UNDERLINE "\x1B[58:5:245m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ITALICS "\x1B[0;3m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ಕೂಡಿಸು_ITALICS "\x1B[3m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT "\x1B[0;1;39m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_UNDERLINE "\x1B[0;1;4m"

/* Fallback colors: 256 → 16 */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_GREY_FALLBACK             "\x1B[0;1;90m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_GREY_FALLBACK_UNDERLINE   "\x1B[0;1;4;90m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_YELLOW_FALLBACK           "\x1B[0;1;33m"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HIGHLIGHT_YELLOW_FALLBACK_UNDERLINE "\x1B[0;1;4;33m"

/* Background colors */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_BACKGROUND_BLUE "\x1B[44m"

/* Reset/clear ANSI styles */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ಸಾಮಾನ್ಯ "\x1B[0m"

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ANSI_FUNC(name, NAME)                            \
        ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_##name(ಶೂನ್ಯ) {           \
                ಹಿಂತಿರುಗಿ colors_enabled() ? ANSI_##NAME : "";     \
        }

/* NB: in 256 mode we always emit the fallback color first, in order to deal with terminals with
 * incomplete 256 color support (most notably Linux console, which a) lacks support for ":"
 * subcommand separator and b) skips over the whole CSI-m sequence if it sees an "invalid" command).
 * In 24-bit mode we don't bother with this however, under the assumption that $COLORTERM and friends
 * reflect the correct status. */

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ANSI_FUNC_256(name, NAME, FALLBACK)                              \
        ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_##name(ಶೂನ್ಯ) {                           \
                ಆಯ್ಕೆ (get_color_ಬಹುಲಕ()) {                                     \
                        ಸ್ಥಿತಿ COLOR_OFF: ಹಿಂತಿರುಗಿ "";                              \
                        ಸ್ಥಿತಿ COLOR_16: ಹಿಂತಿರುಗಿ ANSI_##FALLBACK;                  \
                        ಸ್ಥಿತಿ COLOR_256: ಹಿಂತಿರುಗಿ ANSI_##FALLBACK ANSI_##NAME;     \
                        ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ ANSI_##NAME;                            \
                }                                                               \
        }

DEFINE_ANSI_FUNC(ಸಾಮಾನ್ಯ,            ಸಾಮಾನ್ಯ_2);
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* _ansi_highlight_yellow(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ colors_enabled() ? _ANSI_HIGHLIGHT_YELLOW : "";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_highlight_green_red(bool b) {
        ಹಿಂತಿರುಗಿ b ? ansi_highlight_green() : ansi_highlight_red();
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_underline(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ underline_enabled() ? ANSI_UNDERLINE : "";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_ಕೂಡಿಸು_underline(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ underline_enabled() ? ANSI_ಕೂಡಿಸು_UNDERLINE : "";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_ಕೂಡಿಸು_underline_grey(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ underline_enabled() ?
                (colors_enabled() ? ANSI_ಕೂಡಿಸು_UNDERLINE_GREY : ANSI_ಕೂಡಿಸು_UNDERLINE) : "";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_italics(ಶೂನ್ಯ) {
        /* We hook italics also into the underline checks, close enough */
        ಹಿಂತಿರುಗಿ underline_enabled() ? ANSI_ITALICS : "";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_ಕೂಡಿಸು_italics(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ underline_enabled() ? ANSI_ಕೂಡಿಸು_ITALICS : "";
}

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ANSI_FUNC_UNDERLINE(name, NAME)                          \
        ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_##name##_underline(ಶೂನ್ಯ) {       \
                ಹಿಂತಿರುಗಿ underline_enabled() ? ANSI_##NAME##_UNDERLINE :  \
                        ansi_##name();                                  \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ANSI_FUNC_UNDERLINE_256(name, NAME, FALLBACK)                                            \
        ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ansi_##name##_underline(ಶೂನ್ಯ) {                                       \
                ಆದರೆ (!underline_enabled())                                                               \
                        ಹಿಂತಿರುಗಿ ansi_##name();                                                           \
                                                                                                        \
                ಆಯ್ಕೆ (get_color_ಬಹುಲಕ()) {                                                             \
                        ಸ್ಥಿತಿ COLOR_OFF: ಹಿಂತಿರುಗಿ "";                                                      \
                        ಸ್ಥಿತಿ COLOR_16: ಹಿಂತಿರುಗಿ ANSI_##FALLBACK##_UNDERLINE;                              \
                        ಸ್ಥಿತಿ COLOR_256: ಹಿಂತಿರುಗಿ ANSI_##FALLBACK##_UNDERLINE ANSI_##NAME##_UNDERLINE;     \
                        ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ ANSI_##NAME##_UNDERLINE;                                        \
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
