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
                .جذر = new0(بنية strbuf_عقدة, 1),
                .len = 1,
                .nodes_عدد = 1,
        };
        إذا (!str->buf || !str->جذر)
                أرجع عدم;

        أرجع TAKE_PTR(str);
}

ساكن بنية strbuf_عقدة* strbuf_عقدة_cleanup(بنية strbuf_عقدة *عقدة) {
        أكد(عقدة);

        FOREACH_ARRAY(child, عقدة->children, عقدة->children_عدد)
                strbuf_عقدة_cleanup(child->child);

        حرر(عقدة->children);
        أرجع mfree(عقدة);
}

/* clean up trie data, leave only the string buffer */
فراغ strbuf_complete(بنية strbuf *str) {
        إذا (!str || !str->جذر)
                أرجع;

        str->جذر = strbuf_عقدة_cleanup(str->جذر);
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

ساكن فراغ bubbleinsert(بنية strbuf_عقدة *عقدة,
                         uint8_t c,
                         بنية strbuf_عقدة *عقدة_child) {

        بنية strbuf_child_entry new = {
                .c = c,
                .child = عقدة_child,
        };
        صحيح يسار = 0, يمين = ASSERT_PTR(عقدة)->children_عدد;

        طالما (يمين > يسار) {
                صحيح وسط = (يمين + يسار) / 2 ;
                إذا (strbuf_children_cmp(&عقدة->children[وسط], &new) <= 0)
                        يسار = وسط + 1;
                وإلا
                        يمين = وسط;
        }

        memmove(عقدة->children + يسار + 1, عقدة->children + يسار,
                حجم(بنية strbuf_child_entry) * (عقدة->children_عدد - يسار));
        عقدة->children[يسار] = new;

        عقدة->children_عدد++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(بنية strbuf *str, ثابت حرف *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        إذا (len == SIZE_MAX)
                len = strlen(s);

        إذا (!str->جذر)
                أرجع -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_عدد++;
        إذا (len == 0) {
                str->dedup_عدد++;
                أرجع 0;
        }
        str->in_len += len;

        بنية strbuf_عقدة *عقدة = str->جذر;
        لكل (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = عقدة->قيمة_off + عقدة->قيمة_len - len;
                إذا (depth == len || (عقدة->قيمة_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_عدد++;
                        أرجع off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                بنية strbuf_child_entry *child, بحث = { .c = c };
                child = typesafe_bsearch(&بحث, عقدة->children, عقدة->children_عدد, strbuf_children_cmp);
                إذا (!child)
                        اكسر;
                عقدة = child->child;
        }

        /* add new string */
        إذا (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                أرجع -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ بنية strbuf_عقدة *عقدة_child = عدم;

        عقدة_child = new(بنية strbuf_عقدة, 1);
        إذا (!عقدة_child)
                أرجع -ENOMEM;
        *عقدة_child = (بنية strbuf_عقدة) {
                .قيمة_off = off,
                .قيمة_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        إذا (!GREEDY_REALLOC(عقدة->children, عقدة->children_عدد + 1))
                أرجع -ENOMEM;

        str->nodes_عدد++;

        bubbleinsert(عقدة, c, TAKE_PTR(عقدة_child));

        أرجع off;
}
