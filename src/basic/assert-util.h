/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(ശൂന്യം) _weak_ _pure_;

ശൂന്യം log_assert_failed_return(സ്ഥിരം അക്ഷരം *text, സ്ഥിരം അക്ഷരം *file, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *func);

#define assert_log(വ്യഞ്ജകം)                                                \
        (_likely_(വ്യഞ്ജകം) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(വ്യഞ്ജകം, r)                                          \
        ചെയ്യുക {                                                            \
                എങ്കിൽ (!assert_log(വ്യഞ്ജകം))                                  \
                        തിരികെ (r);                                     \
        } ആകുമ്പോൾ (false)

#define assert_return_errno(വ്യഞ്ജകം, r, err)                               \
        ചെയ്യുക {                                                            \
                എങ്കിൽ (!assert_log(വ്യഞ്ജകം)) {                                \
                        errno = err;                                    \
                        തിരികെ (r);                                     \
                }                                                       \
        } ആകുമ്പോൾ (false)
