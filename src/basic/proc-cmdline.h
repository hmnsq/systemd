/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા ગણના ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_PREFIX    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_કિંમત_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

પ્રકારવ્યાખ્યા પૂર્ણાંક (*proc_cmdline_parse_t)(અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત, શૂન્ય *data);

પૂર્ણાંક proc_cmdline_filter_pid1_args(અક્ષર **argv, અક્ષર ***ret);

પૂર્ણાંક proc_cmdline(અક્ષર **ret);
પૂર્ણાંક proc_cmdline_strv(અક્ષર ***ret);

પૂર્ણાંક proc_cmdline_parse(proc_cmdline_parse_t parse, શૂન્ય *userdata, ProcCmdlineFlags flags);

પૂર્ણાંક proc_cmdline_get_ચાવી(અચલ અક્ષર *ચાવી, ProcCmdlineFlags flags, અક્ષર **ret_કિંમત);
પૂર્ણાંક proc_cmdline_get_bool(અચલ અક્ષર *ચાવી, ProcCmdlineFlags flags, bool *ret);

પૂર્ણાંક proc_cmdline_get_ચાવી_many_internal(ProcCmdlineFlags flags, ...);
#define proc_cmdline_get_ચાવી_many(flags, ...) proc_cmdline_get_ચાવી_many_internal(flags, __VA_ARGS__, ખાલી)

અક્ષર* proc_cmdline_ચાવી_startswith(અચલ અક્ષર *s, અચલ અક્ષર *prefix);
bool proc_cmdline_ચાવી_streq(અચલ અક્ષર *x, અચલ અક્ષર *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_કિંમત_missing(અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત);
