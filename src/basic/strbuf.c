/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <string.h>

#شامل "alloc-util.h"
#شامل "sort-util.h"
#شامل "strbuf.h"

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

ساختار strbuf* strbuf_new(تهی) {
        _cleanup_(strbuf_freep) ساختار strbuf *str = پوچ;

        str = new(ساختار strbuf, 1);
        اگر (!str)
                بازگشت پوچ;

        *str = (ساختار strbuf) {
                .buf = new0(نویسه, 1),
                .ریشه = new0(ساختار strbuf_گره, 1),
                .len = 1,
                .nodes_شمار = 1,
        };
        اگر (!str->buf || !str->ریشه)
                بازگشت پوچ;

        بازگشت TAKE_PTR(str);
}

ایستا ساختار strbuf_گره* strbuf_گره_cleanup(ساختار strbuf_گره *گره) {
        أكد(گره);

        FOREACH_ARRAY(child, گره->children, گره->children_شمار)
                strbuf_گره_cleanup(child->child);

        آزاد(گره->children);
        بازگشت mfree(گره);
}

/* clean up trie data, leave only the string buffer */
تهی strbuf_complete(ساختار strbuf *str) {
        اگر (!str || !str->ریشه)
                بازگشت;

        str->ریشه = strbuf_گره_cleanup(str->ریشه);
}

/* clean up everything */
ساختار strbuf* strbuf_free(ساختار strbuf *str) {
        اگر (!str)
                بازگشت پوچ;

        strbuf_complete(str);
        آزاد(str->buf);
        بازگشت mfree(str);
}

ایستا صحیح strbuf_children_cmp(ثابت ساختار strbuf_child_entry *n1, ثابت ساختار strbuf_child_entry *n2) {
        أكد(n1);
        أكد(n2);

        بازگشت CMP(n1->c, n2->c);
}

ایستا تهی bubbleinsert(ساختار strbuf_گره *گره,
                         uint8_t c,
                         ساختار strbuf_گره *گره_child) {

        ساختار strbuf_child_entry new = {
                .c = c,
                .child = گره_child,
        };
        صحیح چپ = 0, راست = ASSERT_PTR(گره)->children_شمار;

        وقتی (راست > چپ) {
                صحیح میانه = (راست + چپ) / 2 ;
                اگر (strbuf_children_cmp(&گره->children[میانه], &new) <= 0)
                        چپ = میانه + 1;
                وگرنه
                        راست = میانه;
        }

        memmove(گره->children + چپ + 1, گره->children + چپ,
                اندازه(ساختار strbuf_child_entry) * (گره->children_شمار - چپ));
        گره->children[چپ] = new;

        گره->children_شمار++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(ساختار strbuf *str, ثابت نویسه *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        اگر (len == SIZE_MAX)
                len = strlen(s);

        اگر (!str->ریشه)
                بازگشت -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_شمار++;
        اگر (len == 0) {
                str->dedup_شمار++;
                بازگشت 0;
        }
        str->in_len += len;

        ساختار strbuf_گره *گره = str->ریشه;
        برای (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = گره->مقدار_off + گره->مقدار_len - len;
                اگر (depth == len || (گره->مقدار_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_شمار++;
                        بازگشت off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                ساختار strbuf_child_entry *child, جستجو = { .c = c };
                child = typesafe_bsearch(&جستجو, گره->children, گره->children_شمار, strbuf_children_cmp);
                اگر (!child)
                        بشکن;
                گره = child->child;
        }

        /* add new string */
        اگر (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                بازگشت -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ ساختار strbuf_گره *گره_child = پوچ;

        گره_child = new(ساختار strbuf_گره, 1);
        اگر (!گره_child)
                بازگشت -ENOMEM;
        *گره_child = (ساختار strbuf_گره) {
                .مقدار_off = off,
                .مقدار_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        اگر (!GREEDY_REALLOC(گره->children, گره->children_شمار + 1))
                بازگشت -ENOMEM;

        str->nodes_شمار++;

        bubbleinsert(گره, c, TAKE_PTR(گره_child));

        بازگشت off;
}
