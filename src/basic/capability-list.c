/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "bitfield.h"
#अन्तर्भाव "capability-list.h"
#अन्तर्भाव "capability-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

स्थिर अचर संरचना capability_name* lookup_capability(पञ्जिका अचर वर्ण *str, पञ्जिका GPERF_LEN_TYPE len);

#अन्तर्भाव "capability-from-name.inc"
#अन्तर्भाव "capability-to-name.inc"

अचर वर्ण* capability_to_name(पूर्णाङ्क id) {
        यदि (id < 0)
                फर्काऊ रिक्त;
        यदि ((अचिन्हित) id >= capability_list_length())
                फर्काऊ रिक्त;

        फर्काऊ capability_names[id];
}

अचर वर्ण* capability_to_string(पूर्णाङ्क id, वर्ण buf[स्थिर CAPABILITY_TO_STRING_MAX]) {
        अचर वर्ण *p;

        यदि (id < 0)
                फर्काऊ रिक्त;
        यदि (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                फर्काऊ रिक्त;

        p = capability_to_name(id);
        यदि (p)
                फर्काऊ p;

        प्र_लिखो(buf, "0x%x", (अचिन्हित) id); /* numerical fallback */
        फर्काऊ buf;
}

पूर्णाङ्क capability_from_name(अचर वर्ण *name) {
        अचर संरचना capability_name *sc;
        पूर्णाङ्क r, i;

        प्रतिज्ञा(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        यदि (r >= 0) {
                यदि (i < 0 || i > CAP_LIMIT)
                        फर्काऊ -EINVAL;

                फर्काऊ i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, म_माप(name));
        यदि (!sc)
                फर्काऊ -EINVAL;

        फर्काऊ sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
अचिन्हित capability_list_length(शून्य) {
        फर्काऊ MIN((अचिन्हित) ELEMENTSOF(capability_names), (अचिन्हित) (CAP_LIMIT + 1));
}

पूर्णाङ्क capability_set_to_string(uint64_t set, वर्ण **ret) {
        _cleanup_free_ वर्ण *str = रिक्त;

        प्रतिज्ञा(ret);

        लागि (अचिन्हित i = 0; i <= cap_last_cap(); i++) {
                अचर वर्ण *p;

                यदि (!BIT_SET(set, i))
                        जारी;

                p = CAPABILITY_TO_STRING(i);
                प्रतिज्ञा(p);

                यदि (!strextend_with_separator(&str, " ", p))
                        फर्काऊ -ENOMEM;
        }

        यदि (!str) {
                str = new0(वर्ण, 1);
                यदि (!str)
                        फर्काऊ -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        फर्काऊ 0;
}

पूर्णाङ्क capability_set_to_string_negative(uint64_t set, वर्ण **ret) {
        _cleanup_free_ वर्ण *a = रिक्त, *b = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        यदि (r < 0)
                फर्काऊ r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        यदि (r < 0)
                फर्काऊ r;

        यदि (म_माप(a) <= 1 + म_माप(b))
                *ret = TAKE_PTR(a);
        अन्यथा {
                वर्ण *c = strjoin("~", b);
                यदि (!c)
                        फर्काऊ -ENOMEM;

                *ret = c;
        }

        फर्काऊ 0;
}

पूर्णाङ्क capability_set_to_strv(uint64_t set, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        लागि (अचिन्हित i = 0; i <= cap_last_cap(); i++) {
                अचर वर्ण *p;

                यदि (!BIT_SET(set, i))
                        जारी;

                p = CAPABILITY_TO_STRING(i);
                प्रतिज्ञा(p);

                r = strv_extend(&l, p);
                यदि (r < 0)
                        फर्काऊ r;
        }

        *ret = TAKE_PTR(l);
        फर्काऊ 0;
}

पूर्णाङ्क capability_set_from_string(अचर वर्ण *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        लागि (अचर वर्ण *p = s;;) {
                _cleanup_free_ वर्ण *word = रिक्त;
                पूर्णाङ्क r;

                r = extract_first_word(&p, &word, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0)
                        रोक;

                r = capability_from_name(word);
                यदि (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } अन्यथा
                        val |= UINT64_C(1) << r;
        }

        यदि (ret)
                *ret = val;

        फर्काऊ good;
}
