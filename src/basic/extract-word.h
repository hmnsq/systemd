/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన వెలికితీయి_Flags {
        వెలికితీయి_RELAX                    = 1 << 0, /* Allow unbalanced quote and eat up trailing backslash. */
        వెలికితీయి_CUNESCAPE                = 1 << 1, /* Unescape known escape sequences. */
        వెలికితీయి_UNESCAPE_RELAX           = 1 << 2, /* Allow and keep unknown escape sequences, allow and keep trailing backslash. */
        వెలికితీయి_UNESCAPE_SEPARATORS      = 1 << 3, /* Unescape separators (those specified, or whitespace by default). */
        వెలికితీయి_KEEP_QUOTE               = 1 << 4, /* Ignore separators in quoting with "" and ''. */
        వెలికితీయి_UNQUOTE                  = 1 << 5, /* Ignore separators in quoting with "" and '', and remove the quotes. */
        వెలికితీయి_DONT_COALESCE_SEPARATORS = 1 << 6, /* Don't treat multiple adjacent separators as one */
        వెలికితీయి_RETAIN_ESCAPE            = 1 << 7, /* Treat escape character '\' as any other character without special meaning */
        వెలికితీయి_RETAIN_SEPARATORS        = 1 << 8, /* Do not advance the original string pointer past the separator(s) */

        /* Note that if no flags are specified, escaped escape characters will be silently stripped. */
} వెలికితీయి_Flags;

పూర్ణాంకం వెలికితీయి_మొదటి_మాట(స్థిరరాశి అక్షరం **p, అక్షరం **ret, స్థిరరాశి అక్షరం *separators, వెలికితీయి_Flags flags);
పూర్ణాంకం వెలికితీయి_మొదటి_మాట_and_warn(స్థిరరాశి అక్షరం **p, అక్షరం **ret, స్థిరరాశి అక్షరం *separators, వెలికితీయి_Flags flags, స్థిరరాశి అక్షరం *unit, స్థిరరాశి అక్షరం *filename, గుర్తులేని పంక్తి, స్థిరరాశి అక్షరం *rvalue);

పూర్ణాంకం వెలికితీయి_many_పదాలు_అంతర్గత(స్థిరరాశి అక్షరం **p, స్థిరరాశి అక్షరం *separators, గుర్తులేని flags, ...) _sentinel_;
#నిర్వచించు వెలికితీయి_many_పదాలు(p, separators, flags, ...) \
        వెలికితీయి_many_పదాలు_అంతర్గత(p, separators, flags, ##__VA_ARGS__, ఖాళీ)
