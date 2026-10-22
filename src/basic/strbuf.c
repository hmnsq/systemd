/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <string.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "sort-util.h"
#அந்தர்பாவ "strbuf.h"

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

கட்டமைப்பு strbuf* strbuf_new(வெறுமை) {
        _cleanup_(strbuf_freep) கட்டமைப்பு strbuf *str = வெற்று;

        str = new(கட்டமைப்பு strbuf, 1);
        எனில் (!str)
                திரும்பு வெற்று;

        *str = (கட்டமைப்பு strbuf) {
                .buf = new0(எழுத்து, 1),
                .வேர் = new0(கட்டமைப்பு strbuf_முனை, 1),
                .len = 1,
                .nodes_மொத்தம் = 1,
        };
        எனில் (!str->buf || !str->வேர்)
                திரும்பு வெற்று;

        திரும்பு TAKE_PTR(str);
}

நிலையான கட்டமைப்பு strbuf_முனை* strbuf_முனை_cleanup(கட்டமைப்பு strbuf_முனை *முனை) {
        ப்ரதிஜ்ஞா(முனை);

        FOREACH_ARRAY(child, முனை->children, முனை->children_மொத்தம்)
                strbuf_முனை_cleanup(child->child);

        விடுவி(முனை->children);
        திரும்பு mfree(முனை);
}

/* clean up trie data, leave only the string buffer */
வெறுமை strbuf_complete(கட்டமைப்பு strbuf *str) {
        எனில் (!str || !str->வேர்)
                திரும்பு;

        str->வேர் = strbuf_முனை_cleanup(str->வேர்);
}

/* clean up everything */
கட்டமைப்பு strbuf* strbuf_free(கட்டமைப்பு strbuf *str) {
        எனில் (!str)
                திரும்பு வெற்று;

        strbuf_complete(str);
        விடுவி(str->buf);
        திரும்பு mfree(str);
}

நிலையான முழு strbuf_children_cmp(மாறிலி கட்டமைப்பு strbuf_child_entry *n1, மாறிலி கட்டமைப்பு strbuf_child_entry *n2) {
        ப்ரதிஜ்ஞா(n1);
        ப்ரதிஜ்ஞா(n2);

        திரும்பு CMP(n1->c, n2->c);
}

நிலையான வெறுமை bubbleinsert(கட்டமைப்பு strbuf_முனை *முனை,
                         uint8_t c,
                         கட்டமைப்பு strbuf_முனை *முனை_child) {

        கட்டமைப்பு strbuf_child_entry new = {
                .c = c,
                .child = முனை_child,
        };
        முழு இடது = 0, வலது = ASSERT_PTR(முனை)->children_மொத்தம்;

        போது (வலது > இடது) {
                முழு நடு = (வலது + இடது) / 2 ;
                எனில் (strbuf_children_cmp(&முனை->children[நடு], &new) <= 0)
                        இடது = நடு + 1;
                இல்லையெனில்
                        வலது = நடு;
        }

        memmove(முனை->children + இடது + 1, முனை->children + இடது,
                அளவு(கட்டமைப்பு strbuf_child_entry) * (முனை->children_மொத்தம் - இடது));
        முனை->children[இடது] = new;

        முனை->children_மொத்தம்++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(கட்டமைப்பு strbuf *str, மாறிலி எழுத்து *s, size_t len) {
        uint8_t c;
        ssize_t off;

        ப்ரதிஜ்ஞா(str);
        ப்ரதிஜ்ஞா(s || len == 0);

        எனில் (len == SIZE_MAX)
                len = strlen(s);

        எனில் (!str->வேர்)
                திரும்பு -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_மொத்தம்++;
        எனில் (len == 0) {
                str->dedup_மொத்தம்++;
                திரும்பு 0;
        }
        str->in_len += len;

        கட்டமைப்பு strbuf_முனை *முனை = str->வேர்;
        க்கு (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = முனை->மதிப்பு_off + முனை->மதிப்பு_len - len;
                எனில் (depth == len || (முனை->மதிப்பு_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_மொத்தம்++;
                        திரும்பு off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                கட்டமைப்பு strbuf_child_entry *child, தேடல் = { .c = c };
                child = typesafe_bsearch(&தேடல், முனை->children, முனை->children_மொத்தம், strbuf_children_cmp);
                எனில் (!child)
                        நிறுத்து;
                முனை = child->child;
        }

        /* add new string */
        எனில் (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                திரும்பு -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ கட்டமைப்பு strbuf_முனை *முனை_child = வெற்று;

        முனை_child = new(கட்டமைப்பு strbuf_முனை, 1);
        எனில் (!முனை_child)
                திரும்பு -ENOMEM;
        *முனை_child = (கட்டமைப்பு strbuf_முனை) {
                .மதிப்பு_off = off,
                .மதிப்பு_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        எனில் (!GREEDY_REALLOC(முனை->children, முனை->children_மொத்தம் + 1))
                திரும்பு -ENOMEM;

        str->nodes_மொத்தம்++;

        bubbleinsert(முனை, c, TAKE_PTR(முனை_child));

        திரும்பு off;
}
