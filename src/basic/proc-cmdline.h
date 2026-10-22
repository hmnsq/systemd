/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

تعريفنوع تعداد ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_پیشوند    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_مقدار_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

تعريفنوع صحيح (*proc_cmdline_parse_t)(ثابت حرف *کلید, ثابت حرف *مقدار, فراغ *داده);

صحيح proc_cmdline_filter_pid1_آرگومانها(حرف **argv, حرف ***ret);

صحيح proc_cmdline(حرف **ret);
صحيح proc_cmdline_strv(حرف ***ret);

صحيح proc_cmdline_parse(proc_cmdline_parse_t parse, فراغ *userdata, ProcCmdlineFlags flags);

صحيح proc_cmdline_get_کلید(ثابت حرف *کلید, ProcCmdlineFlags flags, حرف **ret_مقدار);
صحيح proc_cmdline_get_bool(ثابت حرف *کلید, ProcCmdlineFlags flags, bool *ret);

صحيح proc_cmdline_get_کلید_many_داخلی(ProcCmdlineFlags flags, ...);
#تعریف_کن proc_cmdline_get_کلید_many(flags, ...) proc_cmdline_get_کلید_many_داخلی(flags, __VA_ARGS__, عدم)

حرف* proc_cmdline_کلید_startswith(ثابت حرف *s, ثابت حرف *پیشوند);
bool proc_cmdline_کلید_streq(ثابت حرف *x, ثابت حرف *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_مقدار_missing(ثابت حرف *کلید, ثابت حرف *مقدار);
