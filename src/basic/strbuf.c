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
                .ریښه = new0(بنية strbuf_غوټه, 1),
                .len = 1,
                .nodes_شمېر = 1,
        };
        إذا (!str->buf || !str->ریښه)
                أرجع عدم;

        أرجع TAKE_PTR(str);
}

ساكن بنية strbuf_غوټه* strbuf_غوټه_cleanup(بنية strbuf_غوټه *غوټه) {
        أكد(غوټه);

        FOREACH_ARRAY(ماشوم, غوټه->children, غوټه->children_شمېر)
                strbuf_غوټه_cleanup(ماشوم->ماشوم);

        حرر(غوټه->children);
        أرجع mfree(غوټه);
}

/* clean up trie data, leave only the string buffer */
فراغ strbuf_complete(بنية strbuf *str) {
        إذا (!str || !str->ریښه)
                أرجع;

        str->ریښه = strbuf_غوټه_cleanup(str->ریښه);
}

/* clean up everything */
بنية strbuf* strbuf_free(بنية strbuf *str) {
        إذا (!str)
                أرجع عدم;

        strbuf_complete(str);
        حرر(str->buf);
        أرجع mfree(str);
}

ساكن صحيح strbuf_children_cmp(ثابت بنية strbuf_ماشوم_ثبت *n1, ثابت بنية strbuf_ماشوم_ثبت *n2) {
        أكد(n1);
        أكد(n2);

        أرجع CMP(n1->c, n2->c);
}

ساكن فراغ bubbleinsert(بنية strbuf_غوټه *غوټه,
                         uint8_t c,
                         بنية strbuf_غوټه *غوټه_ماشوم) {

        بنية strbuf_ماشوم_ثبت new = {
                .c = c,
                .ماشوم = غوټه_ماشوم,
        };
        صحيح کیڼ = 0, ښی = ASSERT_PTR(غوټه)->children_شمېر;

        طالما (ښی > کیڼ) {
                صحيح منځ = (ښی + کیڼ) / 2 ;
                إذا (strbuf_children_cmp(&غوټه->children[منځ], &new) <= 0)
                        کیڼ = منځ + 1;
                وإلا
                        ښی = منځ;
        }

        memmove(غوټه->children + کیڼ + 1, غوټه->children + کیڼ,
                حجم(بنية strbuf_ماشوم_ثبت) * (غوټه->children_شمېر - کیڼ));
        غوټه->children[کیڼ] = new;

        غوټه->children_شمېر++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_جمع_کړه_string_ډک(بنية strbuf *str, ثابت حرف *s, size_t len) {
        uint8_t c;
        ssize_t off;

        أكد(str);
        أكد(s || len == 0);

        إذا (len == SIZE_MAX)
                len = مزي_اوږدوالی(s);

        إذا (!str->ریښه)
                أرجع -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->دننه_شمېر++;
        إذا (len == 0) {
                str->dedup_شمېر++;
                أرجع 0;
        }
        str->دننه_len += len;

        بنية strbuf_غوټه *غوټه = str->ریښه;
        لكل (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = غوټه->ارزښت_off + غوټه->ارزښت_len - len;
                إذا (depth == len || (غوټه->ارزښت_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_شمېر++;
                        أرجع off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                بنية strbuf_ماشوم_ثبت *ماشوم, پلټنه = { .c = c };
                ماشوم = typesafe_bsearch(&پلټنه, غوټه->children, غوټه->children_شمېر, strbuf_children_cmp);
                إذا (!ماشوم)
                        اكسر;
                غوټه = ماشوم->ماشوم;
        }

        /* add new string */
        إذا (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                أرجع -ENOMEM;
        off = str->len;
        حافظه_نقل(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ بنية strbuf_غوټه *غوټه_ماشوم = عدم;

        غوټه_ماشوم = new(بنية strbuf_غوټه, 1);
        إذا (!غوټه_ماشوم)
                أرجع -ENOMEM;
        *غوټه_ماشوم = (بنية strbuf_غوټه) {
                .ارزښت_off = off,
                .ارزښت_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        إذا (!GREEDY_REALLOC(غوټه->children, غوټه->children_شمېر + 1))
                أرجع -ENOMEM;

        str->nodes_شمېر++;

        bubbleinsert(غوټه, c, TAKE_PTR(غوټه_ماشوم));

        أرجع off;
}
