/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "sort-util.h"
#અન્તર્ભાવ "strbuf.h"

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

સંરચના strbuf* strbuf_new(શૂન્ય) {
        _cleanup_(strbuf_freep) સંરચના strbuf *str = ખાલી;

        str = new(સંરચના strbuf, 1);
        જો (!str)
                પાછા ખાલી;

        *str = (સંરચના strbuf) {
                .buf = new0(અક્ષર, 1),
                .મૂળ = new0(સંરચના strbuf_ગાંઠ, 1),
                .len = 1,
                .nodes_ગણતરી = 1,
        };
        જો (!str->buf || !str->મૂળ)
                પાછા ખાલી;

        પાછા TAKE_PTR(str);
}

સ્થિર સંરચના strbuf_ગાંઠ* strbuf_ગાંઠ_cleanup(સંરચના strbuf_ગાંઠ *ગાંઠ) {
        પ્રતિજ્ઞા(ગાંઠ);

        FOREACH_ARRAY(child, ગાંઠ->children, ગાંઠ->children_ગણતરી)
                strbuf_ગાંઠ_cleanup(child->child);

        મુક્ત(ગાંઠ->children);
        પાછા mfree(ગાંઠ);
}

/* clean up trie data, leave only the string buffer */
શૂન્ય strbuf_complete(સંરચના strbuf *str) {
        જો (!str || !str->મૂળ)
                પાછા;

        str->મૂળ = strbuf_ગાંઠ_cleanup(str->મૂળ);
}

/* clean up everything */
સંરચના strbuf* strbuf_free(સંરચના strbuf *str) {
        જો (!str)
                પાછા ખાલી;

        strbuf_complete(str);
        મુક્ત(str->buf);
        પાછા mfree(str);
}

સ્થિર પૂર્ણાંક strbuf_children_cmp(અચલ સંરચના strbuf_child_entry *n1, અચલ સંરચના strbuf_child_entry *n2) {
        પ્રતિજ્ઞા(n1);
        પ્રતિજ્ઞા(n2);

        પાછા CMP(n1->c, n2->c);
}

સ્થિર શૂન્ય bubbleinsert(સંરચના strbuf_ગાંઠ *ગાંઠ,
                         uint8_t c,
                         સંરચના strbuf_ગાંઠ *ગાંઠ_child) {

        સંરચના strbuf_child_entry new = {
                .c = c,
                .child = ગાંઠ_child,
        };
        પૂર્ણાંક ડાબું = 0, જમણું = ASSERT_PTR(ગાંઠ)->children_ગણતરી;

        જ્યારે (જમણું > ડાબું) {
                પૂર્ણાંક મધ્ય = (જમણું + ડાબું) / 2 ;
                જો (strbuf_children_cmp(&ગાંઠ->children[મધ્ય], &new) <= 0)
                        ડાબું = મધ્ય + 1;
                નહીંતર
                        જમણું = મધ્ય;
        }

        memmove(ગાંઠ->children + ડાબું + 1, ગાંઠ->children + ડાબું,
                કદ(સંરચના strbuf_child_entry) * (ગાંઠ->children_ગણતરી - ડાબું));
        ગાંઠ->children[ડાબું] = new;

        ગાંઠ->children_ગણતરી++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(સંરચના strbuf *str, અચલ અક્ષર *s, size_t len) {
        uint8_t c;
        ssize_t off;

        પ્રતિજ્ઞા(str);
        પ્રતિજ્ઞા(s || len == 0);

        જો (len == SIZE_MAX)
                len = strlen(s);

        જો (!str->મૂળ)
                પાછા -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_ગણતરી++;
        જો (len == 0) {
                str->dedup_ગણતરી++;
                પાછા 0;
        }
        str->in_len += len;

        સંરચના strbuf_ગાંઠ *ગાંઠ = str->મૂળ;
        માટે (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = ગાંઠ->કિંમત_off + ગાંઠ->કિંમત_len - len;
                જો (depth == len || (ગાંઠ->કિંમત_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_ગણતરી++;
                        પાછા off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                સંરચના strbuf_child_entry *child, શોધ = { .c = c };
                child = typesafe_bsearch(&શોધ, ગાંઠ->children, ગાંઠ->children_ગણતરી, strbuf_children_cmp);
                જો (!child)
                        તોડો;
                ગાંઠ = child->child;
        }

        /* add new string */
        જો (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                પાછા -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ સંરચના strbuf_ગાંઠ *ગાંઠ_child = ખાલી;

        ગાંઠ_child = new(સંરચના strbuf_ગાંઠ, 1);
        જો (!ગાંઠ_child)
                પાછા -ENOMEM;
        *ગાંઠ_child = (સંરચના strbuf_ગાંઠ) {
                .કિંમત_off = off,
                .કિંમત_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        જો (!GREEDY_REALLOC(ગાંઠ->children, ગાંઠ->children_ગણતરી + 1))
                પાછા -ENOMEM;

        str->nodes_ગણતરી++;

        bubbleinsert(ગાંઠ, c, TAKE_PTR(ગાંઠ_child));

        પાછા off;
}
