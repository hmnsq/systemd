/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

ثابت نویسه* capability_to_name(صحیح id);
ثابت نویسه* capability_to_string(صحیح id, نویسه buf[ایستا CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (نویسه[CAPABILITY_TO_STRING_MAX]) {})

صحیح capability_from_name(ثابت نویسه *name);
unsigned capability_list_length(تهی);

صحیح capability_set_to_string(uint64_t set, نویسه **ret);
صحیح capability_set_to_string_negative(uint64_t set, نویسه **ret);
صحیح capability_set_to_strv(uint64_t set, نویسه ***ret);
صحیح capability_set_from_string(ثابت نویسه *s, uint64_t *ret);
