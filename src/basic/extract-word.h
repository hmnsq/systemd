/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع تعداد ایستل_Flags {
        ایستل_RELAX                    = 1 << 0, /* Allow unbalanced quote and eat up trailing backslash. */
        ایستل_CUNESCAPE                = 1 << 1, /* Unescape known escape sequences. */
        ایستل_UNESCAPE_RELAX           = 1 << 2, /* Allow and keep unknown escape sequences, allow and keep trailing backslash. */
        ایستل_UNESCAPE_SEPARATORS      = 1 << 3, /* Unescape separators (those specified, or whitespace by default). */
        ایستل_KEEP_QUOTE               = 1 << 4, /* Ignore separators in quoting with "" and ''. */
        ایستل_UNQUOTE                  = 1 << 5, /* Ignore separators in quoting with "" and '', and remove the quotes. */
        ایستل_DONT_COALESCE_SEPARATORS = 1 << 6, /* Don't treat multiple adjacent separators as one */
        ایستل_RETAIN_ESCAPE            = 1 << 7, /* Treat escape character '\' as any other character without special meaning */
        ایستل_RETAIN_SEPARATORS        = 1 << 8, /* Do not advance the original string pointer past the separator(s) */

        /* Note that if no flags are specified, escaped escape characters will be silently stripped. */
} ایستل_Flags;

صحيح ایستل_لومړی_کلمه(ثابت حرف **p, حرف **ret, ثابت حرف *separators, ایستل_Flags flags);
صحيح ایستل_لومړی_کلمه_and_warn(ثابت حرف **p, حرف **ret, ثابت حرف *separators, ایستل_Flags flags, ثابت حرف *unit, ثابت حرف *filename, بینښه کرښه, ثابت حرف *rvalue);

صحيح ایستل_many_ټکي_دننی(ثابت حرف **p, ثابت حرف *separators, بینښه flags, ...) _sentinel_;
#تعریف_کړه ایستل_many_ټکي(p, separators, flags, ...) \
        ایستل_many_ټکي_دننی(p, separators, flags, ##__VA_ARGS__, عدم)
