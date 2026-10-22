/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع تعداد ڪڍو_Flags {
        ڪڍو_RELAX                    = 1 << 0, /* Allow unbalanced quote and eat up trailing backslash. */
        ڪڍو_CUNESCAPE                = 1 << 1, /* Unescape known escape sequences. */
        ڪڍو_UNESCAPE_RELAX           = 1 << 2, /* Allow and keep unknown escape sequences, allow and keep trailing backslash. */
        ڪڍو_UNESCAPE_SEPARATORS      = 1 << 3, /* Unescape separators (those specified, or whitespace by default). */
        ڪڍو_KEEP_QUOTE               = 1 << 4, /* Ignore separators in quoting with "" and ''. */
        ڪڍو_UNQUOTE                  = 1 << 5, /* Ignore separators in quoting with "" and '', and remove the quotes. */
        ڪڍو_DONT_COALESCE_SEPARATORS = 1 << 6, /* Don't treat multiple adjacent separators as one */
        ڪڍو_RETAIN_ESCAPE            = 1 << 7, /* Treat escape character '\' as any other character without special meaning */
        ڪڍو_RETAIN_SEPARATORS        = 1 << 8, /* Do not advance the original string pointer past the separator(s) */

        /* Note that if no flags are specified, escaped escape characters will be silently stripped. */
} ڪڍو_Flags;

صحيح ڪڍو_پهريون_لفظ(ثابت حرف **p, حرف **ret, ثابت حرف *separators, ڪڍو_Flags flags);
صحيح ڪڍو_پهريون_لفظ_and_warn(ثابت حرف **p, حرف **ret, ثابت حرف *separators, ڪڍو_Flags flags, ثابت حرف *unit, ثابت حرف *filename, بنانشان سٽ, ثابت حرف *rvalue);

صحيح ڪڍو_many_لفظ_اندروني(ثابت حرف **p, ثابت حرف *separators, بنانشان flags, ...) _sentinel_;
#وصف_ڪريو ڪڍو_many_لفظ(p, separators, flags, ...) \
        ڪڍو_many_لفظ_اندروني(p, separators, flags, ##__VA_ARGS__, عدم)
