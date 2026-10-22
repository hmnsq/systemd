/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ExtractFlags {
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା extract_first_word(ଅଚଳ ଅକ୍ଷର **p, ଅକ୍ଷର **ret, ଅଚଳ ଅକ୍ଷର *separators, ExtractFlags flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା extract_first_word_and_warn(ଅଚଳ ଅକ୍ଷର **p, ଅକ୍ଷର **ret, ଅଚଳ ଅକ୍ଷର *separators, ExtractFlags flags, ଅଚଳ ଅକ୍ଷର *unit, ଅଚଳ ଅକ୍ଷର *filename, unsigned ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *rvalue);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା extract_many_ଶବ୍ଦଗୁଡ଼ିକ_internal(ଅଚଳ ଅକ୍ଷର **p, ଅଚଳ ଅକ୍ଷର *separators, unsigned flags, ...) _sentinel_;
#define extract_many_ଶବ୍ଦଗୁଡ଼ିକ(p, separators, flags, ...) \
        extract_many_ଶବ୍ଦଗୁଡ଼ିକ_internal(p, separators, flags, ##__VA_ARGS__, ଖାଲି)
