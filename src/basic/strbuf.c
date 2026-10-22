/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "sort-util.h"
#അന്തര്ഭാവ "strbuf.h"

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

ഘടന strbuf* strbuf_new(ശൂന്യം) {
        _cleanup_(strbuf_freep) ഘടന strbuf *str = ശൂന്യ;

        str = new(ഘടന strbuf, 1);
        എങ്കിൽ (!str)
                തിരികെ ശൂന്യ;

        *str = (ഘടന strbuf) {
                .buf = new0(അക്ഷരം, 1),
                .വേര് = new0(ഘടന strbuf_കണ്ണി, 1),
                .len = 1,
                .nodes_എണ്ണം = 1,
        };
        എങ്കിൽ (!str->buf || !str->വേര്)
                തിരികെ ശൂന്യ;

        തിരികെ TAKE_PTR(str);
}

സ്ഥിതം ഘടന strbuf_കണ്ണി* strbuf_കണ്ണി_cleanup(ഘടന strbuf_കണ്ണി *കണ്ണി) {
        പ്രതിജ്ഞാ(കണ്ണി);

        FOREACH_ARRAY(child, കണ്ണി->children, കണ്ണി->children_എണ്ണം)
                strbuf_കണ്ണി_cleanup(child->child);

        സ്വതന്ത്രം(കണ്ണി->children);
        തിരികെ mfree(കണ്ണി);
}

/* clean up trie data, leave only the string buffer */
ശൂന്യം strbuf_complete(ഘടന strbuf *str) {
        എങ്കിൽ (!str || !str->വേര്)
                തിരികെ;

        str->വേര് = strbuf_കണ്ണി_cleanup(str->വേര്);
}

/* clean up everything */
ഘടന strbuf* strbuf_free(ഘടന strbuf *str) {
        എങ്കിൽ (!str)
                തിരികെ ശൂന്യ;

        strbuf_complete(str);
        സ്വതന്ത്രം(str->buf);
        തിരികെ mfree(str);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ strbuf_children_cmp(സ്ഥിരം ഘടന strbuf_child_entry *n1, സ്ഥിരം ഘടന strbuf_child_entry *n2) {
        പ്രതിജ്ഞാ(n1);
        പ്രതിജ്ഞാ(n2);

        തിരികെ CMP(n1->c, n2->c);
}

സ്ഥിതം ശൂന്യം bubbleinsert(ഘടന strbuf_കണ്ണി *കണ്ണി,
                         uint8_t c,
                         ഘടന strbuf_കണ്ണി *കണ്ണി_child) {

        ഘടന strbuf_child_entry new = {
                .c = c,
                .child = കണ്ണി_child,
        };
        പൂർണ്ണസംഖ്യ ഇടത് = 0, വലത് = ASSERT_PTR(കണ്ണി)->children_എണ്ണം;

        ആകുമ്പോൾ (വലത് > ഇടത്) {
                പൂർണ്ണസംഖ്യ നടുവ് = (വലത് + ഇടത്) / 2 ;
                എങ്കിൽ (strbuf_children_cmp(&കണ്ണി->children[നടുവ്], &new) <= 0)
                        ഇടത് = നടുവ് + 1;
                അല്ലെങ്കിൽ
                        വലത് = നടുവ്;
        }

        memmove(കണ്ണി->children + ഇടത് + 1, കണ്ണി->children + ഇടത്,
                വലുപ്പം(ഘടന strbuf_child_entry) * (കണ്ണി->children_എണ്ണം - ഇടത്));
        കണ്ണി->children[ഇടത്] = new;

        കണ്ണി->children_എണ്ണം++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(ഘടന strbuf *str, സ്ഥിരം അക്ഷരം *s, size_t len) {
        uint8_t c;
        ssize_t off;

        പ്രതിജ്ഞാ(str);
        പ്രതിജ്ഞാ(s || len == 0);

        എങ്കിൽ (len == SIZE_MAX)
                len = strlen(s);

        എങ്കിൽ (!str->വേര്)
                തിരികെ -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_എണ്ണം++;
        എങ്കിൽ (len == 0) {
                str->dedup_എണ്ണം++;
                തിരികെ 0;
        }
        str->in_len += len;

        ഘടന strbuf_കണ്ണി *കണ്ണി = str->വേര്;
        വേണ്ടി (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = കണ്ണി->മൂല്യം_off + കണ്ണി->മൂല്യം_len - len;
                എങ്കിൽ (depth == len || (കണ്ണി->മൂല്യം_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_എണ്ണം++;
                        തിരികെ off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                ഘടന strbuf_child_entry *child, തിരയൽ = { .c = c };
                child = typesafe_bsearch(&തിരയൽ, കണ്ണി->children, കണ്ണി->children_എണ്ണം, strbuf_children_cmp);
                എങ്കിൽ (!child)
                        നിർത്തുക;
                കണ്ണി = child->child;
        }

        /* add new string */
        എങ്കിൽ (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                തിരികെ -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ ഘടന strbuf_കണ്ണി *കണ്ണി_child = ശൂന്യ;

        കണ്ണി_child = new(ഘടന strbuf_കണ്ണി, 1);
        എങ്കിൽ (!കണ്ണി_child)
                തിരികെ -ENOMEM;
        *കണ്ണി_child = (ഘടന strbuf_കണ്ണി) {
                .മൂല്യം_off = off,
                .മൂല്യം_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        എങ്കിൽ (!GREEDY_REALLOC(കണ്ണി->children, കണ്ണി->children_എണ്ണം + 1))
                തിരികെ -ENOMEM;

        str->nodes_എണ്ണം++;

        bubbleinsert(കണ്ണി, c, TAKE_PTR(കണ്ണി_child));

        തിരികെ off;
}
