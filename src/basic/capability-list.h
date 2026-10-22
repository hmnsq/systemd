/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

ଅଚଳ ଅକ୍ଷର* capability_to_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id);
ଅଚଳ ଅକ୍ଷର* capability_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ଅକ୍ଷର buf[ସ୍ଥିର CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (ଅକ୍ଷର[CAPABILITY_TO_STRING_MAX]) {})

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_from_name(ଅଚଳ ଅକ୍ଷର *name);
unsigned capability_list_length(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_set_to_string(uint64_t set, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_set_to_string_negative(uint64_t set, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_set_to_strv(uint64_t set, ଅକ୍ଷର ***ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_set_from_string(ଅଚଳ ଅକ୍ଷର *s, uint64_t *ret);
