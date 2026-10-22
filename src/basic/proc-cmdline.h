/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع تعداد ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_اڳياڙي    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_قدر_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

تعريفنوع صحيح (*proc_cmdline_parse_t)(ثابت حرف *ڪنجي, ثابت حرف *قدر, فراغ *معلومات);

صحيح proc_cmdline_filter_pid1_دليلون(حرف **argv, حرف ***ret);

صحيح proc_cmdline(حرف **ret);
صحيح proc_cmdline_strv(حرف ***ret);

صحيح proc_cmdline_parse(proc_cmdline_parse_t parse, فراغ *userdata, ProcCmdlineFlags flags);

صحيح proc_cmdline_get_ڪنجي(ثابت حرف *ڪنجي, ProcCmdlineFlags flags, حرف **ret_قدر);
صحيح proc_cmdline_get_bool(ثابت حرف *ڪنجي, ProcCmdlineFlags flags, bool *ret);

صحيح proc_cmdline_get_ڪنجي_many_اندروني(ProcCmdlineFlags flags, ...);
#وصف_ڪريو proc_cmdline_get_ڪنجي_many(flags, ...) proc_cmdline_get_ڪنجي_many_اندروني(flags, __VA_ARGS__, عدم)

حرف* proc_cmdline_ڪنجي_startswith(ثابت حرف *s, ثابت حرف *اڳياڙي);
bool proc_cmdline_ڪنجي_streq(ثابت حرف *x, ثابت حرف *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_قدر_missing(ثابت حرف *ڪنجي, ثابت حرف *قدر);
