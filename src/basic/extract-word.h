/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف شمار نکالو_Flags {
        نکالو_RELAX                    = 1 << 0, /* Allow unbalanced quote and eat up trailing backslash. */
        نکالو_CUNESCAPE                = 1 << 1, /* Unescape known escape sequences. */
        نکالو_UNESCAPE_RELAX           = 1 << 2, /* Allow and keep unknown escape sequences, allow and keep trailing backslash. */
        نکالو_UNESCAPE_SEPARATORS      = 1 << 3, /* Unescape separators (those specified, or whitespace by default). */
        نکالو_KEEP_QUOTE               = 1 << 4, /* Ignore separators in quoting with "" and ''. */
        نکالو_UNQUOTE                  = 1 << 5, /* Ignore separators in quoting with "" and '', and remove the quotes. */
        نکالو_DONT_COALESCE_SEPARATORS = 1 << 6, /* Don't treat multiple adjacent separators as one */
        نکالو_RETAIN_ESCAPE            = 1 << 7, /* Treat escape character '\' as any other character without special meaning */
        نکالو_RETAIN_SEPARATORS        = 1 << 8, /* Do not advance the original string pointer past the separator(s) */

        /* Note that if no flags are specified, escaped escape characters will be silently stripped. */
} نکالو_Flags;

صحیح نکالو_پہلا_لفظ(ثابت حرف **p, حرف **ret, ثابت حرف *separators, نکالو_Flags flags);
صحیح نکالو_پہلا_لفظ_and_warn(ثابت حرف **p, حرف **ret, ثابت حرف *separators, نکالو_Flags flags, ثابت حرف *unit, ثابت حرف *filename, غیرعلامتی سطر, ثابت حرف *rvalue);

صحیح نکالو_many_الفاظ_اندرونی(ثابت حرف **p, ثابت حرف *separators, غیرعلامتی flags, ...) _sentinel_;
#تعریف_کرو نکالو_many_الفاظ(p, separators, flags, ...) \
        نکالو_many_الفاظ_اندرونی(p, separators, flags, ##__VA_ARGS__, تہی)
