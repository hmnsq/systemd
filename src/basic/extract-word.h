/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع تعداد کڈھو_Flags {
        کڈھو_RELAX                    = 1 << 0, /* Allow unbalanced quote and eat up trailing backslash. */
        کڈھو_CUNESCAPE                = 1 << 1, /* Unescape known escape sequences. */
        کڈھو_UNESCAPE_RELAX           = 1 << 2, /* Allow and keep unknown escape sequences, allow and keep trailing backslash. */
        کڈھو_UNESCAPE_SEPARATORS      = 1 << 3, /* Unescape separators (those specified, or whitespace by default). */
        کڈھو_KEEP_QUOTE               = 1 << 4, /* Ignore separators in quoting with "" and ''. */
        کڈھو_UNQUOTE                  = 1 << 5, /* Ignore separators in quoting with "" and '', and remove the quotes. */
        کڈھو_DONT_COALESCE_SEPARATORS = 1 << 6, /* Don't treat multiple adjacent separators as one */
        کڈھو_RETAIN_ESCAPE            = 1 << 7, /* Treat escape character '\' as any other character without special meaning */
        کڈھو_RETAIN_SEPARATORS        = 1 << 8, /* Do not advance the original string pointer past the separator(s) */

        /* Note that if no flags are specified, escaped escape characters will be silently stripped. */
} کڈھو_Flags;

صحيح کڈھو_پہلا_لفظ(ثابت حرف **p, حرف **ret, ثابت حرف *separators, کڈھو_Flags flags);
صحيح کڈھو_پہلا_لفظ_and_warn(ثابت حرف **p, حرف **ret, ثابت حرف *separators, کڈھو_Flags flags, ثابت حرف *unit, ثابت حرف *filename, بنانشانی سطر, ثابت حرف *rvalue);

صحيح کڈھو_many_الفاظ_اندرونی(ثابت حرف **p, ثابت حرف *separators, بنانشانی flags, ...) _sentinel_;
#تعریف_کرو کڈھو_many_الفاظ(p, separators, flags, ...) \
        کڈھو_many_الفاظ_اندرونی(p, separators, flags, ##__VA_ARGS__, عدم)
