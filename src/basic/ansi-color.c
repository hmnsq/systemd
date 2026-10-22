/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>

#شامل "ansi-color.h"
#شامل "process-util.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "terminal-util.h"

ساکن متلون صحیح cached_color_منوال = COLOR_منوال_INVALID;
ساکن متلون صحیح cached_underline_enabled = -1;

bool underline_enabled(خالی) {

        اگر (cached_underline_enabled < 0) {

                /* The Linux console doesn't support underlining, turn it off, but only there. */

                اگر (colors_enabled())
                        cached_underline_enabled = !streq_ptr(getenv("TERM"), "linux");
                ورنہ
                        cached_underline_enabled = false;
        }

        واپس cached_underline_enabled;
}

خالی reset_ansi_feature_caches(خالی) {
        cached_color_منوال = COLOR_منوال_INVALID;
        cached_underline_enabled = -1;
}

Color_منوال parse_systemd_colors(خالی) {
        ثابت حرف *e;

        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        e = getenv("SYSTEMD_COLORS");
        اگر (!e)
                واپس COLOR_منوال_INVALID;

        واپس color_منوال_from_string(e);
}

ساکن Color_منوال get_color_منوال_impl(خالی) {
        /* Note: do not log in this function, to avoid infinite recursion issues, as the log functions call
         * this when deciding whether to color the output. */

        /* Returns the mode used to choose output colors. The possible modes are COLOR_OFF for no colors,
         * COLOR_16 for only the base 16 ANSI colors, COLOR_256 for more colors, and COLOR_24BIT for
         * unrestricted color output. */

        /* First, we check $SYSTEMD_COLORS, which is the explicit way to change the mode. */
        Color_منوال m = parse_systemd_colors();
        اگر (m >= 0 && m < COLOR_منوال_FIXED_MAX)
                واپس m;

        /* If SYSTEMD_COLORS=true was set explicitly, skip the environment checks below — the user
         * explicitly requested colors, so honor it even when stdout is piped or $NO_COLOR is set. */
        اگر (m != COLOR_TRUE) {
                /* Check for the presence of $NO_COLOR; value is ignored. */
                اگر (getenv("NO_COLOR"))
                        واپس COLOR_OFF;

                /* Turn colors off unless we are on a TTY. And if we are on a TTY we turn it off if $TERM
                 * is set to "dumb". There's one special tweak though: if we are PID 1 then we do not check
                 * whether we are connected to a TTY, because we don't keep /dev/console open continuously
                 * due to fear of SAK, and hence things are a bit weird. */
                اگر (getpid_cached() == 1 ? getenv_terminal_is_dumb() : terminal_is_dumb())
                        واپس COLOR_OFF;
        }

        /* We failed to figure out any reason to *disable* colors. Let's see how many colors we shall use. */
        اگر (m == COLOR_AUTO_16)
                واپس COLOR_16;
        اگر (m == COLOR_AUTO_256)
                واپس COLOR_256;
        اگر (m == COLOR_AUTO_24_ثنائی_ہندسہ)
                واپس COLOR_24_ثنائی_ہندسہ;

        اگر (STRPTR_اندر_سیٹ(getenv("COLORTERM"),
                          "truecolor",
                          "24bit"))
                واپس COLOR_24_ثنائی_ہندسہ;

        /* Note that the Linux console can only display 16 colors. We still enable 256 color mode
         * even for PID1 output though (which typically goes to the Linux console), since the Linux
         * console is able to parse the 256 color sequences and automatically map them to the closest
         * color in the 16 color palette (since kernel 3.16). Doing 256 colors is nice for people who
         * invoke systemd in a container or via a serial link or such, and use a true 256 color
         * terminal to do so. */
        واپس COLOR_256;
}

Color_منوال get_color_منوال(خالی) {
        اگر (cached_color_منوال < 0) {
                cached_color_منوال = get_color_منوال_impl();
                أكد(cached_color_منوال >= 0 && cached_color_منوال < COLOR_منوال_FIXED_MAX);
        }

        واپس cached_color_منوال;
}

ساکن ثابت حرف* ثابت color_منوال_جدول[COLOR_منوال_MAX] = {
        [COLOR_OFF]        = "off",
        [COLOR_16]         = "16",
        [COLOR_256]        = "256",
        [COLOR_24_ثنائی_ہندسہ]      = "24bit",
        [COLOR_AUTO_16]    = "auto-16",
        [COLOR_AUTO_256]   = "auto-256",
        [COLOR_AUTO_24_ثنائی_ہندسہ] = "auto-24bit",
        [COLOR_TRUE]       = "true",
};

DEFINE_STRING_جدول_LOOKUP_WITH_BOOLEAN(color_منوال, Color_منوال, COLOR_TRUE);

/*
 * Check that the string is formatted like an ANSI color code, i.e. that it consists of one or more
 * sequences of ASCII digits separated by semicolons. This is equivalent to matching the regex:
 *      ^[0-9]+(;[0-9]+)*$
 * This can be used to partially validate escape codes of the form "\x1B[%sm", accepting all valid
 * ANSI color codes while rejecting anything that would result in garbled output (such as injecting
 * text or changing the type of escape code).
 */
bool looks_like_ansi_color_رمز(ثابت حرف *str) {
        أكد(str);

        bool پچھلا_char_was_digit = false;

        برائے (حرف c = *str; c != '\0'; c = *(++str)) {
                اگر (ascii_isdigit(c))
                        پچھلا_char_was_digit = true;
                ورنہ اگر (پچھلا_char_was_digit && c == ';')
                        پچھلا_char_was_digit = false;
                ورنہ
                        واپس false;
        }

        واپس پچھلا_char_was_digit;
}
