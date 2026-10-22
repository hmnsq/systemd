/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ExtractFlags {
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

ಪೂರ್ಣಾಙ್ಕ extract_first_word(ಧ್ರುವ ವರ್ಣ **p, ವರ್ಣ **ret, ಧ್ರುವ ವರ್ಣ *separators, ExtractFlags flags);
ಪೂರ್ಣಾಙ್ಕ extract_first_word_and_warn(ಧ್ರುವ ವರ್ಣ **p, ವರ್ಣ **ret, ಧ್ರುವ ವರ್ಣ *separators, ExtractFlags flags, ಧ್ರುವ ವರ್ಣ *unit, ಧ್ರುವ ವರ್ಣ *filename, unsigned ಸಾಲು, ಧ್ರುವ ವರ್ಣ *rvalue);

ಪೂರ್ಣಾಙ್ಕ extract_many_ಪದಗಳು_internal(ಧ್ರುವ ವರ್ಣ **p, ಧ್ರುವ ವರ್ಣ *separators, unsigned flags, ...) _sentinel_;
#define extract_many_ಪದಗಳು(p, separators, flags, ...) \
        extract_many_ಪದಗಳು_internal(p, separators, flags, ##__VA_ARGS__, ರಿಕ್ತ)
