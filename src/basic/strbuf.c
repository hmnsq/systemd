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
        if (!str)
                return NULL;

        *str = (struct strbuf) {
                .buf = new0(char, 1),
                .उगात = new0(struct strbuf_बुहोल, 1),
                .len = 1,
                .nodes_बिलांग = 1,
        };
        if (!str->buf || !str->उगात)
                return NULL;

        return TAKE_PTR(str);
}

static struct strbuf_बुहोल* strbuf_बुहोल_cleanup(struct strbuf_बुहोल *बुहोल) {
        assert(बुहोल);

        FOREACH_ARRAY(child, बुहोल->children, बुहोल->children_बिलांग)
                strbuf_बुहोल_cleanup(child->child);

        free(बुहोल->children);
        return mfree(बुहोल);
}

/* clean up trie data, leave only the string buffer */
void strbuf_complete(struct strbuf *str) {
        if (!str || !str->उगात)
                return;

        str->उगात = strbuf_बुहोल_cleanup(str->उगात);
}

/* clean up everything */
struct strbuf* strbuf_free(struct strbuf *str) {
        if (!str)
                return NULL;

        strbuf_complete(str);
        free(str->buf);
        return mfree(str);
}

static int strbuf_children_cmp(const struct strbuf_child_entry *n1, const struct strbuf_child_entry *n2) {
        assert(n1);
        assert(n2);

        return CMP(n1->c, n2->c);
}

static void bubbleinsert(struct strbuf_बुहोल *बुहोल,
                         uint8_t c,
                         struct strbuf_बुहोल *बुहोल_child) {

        struct strbuf_child_entry new = {
                .c = c,
                .child = बुहोल_child,
        };
        int कालिवा = 0, कानान = ASSERT_PTR(बुहोल)->children_बिलांग;

        while (कानान > कालिवा) {
                int गित्ना = (कानान + कालिवा) / 2 ;
                if (strbuf_children_cmp(&बुहोल->children[गित्ना], &new) <= 0)
                        कालिवा = गित्ना + 1;
                else
                        कानान = गित्ना;
        }

        memmove(बुहोल->children + कालिवा + 1, बुहोल->children + कालिवा,
                sizeof(struct strbuf_child_entry) * (बुहोल->children_बिलांग - कालिवा));
        बुहोल->children[कालिवा] = new;

        बुहोल->children_बिलांग++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(struct strbuf *str, const char *s, size_t len) {
        uint8_t c;
        ssize_t off;

        assert(str);
        assert(s || len == 0);

        if (len == SIZE_MAX)
                len = strlen(s);

        if (!str->उगात)
                return -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_बिलांग++;
        if (len == 0) {
                str->dedup_बिलांग++;
                return 0;
        }
        str->in_len += len;

        struct strbuf_बुहोल *बुहोल = str->उगात;
        for (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = बुहोल->हालागा_off + बुहोल->हालागा_len - len;
                if (depth == len || (बुहोल->हालागा_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_बिलांग++;
                        return off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                struct strbuf_child_entry *child, हानाप = { .c = c };
                child = typesafe_bsearch(&हानाप, बुहोल->children, बुहोल->children_बिलांग, strbuf_children_cmp);
                if (!child)
                        break;
                बुहोल = child->child;
        }

        /* add new string */
        if (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                return -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ struct strbuf_बुहोल *बुहोल_child = NULL;

        बुहोल_child = new(struct strbuf_बुहोल, 1);
        if (!बुहोल_child)
                return -ENOMEM;
        *बुहोल_child = (struct strbuf_बुहोल) {
                .हालागा_off = off,
                .हालागा_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        if (!GREEDY_REALLOC(बुहोल->children, बुहोल->children_बिलांग + 1))
                return -ENOMEM;

        str->nodes_बिलांग++;

        bubbleinsert(बुहोल, c, TAKE_PTR(बुहोल_child));

        return off;
}
