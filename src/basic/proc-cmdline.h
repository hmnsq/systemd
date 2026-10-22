/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع شمارش ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_PREFIX    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_مقدار_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

تعریفنوع صحیح (*proc_cmdline_parse_t)(ثابت نویسه *کلید, ثابت نویسه *مقدار, تهی *data);

صحیح proc_cmdline_filter_pid1_args(نویسه **argv, نویسه ***ret);

صحیح proc_cmdline(نویسه **ret);
صحیح proc_cmdline_strv(نویسه ***ret);

صحیح proc_cmdline_parse(proc_cmdline_parse_t parse, تهی *userdata, ProcCmdlineFlags flags);

صحیح proc_cmdline_get_کلید(ثابت نویسه *کلید, ProcCmdlineFlags flags, نویسه **ret_مقدار);
صحیح proc_cmdline_get_bool(ثابت نویسه *کلید, ProcCmdlineFlags flags, bool *ret);

صحیح proc_cmdline_get_کلید_many_internal(ProcCmdlineFlags flags, ...);
#define proc_cmdline_get_کلید_many(flags, ...) proc_cmdline_get_کلید_many_internal(flags, __VA_ARGS__, پوچ)

نویسه* proc_cmdline_کلید_startswith(ثابت نویسه *s, ثابت نویسه *prefix);
bool proc_cmdline_کلید_streq(ثابت نویسه *x, ثابت نویسه *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_مقدار_missing(ثابت نویسه *کلید, ثابت نویسه *مقدار);
