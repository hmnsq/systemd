/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

/* Limits the use of ANSI colors to a subset. */
प्रकार क्रमागत ColorMode {
        COLOR_OFF,        /* No colors, monochrome output. */
        COLOR_16,         /* Only the base 16 colors. */
        COLOR_256,        /* Only 256 colors. */
        COLOR_24BIT,      /* For truecolor or 24bit color support, no restriction. */
        _COLOR_MODE_FIXED_MAX,

        /* The "AUTO" modes are as the above, but subject to suitable settings for the environment variables
         * TERM and NO_COLOR. */
        COLOR_AUTO_16 = _COLOR_MODE_FIXED_MAX,
        COLOR_AUTO_256,
        COLOR_AUTO_24BIT,

        /* Same as default (unset), except that $NO_COLOR is ignored/overridden */
        COLOR_TRUE,

        _COLOR_MODE_MAX,
        _COLOR_MODE_INVALID = -EINVAL,
} ColorMode;

DECLARE_STRING_TABLE_LOOKUP(color_mode, ColorMode);

ColorMode get_color_mode(व्योम);
अटल अंतरभूत bool colors_enabled(व्योम) {
        /* Returns true if colors are considered supported on our stdout. */
        वापस get_color_mode() != COLOR_OFF;
}

ColorMode parse_systemd_colors(व्योम);

bool underline_enabled(व्योम);

व्योम reset_ansi_feature_caches(व्योम);

bool looks_like_ansi_color_code(स्थिर अक्षर *str);

/* Regular colors */
#घोषणा ANSI_BLACK   "\x1B[0;30m" /* Some type of grey usually. */
#घोषणा ANSI_RED     "\x1B[0;31m"
#घोषणा ANSI_GREEN   "\x1B[0;32m"
#घोषणा ANSI_YELLOW  "\x1B[0;33m"
#घोषणा ANSI_BLUE    "\x1B[0;34m"
#घोषणा ANSI_MAGENTA "\x1B[0;35m"
#घोषणा ANSI_CYAN    "\x1B[0;36m"
#घोषणा ANSI_WHITE   "\x1B[0;37m" /* This is actually rendered as light grey, legible even on a white
                                   * background. See ANSI_HIGHLIGHT_WHITE for real white. */

#घोषणा ANSI_BRIGHT_BLACK   "\x1B[0;90m"
#घोषणा ANSI_BRIGHT_RED     "\x1B[0;91m"
#घोषणा ANSI_BRIGHT_GREEN   "\x1B[0;92m"
#घोषणा ANSI_BRIGHT_YELLOW  "\x1B[0;93m"
#घोषणा ANSI_BRIGHT_BLUE    "\x1B[0;94m"
#घोषणा ANSI_BRIGHT_MAGENTA "\x1B[0;95m"
#घोषणा ANSI_BRIGHT_CYAN    "\x1B[0;96m"
#घोषणा ANSI_BRIGHT_WHITE   "\x1B[0;97m"

#घोषणा ANSI_GREY    "\x1B[0;38:5:245m"

/* Bold/highlighted */
#घोषणा ANSI_HIGHLIGHT_BLACK    "\x1B[0;1;30m"
#घोषणा ANSI_HIGHLIGHT_RED      "\x1B[0;1;31m"
#घोषणा ANSI_HIGHLIGHT_GREEN    "\x1B[0;1;32m"
#घोषणा _ANSI_HIGHLIGHT_YELLOW  "\x1B[0;1;33m" /* This yellow is currently not displayed well by some terminals */
#घोषणा ANSI_HIGHLIGHT_BLUE     "\x1B[0;1;34m"
#घोषणा ANSI_HIGHLIGHT_MAGENTA  "\x1B[0;1;35m"
#घोषणा ANSI_HIGHLIGHT_CYAN     "\x1B[0;1;36m"
#घोषणा ANSI_HIGHLIGHT_WHITE    "\x1B[0;1;37m"
#घोषणा ANSI_HIGHLIGHT_YELLOW4  "\x1B[0;1;38:5:100m"
#घोषणा ANSI_HIGHLIGHT_KHAKI3   "\x1B[0;1;38:5:185m"
#घोषणा ANSI_HIGHLIGHT_GREY     "\x1B[0;1;38:5:245m"

#घोषणा ANSI_HIGHLIGHT_YELLOW   ANSI_HIGHLIGHT_KHAKI3 /* Replacement yellow that is more legible */

/* Underlined */
#घोषणा ANSI_GREY_UNDERLINE              "\x1B[0;4;38:5:245m"
#घोषणा ANSI_BRIGHT_BLACK_UNDERLINE      "\x1B[0;4;90m"
#घोषणा ANSI_HIGHLIGHT_RED_UNDERLINE     "\x1B[0;1;4;31m"
#घोषणा ANSI_HIGHLIGHT_GREEN_UNDERLINE   "\x1B[0;1;4;32m"
#घोषणा ANSI_HIGHLIGHT_YELLOW_UNDERLINE  "\x1B[0;1;4;38:5:185m"
#घोषणा ANSI_HIGHLIGHT_BLUE_UNDERLINE    "\x1B[0;1;4;34m"
#घोषणा ANSI_HIGHLIGHT_MAGENTA_UNDERLINE "\x1B[0;1;4;35m"
#घोषणा ANSI_HIGHLIGHT_GREY_UNDERLINE    "\x1B[0;1;4;38:5:245m"

/* Other ANSI codes */
#घोषणा ANSI_UNDERLINE "\x1B[0;4m"
#घोषणा ANSI_ADD_UNDERLINE "\x1B[4m"
#घोषणा ANSI_ADD_UNDERLINE_GREY ANSI_ADD_UNDERLINE "\x1B[58:5:245m"
#घोषणा ANSI_ITALICS "\x1B[0;3m"
#घोषणा ANSI_ADD_ITALICS "\x1B[3m"
#घोषणा ANSI_HIGHLIGHT "\x1B[0;1;39m"
#घोषणा ANSI_HIGHLIGHT_UNDERLINE "\x1B[0;1;4m"

/* Fallback colors: 256 → 16 */
#घोषणा ANSI_HIGHLIGHT_GREY_FALLBACK             "\x1B[0;1;90m"
#घोषणा ANSI_HIGHLIGHT_GREY_FALLBACK_UNDERLINE   "\x1B[0;1;4;90m"
#घोषणा ANSI_HIGHLIGHT_YELLOW_FALLBACK           "\x1B[0;1;33m"
#घोषणा ANSI_HIGHLIGHT_YELLOW_FALLBACK_UNDERLINE "\x1B[0;1;4;33m"

/* Background colors */
#घोषणा ANSI_BACKGROUND_BLUE "\x1B[44m"

/* Reset/clear ANSI styles */
#घोषणा ANSI_NORMAL "\x1B[0m"

#घोषणा DEFINE_ANSI_FUNC(name, NAME)                            \
        अटल अंतरभूत स्थिर अक्षर* ansi_##name(व्योम) {           \
                वापस colors_enabled() ? ANSI_##NAME : "";     \
        }

/* NB: in 256 mode we always emit the fallback color first, in order to deal with terminals with
 * incomplete 256 color support (most notably Linux console, which a) lacks support for ":"
 * subcommand separator and b) skips over the whole CSI-m sequence if it sees an "invalid" command).
 * In 24-bit mode we don't bother with this however, under the assumption that $COLORTERM and friends
 * reflect the correct status. */

#घोषणा DEFINE_ANSI_FUNC_256(name, NAME, FALLBACK)                              \
        अटल अंतरभूत स्थिर अक्षर* ansi_##name(व्योम) {                           \
                चयन (get_color_mode()) {                                     \
                        हाल COLOR_OFF: वापस "";                              \
                        हाल COLOR_16: वापस ANSI_##FALLBACK;                  \
                        हाल COLOR_256: वापस ANSI_##FALLBACK ANSI_##NAME;     \
                        शेष: वापस ANSI_##NAME;                            \
                }                                                               \
        }

DEFINE_ANSI_FUNC(normal,            NORMAL);
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

अटल अंतरभूत स्थिर अक्षर* _ansi_highlight_yellow(व्योम) {
        वापस colors_enabled() ? _ANSI_HIGHLIGHT_YELLOW : "";
}

अटल अंतरभूत स्थिर अक्षर* ansi_highlight_green_red(bool b) {
        वापस b ? ansi_highlight_green() : ansi_highlight_red();
}

अटल अंतरभूत स्थिर अक्षर* ansi_underline(व्योम) {
        वापस underline_enabled() ? ANSI_UNDERLINE : "";
}

अटल अंतरभूत स्थिर अक्षर* ansi_add_underline(व्योम) {
        वापस underline_enabled() ? ANSI_ADD_UNDERLINE : "";
}

अटल अंतरभूत स्थिर अक्षर* ansi_add_underline_grey(व्योम) {
        वापस underline_enabled() ?
                (colors_enabled() ? ANSI_ADD_UNDERLINE_GREY : ANSI_ADD_UNDERLINE) : "";
}

अटल अंतरभूत स्थिर अक्षर* ansi_italics(व्योम) {
        /* We hook italics also into the underline checks, close enough */
        वापस underline_enabled() ? ANSI_ITALICS : "";
}

अटल अंतरभूत स्थिर अक्षर* ansi_add_italics(व्योम) {
        वापस underline_enabled() ? ANSI_ADD_ITALICS : "";
}

#घोषणा DEFINE_ANSI_FUNC_UNDERLINE(name, NAME)                          \
        अटल अंतरभूत स्थिर अक्षर* ansi_##name##_underline(व्योम) {       \
                वापस underline_enabled() ? ANSI_##NAME##_UNDERLINE :  \
                        ansi_##name();                                  \
        }

#घोषणा DEFINE_ANSI_FUNC_UNDERLINE_256(name, NAME, FALLBACK)                                            \
        अटल अंतरभूत स्थिर अक्षर* ansi_##name##_underline(व्योम) {                                       \
                अगर (!underline_enabled())                                                               \
                        वापस ansi_##name();                                                           \
                                                                                                        \
                चयन (get_color_mode()) {                                                             \
                        हाल COLOR_OFF: वापस "";                                                      \
                        हाल COLOR_16: वापस ANSI_##FALLBACK##_UNDERLINE;                              \
                        हाल COLOR_256: वापस ANSI_##FALLBACK##_UNDERLINE ANSI_##NAME##_UNDERLINE;     \
                        शेष: वापस ANSI_##NAME##_UNDERLINE;                                        \
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
