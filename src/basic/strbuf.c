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

sa_nracanaa strbuf* strbuf_new(void) {
        _cleanup_(strbuf_freep) sa_nracanaa strbuf *str = NULL;

        str = new(sa_nracanaa strbuf, 1);
        གལ་སྲིད (!str)
                ལོག NULL;

        *str = (sa_nracanaa strbuf) {
                .buf = new0(var_na, 1),
                .root = new0(sa_nracanaa strbuf_node, 1),
                .len = 1,
                .nodes_count = 1,
        };
        གལ་སྲིད (!str->buf || !str->root)
                ལོག NULL;

        ལོག TAKE_PTR(str);
}

static sa_nracanaa strbuf_node* strbuf_node_cleanup(sa_nracanaa strbuf_node *node) {
        assert(node);

        FOREACH_ARRAY(child, node->children, node->children_count)
                strbuf_node_cleanup(child->child);

        free(node->children);
        ལོག mfree(node);
}

/* clean up trie data, leave only the string buffer */
void strbuf_complete(sa_nracanaa strbuf *str) {
        གལ་སྲིད (!str || !str->root)
                ལོག;

        str->root = strbuf_node_cleanup(str->root);
}

/* clean up everything */
sa_nracanaa strbuf* strbuf_free(sa_nracanaa strbuf *str) {
        གལ་སྲིད (!str)
                ལོག NULL;

        strbuf_complete(str);
        free(str->buf);
        ལོག mfree(str);
}

static ཧྲིལ strbuf_children_cmp(const sa_nracanaa strbuf_child_entry *n1, const sa_nracanaa strbuf_child_entry *n2) {
        assert(n1);
        assert(n2);

        ལོག CMP(n1->c, n2->c);
}

static void bubbleinsert(sa_nracanaa strbuf_node *node,
                         uint8_t c,
                         sa_nracanaa strbuf_node *node_child) {

        sa_nracanaa strbuf_child_entry new = {
                .c = c,
                .child = node_child,
        };
        ཧྲིལ left = 0, right = ASSERT_PTR(node)->children_count;

        སྐབས (right > left) {
                ཧྲིལ middle = (right + left) / 2 ;
                གལ་སྲིད (strbuf_children_cmp(&node->children[middle], &new) <= 0)
                        left = middle + 1;
                དེ་མིན
                        right = middle;
        }

        memmove(node->children + left + 1, node->children + left,
                parimaa_na(sa_nracanaa strbuf_child_entry) * (node->children_count - left));
        node->children[left] = new;

        node->children_count++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(sa_nracanaa strbuf *str, const var_na *s, size_t len) {
        uint8_t c;
        ssize_t off;

        assert(str);
        assert(s || len == 0);

        གལ་སྲིད (len == SIZE_MAX)
                len = strlen(s);

        གལ་སྲིད (!str->root)
                ལོག -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_count++;
        གལ་སྲིད (len == 0) {
                str->dedup_count++;
                ལོག 0;
        }
        str->in_len += len;

        sa_nracanaa strbuf_node *node = str->root;
        དོན་ལུ (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = node->value_off + node->value_len - len;
                གལ་སྲིད (depth == len || (node->value_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_count++;
                        ལོག off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                sa_nracanaa strbuf_child_entry *child, search = { .c = c };
                child = typesafe_bsearch(&search, node->children, node->children_count, strbuf_children_cmp);
                གལ་སྲིད (!child)
                        bha_nga;
                node = child->child;
        }

        /* add new string */
        གལ་སྲིད (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                ལོག -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ sa_nracanaa strbuf_node *node_child = NULL;

        node_child = new(sa_nracanaa strbuf_node, 1);
        གལ་སྲིད (!node_child)
                ལོག -ENOMEM;
        *node_child = (sa_nracanaa strbuf_node) {
                .value_off = off,
                .value_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        གལ་སྲིད (!GREEDY_REALLOC(node->children, node->children_count + 1))
                ལོག -ENOMEM;

        str->nodes_count++;

        bubbleinsert(node, c, TAKE_PTR(node_child));

        ལོག off;
}
