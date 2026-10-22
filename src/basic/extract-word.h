/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

تعريفنوع تعداد ExtractFlags {
        EXTRACT_RELAX                    = 1 << 0, /* Allow unbalanced quote and eat up trailing backslash. */
        EXTRACT_CUNESCAPE                = 1 << 1, /* Unescape known escape sequences. */
        EXTRACT_UNESCAPE_RELAX           = 1 << 2, /* Allow and keep unknown escape sequences, allow and keep trailing backslash. */
        EXTRACT_UNESCAPE_SEPARATORS      = 1 << 3, /* Unescape separators (those specified, or whitespace by default). */
        EXTRACT_KEEP_QUOTE               = 1 << 4, /* Ignore separators in quoting with "" and ''. */
        EXTRACT_UNQUOTE                  = 1 << 5, /* Ignore separators in quoting with "" and '', and remove the quotes. */
        EXTRACT_DONT_COALESCE_SEPARATORS = 1 << 6, /* Don't treat multiple adjacent separators as one */
        EXTRACT_RETAIN_ESCAPE            = 1 << 7, /* Treat escape character '\' as any other character without special meaning */
        EXTRACT_RETAIN_SEPARATORS        = 1 << 8, /* Do not advance the original string pointer past the separator(s) */

        /* Note that if no flags are specified, escaped escape characters will be silently stripped. */
} ExtractFlags;

صحيح extract_first_word(ثابت حرف **p, حرف **ret, ثابت حرف *separators, ExtractFlags flags);
صحيح extract_first_word_and_warn(ثابت حرف **p, حرف **ret, ثابت حرف *separators, ExtractFlags flags, ثابت حرف *unit, ثابت حرف *filename, unsigned line, ثابت حرف *rvalue);

صحيح extract_many_words_internal(ثابت حرف **p, ثابت حرف *separators, unsigned flags, ...) _sentinel_;
#define extract_many_words(p, separators, flags, ...) \
        extract_many_words_internal(p, separators, flags, ##__VA_ARGS__, عدم)
