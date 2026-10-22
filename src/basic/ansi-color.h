/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

/* Limits the use of ANSI colors to a subset. */
تعريفنوع تعداد Color_ډېرپېښ {
        COLOR_OFF,        /* No colors, monochrome output. */
        COLOR_16,         /* Only the base 16 colors. */
        COLOR_256,        /* Only 256 colors. */
        COLOR_24_بټ,      /* For truecolor or 24bit color support, no restriction. */
        COLOR_ډېرپېښ_FIXED_MAX,

        /* The "AUTO" modes are as the above, but subject to suitable settings for the environment variables
         * TERM and NO_COLOR. */
        COLOR_AUTO_16 = COLOR_ډېرپېښ_FIXED_MAX,
        COLOR_AUTO_256,
        COLOR_AUTO_24_بټ,

        /* Same as default (unset), except that $NO_COLOR is ignored/overridden */
        COLOR_TRUE,

        COLOR_ډېرپېښ_MAX,
        COLOR_ډېرپېښ_INVALID = -EINVAL,
} Color_ډېرپېښ;

DECLARE_STRING_جدول_LOOKUP(color_ډېرپېښ, Color_ډېرپېښ);

Color_ډېرپېښ get_color_ډېرپېښ(فراغ);
ساكن مضمن bool colors_enabled(فراغ) {
        /* Returns true if colors are considered supported on our stdout. */
        أرجع get_color_ډېرپېښ() != COLOR_OFF;
}

Color_ډېرپېښ parse_systemd_colors(فراغ);

bool underline_enabled(فراغ);

فراغ reset_ansi_feature_caches(فراغ);

bool looks_like_ansi_color_کوډ(ثابت حرف *str);

/* Regular colors */
#تعریف_کړه ANSI_BLACK   "\x1B[0;30m" /* Some type of grey usually. */
#تعریف_کړه ANSI_RED     "\x1B[0;31m"
#تعریف_کړه ANSI_GREEN   "\x1B[0;32m"
#تعریف_کړه ANSI_YELLOW  "\x1B[0;33m"
#تعریف_کړه ANSI_BLUE    "\x1B[0;34m"
#تعریف_کړه ANSI_MAGENTA "\x1B[0;35m"
#تعریف_کړه ANSI_CYAN    "\x1B[0;36m"
#تعریف_کړه ANSI_WHITE   "\x1B[0;37m" /* This is actually rendered as light grey, legible even on a white
                                   * background. See ANSI_HIGHLIGHT_WHITE for real white. */

#تعریف_کړه ANSI_BRIGHT_BLACK   "\x1B[0;90m"
#تعریف_کړه ANSI_BRIGHT_RED     "\x1B[0;91m"
#تعریف_کړه ANSI_BRIGHT_GREEN   "\x1B[0;92m"
#تعریف_کړه ANSI_BRIGHT_YELLOW  "\x1B[0;93m"
#تعریف_کړه ANSI_BRIGHT_BLUE    "\x1B[0;94m"
#تعریف_کړه ANSI_BRIGHT_MAGENTA "\x1B[0;95m"
#تعریف_کړه ANSI_BRIGHT_CYAN    "\x1B[0;96m"
#تعریف_کړه ANSI_BRIGHT_WHITE   "\x1B[0;97m"

#تعریف_کړه ANSI_GREY    "\x1B[0;38:5:245m"

/* Bold/highlighted */
#تعریف_کړه ANSI_HIGHLIGHT_BLACK    "\x1B[0;1;30m"
#تعریف_کړه ANSI_HIGHLIGHT_RED      "\x1B[0;1;31m"
#تعریف_کړه ANSI_HIGHLIGHT_GREEN    "\x1B[0;1;32m"
#تعریف_کړه _ANSI_HIGHLIGHT_YELLOW  "\x1B[0;1;33m" /* This yellow is currently not displayed well by some terminals */
#تعریف_کړه ANSI_HIGHLIGHT_BLUE     "\x1B[0;1;34m"
#تعریف_کړه ANSI_HIGHLIGHT_MAGENTA  "\x1B[0;1;35m"
#تعریف_کړه ANSI_HIGHLIGHT_CYAN     "\x1B[0;1;36m"
#تعریف_کړه ANSI_HIGHLIGHT_WHITE    "\x1B[0;1;37m"
#تعریف_کړه ANSI_HIGHLIGHT_YELLOW4  "\x1B[0;1;38:5:100m"
#تعریف_کړه ANSI_HIGHLIGHT_KHAKI3   "\x1B[0;1;38:5:185m"
#تعریف_کړه ANSI_HIGHLIGHT_GREY     "\x1B[0;1;38:5:245m"

#تعریف_کړه ANSI_HIGHLIGHT_YELLOW   ANSI_HIGHLIGHT_KHAKI3 /* Replacement yellow that is more legible */

/* Underlined */
#تعریف_کړه ANSI_GREY_UNDERLINE              "\x1B[0;4;38:5:245m"
#تعریف_کړه ANSI_BRIGHT_BLACK_UNDERLINE      "\x1B[0;4;90m"
#تعریف_کړه ANSI_HIGHLIGHT_RED_UNDERLINE     "\x1B[0;1;4;31m"
#تعریف_کړه ANSI_HIGHLIGHT_GREEN_UNDERLINE   "\x1B[0;1;4;32m"
#تعریف_کړه ANSI_HIGHLIGHT_YELLOW_UNDERLINE  "\x1B[0;1;4;38:5:185m"
#تعریف_کړه ANSI_HIGHLIGHT_BLUE_UNDERLINE    "\x1B[0;1;4;34m"
#تعریف_کړه ANSI_HIGHLIGHT_MAGENTA_UNDERLINE "\x1B[0;1;4;35m"
#تعریف_کړه ANSI_HIGHLIGHT_GREY_UNDERLINE    "\x1B[0;1;4;38:5:245m"

/* Other ANSI codes */
#تعریف_کړه ANSI_UNDERLINE "\x1B[0;4m"
#تعریف_کړه ANSI_جمع_کړه_UNDERLINE "\x1B[4m"
#تعریف_کړه ANSI_جمع_کړه_UNDERLINE_GREY ANSI_جمع_کړه_UNDERLINE "\x1B[58:5:245m"
#تعریف_کړه ANSI_ITALICS "\x1B[0;3m"
#تعریف_کړه ANSI_جمع_کړه_ITALICS "\x1B[3m"
#تعریف_کړه ANSI_HIGHLIGHT "\x1B[0;1;39m"
#تعریف_کړه ANSI_HIGHLIGHT_UNDERLINE "\x1B[0;1;4m"

/* Fallback colors: 256 → 16 */
#تعریف_کړه ANSI_HIGHLIGHT_GREY_FALLBACK             "\x1B[0;1;90m"
#تعریف_کړه ANSI_HIGHLIGHT_GREY_FALLBACK_UNDERLINE   "\x1B[0;1;4;90m"
#تعریف_کړه ANSI_HIGHLIGHT_YELLOW_FALLBACK           "\x1B[0;1;33m"
#تعریف_کړه ANSI_HIGHLIGHT_YELLOW_FALLBACK_UNDERLINE "\x1B[0;1;4;33m"

/* Background colors */
#تعریف_کړه ANSI_BACKGROUND_BLUE "\x1B[44m"

/* Reset/clear ANSI styles */
#تعریف_کړه ANSI_نورمال "\x1B[0m"

#تعریف_کړه DEFINE_ANSI_FUNC(name, NAME)                            \
        ساكن مضمن ثابت حرف* ansi_##name(فراغ) {           \
                أرجع colors_enabled() ? ANSI_##NAME : "";     \
        }

/* NB: in 256 mode we always emit the fallback color first, in order to deal with terminals with
 * incomplete 256 color support (most notably Linux console, which a) lacks support for ":"
 * subcommand separator and b) skips over the whole CSI-m sequence if it sees an "invalid" command).
 * In 24-bit mode we don't bother with this however, under the assumption that $COLORTERM and friends
 * reflect the correct status. */

#تعریف_کړه DEFINE_ANSI_FUNC_256(name, NAME, FALLBACK)                              \
        ساكن مضمن ثابت حرف* ansi_##name(فراغ) {                           \
                بدل (get_color_ډېرپېښ()) {                                     \
                        حالة COLOR_OFF: أرجع "";                              \
                        حالة COLOR_16: أرجع ANSI_##FALLBACK;                  \
                        حالة COLOR_256: أرجع ANSI_##FALLBACK ANSI_##NAME;     \
                        افتراضي: أرجع ANSI_##NAME;                            \
                }                                                               \
        }

DEFINE_ANSI_FUNC(نورمال,            نورمال_2);
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

ساكن مضمن ثابت حرف* _ansi_highlight_yellow(فراغ) {
        أرجع colors_enabled() ? _ANSI_HIGHLIGHT_YELLOW : "";
}

ساكن مضمن ثابت حرف* ansi_highlight_green_red(bool b) {
        أرجع b ? ansi_highlight_green() : ansi_highlight_red();
}

ساكن مضمن ثابت حرف* ansi_underline(فراغ) {
        أرجع underline_enabled() ? ANSI_UNDERLINE : "";
}

ساكن مضمن ثابت حرف* ansi_جمع_کړه_underline(فراغ) {
        أرجع underline_enabled() ? ANSI_جمع_کړه_UNDERLINE : "";
}

ساكن مضمن ثابت حرف* ansi_جمع_کړه_underline_grey(فراغ) {
        أرجع underline_enabled() ?
                (colors_enabled() ? ANSI_جمع_کړه_UNDERLINE_GREY : ANSI_جمع_کړه_UNDERLINE) : "";
}

ساكن مضمن ثابت حرف* ansi_italics(فراغ) {
        /* We hook italics also into the underline checks, close enough */
        أرجع underline_enabled() ? ANSI_ITALICS : "";
}

ساكن مضمن ثابت حرف* ansi_جمع_کړه_italics(فراغ) {
        أرجع underline_enabled() ? ANSI_جمع_کړه_ITALICS : "";
}

#تعریف_کړه DEFINE_ANSI_FUNC_UNDERLINE(name, NAME)                          \
        ساكن مضمن ثابت حرف* ansi_##name##_underline(فراغ) {       \
                أرجع underline_enabled() ? ANSI_##NAME##_UNDERLINE :  \
                        ansi_##name();                                  \
        }

#تعریف_کړه DEFINE_ANSI_FUNC_UNDERLINE_256(name, NAME, FALLBACK)                                            \
        ساكن مضمن ثابت حرف* ansi_##name##_underline(فراغ) {                                       \
                إذا (!underline_enabled())                                                               \
                        أرجع ansi_##name();                                                           \
                                                                                                        \
                بدل (get_color_ډېرپېښ()) {                                                             \
                        حالة COLOR_OFF: أرجع "";                                                      \
                        حالة COLOR_16: أرجع ANSI_##FALLBACK##_UNDERLINE;                              \
                        حالة COLOR_256: أرجع ANSI_##FALLBACK##_UNDERLINE ANSI_##NAME##_UNDERLINE;     \
                        افتراضي: أرجع ANSI_##NAME##_UNDERLINE;                                        \
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
