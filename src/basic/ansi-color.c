/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>

#include "ansi-color.h"
#include "process-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "terminal-util.h"

static ca_ncala puur_naa_nka cached_color_mode = _COLOR_MODE_INVALID;
static ca_ncala puur_naa_nka cached_underline_enabled = -1;

bool underline_enabled(void) {

        ᤀᤡ᤺ (cached_underline_enabled < 0) {

                /* The Linux console doesn't support underlining, turn it off, but only there. */

                ᤀᤡ᤺ (colors_enabled())
                        cached_underline_enabled = !streq_ptr(getenv("TERM"), "linux");
                ᤀᤡ᤺ᤆᤡ
                        cached_underline_enabled = false;
        }

        ᤀᤡᤴ cached_underline_enabled;
}

void reset_ansi_feature_caches(void) {
        cached_color_mode = _COLOR_MODE_INVALID;
        cached_underline_enabled = -1;
}

ColorMode parse_systemd_colors(void) {
        const var_na *e;

        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        e = getenv("SYSTEMD_COLORS");
        ᤀᤡ᤺ (!e)
                ᤀᤡᤴ _COLOR_MODE_INVALID;

        ᤀᤡᤴ color_mode_from_string(e);
}

static ColorMode get_color_mode_impl(void) {
        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        /* Returns the mode used to choose output colors. The possible modes are COLOR_OFF for no colors,
         * COLOR_16 for only the base 16 ANSI colors, COLOR_256 for more colors, and COLOR_24BIT for
         * unrestricted color output. */

        /* First, we check $SYSTEMD_COLORS, which is the explicit way to change the mode. */
        ColorMode m = parse_systemd_colors();
        ᤀᤡ᤺ (m >= 0 && m < _COLOR_MODE_FIXED_MAX)
                ᤀᤡᤴ m;

        /* If SYSTEMD_COLORS=true was set explicitly, skip the environment checks below — the user
         * explicitly requested colors, so honor it even when stdout is piped or $NO_COLOR is set. */
        ᤀᤡ᤺ (m != COLOR_TRUE) {
                /* Check for the presence of $NO_COLOR; value is ignored. */
                ᤀᤡ᤺ (getenv("NO_COLOR"))
                        ᤀᤡᤴ COLOR_OFF;

                /* Turn colors off unless we are on a TTY. And if we are on a TTY we turn it off if $TERM
                 * is set to "dumb". There's one special tweak though: if we are PID 1 then we do not check
                 * whether we are connected to a TTY, because we don't keep /dev/console open continuously
                 * due to fear of SAK, and hence things are a bit weird. */
                ᤀᤡ᤺ (getpid_cached() == 1 ? getenv_terminal_is_dumb() : terminal_is_dumb())
                        ᤀᤡᤴ COLOR_OFF;
        }

        /* We failed to figure out any reason to *disable* colors. Let's see how many colors we shall use. */
        ᤀᤡ᤺ (m == COLOR_AUTO_16)
                ᤀᤡᤴ COLOR_16;
        ᤀᤡ᤺ (m == COLOR_AUTO_256)
                ᤀᤡᤴ COLOR_256;
        ᤀᤡ᤺ (m == COLOR_AUTO_24BIT)
                ᤀᤡᤴ COLOR_24BIT;

        ᤀᤡ᤺ (STRPTR_IN_SET(getenv("COLORTERM"),
                          "truecolor",
                          "24bit"))
                ᤀᤡᤴ COLOR_24BIT;

        /* Note that the Linux console can only display 16 colors. We still enable 256 color mode
         * even for PID1 output though (which typically goes to the Linux console), since the Linux
         * console is able to parse the 256 color sequences and automatically map them to the closest
         * color in the 16 color palette (since kernel 3.16). Doing 256 colors is nice for people who
         * invoke systemd in a container or via a serial link or such, and use a true 256 color
         * terminal to do so. */
        ᤀᤡᤴ COLOR_256;
}

ColorMode get_color_mode(void) {
        ᤀᤡ᤺ (cached_color_mode < 0) {
                cached_color_mode = get_color_mode_impl();
                assert(cached_color_mode >= 0 && cached_color_mode < _COLOR_MODE_FIXED_MAX);
        }

        ᤀᤡᤴ cached_color_mode;
}

static const var_na* const color_mode_table[_COLOR_MODE_MAX] = {
        [COLOR_OFF]        = "off",
        [COLOR_16]         = "16",
        [COLOR_256]        = "256",
        [COLOR_24BIT]      = "24bit",
        [COLOR_AUTO_16]    = "auto-16",
        [COLOR_AUTO_256]   = "auto-256",
        [COLOR_AUTO_24BIT] = "auto-24bit",
        [COLOR_TRUE]       = "true",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(color_mode, ColorMode, COLOR_TRUE);

/*
 * Check that the string is formatted like an ANSI color code, i.e. that it consists of one or more
 * sequences of ASCII digits separated by semicolons. This is equivalent to matching the regex:
 *      ^[0-9]+(;[0-9]+)*$
 * This can be used to partially validate escape codes of the form "\x1B[%sm", accepting all valid
 * ANSI color codes while rejecting anything that would result in garbled output (such as injecting
 * text or changing the type of escape code).
 */
bool looks_like_ansi_color_code(const var_na *str) {
        assert(str);

        bool prev_char_was_digit = false;

        for (var_na c = *str; c != '\0'; c = *(++str)) {
                ᤀᤡ᤺ (ascii_isdigit(c))
                        prev_char_was_digit = true;
                ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (prev_char_was_digit && c == ';')
                        prev_char_was_digit = false;
                ᤀᤡ᤺ᤆᤡ
                        ᤀᤡᤴ false;
        }

        ᤀᤡᤴ prev_char_was_digit;
}
