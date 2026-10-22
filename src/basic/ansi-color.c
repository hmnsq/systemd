/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "ansi-color.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "terminal-util.h"

స్థిర అస్థిర పూర్ణాంకం cached_color_బహుళకం = COLOR_బహుళకం_INVALID;
స్థిర అస్థిర పూర్ణాంకం cached_underline_enabled = -1;

bool underline_enabled(శూన్య) {

        ఒకవేళ (cached_underline_enabled < 0) {

                /* The Linux console doesn't support underlining, turn it off, but only there. */

                ఒకవేళ (colors_enabled())
                        cached_underline_enabled = !streq_ptr(getenv("TERM"), "linux");
                లేకపోతే
                        cached_underline_enabled = false;
        }

        తిరిగి cached_underline_enabled;
}

శూన్య reset_ansi_feature_caches(శూన్య) {
        cached_color_బహుళకం = COLOR_బహుళకం_INVALID;
        cached_underline_enabled = -1;
}

Color_బహుళకం parse_systemd_colors(శూన్య) {
        స్థిరరాశి అక్షరం *e;

        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        e = getenv("SYSTEMD_COLORS");
        ఒకవేళ (!e)
                తిరిగి COLOR_బహుళకం_INVALID;

        తిరిగి color_బహుళకం_from_string(e);
}

స్థిర Color_బహుళకం get_color_బహుళకం_impl(శూన్య) {
        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        /* Returns the mode used to choose output colors. The possible modes are COLOR_OFF for no colors,
         * COLOR_16 for only the base 16 ANSI colors, COLOR_256 for more colors, and COLOR_24BIT for
         * unrestricted color output. */

        /* First, we check $SYSTEMD_COLORS, which is the explicit way to change the mode. */
        Color_బహుళకం m = parse_systemd_colors();
        ఒకవేళ (m >= 0 && m < COLOR_బహుళకం_FIXED_MAX)
                తిరిగి m;

        /* If SYSTEMD_COLORS=true was set explicitly, skip the environment checks below — the user
         * explicitly requested colors, so honor it even when stdout is piped or $NO_COLOR is set. */
        ఒకవేళ (m != COLOR_TRUE) {
                /* Check for the presence of $NO_COLOR; value is ignored. */
                ఒకవేళ (getenv("NO_COLOR"))
                        తిరిగి COLOR_OFF;

                /* Turn colors off unless we are on a TTY. And if we are on a TTY we turn it off if $TERM
                 * is set to "dumb". There's one special tweak though: if we are PID 1 then we do not check
                 * whether we are connected to a TTY, because we don't keep /dev/console open continuously
                 * due to fear of SAK, and hence things are a bit weird. */
                ఒకవేళ (getpid_cached() == 1 ? getenv_terminal_is_dumb() : terminal_is_dumb())
                        తిరిగి COLOR_OFF;
        }

        /* We failed to figure out any reason to *disable* colors. Let's see how many colors we shall use. */
        ఒకవేళ (m == COLOR_AUTO_16)
                తిరిగి COLOR_16;
        ఒకవేళ (m == COLOR_AUTO_256)
                తిరిగి COLOR_256;
        ఒకవేళ (m == COLOR_AUTO_24_బిట్టు)
                తిరిగి COLOR_24_బిట్టు;

        ఒకవేళ (STRPTR_లోపల_సమితి(getenv("COLORTERM"),
                          "truecolor",
                          "24bit"))
                తిరిగి COLOR_24_బిట్టు;

        /* Note that the Linux console can only display 16 colors. We still enable 256 color mode
         * even for PID1 output though (which typically goes to the Linux console), since the Linux
         * console is able to parse the 256 color sequences and automatically map them to the closest
         * color in the 16 color palette (since kernel 3.16). Doing 256 colors is nice for people who
         * invoke systemd in a container or via a serial link or such, and use a true 256 color
         * terminal to do so. */
        తిరిగి COLOR_256;
}

Color_బహుళకం get_color_బహుళకం(శూన్య) {
        ఒకవేళ (cached_color_బహుళకం < 0) {
                cached_color_బహుళకం = get_color_బహుళకం_impl();
                ప్రతిజ్ఞా(cached_color_బహుళకం >= 0 && cached_color_బహుళకం < COLOR_బహుళకం_FIXED_MAX);
        }

        తిరిగి cached_color_బహుళకం;
}

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి color_బహుళకం_పట్టిక[COLOR_బహుళకం_MAX] = {
        [COLOR_OFF]        = "off",
        [COLOR_16]         = "16",
        [COLOR_256]        = "256",
        [COLOR_24_బిట్టు]      = "24bit",
        [COLOR_AUTO_16]    = "auto-16",
        [COLOR_AUTO_256]   = "auto-256",
        [COLOR_AUTO_24_బిట్టు] = "auto-24bit",
        [COLOR_TRUE]       = "true",
};

DEFINE_STRING_పట్టిక_LOOKUP_WITH_BOOLEAN(color_బహుళకం, Color_బహుళకం, COLOR_TRUE);

/*
 * Check that the string is formatted like an ANSI color code, i.e. that it consists of one or more
 * sequences of ASCII digits separated by semicolons. This is equivalent to matching the regex:
 *      ^[0-9]+(;[0-9]+)*$
 * This can be used to partially validate escape codes of the form "\x1B[%sm", accepting all valid
 * ANSI color codes while rejecting anything that would result in garbled output (such as injecting
 * text or changing the type of escape code).
 */
bool looks_like_ansi_color_సంకేతం(స్థిరరాశి అక్షరం *str) {
        ప్రతిజ్ఞా(str);

        bool prev_char_was_digit = false;

        కొరకు (అక్షరం c = *str; c != '\0'; c = *(++str)) {
                ఒకవేళ (ascii_isdigit(c))
                        prev_char_was_digit = true;
                లేకపోతే ఒకవేళ (prev_char_was_digit && c == ';')
                        prev_char_was_digit = false;
                లేకపోతే
                        తిరిగి false;
        }

        తిరిగి prev_char_was_digit;
}
