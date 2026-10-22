/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "bitfield.h"
#അന്തര്ഭാവ "capability-list.h"
#അന്തര്ഭാവ "capability-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"

സ്ഥിതം സ്ഥിരം ഘടന capability_name* lookup_capability(രജിസ്റ്റർ സ്ഥിരം അക്ഷരം *str, രജിസ്റ്റർ GPERF_LEN_TYPE len);

#അന്തര്ഭാവ "capability-from-name.inc"
#അന്തര്ഭാവ "capability-to-name.inc"

സ്ഥിരം അക്ഷരം* capability_to_name(പൂർണ്ണസംഖ്യ id) {
        എങ്കിൽ (id < 0)
                തിരികെ ശൂന്യ;
        എങ്കിൽ ((unsigned) id >= capability_list_length())
                തിരികെ ശൂന്യ;

        തിരികെ capability_names[id];
}

സ്ഥിരം അക്ഷരം* capability_to_string(പൂർണ്ണസംഖ്യ id, അക്ഷരം buf[സ്ഥിതം CAPABILITY_TO_STRING_MAX]) {
        സ്ഥിരം അക്ഷരം *p;

        എങ്കിൽ (id < 0)
                തിരികെ ശൂന്യ;
        എങ്കിൽ (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                തിരികെ ശൂന്യ;

        p = capability_to_name(id);
        എങ്കിൽ (p)
                തിരികെ p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        തിരികെ buf;
}

പൂർണ്ണസംഖ്യ capability_from_name(സ്ഥിരം അക്ഷരം *name) {
        സ്ഥിരം ഘടന capability_name *sc;
        പൂർണ്ണസംഖ്യ r, i;

        പ്രതിജ്ഞാ(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        എങ്കിൽ (r >= 0) {
                എങ്കിൽ (i < 0 || i > CAP_LIMIT)
                        തിരികെ -EINVAL;

                തിരികെ i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        എങ്കിൽ (!sc)
                തിരികെ -EINVAL;

        തിരികെ sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(ശൂന്യം) {
        തിരികെ MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

പൂർണ്ണസംഖ്യ capability_set_to_string(uint64_t set, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *str = ശൂന്യ;

        പ്രതിജ്ഞാ(ret);

        വേണ്ടി (unsigned i = 0; i <= cap_last_cap(); i++) {
                സ്ഥിരം അക്ഷരം *p;

                എങ്കിൽ (!BIT_SET(set, i))
                        തുടരുക;

                p = CAPABILITY_TO_STRING(i);
                പ്രതിജ്ഞാ(p);

                എങ്കിൽ (!strextend_with_separator(&str, " ", p))
                        തിരികെ -ENOMEM;
        }

        എങ്കിൽ (!str) {
                str = new0(അക്ഷരം, 1);
                എങ്കിൽ (!str)
                        തിരികെ -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ capability_set_to_string_negative(uint64_t set, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *a = ശൂന്യ, *b = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        അല്ലെങ്കിൽ {
                അക്ഷരം *c = strjoin("~", b);
                എങ്കിൽ (!c)
                        തിരികെ -ENOMEM;

                *ret = c;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ capability_set_to_strv(uint64_t set, അക്ഷരം ***ret) {
        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        വേണ്ടി (unsigned i = 0; i <= cap_last_cap(); i++) {
                സ്ഥിരം അക്ഷരം *p;

                എങ്കിൽ (!BIT_SET(set, i))
                        തുടരുക;

                p = CAPABILITY_TO_STRING(i);
                പ്രതിജ്ഞാ(p);

                r = strv_extend(&l, p);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        *ret = TAKE_PTR(l);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ capability_set_from_string(സ്ഥിരം അക്ഷരം *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = s;;) {
                _cleanup_free_ അക്ഷരം *word = ശൂന്യ;
                പൂർണ്ണസംഖ്യ r;

                r = extract_first_word(&p, &word, ശൂന്യ, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                r = capability_from_name(word);
                എങ്കിൽ (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } അല്ലെങ്കിൽ
                        val |= UINT64_C(1) << r;
        }

        എങ്കിൽ (ret)
                *ret = val;

        തിരികെ good;
}
