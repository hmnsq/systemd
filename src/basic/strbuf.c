/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <string.h>

#ضمن "alloc-util.h"
#ضمن "sort-util.h"
#ضمن "strbuf.h"

/*
 * Strbuf stores given strings in a single continuous allocated memory
 * area. Identical strings are de-duplicated and return the same offset
 * as the first string stored. If the tail of a string already exists
 * in the buffer, the tail is returned.
 *
 * A trie (http://en.wikipedia.org/wiki/Trie) is used to maintain the
 * information about the stored strings.
 *
 * Example of udev rules:
 *   $ ./udevadm test .
 *   ...
 *   read rules file: /usr/lib/udev/rules.d/99-systemd.rules
 *   rules contain 196608 bytes tokens (16384 * 12 bytes), 39742 bytes strings
 *   23939 strings (207859 bytes), 20404 de-duplicated (171653 bytes), 3536 trie nodes used
 *   ...
 */

بنية strbuf* strbuf_new(فراغ) {
        _cleanup_(strbuf_freep) بنية strbuf *str = عدم;

        str = new(بنية strbuf, 1);
        إذا (!str)
                أرجع عدم;

        *str = (بنية strbuf) {
                .buf = new0(حرف, 1),
                .پاڙ = new0(بنية strbuf_ڳنڍ, 1),
                .len = 1,
                .nodes_ڳڻپ = 1,
        };
        إذا (!str->buf || !str->پاڙ)
                أرجع عدم;

        أرجع TAKE_PTR(str);
}

ساكن بنية strbuf_ڳنڍ* strbuf_ڳنڍ_cleanup(بنية strbuf_ڳنڍ *ڳنڍ) {
        أكد(ڳنڍ);

        FOREACH_ARRAY(ٻار, ڳنڍ->children, ڳنڍ->children_ڳڻپ)
                strbuf_ڳنڍ_cleanup(ٻار->ٻار);

        حرر(ڳنڍ->children);
        أرجع mfree(ڳنڍ);
}

/* clean up trie data, leave only the string buffer */
فراغ strbuf_complete(بنية strbuf *str) {
        إذا (!str || !str->پاڙ)
                أرجع;

        str->پاڙ = strbuf_ڳنڍ_cleanup(str->پاڙ);
}

/* clean up everything */
بنية strbuf* strbuf_free(بنية strbuf *str) {
        إذا (!str)
                أرجع عدم;

        strbuf_complete(str);
        حرر(str->buf);
        أرجع mfree(str);
}

ساكن صحيح strbuf_children_cmp(ثابت بنية strbuf_ٻار_داخلا *n1, ثابت بنية strbuf_ٻار_داخلا *n2) {
        أكد(n1);
        أكد(n2);

        أرجع CMP(n1->c, n2->c);
}

ساكن فراغ bubbleinsert(بنية strbuf_ڳنڍ *ڳنڍ,
                         uint8_t c,
                         بنية strbuf_ڳنڍ *ڳنڍ_ٻار) {

        بنية strbuf_ٻار_داخلا new = {
                .c = c,
                .ٻار = ڳنڍ_ٻار,
        };
        صحيح کاٻو = 0, ساڄو = ASSERT_PTR(ڳنڍ)->children_ڳڻپ;

        طالما (ساڄو > کاٻو) {
                صحيح وچ = (ساڄو + کاٻو) / 2 ;
                إذا (strbuf_children_cmp(&ڳنڍ->children[وچ], &new) <= 0)
                        کاٻو = وچ + 1;
                وإلا
                        ساڄو = وچ;
        }

        memmove(ڳنڍ->children + کاٻو + 1, ڳنڍ->children + کاٻو,
                حجم(بنية strbuf_ٻار_داخلا) * (ڳنڍ->children_ڳڻپ - کاٻو));
        ڳنڍ->children[کاٻو] = new;

        ڳنڍ->children_ڳڻپ++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_جوڙيو_string_ڀريل(بنية strbuf *str, ثابت حرف *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        إذا (len == SIZE_MAX)
                len = لڙي_ڊيگهه(s);

        إذا (!str->پاڙ)
                أرجع -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->اندريون_ڳڻپ++;
        إذا (len == 0) {
                str->dedup_ڳڻپ++;
                أرجع 0;
        }
        str->اندريون_len += len;

        بنية strbuf_ڳنڍ *ڳنڍ = str->پاڙ;
        لكل (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = ڳنڍ->قدر_off + ڳنڍ->قدر_len - len;
                إذا (depth == len || (ڳنڍ->قدر_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_ڳڻپ++;
                        أرجع off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                بنية strbuf_ٻار_داخلا *ٻار, ڳولا = { .c = c };
                ٻار = typesafe_bsearch(&ڳولا, ڳنڍ->children, ڳنڍ->children_ڳڻپ, strbuf_children_cmp);
                إذا (!ٻار)
                        اكسر;
                ڳنڍ = ٻار->ٻار;
        }

        /* add new string */
        إذا (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                أرجع -ENOMEM;
        off = str->len;
        يادگيري_نقل(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ بنية strbuf_ڳنڍ *ڳنڍ_ٻار = عدم;

        ڳنڍ_ٻار = new(بنية strbuf_ڳنڍ, 1);
        إذا (!ڳنڍ_ٻار)
                أرجع -ENOMEM;
        *ڳنڍ_ٻار = (بنية strbuf_ڳنڍ) {
                .قدر_off = off,
                .قدر_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        إذا (!GREEDY_REALLOC(ڳنڍ->children, ڳنڍ->children_ڳڻپ + 1))
                أرجع -ENOMEM;

        str->nodes_ڳڻپ++;

        bubbleinsert(ڳنڍ, c, TAKE_PTR(ڳنڍ_ٻار));

        أرجع off;
}
