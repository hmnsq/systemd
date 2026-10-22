/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ গণনা ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_PREFIX    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_মান_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

প্রকারলেখ পূর্ণসংখ্যা (*proc_cmdline_parse_t)(ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান, শূন্য *তথ্য);

পূর্ণসংখ্যা proc_cmdline_filter_pid1_args(অক্ষর **argv, অক্ষর ***ret);

পূর্ণসংখ্যা proc_cmdline(অক্ষর **ret);
পূর্ণসংখ্যা proc_cmdline_strv(অক্ষর ***ret);

পূর্ণসংখ্যা proc_cmdline_parse(proc_cmdline_parse_t parse, শূন্য *userdata, ProcCmdlineFlags flags);

পূর্ণসংখ্যা proc_cmdline_get_চাবি(ধ্রুবক অক্ষর *চাবি, ProcCmdlineFlags flags, অক্ষর **ret_মান);
পূর্ণসংখ্যা proc_cmdline_get_bool(ধ্রুবক অক্ষর *চাবি, ProcCmdlineFlags flags, bool *ret);

পূর্ণসংখ্যা proc_cmdline_get_চাবি_many_internal(ProcCmdlineFlags flags, ...);
#define proc_cmdline_get_চাবি_many(flags, ...) proc_cmdline_get_চাবি_many_internal(flags, __VA_ARGS__, ফাঁকা)

অক্ষর* proc_cmdline_চাবি_startswith(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *prefix);
bool proc_cmdline_চাবি_streq(ধ্রুবক অক্ষর *x, ধ্রুবক অক্ষর *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_মান_missing(ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান);
