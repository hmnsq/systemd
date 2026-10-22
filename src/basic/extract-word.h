/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार क्रमागत ExtractFlags {
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

पूर्णांक extract_first_word(स्थिर अक्षर **p, अक्षर **ret, स्थिर अक्षर *separators, ExtractFlags flags);
पूर्णांक extract_first_word_and_warn(स्थिर अक्षर **p, अक्षर **ret, स्थिर अक्षर *separators, ExtractFlags flags, स्थिर अक्षर *unit, स्थिर अक्षर *filename, अचिन्हित पंक्ति, स्थिर अक्षर *rvalue);

पूर्णांक extract_many_शब्दगण_internal(स्थिर अक्षर **p, स्थिर अक्षर *separators, अचिन्हित flags, ...) _sentinel_;
#घोषणा extract_many_शब्दगण(p, separators, flags, ...) \
        extract_many_शब्दगण_internal(p, separators, flags, ##__VA_ARGS__, शून्य)
