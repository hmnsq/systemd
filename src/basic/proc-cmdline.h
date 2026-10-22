/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_PREFIX    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_മൂല്യം_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

തരനിർവചനം പൂർണ്ണസംഖ്യ (*proc_cmdline_parse_t)(സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം, ശൂന്യം *data);

പൂർണ്ണസംഖ്യ proc_cmdline_filter_pid1_args(അക്ഷരം **argv, അക്ഷരം ***ret);

പൂർണ്ണസംഖ്യ proc_cmdline(അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ proc_cmdline_strv(അക്ഷരം ***ret);

പൂർണ്ണസംഖ്യ proc_cmdline_parse(proc_cmdline_parse_t parse, ശൂന്യം *userdata, ProcCmdlineFlags flags);

പൂർണ്ണസംഖ്യ proc_cmdline_get_താക്കോൽ(സ്ഥിരം അക്ഷരം *താക്കോൽ, ProcCmdlineFlags flags, അക്ഷരം **ret_മൂല്യം);
പൂർണ്ണസംഖ്യ proc_cmdline_get_bool(സ്ഥിരം അക്ഷരം *താക്കോൽ, ProcCmdlineFlags flags, bool *ret);

പൂർണ്ണസംഖ്യ proc_cmdline_get_താക്കോൽ_many_internal(ProcCmdlineFlags flags, ...);
#define proc_cmdline_get_താക്കോൽ_many(flags, ...) proc_cmdline_get_താക്കോൽ_many_internal(flags, __VA_ARGS__, ശൂന്യ)

അക്ഷരം* proc_cmdline_താക്കോൽ_startswith(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *prefix);
bool proc_cmdline_താക്കോൽ_streq(സ്ഥിരം അക്ഷരം *x, സ്ഥിരം അക്ഷരം *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_മൂല്യം_missing(സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം);
