/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ ExtractFlags {
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

പൂർണ്ണസംഖ്യ extract_first_word(സ്ഥിരം അക്ഷരം **p, അക്ഷരം **ret, സ്ഥിരം അക്ഷരം *separators, ExtractFlags flags);
പൂർണ്ണസംഖ്യ extract_first_word_and_warn(സ്ഥിരം അക്ഷരം **p, അക്ഷരം **ret, സ്ഥിരം അക്ഷരം *separators, ExtractFlags flags, സ്ഥിരം അക്ഷരം *unit, സ്ഥിരം അക്ഷരം *filename, unsigned വരി, സ്ഥിരം അക്ഷരം *rvalue);

പൂർണ്ണസംഖ്യ extract_many_വാക്കുകൾ_internal(സ്ഥിരം അക്ഷരം **p, സ്ഥിരം അക്ഷരം *separators, unsigned flags, ...) _sentinel_;
#define extract_many_വാക്കുകൾ(p, separators, flags, ...) \
        extract_many_വാക്കുകൾ_internal(p, separators, flags, ##__VA_ARGS__, ശൂന്യ)
