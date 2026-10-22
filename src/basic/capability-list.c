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

स्थिर स्थिरांक संरचना capability_name* lookup_capability(नोंदणी स्थिरांक अक्षर *str, नोंदणी GPERF_LEN_TYPE len);

#अन्तर्भाव "capability-from-name.inc"
#अन्तर्भाव "capability-to-name.inc"

स्थिरांक अक्षर* capability_to_name(पूर्णांक id) {
        जर (id < 0)
                परत रिक्त;
        जर ((अचिन्हित) id >= capability_list_length())
                परत रिक्त;

        परत capability_names[id];
}

स्थिरांक अक्षर* capability_to_string(पूर्णांक id, अक्षर buf[स्थिर CAPABILITY_TO_STRING_MAX]) {
        स्थिरांक अक्षर *p;

        जर (id < 0)
                परत रिक्त;
        जर (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                परत रिक्त;

        p = capability_to_name(id);
        जर (p)
                परत p;

        प्र_लिखो(buf, "0x%x", (अचिन्हित) id); /* numerical fallback */
        परत buf;
}

पूर्णांक capability_from_name(स्थिरांक अक्षर *name) {
        स्थिरांक संरचना capability_name *sc;
        पूर्णांक r, i;

        प्रतिज्ञा(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        जर (r >= 0) {
                जर (i < 0 || i > CAP_LIMIT)
                        परत -EINVAL;

                परत i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, म_माप(name));
        जर (!sc)
                परत -EINVAL;

        परत sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
अचिन्हित capability_list_length(शून्य) {
        परत MIN((अचिन्हित) ELEMENTSOF(capability_names), (अचिन्हित) (CAP_LIMIT + 1));
}

पूर्णांक capability_set_to_string(uint64_t set, अक्षर **ret) {
        _cleanup_free_ अक्षर *str = रिक्त;

        प्रतिज्ञा(ret);

        साठी (अचिन्हित i = 0; i <= cap_last_cap(); i++) {
                स्थिरांक अक्षर *p;

                जर (!BIT_SET(set, i))
                        सुरूठेवा;

                p = CAPABILITY_TO_STRING(i);
                प्रतिज्ञा(p);

                जर (!strextend_with_separator(&str, " ", p))
                        परत -ENOMEM;
        }

        जर (!str) {
                str = new0(अक्षर, 1);
                जर (!str)
                        परत -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        परत 0;
}

पूर्णांक capability_set_to_string_negative(uint64_t set, अक्षर **ret) {
        _cleanup_free_ अक्षर *a = रिक्त, *b = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        जर (r < 0)
                परत r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        जर (r < 0)
                परत r;

        जर (म_माप(a) <= 1 + म_माप(b))
                *ret = TAKE_PTR(a);
        अन्यथा {
                अक्षर *c = strjoin("~", b);
                जर (!c)
                        परत -ENOMEM;

                *ret = c;
        }

        परत 0;
}

पूर्णांक capability_set_to_strv(uint64_t set, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **l = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        साठी (अचिन्हित i = 0; i <= cap_last_cap(); i++) {
                स्थिरांक अक्षर *p;

                जर (!BIT_SET(set, i))
                        सुरूठेवा;

                p = CAPABILITY_TO_STRING(i);
                प्रतिज्ञा(p);

                r = strv_extend(&l, p);
                जर (r < 0)
                        परत r;
        }

        *ret = TAKE_PTR(l);
        परत 0;
}

पूर्णांक capability_set_from_string(स्थिरांक अक्षर *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        साठी (स्थिरांक अक्षर *p = s;;) {
                _cleanup_free_ अक्षर *word = रिक्त;
                पूर्णांक r;

                r = extract_first_word(&p, &word, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        थांबा;

                r = capability_from_name(word);
                जर (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } अन्यथा
                        val |= UINT64_C(1) << r;
        }

        जर (ret)
                *ret = val;

        परत good;
}
