/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

ধ্রুব বর্ণ* capability_to_name(পূর্ণাঙ্ক id);
ধ্রুব বর্ণ* capability_to_string(পূর্ণাঙ্ক id, বর্ণ buf[স্থির CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (বর্ণ[CAPABILITY_TO_STRING_MAX]) {})

পূর্ণাঙ্ক capability_from_name(ধ্রুব বর্ণ *name);
unsigned capability_list_length(শূন্য);

পূর্ণাঙ্ক capability_set_to_string(uint64_t set, বর্ণ **ret);
পূর্ণাঙ্ক capability_set_to_string_negative(uint64_t set, বর্ণ **ret);
পূর্ণাঙ্ক capability_set_to_strv(uint64_t set, বর্ণ ***ret);
পূর্ণাঙ্ক capability_set_from_string(ধ্রুব বর্ণ *s, uint64_t *ret);
