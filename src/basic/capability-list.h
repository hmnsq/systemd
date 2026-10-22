/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

ধ্রুবক অক্ষর* capability_to_name(পূর্ণসংখ্যা id);
ধ্রুবক অক্ষর* capability_to_string(পূর্ণসংখ্যা id, অক্ষর buf[স্থির CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (অক্ষর[CAPABILITY_TO_STRING_MAX]) {})

পূর্ণসংখ্যা capability_from_name(ধ্রুবক অক্ষর *name);
unsigned capability_list_দৈর্ঘ্য(শূন্য);

পূর্ণসংখ্যা capability_set_to_string(uint64_t set, অক্ষর **ret);
পূর্ণসংখ্যা capability_set_to_string_negative(uint64_t set, অক্ষর **ret);
পূর্ণসংখ্যা capability_set_to_strv(uint64_t set, অক্ষর ***ret);
পূর্ণসংখ্যা capability_set_from_string(ধ্রুবক অক্ষর *s, uint64_t *ret);
