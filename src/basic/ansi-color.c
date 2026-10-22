/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "ansi-color.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "terminal-util.h"

स्थिर चञ्चल पूर्णाङ्क cached_color_बहुलकम् = COLOR_बहुलकम्_INVALID;
स्थिर चञ्चल पूर्णाङ्क cached_underline_enabled = -1;

bool underline_enabled(शून्य) {

        यदि (cached_underline_enabled < 0) {

                /* The Linux console doesn't support underlining, turn it off, but only there. */

                यदि (colors_enabled())
                        cached_underline_enabled = !streq_ptr(getenv("TERM"), "linux");
                अन्यथा
                        cached_underline_enabled = false;
        }

        प्रत्यागम cached_underline_enabled;
}

शून्य reset_ansi_feature_caches(शून्य) {
        cached_color_बहुलकम् = COLOR_बहुलकम्_INVALID;
        cached_underline_enabled = -1;
}

Color_बहुलकम् parse_systemd_colors(शून्य) {
        ध्रुव वर्ण *e;

        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        e = getenv("SYSTEMD_COLORS");
        यदि (!e)
                प्रत्यागम COLOR_बहुलकम्_INVALID;

        प्रत्यागम color_बहुलकम्_from_string(e);
}

स्थिर Color_बहुलकम् get_color_बहुलकम्_impl(शून्य) {
        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        /* Returns the mode used to choose output colors. The possible modes are COLOR_OFF for no colors,
         * COLOR_16 for only the base 16 ANSI colors, COLOR_256 for more colors, and COLOR_24BIT for
         * unrestricted color output. */

        /* First, we check $SYSTEMD_COLORS, which is the explicit way to change the mode. */
        Color_बहुलकम् m = parse_systemd_colors();
        यदि (m >= 0 && m < COLOR_बहुलकम्_FIXED_MAX)
                प्रत्यागम m;

        /* If SYSTEMD_COLORS=true was set explicitly, skip the environment checks below — the user
         * explicitly requested colors, so honor it even when stdout is piped or $NO_COLOR is set. */
        यदि (m != COLOR_TRUE) {
                /* Check for the presence of $NO_COLOR; value is ignored. */
                यदि (getenv("NO_COLOR"))
                        प्रत्यागम COLOR_OFF;

                /* Turn colors off unless we are on a TTY. And if we are on a TTY we turn it off if $TERM
                 * is set to "dumb". There's one special tweak though: if we are PID 1 then we do not check
                 * whether we are connected to a TTY, because we don't keep /dev/console open continuously
                 * due to fear of SAK, and hence things are a bit weird. */
                यदि (getpid_cached() == 1 ? getenv_terminal_is_dumb() : terminal_is_dumb())
                        प्रत्यागम COLOR_OFF;
        }

        /* We failed to figure out any reason to *disable* colors. Let's see how many colors we shall use. */
        यदि (m == COLOR_AUTO_16)
                प्रत्यागम COLOR_16;
        यदि (m == COLOR_AUTO_256)
                प्रत्यागम COLOR_256;
        यदि (m == COLOR_AUTO_24BIT)
                प्रत्यागम COLOR_24BIT;

        यदि (STRPTR_अन्तर्_समुच्चयः(getenv("COLORTERM"),
                          "truecolor",
                          "24bit"))
                प्रत्यागम COLOR_24BIT;

        /* Note that the Linux console can only display 16 colors. We still enable 256 color mode
         * even for PID1 output though (which typically goes to the Linux console), since the Linux
         * console is able to parse the 256 color sequences and automatically map them to the closest
         * color in the 16 color palette (since kernel 3.16). Doing 256 colors is nice for people who
         * invoke systemd in a container or via a serial link or such, and use a true 256 color
         * terminal to do so. */
        प्रत्यागम COLOR_256;
}

Color_बहुलकम् get_color_बहुलकम्(शून्य) {
        यदि (cached_color_बहुलकम् < 0) {
                cached_color_बहुलकम् = get_color_बहुलकम्_impl();
                प्रतिज्ञा(cached_color_बहुलकम् >= 0 && cached_color_बहुलकम् < COLOR_बहुलकम्_FIXED_MAX);
        }

        प्रत्यागम cached_color_बहुलकम्;
}

स्थिर ध्रुव वर्ण* ध्रुव color_बहुलकम्_सारणी[COLOR_बहुलकम्_MAX] = {
        [COLOR_OFF]        = "off",
        [COLOR_16]         = "16",
        [COLOR_256]        = "256",
        [COLOR_24BIT]      = "24bit",
        [COLOR_AUTO_16]    = "auto-16",
        [COLOR_AUTO_256]   = "auto-256",
        [COLOR_AUTO_24BIT] = "auto-24bit",
        [COLOR_TRUE]       = "true",
};

DEFINE_STRING_सारणी_LOOKUP_WITH_BOOLEAN(color_बहुलकम्, Color_बहुलकम्, COLOR_TRUE);

/*
 * Check that the string is formatted like an ANSI color code, i.e. that it consists of one or more
 * sequences of ASCII digits separated by semicolons. This is equivalent to matching the regex:
 *      ^[0-9]+(;[0-9]+)*$
 * This can be used to partially validate escape codes of the form "\x1B[%sm", accepting all valid
 * ANSI color codes while rejecting anything that would result in garbled output (such as injecting
 * text or changing the type of escape code).
 */
bool looks_like_ansi_color_संहिता(ध्रुव वर्ण *str) {
        प्रतिज्ञा(str);

        bool पूर्व_char_was_digit = false;

        प्रति (वर्ण c = *str; c != '\0'; c = *(++str)) {
                यदि (ascii_isdigit(c))
                        पूर्व_char_was_digit = true;
                अन्यथा यदि (पूर्व_char_was_digit && c == ';')
                        पूर्व_char_was_digit = false;
                अन्यथा
                        प्रत्यागम false;
        }

        प्रत्यागम पूर्व_char_was_digit;
}
