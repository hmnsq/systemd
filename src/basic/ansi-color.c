/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "ansi-color.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "terminal-util.h"

स्थिर चञ्चल पूर्णाङ्क cached_color_mode = _COLOR_MODE_INVALID;
स्थिर चञ्चल पूर्णाङ्क cached_underline_enabled = -1;

bool underline_enabled(शून्य) {

        सचे (cached_underline_enabled < 0) {

                /* The Linux console doesn't support underlining, turn it off, but only there. */

                सचे (colors_enabled())
                        cached_underline_enabled = !streq_ptr(getenv("TERM"), "linux");
                अञ्ञथा
                        cached_underline_enabled = false;
        }

        पटिनिवत्त cached_underline_enabled;
}

शून्य reset_ansi_feature_caches(शून्य) {
        cached_color_mode = _COLOR_MODE_INVALID;
        cached_underline_enabled = -1;
}

ColorMode parse_systemd_colors(शून्य) {
        ध्रुव वर्ण *e;

        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        e = getenv("SYSTEMD_COLORS");
        सचे (!e)
                पटिनिवत्त _COLOR_MODE_INVALID;

        पटिनिवत्त color_mode_from_string(e);
}

स्थिर ColorMode get_color_mode_impl(शून्य) {
        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        /* Returns the mode used to choose output colors. The possible modes are COLOR_OFF for no colors,
         * COLOR_16 for only the base 16 ANSI colors, COLOR_256 for more colors, and COLOR_24BIT for
         * unrestricted color output. */

        /* First, we check $SYSTEMD_COLORS, which is the explicit way to change the mode. */
        ColorMode m = parse_systemd_colors();
        सचे (m >= 0 && m < _COLOR_MODE_FIXED_MAX)
                पटिनिवत्त m;

        /* If SYSTEMD_COLORS=true was set explicitly, skip the environment checks below — the user
         * explicitly requested colors, so honor it even when stdout is piped or $NO_COLOR is set. */
        सचे (m != COLOR_TRUE) {
                /* Check for the presence of $NO_COLOR; value is ignored. */
                सचे (getenv("NO_COLOR"))
                        पटिनिवत्त COLOR_OFF;

                /* Turn colors off unless we are on a TTY. And if we are on a TTY we turn it off if $TERM
                 * is set to "dumb". There's one special tweak though: if we are PID 1 then we do not check
                 * whether we are connected to a TTY, because we don't keep /dev/console open continuously
                 * due to fear of SAK, and hence things are a bit weird. */
                सचे (getpid_cached() == 1 ? getenv_terminal_is_dumb() : terminal_is_dumb())
                        पटिनिवत्त COLOR_OFF;
        }

        /* We failed to figure out any reason to *disable* colors. Let's see how many colors we shall use. */
        सचे (m == COLOR_AUTO_16)
                पटिनिवत्त COLOR_16;
        सचे (m == COLOR_AUTO_256)
                पटिनिवत्त COLOR_256;
        सचे (m == COLOR_AUTO_24BIT)
                पटिनिवत्त COLOR_24BIT;

        सचे (STRPTR_IN_SET(getenv("COLORTERM"),
                          "truecolor",
                          "24bit"))
                पटिनिवत्त COLOR_24BIT;

        /* Note that the Linux console can only display 16 colors. We still enable 256 color mode
         * even for PID1 output though (which typically goes to the Linux console), since the Linux
         * console is able to parse the 256 color sequences and automatically map them to the closest
         * color in the 16 color palette (since kernel 3.16). Doing 256 colors is nice for people who
         * invoke systemd in a container or via a serial link or such, and use a true 256 color
         * terminal to do so. */
        पटिनिवत्त COLOR_256;
}

ColorMode get_color_mode(शून्य) {
        सचे (cached_color_mode < 0) {
                cached_color_mode = get_color_mode_impl();
                प्रतिज्ञा(cached_color_mode >= 0 && cached_color_mode < _COLOR_MODE_FIXED_MAX);
        }

        पटिनिवत्त cached_color_mode;
}

स्थिर ध्रुव वर्ण* ध्रुव color_mode_table[_COLOR_MODE_MAX] = {
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
bool looks_like_ansi_color_code(ध्रुव वर्ण *str) {
        प्रतिज्ञा(str);

        bool prev_char_was_digit = false;

        अत्थाय (वर्ण c = *str; c != '\0'; c = *(++str)) {
                सचे (ascii_isdigit(c))
                        prev_char_was_digit = true;
                अञ्ञथा सचे (prev_char_was_digit && c == ';')
                        prev_char_was_digit = false;
                अञ्ञथा
                        पटिनिवत्त false;
        }

        पटिनिवत्त prev_char_was_digit;
}
