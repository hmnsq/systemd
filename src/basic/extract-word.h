/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या गणना ExtractFlags {
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

पूर्णांक extract_first_word(स्थिरांक अक्षर **p, अक्षर **ret, स्थिरांक अक्षर *separators, ExtractFlags flags);
पूर्णांक extract_first_word_and_warn(स्थिरांक अक्षर **p, अक्षर **ret, स्थिरांक अक्षर *separators, ExtractFlags flags, स्थिरांक अक्षर *unit, स्थिरांक अक्षर *filename, अचिन्हित ओळ, स्थिरांक अक्षर *rvalue);

पूर्णांक extract_many_शब्द_internal(स्थिरांक अक्षर **p, स्थिरांक अक्षर *separators, अचिन्हित flags, ...) _sentinel_;
#define extract_many_शब्द(p, separators, flags, ...) \
        extract_many_शब्द_internal(p, separators, flags, ##__VA_ARGS__, रिक्त)
