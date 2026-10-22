/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <string.h>

#شامل "alloc-util.h"
#شامل "sort-util.h"
#شامل "strbuf.h"

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

ساخت strbuf* strbuf_new(خالی) {
        _cleanup_(strbuf_freep) ساخت strbuf *str = تہی;

        str = new(ساخت strbuf, 1);
        اگر (!str)
                واپس تہی;

        *str = (ساخت strbuf) {
                .buf = new0(حرف, 1),
                .جڑ = new0(ساخت strbuf_گرہ, 1),
                .len = 1,
                .nodes_گنتی = 1,
        };
        اگر (!str->buf || !str->جڑ)
                واپس تہی;

        واپس TAKE_PTR(str);
}

ساکن ساخت strbuf_گرہ* strbuf_گرہ_cleanup(ساخت strbuf_گرہ *گرہ) {
        أكد(گرہ);

        FOREACH_ARRAY(بچہ, گرہ->children, گرہ->children_گنتی)
                strbuf_گرہ_cleanup(بچہ->بچہ);

        آزاد(گرہ->children);
        واپس mfree(گرہ);
}

/* clean up trie data, leave only the string buffer */
خالی strbuf_complete(ساخت strbuf *str) {
        اگر (!str || !str->جڑ)
                واپس;

        str->جڑ = strbuf_گرہ_cleanup(str->جڑ);
}

/* clean up everything */
ساخت strbuf* strbuf_free(ساخت strbuf *str) {
        اگر (!str)
                واپس تہی;

        strbuf_complete(str);
        آزاد(str->buf);
        واپس mfree(str);
}

ساکن صحیح strbuf_children_cmp(ثابت ساخت strbuf_بچہ_اندراج *n1, ثابت ساخت strbuf_بچہ_اندراج *n2) {
        أكد(n1);
        أكد(n2);

        واپس CMP(n1->c, n2->c);
}

ساکن خالی bubbleinsert(ساخت strbuf_گرہ *گرہ,
                         uint8_t c,
                         ساخت strbuf_گرہ *گرہ_بچہ) {

        ساخت strbuf_بچہ_اندراج new = {
                .c = c,
                .بچہ = گرہ_بچہ,
        };
        صحیح بایاں = 0, دایاں = ASSERT_PTR(گرہ)->children_گنتی;

        جب (دایاں > بایاں) {
                صحیح درمیان = (دایاں + بایاں) / 2 ;
                اگر (strbuf_children_cmp(&گرہ->children[درمیان], &new) <= 0)
                        بایاں = درمیان + 1;
                ورنہ
                        دایاں = درمیان;
        }

        memmove(گرہ->children + بایاں + 1, گرہ->children + بایاں,
                حجم(ساخت strbuf_بچہ_اندراج) * (گرہ->children_گنتی - بایاں));
        گرہ->children[بایاں] = new;

        گرہ->children_گنتی++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_جوڑو_string_بھرا(ساخت strbuf *str, ثابت حرف *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        اگر (len == SIZE_MAX)
                len = لڑی_لمبائی(s);

        اگر (!str->جڑ)
                واپس -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->اندر_گنتی++;
        اگر (len == 0) {
                str->dedup_گنتی++;
                واپس 0;
        }
        str->اندر_len += len;

        ساخت strbuf_گرہ *گرہ = str->جڑ;
        برائے (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = گرہ->قدر_off + گرہ->قدر_len - len;
                اگر (depth == len || (گرہ->قدر_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_گنتی++;
                        واپس off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                ساخت strbuf_بچہ_اندراج *بچہ, تلاش = { .c = c };
                بچہ = typesafe_bsearch(&تلاش, گرہ->children, گرہ->children_گنتی, strbuf_children_cmp);
                اگر (!بچہ)
                        توڑو;
                گرہ = بچہ->بچہ;
        }

        /* add new string */
        اگر (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                واپس -ENOMEM;
        off = str->len;
        حافظہ_نقل(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ ساخت strbuf_گرہ *گرہ_بچہ = تہی;

        گرہ_بچہ = new(ساخت strbuf_گرہ, 1);
        اگر (!گرہ_بچہ)
                واپس -ENOMEM;
        *گرہ_بچہ = (ساخت strbuf_گرہ) {
                .قدر_off = off,
                .قدر_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        اگر (!GREEDY_REALLOC(گرہ->children, گرہ->children_گنتی + 1))
                واپس -ENOMEM;

        str->nodes_گنتی++;

        bubbleinsert(گرہ, c, TAKE_PTR(گرہ_بچہ));

        واپس off;
}
