/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

تعريفنوع تعداد استخراج_Flags {
        استخراج_RELAX                    = 1 << 0, /* Allow unbalanced quote and eat up trailing backslash. */
        استخراج_CUNESCAPE                = 1 << 1, /* Unescape known escape sequences. */
        استخراج_UNESCAPE_RELAX           = 1 << 2, /* Allow and keep unknown escape sequences, allow and keep trailing backslash. */
        استخراج_UNESCAPE_SEPARATORS      = 1 << 3, /* Unescape separators (those specified, or whitespace by default). */
        استخراج_KEEP_QUOTE               = 1 << 4, /* Ignore separators in quoting with "" and ''. */
        استخراج_UNQUOTE                  = 1 << 5, /* Ignore separators in quoting with "" and '', and remove the quotes. */
        استخراج_DONT_COALESCE_SEPARATORS = 1 << 6, /* Don't treat multiple adjacent separators as one */
        استخراج_RETAIN_ESCAPE            = 1 << 7, /* Treat escape character '\' as any other character without special meaning */
        استخراج_RETAIN_SEPARATORS        = 1 << 8, /* Do not advance the original string pointer past the separator(s) */

        /* Note that if no flags are specified, escaped escape characters will be silently stripped. */
} استخراج_Flags;

صحيح استخراج_اولی_کلمه(ثابت حرف **p, حرف **ret, ثابت حرف *separators, استخراج_Flags flags);
صحيح استخراج_اولی_کلمه_and_warn(ثابت حرف **p, حرف **ret, ثابت حرف *separators, استخراج_Flags flags, ثابت حرف *unit, ثابت حرف *filename, بیعلامت سطر, ثابت حرف *rvalue);

صحيح استخراج_many_واژگان_داخلی(ثابت حرف **p, ثابت حرف *separators, بیعلامت flags, ...) _sentinel_;
#تعریف_کن استخراج_many_واژگان(p, separators, flags, ...) \
        استخراج_many_واژگان_داخلی(p, separators, flags, ##__VA_ARGS__, عدم)
