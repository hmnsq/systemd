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
                .يىلتىز = new0(بنية strbuf_تۈگۈن, 1),
                .len = 1,
                .nodes_سانى = 1,
        };
        إذا (!str->buf || !str->يىلتىز)
                أرجع عدم;

        أرجع TAKE_PTR(str);
}

ساكن بنية strbuf_تۈگۈن* strbuf_تۈگۈن_cleanup(بنية strbuf_تۈگۈن *تۈگۈن) {
        أكد(تۈگۈن);

        FOREACH_ARRAY(child, تۈگۈن->children, تۈگۈن->children_سانى)
                strbuf_تۈگۈن_cleanup(child->child);

        حرر(تۈگۈن->children);
        أرجع mfree(تۈگۈن);
}

/* clean up trie data, leave only the string buffer */
فراغ strbuf_complete(بنية strbuf *str) {
        إذا (!str || !str->يىلتىز)
                أرجع;

        str->يىلتىز = strbuf_تۈگۈن_cleanup(str->يىلتىز);
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

ساكن فراغ bubbleinsert(بنية strbuf_تۈگۈن *تۈگۈن,
                         uint8_t c,
                         بنية strbuf_تۈگۈن *تۈگۈن_child) {

        بنية strbuf_child_entry new = {
                .c = c,
                .child = تۈگۈن_child,
        };
        صحيح سول = 0, ئوڭ = ASSERT_PTR(تۈگۈن)->children_سانى;

        طالما (ئوڭ > سول) {
                صحيح ئوتتۇرا = (ئوڭ + سول) / 2 ;
                إذا (strbuf_children_cmp(&تۈگۈن->children[ئوتتۇرا], &new) <= 0)
                        سول = ئوتتۇرا + 1;
                وإلا
                        ئوڭ = ئوتتۇرا;
        }

        memmove(تۈگۈن->children + سول + 1, تۈگۈن->children + سول,
                حجم(بنية strbuf_child_entry) * (تۈگۈن->children_سانى - سول));
        تۈگۈن->children[سول] = new;

        تۈگۈن->children_سانى++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(بنية strbuf *str, ثابت حرف *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        إذا (len == SIZE_MAX)
                len = strlen(s);

        إذا (!str->يىلتىز)
                أرجع -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_سانى++;
        إذا (len == 0) {
                str->dedup_سانى++;
                أرجع 0;
        }
        str->in_len += len;

        بنية strbuf_تۈگۈن *تۈگۈن = str->يىلتىز;
        لكل (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = تۈگۈن->قىممەت_off + تۈگۈن->قىممەت_len - len;
                إذا (depth == len || (تۈگۈن->قىممەت_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_سانى++;
                        أرجع off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                بنية strbuf_child_entry *child, ئىزدەش = { .c = c };
                child = typesafe_bsearch(&ئىزدەش, تۈگۈن->children, تۈگۈن->children_سانى, strbuf_children_cmp);
                إذا (!child)
                        اكسر;
                تۈگۈن = child->child;
        }

        /* add new string */
        إذا (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                أرجع -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ بنية strbuf_تۈگۈن *تۈگۈن_child = عدم;

        تۈگۈن_child = new(بنية strbuf_تۈگۈن, 1);
        إذا (!تۈگۈن_child)
                أرجع -ENOMEM;
        *تۈگۈن_child = (بنية strbuf_تۈگۈن) {
                .قىممەت_off = off,
                .قىممەت_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        إذا (!GREEDY_REALLOC(تۈگۈن->children, تۈگۈن->children_سانى + 1))
                أرجع -ENOMEM;

        str->nodes_سانى++;

        bubbleinsert(تۈگۈن, c, TAKE_PTR(تۈگۈن_child));

        أرجع off;
}
