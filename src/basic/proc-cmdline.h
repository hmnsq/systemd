/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_PREFIX    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*proc_cmdline_parse_t)(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, ಶೂನ್ಯ *data);

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_filter_pid1_args(ವರ್ಣ **argv, ವರ್ಣ ***ret);

ಪೂರ್ಣಾಙ್ಕ proc_cmdline(ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ proc_cmdline_strv(ವರ್ಣ ***ret);

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_parse(proc_cmdline_parse_t parse, ಶೂನ್ಯ *userdata, ProcCmdlineFlags flags);

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_get_ಕೀಲಿ(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ProcCmdlineFlags flags, ವರ್ಣ **ret_ಮೌಲ್ಯ);
ಪೂರ್ಣಾಙ್ಕ proc_cmdline_get_bool(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ProcCmdlineFlags flags, bool *ret);

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_get_ಕೀಲಿ_many_internal(ProcCmdlineFlags flags, ...);
#define proc_cmdline_get_ಕೀಲಿ_many(flags, ...) proc_cmdline_get_ಕೀಲಿ_many_internal(flags, __VA_ARGS__, ರಿಕ್ತ)

ವರ್ಣ* proc_cmdline_ಕೀಲಿ_startswith(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *prefix);
bool proc_cmdline_ಕೀಲಿ_streq(ಧ್ರುವ ವರ್ಣ *x, ಧ್ರುವ ವರ್ಣ *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_ಮೌಲ್ಯ_missing(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
