/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#تعریف_کرو CAPABILITY_منزل_STRING_MAX (2 + 2 + 1)

ثابت حرف* capability_منزل_name(صحيح id);
ثابت حرف* capability_منزل_string(صحيح id, حرف buf[ساكن CAPABILITY_منزل_STRING_MAX]);
#تعریف_کرو CAPABILITY_منزل_STRING(id) capability_منزل_string(id, (حرف[CAPABILITY_منزل_STRING_MAX]) {})

صحيح capability_from_name(ثابت حرف *name);
بنانشانی capability_list_لمائی(فراغ);

صحيح capability_سیٹ_منزل_string(uint64_t سیٹ, حرف **ret);
صحيح capability_سیٹ_منزل_string_negative(uint64_t سیٹ, حرف **ret);
صحيح capability_سیٹ_منزل_strv(uint64_t سیٹ, حرف ***ret);
صحيح capability_سیٹ_from_string(ثابت حرف *s, uint64_t *ret);
