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
                .मूलम् = new0(संरचना strbuf_ग्रन्थिः, 1),
                .len = 1,
                .nodes_गणनम् = 1,
        };
        यदि (!str->buf || !str->मूलम्)
                प्रत्यागम रिक्त;

        प्रत्यागम TAKE_PTR(str);
}

स्थिर संरचना strbuf_ग्रन्थिः* strbuf_ग्रन्थिः_cleanup(संरचना strbuf_ग्रन्थिः *ग्रन्थिः) {
        प्रतिज्ञा(ग्रन्थिः);

        FOREACH_ARRAY(child, ग्रन्थिः->children, ग्रन्थिः->children_गणनम्)
                strbuf_ग्रन्थिः_cleanup(child->child);

        मुक्त(ग्रन्थिः->children);
        प्रत्यागम mfree(ग्रन्थिः);
}

/* clean up trie data, leave only the string buffer */
शून्य strbuf_complete(संरचना strbuf *str) {
        यदि (!str || !str->मूलम्)
                प्रत्यागम;

        str->मूलम् = strbuf_ग्रन्थिः_cleanup(str->मूलम्);
}

/* clean up everything */
संरचना strbuf* strbuf_free(संरचना strbuf *str) {
        यदि (!str)
                प्रत्यागम रिक्त;

        strbuf_complete(str);
        मुक्त(str->buf);
        प्रत्यागम mfree(str);
}

स्थिर पूर्णाङ्क strbuf_children_cmp(ध्रुव संरचना strbuf_child_प्रविष्टिः *n1, ध्रुव संरचना strbuf_child_प्रविष्टिः *n2) {
        प्रतिज्ञा(n1);
        प्रतिज्ञा(n2);

        प्रत्यागम CMP(n1->c, n2->c);
}

स्थिर शून्य bubbleinsert(संरचना strbuf_ग्रन्थिः *ग्रन्थिः,
                         uint8_t c,
                         संरचना strbuf_ग्रन्थिः *ग्रन्थिः_child) {

        संरचना strbuf_child_प्रविष्टिः new = {
                .c = c,
                .child = ग्रन्थिः_child,
        };
        पूर्णाङ्क वामः = 0, दक्षिणः = ASSERT_PTR(ग्रन्थिः)->children_गणनम्;

        यावत् (दक्षिणः > वामः) {
                पूर्णाङ्क मध्यः = (दक्षिणः + वामः) / 2 ;
                यदि (strbuf_children_cmp(&ग्रन्थिः->children[मध्यः], &new) <= 0)
                        वामः = मध्यः + 1;
                अन्यथा
                        दक्षिणः = मध्यः;
        }

        memmove(ग्रन्थिः->children + वामः + 1, ग्रन्थिः->children + वामः,
                परिमाण(संरचना strbuf_child_प्रविष्टिः) * (ग्रन्थिः->children_गणनम् - वामः));
        ग्रन्थिः->children[वामः] = new;

        ग्रन्थिः->children_गणनम्++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_सङ्कलय_string_पूर्णम्(संरचना strbuf *str, ध्रुव वर्ण *s, size_t len) {
        uint8_t c;
        ssize_t off;

        प्रतिज्ञा(str);
        प्रतिज्ञा(s || len == 0);

        यदि (len == SIZE_MAX)
                len = वाक्यदैर्घ्यम्(s);

        यदि (!str->मूलम्)
                प्रत्यागम -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->अन्तर्_गणनम्++;
        यदि (len == 0) {
                str->dedup_गणनम्++;
                प्रत्यागम 0;
        }
        str->अन्तर्_len += len;

        संरचना strbuf_ग्रन्थिः *ग्रन्थिः = str->मूलम्;
        प्रति (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = ग्रन्थिः->मानम्_off + ग्रन्थिः->मानम्_len - len;
                यदि (depth == len || (ग्रन्थिः->मानम्_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_गणनम्++;
                        प्रत्यागम off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                संरचना strbuf_child_प्रविष्टिः *child, अन्वेषणम् = { .c = c };
                child = typesafe_bsearch(&अन्वेषणम्, ग्रन्थिः->children, ग्रन्थिः->children_गणनम्, strbuf_children_cmp);
                यदि (!child)
                        भङ्ग;
                ग्रन्थिः = child->child;
        }

        /* add new string */
        यदि (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                प्रत्यागम -ENOMEM;
        off = str->len;
        स्मृतिं_प्रतिलिख(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ संरचना strbuf_ग्रन्थिः *ग्रन्थिः_child = रिक्त;

        ग्रन्थिः_child = new(संरचना strbuf_ग्रन्थिः, 1);
        यदि (!ग्रन्थिः_child)
                प्रत्यागम -ENOMEM;
        *ग्रन्थिः_child = (संरचना strbuf_ग्रन्थिः) {
                .मानम्_off = off,
                .मानम्_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        यदि (!GREEDY_REALLOC(ग्रन्थिः->children, ग्रन्थिः->children_गणनम् + 1))
                प्रत्यागम -ENOMEM;

        str->nodes_गणनम्++;

        bubbleinsert(ग्रन्थिः, c, TAKE_PTR(ग्रन्थिः_child));

        प्रत्यागम off;
}
