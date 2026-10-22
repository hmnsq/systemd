/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>

#ضمن "ansi-color.h"
#ضمن "process-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "terminal-util.h"

ساكن متطاير صحيح cached_color_نما = COLOR_نما_INVALID;
ساكن متطاير صحيح cached_underline_enabled = -1;

bool underline_enabled(فراغ) {

        إذا (cached_underline_enabled < 0) {

                /* The Linux console doesn't support underlining, turn it off, but only there. */

                إذا (colors_enabled())
                        cached_underline_enabled = !streq_ptr(getenv("TERM"), "linux");
                وإلا
                        cached_underline_enabled = false;
        }

        أرجع cached_underline_enabled;
}

فراغ reset_ansi_feature_caches(فراغ) {
        cached_color_نما = COLOR_نما_INVALID;
        cached_underline_enabled = -1;
}

Color_نما parse_systemd_colors(فراغ) {
        ثابت حرف *e;

        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        e = getenv("SYSTEMD_COLORS");
        إذا (!e)
                أرجع COLOR_نما_INVALID;

        أرجع color_نما_from_string(e);
}

ساكن Color_نما get_color_نما_impl(فراغ) {
        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        /* Returns the mode used to choose output colors. The possible modes are COLOR_OFF for no colors,
         * COLOR_16 for only the base 16 ANSI colors, COLOR_256 for more colors, and COLOR_24BIT for
         * unrestricted color output. */

        /* First, we check $SYSTEMD_COLORS, which is the explicit way to change the mode. */
        Color_نما m = parse_systemd_colors();
        إذا (m >= 0 && m < COLOR_نما_FIXED_MAX)
                أرجع m;

        /* If SYSTEMD_COLORS=true was set explicitly, skip the environment checks below — the user
         * explicitly requested colors, so honor it even when stdout is piped or $NO_COLOR is set. */
        إذا (m != COLOR_TRUE) {
                /* Check for the presence of $NO_COLOR; value is ignored. */
                إذا (getenv("NO_COLOR"))
                        أرجع COLOR_OFF;

                /* Turn colors off unless we are on a TTY. And if we are on a TTY we turn it off if $TERM
                 * is set to "dumb". There's one special tweak though: if we are PID 1 then we do not check
                 * whether we are connected to a TTY, because we don't keep /dev/console open continuously
                 * due to fear of SAK, and hence things are a bit weird. */
                إذا (getpid_cached() == 1 ? getenv_terminal_is_dumb() : terminal_is_dumb())
                        أرجع COLOR_OFF;
        }

        /* We failed to figure out any reason to *disable* colors. Let's see how many colors we shall use. */
        إذا (m == COLOR_AUTO_16)
                أرجع COLOR_16;
        إذا (m == COLOR_AUTO_256)
                أرجع COLOR_256;
        إذا (m == COLOR_AUTO_24_بیت)
                أرجع COLOR_24_بیت;

        إذا (STRPTR_درون_مجموعه(getenv("COLORTERM"),
                          "truecolor",
                          "24bit"))
                أرجع COLOR_24_بیت;

        /* Note that the Linux console can only display 16 colors. We still enable 256 color mode
         * even for PID1 output though (which typically goes to the Linux console), since the Linux
         * console is able to parse the 256 color sequences and automatically map them to the closest
         * color in the 16 color palette (since kernel 3.16). Doing 256 colors is nice for people who
         * invoke systemd in a container or via a serial link or such, and use a true 256 color
         * terminal to do so. */
        أرجع COLOR_256;
}

Color_نما get_color_نما(فراغ) {
        إذا (cached_color_نما < 0) {
                cached_color_نما = get_color_نما_impl();
                أكد(cached_color_نما >= 0 && cached_color_نما < COLOR_نما_FIXED_MAX);
        }

        أرجع cached_color_نما;
}

ساكن ثابت حرف* ثابت color_نما_جدول[COLOR_نما_MAX] = {
        [COLOR_OFF]        = "off",
        [COLOR_16]         = "16",
        [COLOR_256]        = "256",
        [COLOR_24_بیت]      = "24bit",
        [COLOR_AUTO_16]    = "auto-16",
        [COLOR_AUTO_256]   = "auto-256",
        [COLOR_AUTO_24_بیت] = "auto-24bit",
        [COLOR_TRUE]       = "true",
};

DEFINE_STRING_جدول_LOOKUP_WITH_BOOLEAN(color_نما, Color_نما, COLOR_TRUE);

/*
 * Check that the string is formatted like an ANSI color code, i.e. that it consists of one or more
 * sequences of ASCII digits separated by semicolons. This is equivalent to matching the regex:
 *      ^[0-9]+(;[0-9]+)*$
 * This can be used to partially validate escape codes of the form "\x1B[%sm", accepting all valid
 * ANSI color codes while rejecting anything that would result in garbled output (such as injecting
 * text or changing the type of escape code).
 */
bool looks_like_ansi_color_کد(ثابت حرف *str) {
        أكد(str);

        bool prev_char_was_digit = false;

        لكل (حرف c = *str; c != '\0'; c = *(++str)) {
                إذا (ascii_isdigit(c))
                        prev_char_was_digit = true;
                وإلا إذا (prev_char_was_digit && c == ';')
                        prev_char_was_digit = false;
                وإلا
                        أرجع false;
        }

        أرجع prev_char_was_digit;
}
