/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_ਅਗੇਤਰ    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_ਮੁੱਲ_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*proc_cmdline_parse_t)(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, ਖਾਲੀ *ਅੰਕੜੇ);

ਪੂਰਨ proc_cmdline_filter_pid1_ਦਲੀਲਾਂ(ਅੱਖਰ **argv, ਅੱਖਰ ***ret);

ਪੂਰਨ proc_cmdline(ਅੱਖਰ **ret);
ਪੂਰਨ proc_cmdline_strv(ਅੱਖਰ ***ret);

ਪੂਰਨ proc_cmdline_parse(proc_cmdline_parse_t parse, ਖਾਲੀ *userdata, ProcCmdlineFlags flags);

ਪੂਰਨ proc_cmdline_get_ਕੁੰਜੀ(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ProcCmdlineFlags flags, ਅੱਖਰ **ret_ਮੁੱਲ);
ਪੂਰਨ proc_cmdline_get_bool(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ProcCmdlineFlags flags, bool *ret);

ਪੂਰਨ proc_cmdline_get_ਕੁੰਜੀ_many_ਅੰਦਰੂਨੀ(ProcCmdlineFlags flags, ...);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ proc_cmdline_get_ਕੁੰਜੀ_many(flags, ...) proc_cmdline_get_ਕੁੰਜੀ_many_ਅੰਦਰੂਨੀ(flags, __VA_ARGS__, ਨਿਰਾ)

ਅੱਖਰ* proc_cmdline_ਕੁੰਜੀ_startswith(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ);
bool proc_cmdline_ਕੁੰਜੀ_streq(ਅਚਲ ਅੱਖਰ *x, ਅਚਲ ਅੱਖਰ *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_ਮੁੱਲ_missing(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);
