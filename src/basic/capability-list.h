/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#define CAPABILITY_TO_STRING_MAX (2 + 2 + 1)

ध्रुव वर्ण* capability_to_name(पूर्णाङ्क id);
ध्रुव वर्ण* capability_to_string(पूर्णाङ्क id, वर्ण buf[स्थिर CAPABILITY_TO_STRING_MAX]);
#define CAPABILITY_TO_STRING(id) capability_to_string(id, (वर्ण[CAPABILITY_TO_STRING_MAX]) {})

पूर्णाङ्क capability_from_name(ध्रुव वर्ण *name);
unsigned capability_list_length(शून्य);

पूर्णाङ्क capability_set_to_string(uint64_t set, वर्ण **ret);
पूर्णाङ्क capability_set_to_string_negative(uint64_t set, वर्ण **ret);
पूर्णाङ्क capability_set_to_strv(uint64_t set, वर्ण ***ret);
पूर्णाङ्क capability_set_from_string(ध्रुव वर्ण *s, uint64_t *ret);
