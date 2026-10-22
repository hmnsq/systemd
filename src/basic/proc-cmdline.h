/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_PREFIX    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_मान_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

प्रकारनिर्देश पूर्णाङ्क (*proc_cmdline_parse_t)(अचर वर्ण *साँचो, अचर वर्ण *मान, शून्य *data);

पूर्णाङ्क proc_cmdline_filter_pid1_args(वर्ण **argv, वर्ण ***ret);

पूर्णाङ्क proc_cmdline(वर्ण **ret);
पूर्णाङ्क proc_cmdline_strv(वर्ण ***ret);

पूर्णाङ्क proc_cmdline_parse(proc_cmdline_parse_t parse, शून्य *userdata, ProcCmdlineFlags flags);

पूर्णाङ्क proc_cmdline_get_साँचो(अचर वर्ण *साँचो, ProcCmdlineFlags flags, वर्ण **ret_मान);
पूर्णाङ्क proc_cmdline_get_bool(अचर वर्ण *साँचो, ProcCmdlineFlags flags, bool *ret);

पूर्णाङ्क proc_cmdline_get_साँचो_many_internal(ProcCmdlineFlags flags, ...);
#define proc_cmdline_get_साँचो_many(flags, ...) proc_cmdline_get_साँचो_many_internal(flags, __VA_ARGS__, रिक्त)

वर्ण* proc_cmdline_साँचो_startswith(अचर वर्ण *s, अचर वर्ण *prefix);
bool proc_cmdline_साँचो_streq(अचर वर्ण *x, अचर वर्ण *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_मान_missing(अचर वर्ण *साँचो, अचर वर्ण *मान);
