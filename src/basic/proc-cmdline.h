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
        PROC_CMDLINE_मूल्य_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

प्रकारनिर्देश पूर्णाङ्क (*proc_cmdline_parse_t)(ध्रुव वर्ण *किल्ली, ध्रुव वर्ण *मूल्य, शून्य *data);

पूर्णाङ्क proc_cmdline_filter_pid1_args(वर्ण **argv, वर्ण ***ret);

पूर्णाङ्क proc_cmdline(वर्ण **ret);
पूर्णाङ्क proc_cmdline_strv(वर्ण ***ret);

पूर्णाङ्क proc_cmdline_parse(proc_cmdline_parse_t parse, शून्य *userdata, ProcCmdlineFlags flags);

पूर्णाङ्क proc_cmdline_get_किल्ली(ध्रुव वर्ण *किल्ली, ProcCmdlineFlags flags, वर्ण **ret_मूल्य);
पूर्णाङ्क proc_cmdline_get_bool(ध्रुव वर्ण *किल्ली, ProcCmdlineFlags flags, bool *ret);

पूर्णाङ्क proc_cmdline_get_किल्ली_many_internal(ProcCmdlineFlags flags, ...);
#define proc_cmdline_get_किल्ली_many(flags, ...) proc_cmdline_get_किल्ली_many_internal(flags, __VA_ARGS__, रिक्त)

वर्ण* proc_cmdline_किल्ली_startswith(ध्रुव वर्ण *s, ध्रुव वर्ण *prefix);
bool proc_cmdline_किल्ली_streq(ध्रुव वर्ण *x, ध्रुव वर्ण *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_मूल्य_missing(ध्रुव वर्ण *किल्ली, ध्रुव वर्ण *मूल्य);
