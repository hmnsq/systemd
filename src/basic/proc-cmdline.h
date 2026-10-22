/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_ಪೂರ್ವಪ್ರತ್ಯಯ    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*proc_cmdline_parse_t)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, ಶೂನ್ಯ *ದತ್ತಾಂಶ);

ಪೂರ್ಣಾಂಕ proc_cmdline_filter_pid1_ವಾದಗಳು(ಅಕ್ಷರ **argv, ಅಕ್ಷರ ***ret);

ಪೂರ್ಣಾಂಕ proc_cmdline(ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ proc_cmdline_strv(ಅಕ್ಷರ ***ret);

ಪೂರ್ಣಾಂಕ proc_cmdline_parse(proc_cmdline_parse_t parse, ಶೂನ್ಯ *userdata, ProcCmdlineFlags flags);

ಪೂರ್ಣಾಂಕ proc_cmdline_get_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ProcCmdlineFlags flags, ಅಕ್ಷರ **ret_ಮೌಲ್ಯ);
ಪೂರ್ಣಾಂಕ proc_cmdline_get_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ProcCmdlineFlags flags, bool *ret);

ಪೂರ್ಣಾಂಕ proc_cmdline_get_ಕೀಲಿ_many_ಆಂತರಿಕ(ProcCmdlineFlags flags, ...);
#ವ್ಯಾಖ್ಯಾನಿಸು proc_cmdline_get_ಕೀಲಿ_many(flags, ...) proc_cmdline_get_ಕೀಲಿ_many_ಆಂತರಿಕ(flags, __VA_ARGS__, ಖಾಲಿ)

ಅಕ್ಷರ* proc_cmdline_ಕೀಲಿ_startswith(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ);
bool proc_cmdline_ಕೀಲಿ_streq(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_ಮೌಲ್ಯ_missing(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);
