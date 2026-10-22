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

स्थिर ध्रुव संरचना capability_name* lookup_capability(पञ्जिका ध्रुव वर्ण *str, पञ्जिका GPERF_LEN_TYPE len);

#अन्तर्भाव "capability-from-name.inc"
#अन्तर्भाव "capability-to-name.inc"

ध्रुव वर्ण* capability_to_name(पूर्णाङ्क id) {
        सचे (id < 0)
                पटिनिवत्त रिक्त;
        सचे ((unsigned) id >= capability_list_length())
                पटिनिवत्त रिक्त;

        पटिनिवत्त capability_names[id];
}

ध्रुव वर्ण* capability_to_string(पूर्णाङ्क id, वर्ण buf[स्थिर CAPABILITY_TO_STRING_MAX]) {
        ध्रुव वर्ण *p;

        सचे (id < 0)
                पटिनिवत्त रिक्त;
        सचे (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                पटिनिवत्त रिक्त;

        p = capability_to_name(id);
        सचे (p)
                पटिनिवत्त p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        पटिनिवत्त buf;
}

पूर्णाङ्क capability_from_name(ध्रुव वर्ण *name) {
        ध्रुव संरचना capability_name *sc;
        पूर्णाङ्क r, i;

        प्रतिज्ञा(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        सचे (r >= 0) {
                सचे (i < 0 || i > CAP_LIMIT)
                        पटिनिवत्त -EINVAL;

                पटिनिवत्त i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        सचे (!sc)
                पटिनिवत्त -EINVAL;

        पटिनिवत्त sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(शून्य) {
        पटिनिवत्त MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

पूर्णाङ्क capability_set_to_string(uint64_t set, वर्ण **ret) {
        _cleanup_free_ वर्ण *str = रिक्त;

        प्रतिज्ञा(ret);

        अत्थाय (unsigned i = 0; i <= cap_last_cap(); i++) {
                ध्रुव वर्ण *p;

                सचे (!BIT_SET(set, i))
                        अनुवर्त;

                p = CAPABILITY_TO_STRING(i);
                प्रतिज्ञा(p);

                सचे (!strextend_with_separator(&str, " ", p))
                        पटिनिवत्त -ENOMEM;
        }

        सचे (!str) {
                str = new0(वर्ण, 1);
                सचे (!str)
                        पटिनिवत्त -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        पटिनिवत्त 0;
}

पूर्णाङ्क capability_set_to_string_negative(uint64_t set, वर्ण **ret) {
        _cleanup_free_ वर्ण *a = रिक्त, *b = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        सचे (r < 0)
                पटिनिवत्त r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        अञ्ञथा {
                वर्ण *c = strjoin("~", b);
                सचे (!c)
                        पटिनिवत्त -ENOMEM;

                *ret = c;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क capability_set_to_strv(uint64_t set, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        अत्थाय (unsigned i = 0; i <= cap_last_cap(); i++) {
                ध्रुव वर्ण *p;

                सचे (!BIT_SET(set, i))
                        अनुवर्त;

                p = CAPABILITY_TO_STRING(i);
                प्रतिज्ञा(p);

                r = strv_extend(&l, p);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        *ret = TAKE_PTR(l);
        पटिनिवत्त 0;
}

पूर्णाङ्क capability_set_from_string(ध्रुव वर्ण *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        अत्थाय (ध्रुव वर्ण *p = s;;) {
                _cleanup_free_ वर्ण *word = रिक्त;
                पूर्णाङ्क r;

                r = extract_first_word(&p, &word, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        भङ्ग;

                r = capability_from_name(word);
                सचे (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } अञ्ञथा
                        val |= UINT64_C(1) << r;
        }

        सचे (ret)
                *ret = val;

        पटिनिवत्त good;
}
