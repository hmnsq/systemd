/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

സ്ഥിരം അക്ഷരം* errno_name_no_fallback(പൂർണ്ണസംഖ്യ id) _const_;
പൂർണ്ണസംഖ്യ errno_from_name(സ്ഥിരം അക്ഷരം *name) _pure_;

സ്ഥിതം വരിയിൽ bool errno_is_valid(പൂർണ്ണസംഖ്യ n) {
        തിരികെ n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(പൂർണ്ണസംഖ്യ)
/* Like errno_name, but always returns a string. */
സ്ഥിരം അക്ഷരം* errno_name(പൂർണ്ണസംഖ്യ id, അക്ഷരം buf[സ്ഥിതം ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (അക്ഷരം[ERRNO_NAME_BUF_LEN]){})
