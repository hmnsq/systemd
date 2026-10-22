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

ಸಂರಚನಾ strbuf* strbuf_new(ಶೂನ್ಯ) {
        _cleanup_(strbuf_freep) ಸಂರಚನಾ strbuf *str = ರಿಕ್ತ;

        str = new(ಸಂರಚನಾ strbuf, 1);
        ಯದಿ (!str)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        *str = (ಸಂರಚನಾ strbuf) {
                .buf = new0(ವರ್ಣ, 1),
                .ಬೇರು = new0(ಸಂರಚನಾ strbuf_ಗಂಟು, 1),
                .len = 1,
                .nodes_ಎಣಿಕೆ = 1,
        };
        ಯದಿ (!str->buf || !str->ಬೇರು)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(str);
}

ಸ್ಥಿರ ಸಂರಚನಾ strbuf_ಗಂಟು* strbuf_ಗಂಟು_cleanup(ಸಂರಚನಾ strbuf_ಗಂಟು *ಗಂಟು) {
        ಪ್ರತಿಜ್ಞಾ(ಗಂಟು);

        FOREACH_ARRAY(child, ಗಂಟು->children, ಗಂಟು->children_ಎಣಿಕೆ)
                strbuf_ಗಂಟು_cleanup(child->child);

        ಮುಕ್ತ(ಗಂಟು->children);
        ಪ್ರತ್ಯಾಗಮ mfree(ಗಂಟು);
}

/* clean up trie data, leave only the string buffer */
ಶೂನ್ಯ strbuf_complete(ಸಂರಚನಾ strbuf *str) {
        ಯದಿ (!str || !str->ಬೇರು)
                ಪ್ರತ್ಯಾಗಮ;

        str->ಬೇರು = strbuf_ಗಂಟು_cleanup(str->ಬೇರು);
}

/* clean up everything */
ಸಂರಚನಾ strbuf* strbuf_free(ಸಂರಚನಾ strbuf *str) {
        ಯದಿ (!str)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        strbuf_complete(str);
        ಮುಕ್ತ(str->buf);
        ಪ್ರತ್ಯಾಗಮ mfree(str);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ strbuf_children_cmp(ಧ್ರುವ ಸಂರಚನಾ strbuf_child_entry *n1, ಧ್ರುವ ಸಂರಚನಾ strbuf_child_entry *n2) {
        ಪ್ರತಿಜ್ಞಾ(n1);
        ಪ್ರತಿಜ್ಞಾ(n2);

        ಪ್ರತ್ಯಾಗಮ CMP(n1->c, n2->c);
}

ಸ್ಥಿರ ಶೂನ್ಯ bubbleinsert(ಸಂರಚನಾ strbuf_ಗಂಟು *ಗಂಟು,
                         uint8_t c,
                         ಸಂರಚನಾ strbuf_ಗಂಟು *ಗಂಟು_child) {

        ಸಂರಚನಾ strbuf_child_entry new = {
                .c = c,
                .child = ಗಂಟು_child,
        };
        ಪೂರ್ಣಾಙ್ಕ ಎಡ = 0, ಬಲ = ASSERT_PTR(ಗಂಟು)->children_ಎಣಿಕೆ;

        ಯಾವತ್ (ಬಲ > ಎಡ) {
                ಪೂರ್ಣಾಙ್ಕ ಮಧ್ಯ = (ಬಲ + ಎಡ) / 2 ;
                ಯದಿ (strbuf_children_cmp(&ಗಂಟು->children[ಮಧ್ಯ], &new) <= 0)
                        ಎಡ = ಮಧ್ಯ + 1;
                ಅನ್ಯಥಾ
                        ಬಲ = ಮಧ್ಯ;
        }

        memmove(ಗಂಟು->children + ಎಡ + 1, ಗಂಟು->children + ಎಡ,
                ಪರಿಮಾಣ(ಸಂರಚನಾ strbuf_child_entry) * (ಗಂಟು->children_ಎಣಿಕೆ - ಎಡ));
        ಗಂಟು->children[ಎಡ] = new;

        ಗಂಟು->children_ಎಣಿಕೆ++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(ಸಂರಚನಾ strbuf *str, ಧ್ರುವ ವರ್ಣ *s, size_t len) {
        uint8_t c;
        ssize_t off;

        ಪ್ರತಿಜ್ಞಾ(str);
        ಪ್ರತಿಜ್ಞಾ(s || len == 0);

        ಯದಿ (len == SIZE_MAX)
                len = strlen(s);

        ಯದಿ (!str->ಬೇರು)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_ಎಣಿಕೆ++;
        ಯದಿ (len == 0) {
                str->dedup_ಎಣಿಕೆ++;
                ಪ್ರತ್ಯಾಗಮ 0;
        }
        str->in_len += len;

        ಸಂರಚನಾ strbuf_ಗಂಟು *ಗಂಟು = str->ಬೇರು;
        ಪ್ರತಿ (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = ಗಂಟು->ಮೌಲ್ಯ_off + ಗಂಟು->ಮೌಲ್ಯ_len - len;
                ಯದಿ (depth == len || (ಗಂಟು->ಮೌಲ್ಯ_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_ಎಣಿಕೆ++;
                        ಪ್ರತ್ಯಾಗಮ off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                ಸಂರಚನಾ strbuf_child_entry *child, ಹುಡುಕು = { .c = c };
                child = typesafe_bsearch(&ಹುಡುಕು, ಗಂಟು->children, ಗಂಟು->children_ಎಣಿಕೆ, strbuf_children_cmp);
                ಯದಿ (!child)
                        ಭಙ್ಗ;
                ಗಂಟು = child->child;
        }

        /* add new string */
        ಯದಿ (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ ಸಂರಚನಾ strbuf_ಗಂಟು *ಗಂಟು_child = ರಿಕ್ತ;

        ಗಂಟು_child = new(ಸಂರಚನಾ strbuf_ಗಂಟು, 1);
        ಯದಿ (!ಗಂಟು_child)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        *ಗಂಟು_child = (ಸಂರಚನಾ strbuf_ಗಂಟು) {
                .ಮೌಲ್ಯ_off = off,
                .ಮೌಲ್ಯ_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        ಯದಿ (!GREEDY_REALLOC(ಗಂಟು->children, ಗಂಟು->children_ಎಣಿಕೆ + 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        str->nodes_ಎಣಿಕೆ++;

        bubbleinsert(ಗಂಟು, c, TAKE_PTR(ಗಂಟು_child));

        ಪ್ರತ್ಯಾಗಮ off;
}
