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
                फर्काऊ रिक्त;

        *str = (संरचना strbuf) {
                .buf = new0(वर्ण, 1),
                .जरा = new0(संरचना strbuf_गाँठो, 1),
                .len = 1,
                .nodes_गन्ती = 1,
        };
        यदि (!str->buf || !str->जरा)
                फर्काऊ रिक्त;

        फर्काऊ TAKE_PTR(str);
}

स्थिर संरचना strbuf_गाँठो* strbuf_गाँठो_cleanup(संरचना strbuf_गाँठो *गाँठो) {
        प्रतिज्ञा(गाँठो);

        FOREACH_ARRAY(child, गाँठो->children, गाँठो->children_गन्ती)
                strbuf_गाँठो_cleanup(child->child);

        मुक्त(गाँठो->children);
        फर्काऊ mfree(गाँठो);
}

/* clean up trie data, leave only the string buffer */
शून्य strbuf_complete(संरचना strbuf *str) {
        यदि (!str || !str->जरा)
                फर्काऊ;

        str->जरा = strbuf_गाँठो_cleanup(str->जरा);
}

/* clean up everything */
संरचना strbuf* strbuf_free(संरचना strbuf *str) {
        यदि (!str)
                फर्काऊ रिक्त;

        strbuf_complete(str);
        मुक्त(str->buf);
        फर्काऊ mfree(str);
}

स्थिर पूर्णाङ्क strbuf_children_cmp(अचर संरचना strbuf_child_entry *n1, अचर संरचना strbuf_child_entry *n2) {
        प्रतिज्ञा(n1);
        प्रतिज्ञा(n2);

        फर्काऊ CMP(n1->c, n2->c);
}

स्थिर शून्य bubbleinsert(संरचना strbuf_गाँठो *गाँठो,
                         uint8_t c,
                         संरचना strbuf_गाँठो *गाँठो_child) {

        संरचना strbuf_child_entry new = {
                .c = c,
                .child = गाँठो_child,
        };
        पूर्णाङ्क बायाँ = 0, दायाँ = ASSERT_PTR(गाँठो)->children_गन्ती;

        जबसम्म (दायाँ > बायाँ) {
                पूर्णाङ्क बीच = (दायाँ + बायाँ) / 2 ;
                यदि (strbuf_children_cmp(&गाँठो->children[बीच], &new) <= 0)
                        बायाँ = बीच + 1;
                अन्यथा
                        दायाँ = बीच;
        }

        स_हटाओ(गाँठो->children + बायाँ + 1, गाँठो->children + बायाँ,
                परिमाण(संरचना strbuf_child_entry) * (गाँठो->children_गन्ती - बायाँ));
        गाँठो->children[बायाँ] = new;

        गाँठो->children_गन्ती++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(संरचना strbuf *str, अचर वर्ण *s, माप_प्रकार len) {
        uint8_t c;
        ssize_t off;

        प्रतिज्ञा(str);
        प्रतिज्ञा(s || len == 0);

        यदि (len == SIZE_MAX)
                len = म_माप(s);

        यदि (!str->जरा)
                फर्काऊ -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_गन्ती++;
        यदि (len == 0) {
                str->dedup_गन्ती++;
                फर्काऊ 0;
        }
        str->in_len += len;

        संरचना strbuf_गाँठो *गाँठो = str->जरा;
        लागि (माप_प्रकार depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = गाँठो->मान_off + गाँठो->मान_len - len;
                यदि (depth == len || (गाँठो->मान_len >= len && स_भेद(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_गन्ती++;
                        फर्काऊ off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                संरचना strbuf_child_entry *child, खोज = { .c = c };
                child = typesafe_bsearch(&खोज, गाँठो->children, गाँठो->children_गन्ती, strbuf_children_cmp);
                यदि (!child)
                        रोक;
                गाँठो = child->child;
        }

        /* add new string */
        यदि (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                फर्काऊ -ENOMEM;
        off = str->len;
        स_नकल(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ संरचना strbuf_गाँठो *गाँठो_child = रिक्त;

        गाँठो_child = new(संरचना strbuf_गाँठो, 1);
        यदि (!गाँठो_child)
                फर्काऊ -ENOMEM;
        *गाँठो_child = (संरचना strbuf_गाँठो) {
                .मान_off = off,
                .मान_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        यदि (!GREEDY_REALLOC(गाँठो->children, गाँठो->children_गन्ती + 1))
                फर्काऊ -ENOMEM;

        str->nodes_गन्ती++;

        bubbleinsert(गाँठो, c, TAKE_PTR(गाँठो_child));

        फर्काऊ off;
}
