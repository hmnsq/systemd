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
        ܐܢ (!str)
                ܦܢܐ عدم;

        *str = (بنية strbuf) {
                .buf = new0(حرف, 1),
                .root = new0(بنية strbuf_node, 1),
                .len = 1,
                .nodes_count = 1,
        };
        ܐܢ (!str->buf || !str->root)
                ܦܢܐ عدم;

        ܦܢܐ TAKE_PTR(str);
}

ساكن بنية strbuf_node* strbuf_node_cleanup(بنية strbuf_node *node) {
        أكد(node);

        FOREACH_ARRAY(child, node->children, node->children_count)
                strbuf_node_cleanup(child->child);

        حرر(node->children);
        ܦܢܐ mfree(node);
}

/* clean up trie data, leave only the string buffer */
فراغ strbuf_complete(بنية strbuf *str) {
        ܐܢ (!str || !str->root)
                ܦܢܐ;

        str->root = strbuf_node_cleanup(str->root);
}

/* clean up everything */
بنية strbuf* strbuf_free(بنية strbuf *str) {
        ܐܢ (!str)
                ܦܢܐ عدم;

        strbuf_complete(str);
        حرر(str->buf);
        ܦܢܐ mfree(str);
}

ساكن صحيح strbuf_children_cmp(ثابت بنية strbuf_child_entry *n1, ثابت بنية strbuf_child_entry *n2) {
        أكد(n1);
        أكد(n2);

        ܦܢܐ CMP(n1->c, n2->c);
}

ساكن فراغ bubbleinsert(بنية strbuf_node *node,
                         uint8_t c,
                         بنية strbuf_node *node_child) {

        بنية strbuf_child_entry new = {
                .c = c,
                .child = node_child,
        };
        صحيح left = 0, right = ASSERT_PTR(node)->children_count;

        ܟܡܐ (right > left) {
                صحيح middle = (right + left) / 2 ;
                ܐܢ (strbuf_children_cmp(&node->children[middle], &new) <= 0)
                        left = middle + 1;
                ܐܚܪܢܐ
                        right = middle;
        }

        memmove(node->children + left + 1, node->children + left,
                حجم(بنية strbuf_child_entry) * (node->children_count - left));
        node->children[left] = new;

        node->children_count++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(بنية strbuf *str, ثابت حرف *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        ܐܢ (len == SIZE_MAX)
                len = strlen(s);

        ܐܢ (!str->root)
                ܦܢܐ -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_count++;
        ܐܢ (len == 0) {
                str->dedup_count++;
                ܦܢܐ 0;
        }
        str->in_len += len;

        بنية strbuf_node *node = str->root;
        لكل (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = node->value_off + node->value_len - len;
                ܐܢ (depth == len || (node->value_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_count++;
                        ܦܢܐ off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                بنية strbuf_child_entry *child, search = { .c = c };
                child = typesafe_bsearch(&search, node->children, node->children_count, strbuf_children_cmp);
                ܐܢ (!child)
                        اكسر;
                node = child->child;
        }

        /* add new string */
        ܐܢ (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                ܦܢܐ -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ بنية strbuf_node *node_child = عدم;

        node_child = new(بنية strbuf_node, 1);
        ܐܢ (!node_child)
                ܦܢܐ -ENOMEM;
        *node_child = (بنية strbuf_node) {
                .value_off = off,
                .value_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        ܐܢ (!GREEDY_REALLOC(node->children, node->children_count + 1))
                ܦܢܐ -ENOMEM;

        str->nodes_count++;

        bubbleinsert(node, c, TAKE_PTR(node_child));

        ܦܢܐ off;
}
