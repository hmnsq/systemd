/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

மாறிலி எழுத்து* capability_to_name(முழு id);
மாறிலி எழுத்து* capability_to_string(முழு id, எழுத்து buf[நிலையான CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (எழுத்து[CAPABILITY_TO_STRING_MAX]) {})

முழு capability_from_name(மாறிலி எழுத்து *name);
unsigned capability_list_length(வெறுமை);

முழு capability_set_to_string(uint64_t set, எழுத்து **ret);
முழு capability_set_to_string_negative(uint64_t set, எழுத்து **ret);
முழு capability_set_to_strv(uint64_t set, எழுத்து ***ret);
முழு capability_set_from_string(மாறிலி எழுத்து *s, uint64_t *ret);
