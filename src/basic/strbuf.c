/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>

#include "alloc-util.h"
#include "sort-util.h"
#include "strbuf.h"

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

struct strbuf* strbuf_new(void) {
        _cleanup_(strbuf_freep) struct strbuf *str = NULL;

        str = new(struct strbuf, 1);
        nếu (!str)
                trả_về NULL;

        *str = (struct strbuf) {
                .buf = new0(char, 1),
                .root = new0(struct strbuf_node, 1),
                .len = 1,
                .nodes_count = 1,
        };
        nếu (!str->buf || !str->root)
                trả_về NULL;

        trả_về TAKE_PTR(str);
}

static struct strbuf_node* strbuf_node_cleanup(struct strbuf_node *node) {
        assert(node);

        FOREACH_ARRAY(child, node->children, node->children_count)
                strbuf_node_cleanup(child->child);

        free(node->children);
        trả_về mfree(node);
}

/* clean up trie data, leave only the string buffer */
void strbuf_complete(struct strbuf *str) {
        nếu (!str || !str->root)
                trả_về;

        str->root = strbuf_node_cleanup(str->root);
}

/* clean up everything */
struct strbuf* strbuf_free(struct strbuf *str) {
        nếu (!str)
                trả_về NULL;

        strbuf_complete(str);
        free(str->buf);
        trả_về mfree(str);
}

static số_nguyên strbuf_children_cmp(const struct strbuf_child_entry *n1, const struct strbuf_child_entry *n2) {
        assert(n1);
        assert(n2);

        trả_về CMP(n1->c, n2->c);
}

static void bubbleinsert(struct strbuf_node *node,
                         uint8_t c,
                         struct strbuf_node *node_child) {

        struct strbuf_child_entry new = {
                .c = c,
                .child = node_child,
        };
        số_nguyên left = 0, right = ASSERT_PTR(node)->children_count;

        trong_khi (right > left) {
                số_nguyên middle = (right + left) / 2 ;
                nếu (strbuf_children_cmp(&node->children[middle], &new) <= 0)
                        left = middle + 1;
                không_thì
                        right = middle;
        }

        memmove(node->children + left + 1, node->children + left,
                sizeof(struct strbuf_child_entry) * (node->children_count - left));
        node->children[left] = new;

        node->children_count++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(struct strbuf *str, const char *s, size_t len) {
        uint8_t c;
        ssize_t off;

        assert(str);
        assert(s || len == 0);

        nếu (len == SIZE_MAX)
                len = strlen(s);

        nếu (!str->root)
                trả_về -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_count++;
        nếu (len == 0) {
                str->dedup_count++;
                trả_về 0;
        }
        str->in_len += len;

        struct strbuf_node *node = str->root;
        cho (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = node->value_off + node->value_len - len;
                nếu (depth == len || (node->value_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_count++;
                        trả_về off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                struct strbuf_child_entry *child, search = { .c = c };
                child = typesafe_bsearch(&search, node->children, node->children_count, strbuf_children_cmp);
                nếu (!child)
                        break;
                node = child->child;
        }

        /* add new string */
        nếu (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                trả_về -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ struct strbuf_node *node_child = NULL;

        node_child = new(struct strbuf_node, 1);
        nếu (!node_child)
                trả_về -ENOMEM;
        *node_child = (struct strbuf_node) {
                .value_off = off,
                .value_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        nếu (!GREEDY_REALLOC(node->children, node->children_count + 1))
                trả_về -ENOMEM;

        str->nodes_count++;

        bubbleinsert(node, c, TAKE_PTR(node_child));

        trả_về off;
}
