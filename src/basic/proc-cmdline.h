/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_PREFIX    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_ମୂଲ୍ୟ_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*proc_cmdline_parse_t)(ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, ଶୂନ୍ୟ *data);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_filter_pid1_args(ଅକ୍ଷର **argv, ଅକ୍ଷର ***ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline(ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_strv(ଅକ୍ଷର ***ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_parse(proc_cmdline_parse_t parse, ଶୂନ୍ୟ *userdata, ProcCmdlineFlags flags);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_get_ଚାବି(ଅଚଳ ଅକ୍ଷର *ଚାବି, ProcCmdlineFlags flags, ଅକ୍ଷର **ret_ମୂଲ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_get_bool(ଅଚଳ ଅକ୍ଷର *ଚାବି, ProcCmdlineFlags flags, bool *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_get_ଚାବି_many_internal(ProcCmdlineFlags flags, ...);
#define proc_cmdline_get_ଚାବି_many(flags, ...) proc_cmdline_get_ଚାବି_many_internal(flags, __VA_ARGS__, ଖାଲି)

ଅକ୍ଷର* proc_cmdline_ଚାବି_startswith(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *prefix);
bool proc_cmdline_ଚାବି_streq(ଅଚଳ ଅକ୍ଷର *x, ଅଚଳ ଅକ୍ଷର *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_ମୂଲ୍ୟ_missing(ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);
