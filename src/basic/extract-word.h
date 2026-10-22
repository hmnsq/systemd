/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

প্রকারনির্দেশ গণনা ExtractFlags {
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

পূর্ণাঙ্ক extract_first_word(ধ্রুৱ ৱর্ণ **p, ৱর্ণ **ret, ধ্রুৱ ৱর্ণ *separators, ExtractFlags flags);
পূর্ণাঙ্ক extract_first_word_and_warn(ধ্রুৱ ৱর্ণ **p, ৱর্ণ **ret, ধ্রুৱ ৱর্ণ *separators, ExtractFlags flags, ধ্রুৱ ৱর্ণ *unit, ধ্রুৱ ৱর্ণ *filename, unsigned সাৰি, ধ্রুৱ ৱর্ণ *rvalue);

পূর্ণাঙ্ক extract_many_শব্দগুলি_internal(ধ্রুৱ ৱর্ণ **p, ধ্রুৱ ৱর্ণ *separators, unsigned flags, ...) _sentinel_;
#define extract_many_শব্দগুলি(p, separators, flags, ...) \
        extract_many_শব্দগুলি_internal(p, separators, flags, ##__VA_ARGS__, রিক্ত)
