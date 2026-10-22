/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

અચલ અક્ષર* capability_to_name(પૂર્ણાંક id);
અચલ અક્ષર* capability_to_string(પૂર્ણાંક id, અક્ષર buf[સ્થિર CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (અક્ષર[CAPABILITY_TO_STRING_MAX]) {})

પૂર્ણાંક capability_from_name(અચલ અક્ષર *name);
unsigned capability_list_length(શૂન્ય);

પૂર્ણાંક capability_set_to_string(uint64_t set, અક્ષર **ret);
પૂર્ણાંક capability_set_to_string_negative(uint64_t set, અક્ષર **ret);
પૂર્ણાંક capability_set_to_strv(uint64_t set, અક્ષર ***ret);
પૂર્ણાંક capability_set_from_string(અચલ અક્ષર *s, uint64_t *ret);
