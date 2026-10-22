/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన ProcCmdlineFlags {
        PROC_CMDLINE_RD_STRICT          = 1 << 0, /* Only look at options with the "rd." prefix when in the initrd and only
                                                   * at options without the prefix when not in the initrd.
                                                   */
        PROC_CMDLINE_STRIP_RD_ఉపసర్గ    = 1 << 1, /* Automatically strip "rd." prefix if we are in the initrd.
                                                   * When this is specified, the handler function must check for unprefixed
                                                   * option names. */
        PROC_CMDLINE_విలువ_OPTIONAL     = 1 << 2, /* The value is optional (for boolean switches that can omit the value). */
        PROC_CMDLINE_TRUE_WHEN_MISSING  = 1 << 3, /* Make proc_cmdline_get_bool() return true instead of false (the default)
                                                   * when the key is not present on the command line. */
} ProcCmdlineFlags;

రకనిర్వచనం పూర్ణాంకం (*proc_cmdline_parse_t)(స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ, శూన్య *దత్తాంశం);

పూర్ణాంకం proc_cmdline_filter_pid1_వాదనలు(అక్షరం **argv, అక్షరం ***ret);

పూర్ణాంకం proc_cmdline(అక్షరం **ret);
పూర్ణాంకం proc_cmdline_strv(అక్షరం ***ret);

పూర్ణాంకం proc_cmdline_parse(proc_cmdline_parse_t parse, శూన్య *userdata, ProcCmdlineFlags flags);

పూర్ణాంకం proc_cmdline_get_కీలకం(స్థిరరాశి అక్షరం *కీలకం, ProcCmdlineFlags flags, అక్షరం **ret_విలువ);
పూర్ణాంకం proc_cmdline_get_bool(స్థిరరాశి అక్షరం *కీలకం, ProcCmdlineFlags flags, bool *ret);

పూర్ణాంకం proc_cmdline_get_కీలకం_many_అంతర్గత(ProcCmdlineFlags flags, ...);
#నిర్వచించు proc_cmdline_get_కీలకం_many(flags, ...) proc_cmdline_get_కీలకం_many_అంతర్గత(flags, __VA_ARGS__, ఖాళీ)

అక్షరం* proc_cmdline_కీలకం_startswith(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *ఉపసర్గ);
bool proc_cmdline_కీలకం_streq(స్థిరరాశి అక్షరం *x, స్థిరరాశి అక్షరం *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_విలువ_missing(స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ);
