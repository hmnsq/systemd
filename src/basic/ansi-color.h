/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* Limits the use of ANSI colors to a subset. */
రకనిర్వచనం గణన Color_బహుళకం {
        COLOR_OFF,        /* No colors, monochrome output. */
        COLOR_16,         /* Only the base 16 colors. */
        COLOR_256,        /* Only 256 colors. */
        COLOR_24_బిట్టు,      /* For truecolor or 24bit color support, no restriction. */
        COLOR_బహుళకం_FIXED_MAX,

        /* The "AUTO" modes are as the above, but subject to suitable settings for the environment variables
         * TERM and NO_COLOR. */
        COLOR_AUTO_16 = COLOR_బహుళకం_FIXED_MAX,
        COLOR_AUTO_256,
        COLOR_AUTO_24_బిట్టు,

        /* Same as default (unset), except that $NO_COLOR is ignored/overridden */
        COLOR_TRUE,

        COLOR_బహుళకం_MAX,
        COLOR_బహుళకం_INVALID = -EINVAL,
} Color_బహుళకం;

DECLARE_STRING_పట్టిక_LOOKUP(color_బహుళకం, Color_బహుళకం);

Color_బహుళకం get_color_బహుళకం(శూన్య);
స్థిర అంతఃవరుస bool colors_enabled(శూన్య) {
        /* Returns true if colors are considered supported on our stdout. */
        తిరిగి get_color_బహుళకం() != COLOR_OFF;
}

Color_బహుళకం parse_systemd_colors(శూన్య);

bool underline_enabled(శూన్య);

శూన్య reset_ansi_feature_caches(శూన్య);

bool looks_like_ansi_color_సంకేతం(స్థిరరాశి అక్షరం *str);

/* Regular colors */
#నిర్వచించు ANSI_BLACK   "\x1B[0;30m" /* Some type of grey usually. */
#నిర్వచించు ANSI_RED     "\x1B[0;31m"
#నిర్వచించు ANSI_GREEN   "\x1B[0;32m"
#నిర్వచించు ANSI_YELLOW  "\x1B[0;33m"
#నిర్వచించు ANSI_BLUE    "\x1B[0;34m"
#నిర్వచించు ANSI_MAGENTA "\x1B[0;35m"
#నిర్వచించు ANSI_CYAN    "\x1B[0;36m"
#నిర్వచించు ANSI_WHITE   "\x1B[0;37m" /* This is actually rendered as light grey, legible even on a white
                                   * background. See ANSI_HIGHLIGHT_WHITE for real white. */

#నిర్వచించు ANSI_BRIGHT_BLACK   "\x1B[0;90m"
#నిర్వచించు ANSI_BRIGHT_RED     "\x1B[0;91m"
#నిర్వచించు ANSI_BRIGHT_GREEN   "\x1B[0;92m"
#నిర్వచించు ANSI_BRIGHT_YELLOW  "\x1B[0;93m"
#నిర్వచించు ANSI_BRIGHT_BLUE    "\x1B[0;94m"
#నిర్వచించు ANSI_BRIGHT_MAGENTA "\x1B[0;95m"
#నిర్వచించు ANSI_BRIGHT_CYAN    "\x1B[0;96m"
#నిర్వచించు ANSI_BRIGHT_WHITE   "\x1B[0;97m"

#నిర్వచించు ANSI_GREY    "\x1B[0;38:5:245m"

/* Bold/highlighted */
#నిర్వచించు ANSI_HIGHLIGHT_BLACK    "\x1B[0;1;30m"
#నిర్వచించు ANSI_HIGHLIGHT_RED      "\x1B[0;1;31m"
#నిర్వచించు ANSI_HIGHLIGHT_GREEN    "\x1B[0;1;32m"
#నిర్వచించు _ANSI_HIGHLIGHT_YELLOW  "\x1B[0;1;33m" /* This yellow is currently not displayed well by some terminals */
#నిర్వచించు ANSI_HIGHLIGHT_BLUE     "\x1B[0;1;34m"
#నిర్వచించు ANSI_HIGHLIGHT_MAGENTA  "\x1B[0;1;35m"
#నిర్వచించు ANSI_HIGHLIGHT_CYAN     "\x1B[0;1;36m"
#నిర్వచించు ANSI_HIGHLIGHT_WHITE    "\x1B[0;1;37m"
#నిర్వచించు ANSI_HIGHLIGHT_YELLOW4  "\x1B[0;1;38:5:100m"
#నిర్వచించు ANSI_HIGHLIGHT_KHAKI3   "\x1B[0;1;38:5:185m"
#నిర్వచించు ANSI_HIGHLIGHT_GREY     "\x1B[0;1;38:5:245m"

#నిర్వచించు ANSI_HIGHLIGHT_YELLOW   ANSI_HIGHLIGHT_KHAKI3 /* Replacement yellow that is more legible */

/* Underlined */
#నిర్వచించు ANSI_GREY_UNDERLINE              "\x1B[0;4;38:5:245m"
#నిర్వచించు ANSI_BRIGHT_BLACK_UNDERLINE      "\x1B[0;4;90m"
#నిర్వచించు ANSI_HIGHLIGHT_RED_UNDERLINE     "\x1B[0;1;4;31m"
#నిర్వచించు ANSI_HIGHLIGHT_GREEN_UNDERLINE   "\x1B[0;1;4;32m"
#నిర్వచించు ANSI_HIGHLIGHT_YELLOW_UNDERLINE  "\x1B[0;1;4;38:5:185m"
#నిర్వచించు ANSI_HIGHLIGHT_BLUE_UNDERLINE    "\x1B[0;1;4;34m"
#నిర్వచించు ANSI_HIGHLIGHT_MAGENTA_UNDERLINE "\x1B[0;1;4;35m"
#నిర్వచించు ANSI_HIGHLIGHT_GREY_UNDERLINE    "\x1B[0;1;4;38:5:245m"

/* Other ANSI codes */
#నిర్వచించు ANSI_UNDERLINE "\x1B[0;4m"
#నిర్వచించు ANSI_కూడు_UNDERLINE "\x1B[4m"
#నిర్వచించు ANSI_కూడు_UNDERLINE_GREY ANSI_కూడు_UNDERLINE "\x1B[58:5:245m"
#నిర్వచించు ANSI_ITALICS "\x1B[0;3m"
#నిర్వచించు ANSI_కూడు_ITALICS "\x1B[3m"
#నిర్వచించు ANSI_HIGHLIGHT "\x1B[0;1;39m"
#నిర్వచించు ANSI_HIGHLIGHT_UNDERLINE "\x1B[0;1;4m"

/* Fallback colors: 256 → 16 */
#నిర్వచించు ANSI_HIGHLIGHT_GREY_FALLBACK             "\x1B[0;1;90m"
#నిర్వచించు ANSI_HIGHLIGHT_GREY_FALLBACK_UNDERLINE   "\x1B[0;1;4;90m"
#నిర్వచించు ANSI_HIGHLIGHT_YELLOW_FALLBACK           "\x1B[0;1;33m"
#నిర్వచించు ANSI_HIGHLIGHT_YELLOW_FALLBACK_UNDERLINE "\x1B[0;1;4;33m"

/* Background colors */
#నిర్వచించు ANSI_BACKGROUND_BLUE "\x1B[44m"

/* Reset/clear ANSI styles */
#నిర్వచించు ANSI_సాధారణ "\x1B[0m"

#నిర్వచించు DEFINE_ANSI_FUNC(name, NAME)                            \
        స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ansi_##name(శూన్య) {           \
                తిరిగి colors_enabled() ? ANSI_##NAME : "";     \
        }

/* NB: in 256 mode we always emit the fallback color first, in order to deal with terminals with
 * incomplete 256 color support (most notably Linux console, which a) lacks support for ":"
 * subcommand separator and b) skips over the whole CSI-m sequence if it sees an "invalid" command).
 * In 24-bit mode we don't bother with this however, under the assumption that $COLORTERM and friends
 * reflect the correct status. */

#నిర్వచించు DEFINE_ANSI_FUNC_256(name, NAME, FALLBACK)                              \
        స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ansi_##name(శూన్య) {                           \
                ఎంపిక (get_color_బహుళకం()) {                                     \
                        స్థితి COLOR_OFF: తిరిగి "";                              \
                        స్థితి COLOR_16: తిరిగి ANSI_##FALLBACK;                  \
                        స్థితి COLOR_256: తిరిగి ANSI_##FALLBACK ANSI_##NAME;     \
                        అప్రమేయ: తిరిగి ANSI_##NAME;                            \
                }                                                               \
        }

DEFINE_ANSI_FUNC(సాధారణ,            సాధారణ_2);
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

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* _ansi_highlight_yellow(శూన్య) {
        తిరిగి colors_enabled() ? _ANSI_HIGHLIGHT_YELLOW : "";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ansi_highlight_green_red(bool b) {
        తిరిగి b ? ansi_highlight_green() : ansi_highlight_red();
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ansi_underline(శూన్య) {
        తిరిగి underline_enabled() ? ANSI_UNDERLINE : "";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ansi_కూడు_underline(శూన్య) {
        తిరిగి underline_enabled() ? ANSI_కూడు_UNDERLINE : "";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ansi_కూడు_underline_grey(శూన్య) {
        తిరిగి underline_enabled() ?
                (colors_enabled() ? ANSI_కూడు_UNDERLINE_GREY : ANSI_కూడు_UNDERLINE) : "";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ansi_italics(శూన్య) {
        /* We hook italics also into the underline checks, close enough */
        తిరిగి underline_enabled() ? ANSI_ITALICS : "";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ansi_కూడు_italics(శూన్య) {
        తిరిగి underline_enabled() ? ANSI_కూడు_ITALICS : "";
}

#నిర్వచించు DEFINE_ANSI_FUNC_UNDERLINE(name, NAME)                          \
        స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ansi_##name##_underline(శూన్య) {       \
                తిరిగి underline_enabled() ? ANSI_##NAME##_UNDERLINE :  \
                        ansi_##name();                                  \
        }

#నిర్వచించు DEFINE_ANSI_FUNC_UNDERLINE_256(name, NAME, FALLBACK)                                            \
        స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ansi_##name##_underline(శూన్య) {                                       \
                ఒకవేళ (!underline_enabled())                                                               \
                        తిరిగి ansi_##name();                                                           \
                                                                                                        \
                ఎంపిక (get_color_బహుళకం()) {                                                             \
                        స్థితి COLOR_OFF: తిరిగి "";                                                      \
                        స్థితి COLOR_16: తిరిగి ANSI_##FALLBACK##_UNDERLINE;                              \
                        స్థితి COLOR_256: తిరిగి ANSI_##FALLBACK##_UNDERLINE ANSI_##NAME##_UNDERLINE;     \
                        అప్రమేయ: తిరిగి ANSI_##NAME##_UNDERLINE;                                        \
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
