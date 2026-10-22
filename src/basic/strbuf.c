/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sort-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strbuf.h"

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

ਬਣਤਰ strbuf* strbuf_new(ਖਾਲੀ) {
        _cleanup_(strbuf_freep) ਬਣਤਰ strbuf *str = ਨਿਰਾ;

        str = new(ਬਣਤਰ strbuf, 1);
        ਜੇ (!str)
                ਵਾਪਸ ਨਿਰਾ;

        *str = (ਬਣਤਰ strbuf) {
                .buf = new0(ਅੱਖਰ, 1),
                .ਜੜ੍ਹ = new0(ਬਣਤਰ strbuf_ਗੰਢ, 1),
                .len = 1,
                .nodes_ਗਿਣਤੀ = 1,
        };
        ਜੇ (!str->buf || !str->ਜੜ੍ਹ)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ TAKE_PTR(str);
}

ਸਥਿਰ ਬਣਤਰ strbuf_ਗੰਢ* strbuf_ਗੰਢ_cleanup(ਬਣਤਰ strbuf_ਗੰਢ *ਗੰਢ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਗੰਢ);

        FOREACH_ARRAY(ਬੱਚਾ, ਗੰਢ->children, ਗੰਢ->children_ਗਿਣਤੀ)
                strbuf_ਗੰਢ_cleanup(ਬੱਚਾ->ਬੱਚਾ);

        ਮੁਕਤ(ਗੰਢ->children);
        ਵਾਪਸ mfree(ਗੰਢ);
}

/* clean up trie data, leave only the string buffer */
ਖਾਲੀ strbuf_complete(ਬਣਤਰ strbuf *str) {
        ਜੇ (!str || !str->ਜੜ੍ਹ)
                ਵਾਪਸ;

        str->ਜੜ੍ਹ = strbuf_ਗੰਢ_cleanup(str->ਜੜ੍ਹ);
}

/* clean up everything */
ਬਣਤਰ strbuf* strbuf_free(ਬਣਤਰ strbuf *str) {
        ਜੇ (!str)
                ਵਾਪਸ ਨਿਰਾ;

        strbuf_complete(str);
        ਮੁਕਤ(str->buf);
        ਵਾਪਸ mfree(str);
}

ਸਥਿਰ ਪੂਰਨ strbuf_children_cmp(ਅਚਲ ਬਣਤਰ strbuf_ਬੱਚਾ_ਇੰਦਰਾਜ *n1, ਅਚਲ ਬਣਤਰ strbuf_ਬੱਚਾ_ਇੰਦਰਾਜ *n2) {
        ਪ੍ਰਤਿਜ੍ਞਾ(n1);
        ਪ੍ਰਤਿਜ੍ਞਾ(n2);

        ਵਾਪਸ CMP(n1->c, n2->c);
}

ਸਥਿਰ ਖਾਲੀ bubbleinsert(ਬਣਤਰ strbuf_ਗੰਢ *ਗੰਢ,
                         uint8_t c,
                         ਬਣਤਰ strbuf_ਗੰਢ *ਗੰਢ_ਬੱਚਾ) {

        ਬਣਤਰ strbuf_ਬੱਚਾ_ਇੰਦਰਾਜ new = {
                .c = c,
                .ਬੱਚਾ = ਗੰਢ_ਬੱਚਾ,
        };
        ਪੂਰਨ ਖੱਬਾ = 0, ਸੱਜਾ = ASSERT_PTR(ਗੰਢ)->children_ਗਿਣਤੀ;

        ਜਦੋਂ (ਸੱਜਾ > ਖੱਬਾ) {
                ਪੂਰਨ ਵਿਚਕਾਰ = (ਸੱਜਾ + ਖੱਬਾ) / 2 ;
                ਜੇ (strbuf_children_cmp(&ਗੰਢ->children[ਵਿਚਕਾਰ], &new) <= 0)
                        ਖੱਬਾ = ਵਿਚਕਾਰ + 1;
                ਨਹੀਂਤਾਂ
                        ਸੱਜਾ = ਵਿਚਕਾਰ;
        }

        memmove(ਗੰਢ->children + ਖੱਬਾ + 1, ਗੰਢ->children + ਖੱਬਾ,
                ਆਕਾਰ(ਬਣਤਰ strbuf_ਬੱਚਾ_ਇੰਦਰਾਜ) * (ਗੰਢ->children_ਗਿਣਤੀ - ਖੱਬਾ));
        ਗੰਢ->children[ਖੱਬਾ] = new;

        ਗੰਢ->children_ਗਿਣਤੀ++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_ਜੋੜੋ_string_ਭਰਿਆ(ਬਣਤਰ strbuf *str, ਅਚਲ ਅੱਖਰ *s, size_t len) {
        uint8_t c;
        ssize_t off;

        ਪ੍ਰਤਿਜ੍ਞਾ(str);
        ਪ੍ਰਤਿਜ੍ਞਾ(s || len == 0);

        ਜੇ (len == SIZE_MAX)
                len = ਲੜੀ_ਲੰਬਾਈ(s);

        ਜੇ (!str->ਜੜ੍ਹ)
                ਵਾਪਸ -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->ਵਿੱਚ_ਗਿਣਤੀ++;
        ਜੇ (len == 0) {
                str->dedup_ਗਿਣਤੀ++;
                ਵਾਪਸ 0;
        }
        str->ਵਿੱਚ_len += len;

        ਬਣਤਰ strbuf_ਗੰਢ *ਗੰਢ = str->ਜੜ੍ਹ;
        ਲਈ (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = ਗੰਢ->ਮੁੱਲ_off + ਗੰਢ->ਮੁੱਲ_len - len;
                ਜੇ (depth == len || (ਗੰਢ->ਮੁੱਲ_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_ਗਿਣਤੀ++;
                        ਵਾਪਸ off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                ਬਣਤਰ strbuf_ਬੱਚਾ_ਇੰਦਰਾਜ *ਬੱਚਾ, ਖੋਜ = { .c = c };
                ਬੱਚਾ = typesafe_bsearch(&ਖੋਜ, ਗੰਢ->children, ਗੰਢ->children_ਗਿਣਤੀ, strbuf_children_cmp);
                ਜੇ (!ਬੱਚਾ)
                        ਰੋਕੋ;
                ਗੰਢ = ਬੱਚਾ->ਬੱਚਾ;
        }

        /* add new string */
        ਜੇ (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                ਵਾਪਸ -ENOMEM;
        off = str->len;
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ ਬਣਤਰ strbuf_ਗੰਢ *ਗੰਢ_ਬੱਚਾ = ਨਿਰਾ;

        ਗੰਢ_ਬੱਚਾ = new(ਬਣਤਰ strbuf_ਗੰਢ, 1);
        ਜੇ (!ਗੰਢ_ਬੱਚਾ)
                ਵਾਪਸ -ENOMEM;
        *ਗੰਢ_ਬੱਚਾ = (ਬਣਤਰ strbuf_ਗੰਢ) {
                .ਮੁੱਲ_off = off,
                .ਮੁੱਲ_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        ਜੇ (!GREEDY_REALLOC(ਗੰਢ->children, ਗੰਢ->children_ਗਿਣਤੀ + 1))
                ਵਾਪਸ -ENOMEM;

        str->nodes_ਗਿਣਤੀ++;

        bubbleinsert(ਗੰਢ, c, TAKE_PTR(ਗੰਢ_ਬੱਚਾ));

        ਵਾਪਸ off;
}
