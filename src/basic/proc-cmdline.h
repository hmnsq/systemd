/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف شمار ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_سابقہ    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_قدر_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

قسمتعریف صحیح (*proc_cmdline_parse_t)(ثابت حرف *کلید, ثابت حرف *قدر, خالی *ڈیٹا);

صحیح proc_cmdline_filter_pid1_دلائل(حرف **argv, حرف ***ret);

صحیح proc_cmdline(حرف **ret);
صحیح proc_cmdline_strv(حرف ***ret);

صحیح proc_cmdline_parse(proc_cmdline_parse_t parse, خالی *userdata, ProcCmdlineFlags flags);

صحیح proc_cmdline_get_کلید(ثابت حرف *کلید, ProcCmdlineFlags flags, حرف **ret_قدر);
صحیح proc_cmdline_get_bool(ثابت حرف *کلید, ProcCmdlineFlags flags, bool *ret);

صحیح proc_cmdline_get_کلید_many_اندرونی(ProcCmdlineFlags flags, ...);
#تعریف_کرو proc_cmdline_get_کلید_many(flags, ...) proc_cmdline_get_کلید_many_اندرونی(flags, __VA_ARGS__, تہی)

حرف* proc_cmdline_کلید_startswith(ثابت حرف *s, ثابت حرف *سابقہ);
bool proc_cmdline_کلید_streq(ثابت حرف *x, ثابت حرف *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_قدر_missing(ثابت حرف *کلید, ثابت حرف *قدر);
