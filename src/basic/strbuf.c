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
        إذا (!str)
                أرجع عدم;

        *str = (بنية strbuf) {
                .buf = new0(حرف, 1),
                .ریشه = new0(بنية strbuf_گره, 1),
                .len = 1,
                .nodes_شمار = 1,
        };
        إذا (!str->buf || !str->ریشه)
                أرجع عدم;

        أرجع TAKE_PTR(str);
}

ساكن بنية strbuf_گره* strbuf_گره_cleanup(بنية strbuf_گره *گره) {
        أكد(گره);

        FOREACH_ARRAY(فرزند, گره->children, گره->children_شمار)
                strbuf_گره_cleanup(فرزند->فرزند);

        حرر(گره->children);
        أرجع mfree(گره);
}

/* clean up trie data, leave only the string buffer */
فراغ strbuf_complete(بنية strbuf *str) {
        إذا (!str || !str->ریشه)
                أرجع;

        str->ریشه = strbuf_گره_cleanup(str->ریشه);
}

/* clean up everything */
بنية strbuf* strbuf_free(بنية strbuf *str) {
        إذا (!str)
                أرجع عدم;

        strbuf_complete(str);
        حرر(str->buf);
        أرجع mfree(str);
}

ساكن صحيح strbuf_children_cmp(ثابت بنية strbuf_فرزند_مدخل *n1, ثابت بنية strbuf_فرزند_مدخل *n2) {
        أكد(n1);
        أكد(n2);

        أرجع CMP(n1->c, n2->c);
}

ساكن فراغ bubbleinsert(بنية strbuf_گره *گره,
                         uint8_t c,
                         بنية strbuf_گره *گره_فرزند) {

        بنية strbuf_فرزند_مدخل new = {
                .c = c,
                .فرزند = گره_فرزند,
        };
        صحيح چپ = 0, راست = ASSERT_PTR(گره)->children_شمار;

        طالما (راست > چپ) {
                صحيح میانه = (راست + چپ) / 2 ;
                إذا (strbuf_children_cmp(&گره->children[میانه], &new) <= 0)
                        چپ = میانه + 1;
                وإلا
                        راست = میانه;
        }

        memmove(گره->children + چپ + 1, گره->children + چپ,
                حجم(بنية strbuf_فرزند_مدخل) * (گره->children_شمار - چپ));
        گره->children[چپ] = new;

        گره->children_شمار++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_جمع_کن_string_پر(بنية strbuf *str, ثابت حرف *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        إذا (len == SIZE_MAX)
                len = طول_رشته(s);

        إذا (!str->ریشه)
                أرجع -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->درون_شمار++;
        إذا (len == 0) {
                str->dedup_شمار++;
                أرجع 0;
        }
        str->درون_len += len;

        بنية strbuf_گره *گره = str->ریشه;
        لكل (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = گره->مقدار_off + گره->مقدار_len - len;
                إذا (depth == len || (گره->مقدار_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_شمار++;
                        أرجع off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                بنية strbuf_فرزند_مدخل *فرزند, جستجو = { .c = c };
                فرزند = typesafe_bsearch(&جستجو, گره->children, گره->children_شمار, strbuf_children_cmp);
                إذا (!فرزند)
                        اكسر;
                گره = فرزند->فرزند;
        }

        /* add new string */
        إذا (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                أرجع -ENOMEM;
        off = str->len;
        کپی_حافظه(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ بنية strbuf_گره *گره_فرزند = عدم;

        گره_فرزند = new(بنية strbuf_گره, 1);
        إذا (!گره_فرزند)
                أرجع -ENOMEM;
        *گره_فرزند = (بنية strbuf_گره) {
                .مقدار_off = off,
                .مقدار_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        إذا (!GREEDY_REALLOC(گره->children, گره->children_شمار + 1))
                أرجع -ENOMEM;

        str->nodes_شمار++;

        bubbleinsert(گره, c, TAKE_PTR(گره_فرزند));

        أرجع off;
}
