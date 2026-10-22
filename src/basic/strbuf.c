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
                .سيو = new0(بنية strbuf_كلي, 1),
                .len = 1,
                .nodes_كدي = 1,
        };
        إذا (!str->buf || !str->سيو)
                أرجع عدم;

        أرجع TAKE_PTR(str);
}

ساكن بنية strbuf_كلي* strbuf_كلي_cleanup(بنية strbuf_كلي *كلي) {
        أكد(كلي);

        FOREACH_ARRAY(child, كلي->children, كلي->children_كدي)
                strbuf_كلي_cleanup(child->child);

        حرر(كلي->children);
        أرجع mfree(كلي);
}

/* clean up trie data, leave only the string buffer */
فراغ strbuf_complete(بنية strbuf *str) {
        إذا (!str || !str->سيو)
                أرجع;

        str->سيو = strbuf_كلي_cleanup(str->سيو);
}

/* clean up everything */
بنية strbuf* strbuf_free(بنية strbuf *str) {
        إذا (!str)
                أرجع عدم;

        strbuf_complete(str);
        حرر(str->buf);
        أرجع mfree(str);
}

ساكن صحيح strbuf_children_cmp(ثابت بنية strbuf_child_entry *n1, ثابت بنية strbuf_child_entry *n2) {
        أكد(n1);
        أكد(n2);

        أرجع CMP(n1->c, n2->c);
}

ساكن فراغ bubbleinsert(بنية strbuf_كلي *كلي,
                         uint8_t c,
                         بنية strbuf_كلي *كلي_child) {

        بنية strbuf_child_entry new = {
                .c = c,
                .child = كلي_child,
        };
        صحيح هغو = 0, دام = ASSERT_PTR(كلي)->children_كدي;

        طالما (دام > هغو) {
                صحيح ڟكيا = (دام + هغو) / 2 ;
                إذا (strbuf_children_cmp(&كلي->children[ڟكيا], &new) <= 0)
                        هغو = ڟكيا + 1;
                وإلا
                        دام = ڟكيا;
        }

        memmove(كلي->children + هغو + 1, كلي->children + هغو,
                حجم(بنية strbuf_child_entry) * (كلي->children_كدي - هغو));
        كلي->children[هغو] = new;

        كلي->children_كدي++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(بنية strbuf *str, ثابت حرف *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        إذا (len == SIZE_MAX)
                len = strlen(s);

        إذا (!str->سيو)
                أرجع -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_كدي++;
        إذا (len == 0) {
                str->dedup_كدي++;
                أرجع 0;
        }
        str->in_len += len;

        بنية strbuf_كلي *كلي = str->سيو;
        لكل (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = كلي->درج_off + كلي->درج_len - len;
                إذا (depth == len || (كلي->درج_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_كدي++;
                        أرجع off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                بنية strbuf_child_entry *child, نيما = { .c = c };
                child = typesafe_bsearch(&نيما, كلي->children, كلي->children_كدي, strbuf_children_cmp);
                إذا (!child)
                        اكسر;
                كلي = child->child;
        }

        /* add new string */
        إذا (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                أرجع -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ بنية strbuf_كلي *كلي_child = عدم;

        كلي_child = new(بنية strbuf_كلي, 1);
        إذا (!كلي_child)
                أرجع -ENOMEM;
        *كلي_child = (بنية strbuf_كلي) {
                .درج_off = off,
                .درج_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        إذا (!GREEDY_REALLOC(كلي->children, كلي->children_كدي + 1))
                أرجع -ENOMEM;

        str->nodes_كدي++;

        bubbleinsert(كلي, c, TAKE_PTR(كلي_child));

        أرجع off;
}
