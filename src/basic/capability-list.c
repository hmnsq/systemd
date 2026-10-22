/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>

#समावेश "alloc-util.h"
#समावेश "bitfield.h"
#समावेश "capability-list.h"
#समावेश "capability-util.h"
#समावेश "extract-word.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"

अटल स्थिर काष्ठा capability_name* lookup_capability(रेजिस्टर स्थिर अक्षर *str, रेजिस्टर GPERF_LEN_TYPE len);

#समावेश "capability-from-name.inc"
#समावेश "capability-to-name.inc"

स्थिर अक्षर* capability_to_name(पूर्णांक id) {
        अगर (id < 0)
                वापस शून्य;
        अगर ((अचिन्हित) id >= capability_list_लंबाई())
                वापस शून्य;

        वापस capability_names[id];
}

स्थिर अक्षर* capability_to_string(पूर्णांक id, अक्षर buf[अटल CAPABILITY_TO_STRING_MAX]) {
        स्थिर अक्षर *p;

        अगर (id < 0)
                वापस शून्य;
        अगर (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                वापस शून्य;

        p = capability_to_name(id);
        अगर (p)
                वापस p;

        प्र_लिखो(buf, "0x%x", (अचिन्हित) id); /* numerical fallback */
        वापस buf;
}

पूर्णांक capability_from_name(स्थिर अक्षर *name) {
        स्थिर काष्ठा capability_name *sc;
        पूर्णांक r, i;

        निश्चित(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        अगर (r >= 0) {
                अगर (i < 0 || i > CAP_LIMIT)
                        वापस -EINVAL;

                वापस i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, म_माप(name));
        अगर (!sc)
                वापस -EINVAL;

        वापस sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
अचिन्हित capability_list_लंबाई(व्योम) {
        वापस MIN((अचिन्हित) ELEMENTSOF(capability_names), (अचिन्हित) (CAP_LIMIT + 1));
}

पूर्णांक capability_set_to_string(uint64_t set, अक्षर **ret) {
        _cleanup_free_ अक्षर *str = शून्य;

        निश्चित(ret);

        क्रम (अचिन्हित i = 0; i <= cap_last_cap(); i++) {
                स्थिर अक्षर *p;

                अगर (!BIT_SET(set, i))
                        जारी;

                p = CAPABILITY_TO_STRING(i);
                निश्चित(p);

                अगर (!strextend_with_separator(&str, " ", p))
                        वापस -ENOMEM;
        }

        अगर (!str) {
                str = new0(अक्षर, 1);
                अगर (!str)
                        वापस -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        वापस 0;
}

पूर्णांक capability_set_to_string_negative(uint64_t set, अक्षर **ret) {
        _cleanup_free_ अक्षर *a = शून्य, *b = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        अगर (r < 0)
                वापस r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        अगर (r < 0)
                वापस r;

        अगर (म_माप(a) <= 1 + म_माप(b))
                *ret = TAKE_PTR(a);
        अन्यथा {
                अक्षर *c = strjoin("~", b);
                अगर (!c)
                        वापस -ENOMEM;

                *ret = c;
        }

        वापस 0;
}

पूर्णांक capability_set_to_strv(uint64_t set, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **l = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        क्रम (अचिन्हित i = 0; i <= cap_last_cap(); i++) {
                स्थिर अक्षर *p;

                अगर (!BIT_SET(set, i))
                        जारी;

                p = CAPABILITY_TO_STRING(i);
                निश्चित(p);

                r = strv_extend(&l, p);
                अगर (r < 0)
                        वापस r;
        }

        *ret = TAKE_PTR(l);
        वापस 0;
}

पूर्णांक capability_set_from_string(स्थिर अक्षर *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        क्रम (स्थिर अक्षर *p = s;;) {
                _cleanup_free_ अक्षर *word = शून्य;
                पूर्णांक r;

                r = extract_first_word(&p, &word, शून्य, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                r = capability_from_name(word);
                अगर (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } अन्यथा
                        val |= UINT64_C(1) << r;
        }

        अगर (ret)
                *ret = val;

        वापस good;
}
