/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

സ്ഥിരം അക്ഷരം* capability_to_name(പൂർണ്ണസംഖ്യ id);
സ്ഥിരം അക്ഷരം* capability_to_string(പൂർണ്ണസംഖ്യ id, അക്ഷരം buf[സ്ഥിതം CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (അക്ഷരം[CAPABILITY_TO_STRING_MAX]) {})

പൂർണ്ണസംഖ്യ capability_from_name(സ്ഥിരം അക്ഷരം *name);
unsigned capability_list_length(ശൂന്യം);

പൂർണ്ണസംഖ്യ capability_set_to_string(uint64_t set, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ capability_set_to_string_negative(uint64_t set, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ capability_set_to_strv(uint64_t set, അക്ഷരം ***ret);
പൂർണ്ണസംഖ്യ capability_set_from_string(സ്ഥിരം അക്ഷരം *s, uint64_t *ret);
