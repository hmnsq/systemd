/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#परिभाषय CAPABILITY_गन्तव्यम्_STRING_MAX (2 + 2 + 1)

ध्रुव वर्ण* capability_गन्तव्यम्_name(पूर्णाङ्क id);
ध्रुव वर्ण* capability_गन्तव्यम्_string(पूर्णाङ्क id, वर्ण buf[स्थिर CAPABILITY_गन्तव्यम्_STRING_MAX]);
#परिभाषय CAPABILITY_गन्तव्यम्_STRING(id) capability_गन्तव्यम्_string(id, (वर्ण[CAPABILITY_गन्तव्यम्_STRING_MAX]) {})

पूर्णाङ्क capability_from_name(ध्रुव वर्ण *name);
अचिह्नित capability_list_दीर्घता(शून्य);

पूर्णाङ्क capability_समुच्चयः_गन्तव्यम्_string(uint64_t समुच्चयः, वर्ण **ret);
पूर्णाङ्क capability_समुच्चयः_गन्तव्यम्_string_negative(uint64_t समुच्चयः, वर्ण **ret);
पूर्णाङ्क capability_समुच्चयः_गन्तव्यम्_strv(uint64_t समुच्चयः, वर्ण ***ret);
पूर्णाङ्क capability_समुच्चयः_from_string(ध्रुव वर्ण *s, uint64_t *ret);
