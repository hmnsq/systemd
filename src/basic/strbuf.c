/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "sort-util.h"
#ಅನ್ತರ್ಭಾವ "strbuf.h"

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

ರಚನೆ strbuf* strbuf_new(ಶೂನ್ಯ) {
        _cleanup_(strbuf_freep) ರಚನೆ strbuf *str = ಖಾಲಿ;

        str = new(ರಚನೆ strbuf, 1);
        ಆದರೆ (!str)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        *str = (ರಚನೆ strbuf) {
                .buf = new0(ಅಕ್ಷರ, 1),
                .ಬೇರು = new0(ರಚನೆ strbuf_ಗಂಟು, 1),
                .len = 1,
                .nodes_ಎಣಿಕೆ = 1,
        };
        ಆದರೆ (!str->buf || !str->ಬೇರು)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ TAKE_PTR(str);
}

ಸ್ಥಿರ ರಚನೆ strbuf_ಗಂಟು* strbuf_ಗಂಟು_cleanup(ರಚನೆ strbuf_ಗಂಟು *ಗಂಟು) {
        ಪ್ರತಿಜ್ಞಾ(ಗಂಟು);

        FOREACH_ARRAY(ಮಗು, ಗಂಟು->children, ಗಂಟು->children_ಎಣಿಕೆ)
                strbuf_ಗಂಟು_cleanup(ಮಗು->ಮಗು);

        ಬಿಡುಗಡೆ(ಗಂಟು->children);
        ಹಿಂತಿರುಗಿ mfree(ಗಂಟು);
}

/* clean up trie data, leave only the string buffer */
ಶೂನ್ಯ strbuf_complete(ರಚನೆ strbuf *str) {
        ಆದರೆ (!str || !str->ಬೇರು)
                ಹಿಂತಿರುಗಿ;

        str->ಬೇರು = strbuf_ಗಂಟು_cleanup(str->ಬೇರು);
}

/* clean up everything */
ರಚನೆ strbuf* strbuf_free(ರಚನೆ strbuf *str) {
        ಆದರೆ (!str)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        strbuf_complete(str);
        ಬಿಡುಗಡೆ(str->buf);
        ಹಿಂತಿರುಗಿ mfree(str);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ strbuf_children_cmp(ಸ್ಥಿರಾಂಕ ರಚನೆ strbuf_ಮಗು_ನಮೂದು *n1, ಸ್ಥಿರಾಂಕ ರಚನೆ strbuf_ಮಗು_ನಮೂದು *n2) {
        ಪ್ರತಿಜ್ಞಾ(n1);
        ಪ್ರತಿಜ್ಞಾ(n2);

        ಹಿಂತಿರುಗಿ CMP(n1->c, n2->c);
}

ಸ್ಥಿರ ಶೂನ್ಯ bubbleinsert(ರಚನೆ strbuf_ಗಂಟು *ಗಂಟು,
                         uint8_t c,
                         ರಚನೆ strbuf_ಗಂಟು *ಗಂಟು_ಮಗು) {

        ರಚನೆ strbuf_ಮಗು_ನಮೂದು new = {
                .c = c,
                .ಮಗು = ಗಂಟು_ಮಗು,
        };
        ಪೂರ್ಣಾಂಕ ಎಡ = 0, ಬಲ = ASSERT_PTR(ಗಂಟು)->children_ಎಣಿಕೆ;

        ಆಗ (ಬಲ > ಎಡ) {
                ಪೂರ್ಣಾಂಕ ಮಧ್ಯ = (ಬಲ + ಎಡ) / 2 ;
                ಆದರೆ (strbuf_children_cmp(&ಗಂಟು->children[ಮಧ್ಯ], &new) <= 0)
                        ಎಡ = ಮಧ್ಯ + 1;
                ಇಲ್ಲದಿದ್ದರೆ
                        ಬಲ = ಮಧ್ಯ;
        }

        memmove(ಗಂಟು->children + ಎಡ + 1, ಗಂಟು->children + ಎಡ,
                ಗಾತ್ರ(ರಚನೆ strbuf_ಮಗು_ನಮೂದು) * (ಗಂಟು->children_ಎಣಿಕೆ - ಎಡ));
        ಗಂಟು->children[ಎಡ] = new;

        ಗಂಟು->children_ಎಣಿಕೆ++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_ಕೂಡಿಸು_string_ತುಂಬಿದ(ರಚನೆ strbuf *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len) {
        uint8_t c;
        ssize_t off;

        ಪ್ರತಿಜ್ಞಾ(str);
        ಪ್ರತಿಜ್ಞಾ(s || len == 0);

        ಆದರೆ (len == SIZE_MAX)
                len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);

        ಆದರೆ (!str->ಬೇರು)
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->ಒಳ_ಎಣಿಕೆ++;
        ಆದರೆ (len == 0) {
                str->dedup_ಎಣಿಕೆ++;
                ಹಿಂತಿರುಗಿ 0;
        }
        str->ಒಳ_len += len;

        ರಚನೆ strbuf_ಗಂಟು *ಗಂಟು = str->ಬೇರು;
        ಗಾಗಿ (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = ಗಂಟು->ಮೌಲ್ಯ_off + ಗಂಟು->ಮೌಲ್ಯ_len - len;
                ಆದರೆ (depth == len || (ಗಂಟು->ಮೌಲ್ಯ_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_ಎಣಿಕೆ++;
                        ಹಿಂತಿರುಗಿ off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                ರಚನೆ strbuf_ಮಗು_ನಮೂದು *ಮಗು, ಹುಡುಕು = { .c = c };
                ಮಗು = typesafe_bsearch(&ಹುಡುಕು, ಗಂಟು->children, ಗಂಟು->children_ಎಣಿಕೆ, strbuf_children_cmp);
                ಆದರೆ (!ಮಗು)
                        ನಿಲ್ಲಿಸು;
                ಗಂಟು = ಮಗು->ಮಗು;
        }

        /* add new string */
        ಆದರೆ (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;
        off = str->len;
        ಸ್ಮರಣೆ_ನಕಲು(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ ರಚನೆ strbuf_ಗಂಟು *ಗಂಟು_ಮಗು = ಖಾಲಿ;

        ಗಂಟು_ಮಗು = new(ರಚನೆ strbuf_ಗಂಟು, 1);
        ಆದರೆ (!ಗಂಟು_ಮಗು)
                ಹಿಂತಿರುಗಿ -ENOMEM;
        *ಗಂಟು_ಮಗು = (ರಚನೆ strbuf_ಗಂಟು) {
                .ಮೌಲ್ಯ_off = off,
                .ಮೌಲ್ಯ_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        ಆದರೆ (!GREEDY_REALLOC(ಗಂಟು->children, ಗಂಟು->children_ಎಣಿಕೆ + 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        str->nodes_ಎಣಿಕೆ++;

        bubbleinsert(ಗಂಟು, c, TAKE_PTR(ಗಂಟು_ಮಗು));

        ಹಿಂತಿರುಗಿ off;
}
