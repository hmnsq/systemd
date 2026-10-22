/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಹೊರತೆಗೆ_Flags {
        ಹೊರತೆಗೆ_RELAX                    = 1 << 0, /* Allow unbalanced quote and eat up trailing backslash. */
        ಹೊರತೆಗೆ_CUNESCAPE                = 1 << 1, /* Unescape known escape sequences. */
        ಹೊರತೆಗೆ_UNESCAPE_RELAX           = 1 << 2, /* Allow and keep unknown escape sequences, allow and keep trailing backslash. */
        ಹೊರತೆಗೆ_UNESCAPE_SEPARATORS      = 1 << 3, /* Unescape separators (those specified, or whitespace by default). */
        ಹೊರತೆಗೆ_KEEP_QUOTE               = 1 << 4, /* Ignore separators in quoting with "" and ''. */
        ಹೊರತೆಗೆ_UNQUOTE                  = 1 << 5, /* Ignore separators in quoting with "" and '', and remove the quotes. */
        ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS = 1 << 6, /* Don't treat multiple adjacent separators as one */
        ಹೊರತೆಗೆ_RETAIN_ESCAPE            = 1 << 7, /* Treat escape character '\' as any other character without special meaning */
        ಹೊರತೆಗೆ_RETAIN_SEPARATORS        = 1 << 8, /* Do not advance the original string pointer past the separator(s) */

        /* Note that if no flags are specified, escaped escape characters will be silently stripped. */
} ಹೊರತೆಗೆ_Flags;

ಪೂರ್ಣಾಂಕ ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, ಅಕ್ಷರ **ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಹೊರತೆಗೆ_Flags flags);
ಪೂರ್ಣಾಂಕ ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ_and_warn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, ಅಕ್ಷರ **ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಹೊರತೆಗೆ_Flags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಚಿಹ್ನೆರಹಿತ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rvalue);

ಪೂರ್ಣಾಂಕ ಹೊರತೆಗೆ_many_ಪದಗಳು_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಚಿಹ್ನೆರಹಿತ flags, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು ಹೊರತೆಗೆ_many_ಪದಗಳು(p, separators, flags, ...) \
        ಹೊರತೆಗೆ_many_ಪದಗಳು_ಆಂತರಿಕ(p, separators, flags, ##__VA_ARGS__, ಖಾಲಿ)
