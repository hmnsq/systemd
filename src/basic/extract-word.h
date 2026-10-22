/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना ExtractFlags {
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

पूर्णाङ्क extract_first_word(ध्रुव वर्ण **p, वर्ण **ret, ध्रुव वर्ण *separators, ExtractFlags flags);
पूर्णाङ्क extract_first_word_and_warn(ध्रुव वर्ण **p, वर्ण **ret, ध्रुव वर्ण *separators, ExtractFlags flags, ध्रुव वर्ण *unit, ध्रुव वर्ण *filename, unsigned ओळ, ध्रुव वर्ण *rvalue);

पूर्णाङ्क extract_many_शब्द_internal(ध्रुव वर्ण **p, ध्रुव वर्ण *separators, unsigned flags, ...) _sentinel_;
#define extract_many_शब्द(p, separators, flags, ...) \
        extract_many_शब्द_internal(p, separators, flags, ##__VA_ARGS__, रिक्त)
