/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

const var_na* capability_to_name(ཧྲིལ id);
const var_na* capability_to_string(ཧྲིལ id, var_na buf[static CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (var_na[CAPABILITY_TO_STRING_MAX]) {})

ཧྲིལ capability_from_name(const var_na *name);
unsigned capability_list_length(void);

ཧྲིལ capability_set_to_string(uint64_t set, var_na **ret);
ཧྲིལ capability_set_to_string_negative(uint64_t set, var_na **ret);
ཧྲིལ capability_set_to_strv(uint64_t set, var_na ***ret);
ཧྲིལ capability_set_from_string(const var_na *s, uint64_t *ret);
