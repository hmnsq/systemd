/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* Limits the use of ANSI colors to a subset. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Color_ਬਹੁਲਕ {
        COLOR_OFF,        /* No colors, monochrome output. */
        COLOR_16,         /* Only the base 16 colors. */
        COLOR_256,        /* Only 256 colors. */
        COLOR_24_ਦੋਆਧਾਰੀ_ਅੰਕ,      /* For truecolor or 24bit color support, no restriction. */
        COLOR_ਬਹੁਲਕ_FIXED_MAX,

        /* The "AUTO" modes are as the above, but subject to suitable settings for the environment variables
         * TERM and NO_COLOR. */
        COLOR_AUTO_16 = COLOR_ਬਹੁਲਕ_FIXED_MAX,
        COLOR_AUTO_256,
        COLOR_AUTO_24_ਦੋਆਧਾਰੀ_ਅੰਕ,

        /* Same as default (unset), except that $NO_COLOR is ignored/overridden */
        COLOR_TRUE,

        COLOR_ਬਹੁਲਕ_MAX,
        COLOR_ਬਹੁਲਕ_INVALID = -EINVAL,
} Color_ਬਹੁਲਕ;

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(color_ਬਹੁਲਕ, Color_ਬਹੁਲਕ);

Color_ਬਹੁਲਕ get_color_ਬਹੁਲਕ(ਖਾਲੀ);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool colors_enabled(ਖਾਲੀ) {
        /* Returns true if colors are considered supported on our stdout. */
        ਵਾਪਸ get_color_ਬਹੁਲਕ() != COLOR_OFF;
}

Color_ਬਹੁਲਕ parse_systemd_colors(ਖਾਲੀ);

bool underline_enabled(ਖਾਲੀ);

ਖਾਲੀ reset_ansi_feature_caches(ਖਾਲੀ);

bool looks_like_ansi_color_ਕੋਡ(ਅਚਲ ਅੱਖਰ *str);

/* Regular colors */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BLACK   "\x1B[0;30m" /* Some type of grey usually. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_RED     "\x1B[0;31m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_GREEN   "\x1B[0;32m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_YELLOW  "\x1B[0;33m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BLUE    "\x1B[0;34m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_MAGENTA "\x1B[0;35m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_CYAN    "\x1B[0;36m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_WHITE   "\x1B[0;37m" /* This is actually rendered as light grey, legible even on a white
                                   * background. See ANSI_HIGHLIGHT_WHITE for real white. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BRIGHT_BLACK   "\x1B[0;90m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BRIGHT_RED     "\x1B[0;91m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BRIGHT_GREEN   "\x1B[0;92m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BRIGHT_YELLOW  "\x1B[0;93m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BRIGHT_BLUE    "\x1B[0;94m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BRIGHT_MAGENTA "\x1B[0;95m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BRIGHT_CYAN    "\x1B[0;96m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BRIGHT_WHITE   "\x1B[0;97m"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_GREY    "\x1B[0;38:5:245m"

/* Bold/highlighted */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_BLACK    "\x1B[0;1;30m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_RED      "\x1B[0;1;31m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_GREEN    "\x1B[0;1;32m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _ANSI_HIGHLIGHT_YELLOW  "\x1B[0;1;33m" /* This yellow is currently not displayed well by some terminals */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_BLUE     "\x1B[0;1;34m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_MAGENTA  "\x1B[0;1;35m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_CYAN     "\x1B[0;1;36m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_WHITE    "\x1B[0;1;37m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_YELLOW4  "\x1B[0;1;38:5:100m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_KHAKI3   "\x1B[0;1;38:5:185m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_GREY     "\x1B[0;1;38:5:245m"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_YELLOW   ANSI_HIGHLIGHT_KHAKI3 /* Replacement yellow that is more legible */

/* Underlined */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_GREY_UNDERLINE              "\x1B[0;4;38:5:245m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BRIGHT_BLACK_UNDERLINE      "\x1B[0;4;90m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_RED_UNDERLINE     "\x1B[0;1;4;31m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_GREEN_UNDERLINE   "\x1B[0;1;4;32m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_YELLOW_UNDERLINE  "\x1B[0;1;4;38:5:185m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_BLUE_UNDERLINE    "\x1B[0;1;4;34m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_MAGENTA_UNDERLINE "\x1B[0;1;4;35m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_GREY_UNDERLINE    "\x1B[0;1;4;38:5:245m"

/* Other ANSI codes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_UNDERLINE "\x1B[0;4m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ਜੋੜੋ_UNDERLINE "\x1B[4m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ਜੋੜੋ_UNDERLINE_GREY ANSI_ਜੋੜੋ_UNDERLINE "\x1B[58:5:245m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ITALICS "\x1B[0;3m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ਜੋੜੋ_ITALICS "\x1B[3m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT "\x1B[0;1;39m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_UNDERLINE "\x1B[0;1;4m"

/* Fallback colors: 256 → 16 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_GREY_FALLBACK             "\x1B[0;1;90m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_GREY_FALLBACK_UNDERLINE   "\x1B[0;1;4;90m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_YELLOW_FALLBACK           "\x1B[0;1;33m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HIGHLIGHT_YELLOW_FALLBACK_UNDERLINE "\x1B[0;1;4;33m"

/* Background colors */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_BACKGROUND_BLUE "\x1B[44m"

/* Reset/clear ANSI styles */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ਸਾਧਾਰਨ "\x1B[0m"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ANSI_FUNC(name, NAME)                            \
        ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ansi_##name(ਖਾਲੀ) {           \
                ਵਾਪਸ colors_enabled() ? ANSI_##NAME : "";     \
        }

/* NB: in 256 mode we always emit the fallback color first, in order to deal with terminals with
 * incomplete 256 color support (most notably Linux console, which a) lacks support for ":"
 * subcommand separator and b) skips over the whole CSI-m sequence if it sees an "invalid" command).
 * In 24-bit mode we don't bother with this however, under the assumption that $COLORTERM and friends
 * reflect the correct status. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ANSI_FUNC_256(name, NAME, FALLBACK)                              \
        ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ansi_##name(ਖਾਲੀ) {                           \
                ਚੋਣ (get_color_ਬਹੁਲਕ()) {                                     \
                        ਹਾਲਤ COLOR_OFF: ਵਾਪਸ "";                              \
                        ਹਾਲਤ COLOR_16: ਵਾਪਸ ANSI_##FALLBACK;                  \
                        ਹਾਲਤ COLOR_256: ਵਾਪਸ ANSI_##FALLBACK ANSI_##NAME;     \
                        ਮੂਲ: ਵਾਪਸ ANSI_##NAME;                            \
                }                                                               \
        }

DEFINE_ANSI_FUNC(ਸਾਧਾਰਨ,            ਸਾਧਾਰਨ_2);
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

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* _ansi_highlight_yellow(ਖਾਲੀ) {
        ਵਾਪਸ colors_enabled() ? _ANSI_HIGHLIGHT_YELLOW : "";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ansi_highlight_green_red(bool b) {
        ਵਾਪਸ b ? ansi_highlight_green() : ansi_highlight_red();
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ansi_underline(ਖਾਲੀ) {
        ਵਾਪਸ underline_enabled() ? ANSI_UNDERLINE : "";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ansi_ਜੋੜੋ_underline(ਖਾਲੀ) {
        ਵਾਪਸ underline_enabled() ? ANSI_ਜੋੜੋ_UNDERLINE : "";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ansi_ਜੋੜੋ_underline_grey(ਖਾਲੀ) {
        ਵਾਪਸ underline_enabled() ?
                (colors_enabled() ? ANSI_ਜੋੜੋ_UNDERLINE_GREY : ANSI_ਜੋੜੋ_UNDERLINE) : "";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ansi_italics(ਖਾਲੀ) {
        /* We hook italics also into the underline checks, close enough */
        ਵਾਪਸ underline_enabled() ? ANSI_ITALICS : "";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ansi_ਜੋੜੋ_italics(ਖਾਲੀ) {
        ਵਾਪਸ underline_enabled() ? ANSI_ਜੋੜੋ_ITALICS : "";
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ANSI_FUNC_UNDERLINE(name, NAME)                          \
        ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ansi_##name##_underline(ਖਾਲੀ) {       \
                ਵਾਪਸ underline_enabled() ? ANSI_##NAME##_UNDERLINE :  \
                        ansi_##name();                                  \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ANSI_FUNC_UNDERLINE_256(name, NAME, FALLBACK)                                            \
        ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ansi_##name##_underline(ਖਾਲੀ) {                                       \
                ਜੇ (!underline_enabled())                                                               \
                        ਵਾਪਸ ansi_##name();                                                           \
                                                                                                        \
                ਚੋਣ (get_color_ਬਹੁਲਕ()) {                                                             \
                        ਹਾਲਤ COLOR_OFF: ਵਾਪਸ "";                                                      \
                        ਹਾਲਤ COLOR_16: ਵਾਪਸ ANSI_##FALLBACK##_UNDERLINE;                              \
                        ਹਾਲਤ COLOR_256: ਵਾਪਸ ANSI_##FALLBACK##_UNDERLINE ANSI_##NAME##_UNDERLINE;     \
                        ਮੂਲ: ਵਾਪਸ ANSI_##NAME##_UNDERLINE;                                        \
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
