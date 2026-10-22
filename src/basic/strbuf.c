/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <string.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "sort-util.h"
#অন্তর্ভাব "strbuf.h"

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

কাঠামো strbuf* strbuf_new(শূন্য) {
        _cleanup_(strbuf_freep) কাঠামো strbuf *str = ফাঁকা;

        str = new(কাঠামো strbuf, 1);
        যদি (!str)
                ফেরত ফাঁকা;

        *str = (কাঠামো strbuf) {
                .buf = new0(অক্ষর, 1),
                .মূল = new0(কাঠামো strbuf_গ্রন্থি, 1),
                .len = 1,
                .nodes_গুনতি = 1,
        };
        যদি (!str->buf || !str->মূল)
                ফেরত ফাঁকা;

        ফেরত TAKE_PTR(str);
}

স্থির কাঠামো strbuf_গ্রন্থি* strbuf_গ্রন্থি_cleanup(কাঠামো strbuf_গ্রন্থি *গ্রন্থি) {
        প্রতিজ্ঞা(গ্রন্থি);

        FOREACH_ARRAY(child, গ্রন্থি->children, গ্রন্থি->children_গুনতি)
                strbuf_গ্রন্থি_cleanup(child->child);

        মুক্ত(গ্রন্থি->children);
        ফেরত mfree(গ্রন্থি);
}

/* clean up trie data, leave only the string buffer */
শূন্য strbuf_complete(কাঠামো strbuf *str) {
        যদি (!str || !str->মূল)
                ফেরত;

        str->মূল = strbuf_গ্রন্থি_cleanup(str->মূল);
}

/* clean up everything */
কাঠামো strbuf* strbuf_free(কাঠামো strbuf *str) {
        যদি (!str)
                ফেরত ফাঁকা;

        strbuf_complete(str);
        মুক্ত(str->buf);
        ফেরত mfree(str);
}

স্থির পূর্ণসংখ্যা strbuf_children_cmp(ধ্রুবক কাঠামো strbuf_child_entry *n1, ধ্রুবক কাঠামো strbuf_child_entry *n2) {
        প্রতিজ্ঞা(n1);
        প্রতিজ্ঞা(n2);

        ফেরত CMP(n1->c, n2->c);
}

স্থির শূন্য bubbleinsert(কাঠামো strbuf_গ্রন্থি *গ্রন্থি,
                         uint8_t c,
                         কাঠামো strbuf_গ্রন্থি *গ্রন্থি_child) {

        কাঠামো strbuf_child_entry new = {
                .c = c,
                .child = গ্রন্থি_child,
        };
        পূর্ণসংখ্যা বাম = 0, ডান = ASSERT_PTR(গ্রন্থি)->children_গুনতি;

        যখন (ডান > বাম) {
                পূর্ণসংখ্যা মধ্য = (ডান + বাম) / 2 ;
                যদি (strbuf_children_cmp(&গ্রন্থি->children[মধ্য], &new) <= 0)
                        বাম = মধ্য + 1;
                অন্যথা
                        ডান = মধ্য;
        }

        memmove(গ্রন্থি->children + বাম + 1, গ্রন্থি->children + বাম,
                আকার(কাঠামো strbuf_child_entry) * (গ্রন্থি->children_গুনতি - বাম));
        গ্রন্থি->children[বাম] = new;

        গ্রন্থি->children_গুনতি++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(কাঠামো strbuf *str, ধ্রুবক অক্ষর *s, size_t len) {
        uint8_t c;
        ssize_t off;

        প্রতিজ্ঞা(str);
        প্রতিজ্ঞা(s || len == 0);

        যদি (len == SIZE_MAX)
                len = strlen(s);

        যদি (!str->মূল)
                ফেরত -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_গুনতি++;
        যদি (len == 0) {
                str->dedup_গুনতি++;
                ফেরত 0;
        }
        str->in_len += len;

        কাঠামো strbuf_গ্রন্থি *গ্রন্থি = str->মূল;
        জন্য (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = গ্রন্থি->মান_off + গ্রন্থি->মান_len - len;
                যদি (depth == len || (গ্রন্থি->মান_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_গুনতি++;
                        ফেরত off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                কাঠামো strbuf_child_entry *child, অনুসন্ধান = { .c = c };
                child = typesafe_bsearch(&অনুসন্ধান, গ্রন্থি->children, গ্রন্থি->children_গুনতি, strbuf_children_cmp);
                যদি (!child)
                        বিরতি;
                গ্রন্থি = child->child;
        }

        /* add new string */
        যদি (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                ফেরত -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ কাঠামো strbuf_গ্রন্থি *গ্রন্থি_child = ফাঁকা;

        গ্রন্থি_child = new(কাঠামো strbuf_গ্রন্থি, 1);
        যদি (!গ্রন্থি_child)
                ফেরত -ENOMEM;
        *গ্রন্থি_child = (কাঠামো strbuf_গ্রন্থি) {
                .মান_off = off,
                .মান_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        যদি (!GREEDY_REALLOC(গ্রন্থি->children, গ্রন্থি->children_গুনতি + 1))
                ফেরত -ENOMEM;

        str->nodes_গুনতি++;

        bubbleinsert(গ্রন্থি, c, TAKE_PTR(গ্রন্থি_child));

        ফেরত off;
}
