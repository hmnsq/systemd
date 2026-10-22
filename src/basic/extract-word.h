/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਕੱਢੋ_Flags {
        ਕੱਢੋ_RELAX                    = 1 << 0, /* Allow unbalanced quote and eat up trailing backslash. */
        ਕੱਢੋ_CUNESCAPE                = 1 << 1, /* Unescape known escape sequences. */
        ਕੱਢੋ_UNESCAPE_RELAX           = 1 << 2, /* Allow and keep unknown escape sequences, allow and keep trailing backslash. */
        ਕੱਢੋ_UNESCAPE_SEPARATORS      = 1 << 3, /* Unescape separators (those specified, or whitespace by default). */
        ਕੱਢੋ_KEEP_QUOTE               = 1 << 4, /* Ignore separators in quoting with "" and ''. */
        ਕੱਢੋ_UNQUOTE                  = 1 << 5, /* Ignore separators in quoting with "" and '', and remove the quotes. */
        ਕੱਢੋ_DONT_COALESCE_SEPARATORS = 1 << 6, /* Don't treat multiple adjacent separators as one */
        ਕੱਢੋ_RETAIN_ESCAPE            = 1 << 7, /* Treat escape character '\' as any other character without special meaning */
        ਕੱਢੋ_RETAIN_SEPARATORS        = 1 << 8, /* Do not advance the original string pointer past the separator(s) */

        /* Note that if no flags are specified, escaped escape characters will be silently stripped. */
} ਕੱਢੋ_Flags;

ਪੂਰਨ ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(ਅਚਲ ਅੱਖਰ **p, ਅੱਖਰ **ret, ਅਚਲ ਅੱਖਰ *separators, ਕੱਢੋ_Flags flags);
ਪੂਰਨ ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ_and_warn(ਅਚਲ ਅੱਖਰ **p, ਅੱਖਰ **ret, ਅਚਲ ਅੱਖਰ *separators, ਕੱਢੋ_Flags flags, ਅਚਲ ਅੱਖਰ *unit, ਅਚਲ ਅੱਖਰ *filename, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ, ਅਚਲ ਅੱਖਰ *rvalue);

ਪੂਰਨ ਕੱਢੋ_many_ਸ਼ਬਦ_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ **p, ਅਚਲ ਅੱਖਰ *separators, ਬਿਨਾਨਿਸ਼ਾਨੀ flags, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕੱਢੋ_many_ਸ਼ਬਦ(p, separators, flags, ...) \
        ਕੱਢੋ_many_ਸ਼ਬਦ_ਅੰਦਰੂਨੀ(p, separators, flags, ##__VA_ARGS__, ਨਿਰਾ)
