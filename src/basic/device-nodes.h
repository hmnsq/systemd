/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ encode_devnode_name(സ്ഥിരം അക്ഷരം *str, അക്ഷരം *str_enc, size_t len);
പൂർണ്ണസംഖ്യ allow_listed_char_for_devnode(അക്ഷരം c, സ്ഥിരം അക്ഷരം *additional);

പൂർണ്ണസംഖ്യ devnode_same(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b);
