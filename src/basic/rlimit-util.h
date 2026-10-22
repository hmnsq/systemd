/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <sys/resource.h>       /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ rlimit_from_string_harder(സ്ഥിരം അക്ഷരം *s) _pure_;
ശൂന്യം rlimits_list(സ്ഥിരം അക്ഷരം *prefix);

പൂർണ്ണസംഖ്യ setrlimit_closest(പൂർണ്ണസംഖ്യ resource, സ്ഥിരം ഘടന rlimit *rlim);
പൂർണ്ണസംഖ്യ setrlimit_closest_all(സ്ഥിരം ഘടന rlimit * സ്ഥിരം *rlim, പൂർണ്ണസംഖ്യ *which_failed);

പൂർണ്ണസംഖ്യ rlimit_parse_one(പൂർണ്ണസംഖ്യ resource, സ്ഥിരം അക്ഷരം *val, rlim_t *ret);
പൂർണ്ണസംഖ്യ rlimit_parse(പൂർണ്ണസംഖ്യ resource, സ്ഥിരം അക്ഷരം *val, ഘടന rlimit *ret);

പൂർണ്ണസംഖ്യ rlimit_format(സ്ഥിരം ഘടന rlimit *rl, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ rlimit_copy_all(ഘടന rlimit* target[സ്ഥിതം _RLIMIT_MAX], ഘടന rlimit* സ്ഥിരം source[സ്ഥിതം _RLIMIT_MAX]);
ശൂന്യം rlimit_free_all(ഘടന rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((ഘടന rlimit) { lim, lim })

പൂർണ്ണസംഖ്യ rlimit_nofile_bump(പൂർണ്ണസംഖ്യ limit);
പൂർണ്ണസംഖ്യ rlimit_nofile_safe(ശൂന്യം);

പൂർണ്ണസംഖ്യ pid_getrlimit(pid_t pid, പൂർണ്ണസംഖ്യ resource, ഘടന rlimit *ret);
