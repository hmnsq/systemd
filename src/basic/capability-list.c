/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bitfield.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capability-list.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capability-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

ਸਥਿਰ ਅਚਲ ਬਣਤਰ capability_name* lookup_capability(ਰਜਿਸਟਰ ਅਚਲ ਅੱਖਰ *str, ਰਜਿਸਟਰ GPERF_LEN_TYPE len);

#ਅਨ੍ਤਰ੍ਭਾਵ "capability-from-name.inc"
#ਅਨ੍ਤਰ੍ਭਾਵ "capability-to-name.inc"

ਅਚਲ ਅੱਖਰ* capability_ਮੰਜ਼ਿਲ_name(ਪੂਰਨ id) {
        ਜੇ (id < 0)
                ਵਾਪਸ ਨਿਰਾ;
        ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ) id >= capability_list_ਲੰਮਾਈ())
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ capability_ਨਾਂ[id];
}

ਅਚਲ ਅੱਖਰ* capability_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ id, ਅੱਖਰ buf[ਸਥਿਰ CAPABILITY_ਮੰਜ਼ਿਲ_STRING_MAX]) {
        ਅਚਲ ਅੱਖਰ *p;

        ਜੇ (id < 0)
                ਵਾਪਸ ਨਿਰਾ;
        ਜੇ (id > CAP_ਹੱਦਬੰਦੀ) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ਵਾਪਸ ਨਿਰਾ;

        p = capability_ਮੰਜ਼ਿਲ_name(id);
        ਜੇ (p)
                ਵਾਪਸ p;

        sprintf(buf, "0x%x", (ਬਿਨਾਨਿਸ਼ਾਨੀ) id); /* numerical fallback */
        ਵਾਪਸ buf;
}

ਪੂਰਨ capability_from_name(ਅਚਲ ਅੱਖਰ *name) {
        ਅਚਲ ਬਣਤਰ capability_name *sc;
        ਪੂਰਨ r, i;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        /* Try to parse numeric capability */
        r = ਸੁਰੱਖਿਅਤ_atoi(name, &i);
        ਜੇ (r >= 0) {
                ਜੇ (i < 0 || i > CAP_ਹੱਦਬੰਦੀ)
                        ਵਾਪਸ -EINVAL;

                ਵਾਪਸ i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, ਲੜੀ_ਲੰਬਾਈ(name));
        ਜੇ (!sc)
                ਵਾਪਸ -EINVAL;

        ਵਾਪਸ sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
ਬਿਨਾਨਿਸ਼ਾਨੀ capability_list_ਲੰਮਾਈ(ਖਾਲੀ) {
        ਵਾਪਸ MIN((ਬਿਨਾਨਿਸ਼ਾਨੀ) ELEMENTSOF(capability_ਨਾਂ), (ਬਿਨਾਨਿਸ਼ਾਨੀ) (CAP_ਹੱਦਬੰਦੀ + 1));
}

ਪੂਰਨ capability_ਸੈੱਟ_ਮੰਜ਼ਿਲ_string(uint64_t ਸੈੱਟ, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *str = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i <= cap_ਆਖਰੀ_cap(); i++) {
                ਅਚਲ ਅੱਖਰ *p;

                ਜੇ (!ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(ਸੈੱਟ, i))
                        ਜਾਰੀ;

                p = CAPABILITY_ਮੰਜ਼ਿਲ_STRING(i);
                ਪ੍ਰਤਿਜ੍ਞਾ(p);

                ਜੇ (!strextend_with_separator(&str, " ", p))
                        ਵਾਪਸ -ENOMEM;
        }

        ਜੇ (!str) {
                str = new0(ਅੱਖਰ, 1);
                ਜੇ (!str)
                        ਵਾਪਸ -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ਵਾਪਸ 0;
}

ਪੂਰਨ capability_ਸੈੱਟ_ਮੰਜ਼ਿਲ_string_negative(uint64_t ਸੈੱਟ, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *a = ਨਿਰਾ, *b = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_ਸੈੱਟ_ਮੰਜ਼ਿਲ_string(ਸੈੱਟ, &a);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = capability_ਸੈੱਟ_ਮੰਜ਼ਿਲ_string(~ਸੈੱਟ & all_capabilities(), &b);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(a) <= 1 + ਲੜੀ_ਲੰਬਾਈ(b))
                *ret = TAKE_PTR(a);
        ਨਹੀਂਤਾਂ {
                ਅੱਖਰ *c = strjoin("~", b);
                ਜੇ (!c)
                        ਵਾਪਸ -ENOMEM;

                *ret = c;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ capability_ਸੈੱਟ_ਮੰਜ਼ਿਲ_strv(uint64_t ਸੈੱਟ, ਅੱਖਰ ***ret) {
        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i <= cap_ਆਖਰੀ_cap(); i++) {
                ਅਚਲ ਅੱਖਰ *p;

                ਜੇ (!ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(ਸੈੱਟ, i))
                        ਜਾਰੀ;

                p = CAPABILITY_ਮੰਜ਼ਿਲ_STRING(i);
                ਪ੍ਰਤਿਜ੍ਞਾ(p);

                r = strv_extend(&l, p);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        *ret = TAKE_PTR(l);
        ਵਾਪਸ 0;
}

ਪੂਰਨ capability_ਸੈੱਟ_from_string(ਅਚਲ ਅੱਖਰ *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = s;;) {
                _cleanup_free_ ਅੱਖਰ *ਸ਼ਬਦ = ਨਿਰਾ;
                ਪੂਰਨ r;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&p, &ਸ਼ਬਦ, ਨਿਰਾ, ਕੱਢੋ_UNQUOTE|ਕੱਢੋ_RELAX);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                r = capability_from_name(ਸ਼ਬਦ);
                ਜੇ (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", ਸ਼ਬਦ);
                        good = false;
                } ਨਹੀਂਤਾਂ
                        val |= UINT64_C(1) << r;
        }

        ਜੇ (ret)
                *ret = val;

        ਵਾਪਸ good;
}
