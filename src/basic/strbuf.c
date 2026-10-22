/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <string.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "sort-util.h"
#अन्तर्भाव "strbuf.h"

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

संरचना strbuf* strbuf_new(शून्य) {
        _cleanup_(strbuf_freep) संरचना strbuf *str = रिक्त;

        str = new(संरचना strbuf, 1);
        यदि (!str)
                प्रत्यागम रिक्त;

        *str = (संरचना strbuf) {
                .buf = new0(वर्ण, 1),
                .मूळ = new0(संरचना strbuf_गाठ, 1),
                .len = 1,
                .nodes_मोजणी = 1,
        };
        यदि (!str->buf || !str->मूळ)
                प्रत्यागम रिक्त;

        प्रत्यागम TAKE_PTR(str);
}

स्थिर संरचना strbuf_गाठ* strbuf_गाठ_cleanup(संरचना strbuf_गाठ *गाठ) {
        प्रतिज्ञा(गाठ);

        FOREACH_ARRAY(child, गाठ->children, गाठ->children_मोजणी)
                strbuf_गाठ_cleanup(child->child);

        मुक्त(गाठ->children);
        प्रत्यागम mfree(गाठ);
}

/* clean up trie data, leave only the string buffer */
शून्य strbuf_complete(संरचना strbuf *str) {
        यदि (!str || !str->मूळ)
                प्रत्यागम;

        str->मूळ = strbuf_गाठ_cleanup(str->मूळ);
}

/* clean up everything */
संरचना strbuf* strbuf_free(संरचना strbuf *str) {
        यदि (!str)
                प्रत्यागम रिक्त;

        strbuf_complete(str);
        मुक्त(str->buf);
        प्रत्यागम mfree(str);
}

स्थिर पूर्णाङ्क strbuf_children_cmp(ध्रुव संरचना strbuf_child_entry *n1, ध्रुव संरचना strbuf_child_entry *n2) {
        प्रतिज्ञा(n1);
        प्रतिज्ञा(n2);

        प्रत्यागम CMP(n1->c, n2->c);
}

स्थिर शून्य bubbleinsert(संरचना strbuf_गाठ *गाठ,
                         uint8_t c,
                         संरचना strbuf_गाठ *गाठ_child) {

        संरचना strbuf_child_entry new = {
                .c = c,
                .child = गाठ_child,
        };
        पूर्णाङ्क डावा = 0, उजवा = ASSERT_PTR(गाठ)->children_मोजणी;

        यावत् (उजवा > डावा) {
                पूर्णाङ्क मधला = (उजवा + डावा) / 2 ;
                यदि (strbuf_children_cmp(&गाठ->children[मधला], &new) <= 0)
                        डावा = मधला + 1;
                अन्यथा
                        उजवा = मधला;
        }

        memmove(गाठ->children + डावा + 1, गाठ->children + डावा,
                परिमाण(संरचना strbuf_child_entry) * (गाठ->children_मोजणी - डावा));
        गाठ->children[डावा] = new;

        गाठ->children_मोजणी++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(संरचना strbuf *str, ध्रुव वर्ण *s, size_t len) {
        uint8_t c;
        ssize_t off;

        प्रतिज्ञा(str);
        प्रतिज्ञा(s || len == 0);

        यदि (len == SIZE_MAX)
                len = strlen(s);

        यदि (!str->मूळ)
                प्रत्यागम -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_मोजणी++;
        यदि (len == 0) {
                str->dedup_मोजणी++;
                प्रत्यागम 0;
        }
        str->in_len += len;

        संरचना strbuf_गाठ *गाठ = str->मूळ;
        प्रति (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = गाठ->मूल्य_off + गाठ->मूल्य_len - len;
                यदि (depth == len || (गाठ->मूल्य_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_मोजणी++;
                        प्रत्यागम off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                संरचना strbuf_child_entry *child, शोध = { .c = c };
                child = typesafe_bsearch(&शोध, गाठ->children, गाठ->children_मोजणी, strbuf_children_cmp);
                यदि (!child)
                        भङ्ग;
                गाठ = child->child;
        }

        /* add new string */
        यदि (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                प्रत्यागम -ENOMEM;
        off = str->len;
        memcpy(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ संरचना strbuf_गाठ *गाठ_child = रिक्त;

        गाठ_child = new(संरचना strbuf_गाठ, 1);
        यदि (!गाठ_child)
                प्रत्यागम -ENOMEM;
        *गाठ_child = (संरचना strbuf_गाठ) {
                .मूल्य_off = off,
                .मूल्य_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        यदि (!GREEDY_REALLOC(गाठ->children, गाठ->children_मोजणी + 1))
                प्रत्यागम -ENOMEM;

        str->nodes_मोजणी++;

        bubbleinsert(गाठ, c, TAKE_PTR(गाठ_child));

        प्रत्यागम off;
}
