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
                .اکر = new0(بنية strbuf_نود, 1),
                .len = 1,
                .nodes_کيراءن = 1,
        };
        إذا (!str->buf || !str->اکر)
                أرجع عدم;

        أرجع TAKE_PTR(str);
}

ساكن بنية strbuf_نود* strbuf_نود_cleanup(بنية strbuf_نود *نود) {
        أكد(نود);

        FOREACH_ARRAY(child, نود->children, نود->children_کيراءن)
                strbuf_نود_cleanup(child->child);

        حرر(نود->children);
        أرجع mfree(نود);
}

/* clean up trie data, leave only the string buffer */
فراغ strbuf_complete(بنية strbuf *str) {
        إذا (!str || !str->اکر)
                أرجع;

        str->اکر = strbuf_نود_cleanup(str->اکر);
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

ساكن فراغ bubbleinsert(بنية strbuf_نود *نود,
                         uint8_t c,
                         بنية strbuf_نود *نود_child) {

        بنية strbuf_child_entry new = {
                .c = c,
                .child = نود_child,
        };
        صحيح کيري = 0, کانن = ASSERT_PTR(نود)->children_کيراءن;

        طالما (کانن > کيري) {
                صحيح تڠه = (کانن + کيري) / 2 ;
                إذا (strbuf_children_cmp(&نود->children[تڠه], &new) <= 0)
                        کيري = تڠه + 1;
                وإلا
                        کانن = تڠه;
        }

        memmove(نود->children + کيري + 1, نود->children + کيري,
                حجم(بنية strbuf_child_entry) * (نود->children_کيراءن - کيري));
        نود->children[کيري] = new;

        نود->children_کيراءن++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(بنية strbuf *str, ثابت حرف *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        إذا (len == SIZE_MAX)
                len = strlen(s);

        إذا (!str->اکر)
                أرجع -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_کيراءن++;
        إذا (len == 0) {
                str->dedup_کيراءن++;
                أرجع 0;
        }
        str->in_len += len;

        بنية strbuf_نود *نود = str->اکر;
        لكل (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = نود->نيلاي_off + نود->نيلاي_len - len;
                إذا (depth == len || (نود->نيلاي_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_کيراءن++;
                        أرجع off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                بنية strbuf_child_entry *child, چارين = { .c = c };
                child = typesafe_bsearch(&چارين, نود->children, نود->children_کيراءن, strbuf_children_cmp);
                إذا (!child)
                        اكسر;
                نود = child->child;
        }

        /* add new string */
        إذا (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                أرجع -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ بنية strbuf_نود *نود_child = عدم;

        نود_child = new(بنية strbuf_نود, 1);
        إذا (!نود_child)
                أرجع -ENOMEM;
        *نود_child = (بنية strbuf_نود) {
                .نيلاي_off = off,
                .نيلاي_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        إذا (!GREEDY_REALLOC(نود->children, نود->children_کيراءن + 1))
                أرجع -ENOMEM;

        str->nodes_کيراءن++;

        bubbleinsert(نود, c, TAKE_PTR(نود_child));

        أرجع off;
}
