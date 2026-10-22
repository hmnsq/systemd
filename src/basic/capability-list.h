/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

ثابت حرف* capability_to_name(صحيح id);
ثابت حرف* capability_to_string(صحيح id, حرف buf[ساكن CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (حرف[CAPABILITY_TO_STRING_MAX]) {})

صحيح capability_from_name(ثابت حرف *name);
unsigned capability_list_length(فراغ);

صحيح capability_set_to_string(uint64_t set, حرف **ret);
صحيح capability_set_to_string_negative(uint64_t set, حرف **ret);
صحيح capability_set_to_strv(uint64_t set, حرف ***ret);
صحيح capability_set_from_string(ثابت حرف *s, uint64_t *ret);
