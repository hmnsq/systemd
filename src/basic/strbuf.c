/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "sort-util.h"
#ଅନ୍ତର୍ଭାଵ "strbuf.h"

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

ସଂରଚନା strbuf* strbuf_new(ଶୂନ୍ୟ) {
        _cleanup_(strbuf_freep) ସଂରଚନା strbuf *str = ଖାଲି;

        str = new(ସଂରଚନା strbuf, 1);
        ଯଦି (!str)
                ଫେରନ୍ତୁ ଖାଲି;

        *str = (ସଂରଚନା strbuf) {
                .buf = new0(ଅକ୍ଷର, 1),
                .ମୂଳ = new0(ସଂରଚନା strbuf_ଗଣ୍ଠି, 1),
                .len = 1,
                .nodes_ଗଣନା = 1,
        };
        ଯଦି (!str->buf || !str->ମୂଳ)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ TAKE_PTR(str);
}

ସ୍ଥିର ସଂରଚନା strbuf_ଗଣ୍ଠି* strbuf_ଗଣ୍ଠି_cleanup(ସଂରଚନା strbuf_ଗଣ୍ଠି *ଗଣ୍ଠି) {
        ପ୍ରତିଜ୍ଞା(ଗଣ୍ଠି);

        FOREACH_ARRAY(child, ଗଣ୍ଠି->children, ଗଣ୍ଠି->children_ଗଣନା)
                strbuf_ଗଣ୍ଠି_cleanup(child->child);

        ମୁକ୍ତ(ଗଣ୍ଠି->children);
        ଫେରନ୍ତୁ mfree(ଗଣ୍ଠି);
}

/* clean up trie data, leave only the string buffer */
ଶୂନ୍ୟ strbuf_complete(ସଂରଚନା strbuf *str) {
        ଯଦି (!str || !str->ମୂଳ)
                ଫେରନ୍ତୁ;

        str->ମୂଳ = strbuf_ଗଣ୍ଠି_cleanup(str->ମୂଳ);
}

/* clean up everything */
ସଂରଚନା strbuf* strbuf_free(ସଂରଚନା strbuf *str) {
        ଯଦି (!str)
                ଫେରନ୍ତୁ ଖାଲି;

        strbuf_complete(str);
        ମୁକ୍ତ(str->buf);
        ଫେରନ୍ତୁ mfree(str);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା strbuf_children_cmp(ଅଚଳ ସଂରଚନା strbuf_child_entry *n1, ଅଚଳ ସଂରଚନା strbuf_child_entry *n2) {
        ପ୍ରତିଜ୍ଞା(n1);
        ପ୍ରତିଜ୍ଞା(n2);

        ଫେରନ୍ତୁ CMP(n1->c, n2->c);
}

ସ୍ଥିର ଶୂନ୍ୟ bubbleinsert(ସଂରଚନା strbuf_ଗଣ୍ଠି *ଗଣ୍ଠି,
                         uint8_t c,
                         ସଂରଚନା strbuf_ଗଣ୍ଠି *ଗଣ୍ଠି_child) {

        ସଂରଚନା strbuf_child_entry new = {
                .c = c,
                .child = ଗଣ୍ଠି_child,
        };
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ = 0, ଡାହାଣ = ASSERT_PTR(ଗଣ୍ଠି)->children_ଗଣନା;

        ଯେତେବେଳେ (ଡାହାଣ > ବାମ) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମଧ୍ୟ = (ଡାହାଣ + ବାମ) / 2 ;
                ଯଦି (strbuf_children_cmp(&ଗଣ୍ଠି->children[ମଧ୍ୟ], &new) <= 0)
                        ବାମ = ମଧ୍ୟ + 1;
                ଅନ୍ୟଥା
                        ଡାହାଣ = ମଧ୍ୟ;
        }

        memmove(ଗଣ୍ଠି->children + ବାମ + 1, ଗଣ୍ଠି->children + ବାମ,
                ଆକାର(ସଂରଚନା strbuf_child_entry) * (ଗଣ୍ଠି->children_ଗଣନା - ବାମ));
        ଗଣ୍ଠି->children[ବାମ] = new;

        ଗଣ୍ଠି->children_ଗଣନା++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(ସଂରଚନା strbuf *str, ଅଚଳ ଅକ୍ଷର *s, size_t len) {
        uint8_t c;
        ssize_t off;

        ପ୍ରତିଜ୍ଞା(str);
        ପ୍ରତିଜ୍ଞା(s || len == 0);

        ଯଦି (len == SIZE_MAX)
                len = strlen(s);

        ଯଦି (!str->ମୂଳ)
                ଫେରନ୍ତୁ -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_ଗଣନା++;
        ଯଦି (len == 0) {
                str->dedup_ଗଣନା++;
                ଫେରନ୍ତୁ 0;
        }
        str->in_len += len;

        ସଂରଚନା strbuf_ଗଣ୍ଠି *ଗଣ୍ଠି = str->ମୂଳ;
        ପାଇଁ (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = ଗଣ୍ଠି->ମୂଲ୍ୟ_off + ଗଣ୍ଠି->ମୂଲ୍ୟ_len - len;
                ଯଦି (depth == len || (ଗଣ୍ଠି->ମୂଲ୍ୟ_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_ଗଣନା++;
                        ଫେରନ୍ତୁ off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                ସଂରଚନା strbuf_child_entry *child, ସନ୍ଧାନ = { .c = c };
                child = typesafe_bsearch(&ସନ୍ଧାନ, ଗଣ୍ଠି->children, ଗଣ୍ଠି->children_ଗଣନା, strbuf_children_cmp);
                ଯଦି (!child)
                        ଭାଙ୍ଗ;
                ଗଣ୍ଠି = child->child;
        }

        /* add new string */
        ଯଦି (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                ଫେରନ୍ତୁ -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ ସଂରଚନା strbuf_ଗଣ୍ଠି *ଗଣ୍ଠି_child = ଖାଲି;

        ଗଣ୍ଠି_child = new(ସଂରଚନା strbuf_ଗଣ୍ଠି, 1);
        ଯଦି (!ଗଣ୍ଠି_child)
                ଫେରନ୍ତୁ -ENOMEM;
        *ଗଣ୍ଠି_child = (ସଂରଚନା strbuf_ଗଣ୍ଠି) {
                .ମୂଲ୍ୟ_off = off,
                .ମୂଲ୍ୟ_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        ଯଦି (!GREEDY_REALLOC(ଗଣ୍ଠି->children, ଗଣ୍ଠି->children_ଗଣନା + 1))
                ଫେରନ୍ତୁ -ENOMEM;

        str->nodes_ଗଣନା++;

        bubbleinsert(ଗଣ୍ଠି, c, TAKE_PTR(ଗଣ୍ଠି_child));

        ଫେରନ୍ତୁ off;
}
