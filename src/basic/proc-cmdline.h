/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार क्रमागत ProcCmdlineFlags {
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

प्रकार पूर्णांक (*proc_cmdline_parse_t)(स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य, व्योम *तथ्य);

पूर्णांक proc_cmdline_filter_pid1_args(अक्षर **argv, अक्षर ***ret);

पूर्णांक proc_cmdline(अक्षर **ret);
पूर्णांक proc_cmdline_strv(अक्षर ***ret);

पूर्णांक proc_cmdline_parse(proc_cmdline_parse_t parse, व्योम *userdata, ProcCmdlineFlags flags);

पूर्णांक proc_cmdline_get_कुंजी(स्थिर अक्षर *कुंजी, ProcCmdlineFlags flags, अक्षर **ret_मूल्य);
पूर्णांक proc_cmdline_get_bool(स्थिर अक्षर *कुंजी, ProcCmdlineFlags flags, bool *ret);

पूर्णांक proc_cmdline_get_कुंजी_many_internal(ProcCmdlineFlags flags, ...);
#घोषणा proc_cmdline_get_कुंजी_many(flags, ...) proc_cmdline_get_कुंजी_many_internal(flags, __VA_ARGS__, शून्य)

अक्षर* proc_cmdline_कुंजी_startswith(स्थिर अक्षर *s, स्थिर अक्षर *prefix);
bool proc_cmdline_कुंजी_streq(स्थिर अक्षर *x, स्थिर अक्षर *y);

/* A little helper call, to be used in proc_cmdline_parse_t callbacks */
bool proc_cmdline_मूल्य_missing(स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य);
