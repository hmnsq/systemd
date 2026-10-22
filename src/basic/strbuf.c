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
                .جڑ = new0(بنية strbuf_گرہ, 1),
                .len = 1,
                .nodes_گنتی = 1,
        };
        إذا (!str->buf || !str->جڑ)
                أرجع عدم;

        أرجع TAKE_PTR(str);
}

ساكن بنية strbuf_گرہ* strbuf_گرہ_cleanup(بنية strbuf_گرہ *گرہ) {
        أكد(گرہ);

        FOREACH_ARRAY(بچہ, گرہ->children, گرہ->children_گنتی)
                strbuf_گرہ_cleanup(بچہ->بچہ);

        حرر(گرہ->children);
        أرجع mfree(گرہ);
}

/* clean up trie data, leave only the string buffer */
فراغ strbuf_complete(بنية strbuf *str) {
        إذا (!str || !str->جڑ)
                أرجع;

        str->جڑ = strbuf_گرہ_cleanup(str->جڑ);
}

/* clean up everything */
بنية strbuf* strbuf_free(بنية strbuf *str) {
        إذا (!str)
                أرجع عدم;

        strbuf_complete(str);
        حرر(str->buf);
        أرجع mfree(str);
}

ساكن صحيح strbuf_children_cmp(ثابت بنية strbuf_بچہ_اندراج *n1, ثابت بنية strbuf_بچہ_اندراج *n2) {
        أكد(n1);
        أكد(n2);

        أرجع CMP(n1->c, n2->c);
}

ساكن فراغ bubbleinsert(بنية strbuf_گرہ *گرہ,
                         uint8_t c,
                         بنية strbuf_گرہ *گرہ_بچہ) {

        بنية strbuf_بچہ_اندراج new = {
                .c = c,
                .بچہ = گرہ_بچہ,
        };
        صحيح بایاں = 0, دایاں = ASSERT_PTR(گرہ)->children_گنتی;

        طالما (دایاں > بایاں) {
                صحيح درمیان = (دایاں + بایاں) / 2 ;
                إذا (strbuf_children_cmp(&گرہ->children[درمیان], &new) <= 0)
                        بایاں = درمیان + 1;
                وإلا
                        دایاں = درمیان;
        }

        memmove(گرہ->children + بایاں + 1, گرہ->children + بایاں,
                حجم(بنية strbuf_بچہ_اندراج) * (گرہ->children_گنتی - بایاں));
        گرہ->children[بایاں] = new;

        گرہ->children_گنتی++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_جوڑو_string_بھریا(بنية strbuf *str, ثابت حرف *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        إذا (len == SIZE_MAX)
                len = لڑی_لمبائی(s);

        إذا (!str->جڑ)
                أرجع -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->وچ_گنتی++;
        إذا (len == 0) {
                str->dedup_گنتی++;
                أرجع 0;
        }
        str->وچ_len += len;

        بنية strbuf_گرہ *گرہ = str->جڑ;
        لكل (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = گرہ->قدر_off + گرہ->قدر_len - len;
                إذا (depth == len || (گرہ->قدر_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_گنتی++;
                        أرجع off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                بنية strbuf_بچہ_اندراج *بچہ, تلاش = { .c = c };
                بچہ = typesafe_bsearch(&تلاش, گرہ->children, گرہ->children_گنتی, strbuf_children_cmp);
                إذا (!بچہ)
                        اكسر;
                گرہ = بچہ->بچہ;
        }

        /* add new string */
        إذا (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                أرجع -ENOMEM;
        off = str->len;
        حافظہ_نقل(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ بنية strbuf_گرہ *گرہ_بچہ = عدم;

        گرہ_بچہ = new(بنية strbuf_گرہ, 1);
        إذا (!گرہ_بچہ)
                أرجع -ENOMEM;
        *گرہ_بچہ = (بنية strbuf_گرہ) {
                .قدر_off = off,
                .قدر_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        إذا (!GREEDY_REALLOC(گرہ->children, گرہ->children_گنتی + 1))
                أرجع -ENOMEM;

        str->nodes_گنتی++;

        bubbleinsert(گرہ, c, TAKE_PTR(گرہ_بچہ));

        أرجع off;
}
