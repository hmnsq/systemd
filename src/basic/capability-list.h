/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#تعریف_کرو CAPABILITY_مقصد_STRING_MAX (2 + 2 + 1)

ثابت حرف* capability_مقصد_name(صحیح id);
ثابت حرف* capability_مقصد_string(صحیح id, حرف buf[ساکن CAPABILITY_مقصد_STRING_MAX]);
#تعریف_کرو CAPABILITY_مقصد_STRING(id) capability_مقصد_string(id, (حرف[CAPABILITY_مقصد_STRING_MAX]) {})

صحیح capability_from_name(ثابت حرف *name);
غیرعلامتی capability_list_لمبائی(خالی);

صحیح capability_سیٹ_مقصد_string(uint64_t سیٹ, حرف **ret);
صحیح capability_سیٹ_مقصد_string_negative(uint64_t سیٹ, حرف **ret);
صحیح capability_سیٹ_مقصد_strv(uint64_t سیٹ, حرف ***ret);
صحیح capability_سیٹ_from_string(ثابت حرف *s, uint64_t *ret);
