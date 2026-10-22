/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <glob.h>       /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം DIR* (*opendir_t)(സ്ഥിരം അക്ഷരം *);

പൂർണ്ണസംഖ്യ safe_glob_full(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, opendir_t opendir_func, അക്ഷരം ***ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_glob(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, അക്ഷരം ***ret) {
        തിരികെ safe_glob_full(path, flags, ശൂന്യ, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
പൂർണ്ണസംഖ്യ glob_first(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret);
#define glob_exists(path) glob_first(path, ശൂന്യ)
പൂർണ്ണസംഖ്യ glob_extend(അക്ഷരം ***strv, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags);

പൂർണ്ണസംഖ്യ glob_non_glob_prefix(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret);

bool string_is_glob(സ്ഥിരം അക്ഷരം *p) _pure_;
