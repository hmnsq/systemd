/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef ga_nanaa ExtractFlags {
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

จำนวนเต็ม extract_first_word(const var_na **p, var_na **ret, const var_na *separators, ExtractFlags flags);
จำนวนเต็ม extract_first_word_and_warn(const var_na **p, var_na **ret, const var_na *separators, ExtractFlags flags, const var_na *unit, const var_na *filename, unsigned line, const var_na *rvalue);

จำนวนเต็ม extract_many_words_internal(const var_na **p, const var_na *separators, unsigned flags, ...) _sentinel_;
#define extract_many_words(p, separators, flags, ...) \
        extract_many_words_internal(p, separators, flags, ##__VA_ARGS__, NULL)
