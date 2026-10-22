/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_PREFIX    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_மதிப்பு_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

வகைவரை முழு (*proc_cmdline_parse_t)(மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு, வெறுமை *data);

முழு proc_cmdline_filter_pid1_args(எழுத்து **argv, எழுத்து ***ret);

முழு proc_cmdline(எழுத்து **ret);
முழு proc_cmdline_strv(எழுத்து ***ret);

முழு proc_cmdline_parse(proc_cmdline_parse_t parse, வெறுமை *userdata, ProcCmdlineFlags flags);

முழு proc_cmdline_get_சாவி(மாறிலி எழுத்து *சாவி, ProcCmdlineFlags flags, எழுத்து **ret_மதிப்பு);
முழு proc_cmdline_get_bool(மாறிலி எழுத்து *சாவி, ProcCmdlineFlags flags, bool *ret);

முழு proc_cmdline_get_சாவி_many_internal(ProcCmdlineFlags flags, ...);
#define proc_cmdline_get_சாவி_many(flags, ...) proc_cmdline_get_சாவி_many_internal(flags, __VA_ARGS__, வெற்று)

எழுத்து* proc_cmdline_சாவி_startswith(மாறிலி எழுத்து *s, மாறிலி எழுத்து *prefix);
bool proc_cmdline_சாவி_streq(மாறிலி எழுத்து *x, மாறிலி எழுத்து *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_மதிப்பு_missing(மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு);
