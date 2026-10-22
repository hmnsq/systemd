/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <string.h>

#समावेश "alloc-util.h"
#समावेश "sort-util.h"
#समावेश "strbuf.h"

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

काष्ठा strbuf* strbuf_new(व्योम) {
        _cleanup_(strbuf_freep) काष्ठा strbuf *str = शून्य;

        str = new(काष्ठा strbuf, 1);
        अगर (!str)
                वापस शून्य;

        *str = (काष्ठा strbuf) {
                .buf = new0(अक्षर, 1),
                .मूल = new0(काष्ठा strbuf_गाँठ, 1),
                .len = 1,
                .nodes_गिनती = 1,
        };
        अगर (!str->buf || !str->मूल)
                वापस शून्य;

        वापस TAKE_PTR(str);
}

अटल काष्ठा strbuf_गाँठ* strbuf_गाँठ_cleanup(काष्ठा strbuf_गाँठ *गाँठ) {
        निश्चित(गाँठ);

        FOREACH_ARRAY(child, गाँठ->children, गाँठ->children_गिनती)
                strbuf_गाँठ_cleanup(child->child);

        मुक्त(गाँठ->children);
        वापस mfree(गाँठ);
}

/* clean up trie data, leave only the string buffer */
व्योम strbuf_complete(काष्ठा strbuf *str) {
        अगर (!str || !str->मूल)
                वापस;

        str->मूल = strbuf_गाँठ_cleanup(str->मूल);
}

/* clean up everything */
काष्ठा strbuf* strbuf_free(काष्ठा strbuf *str) {
        अगर (!str)
                वापस शून्य;

        strbuf_complete(str);
        मुक्त(str->buf);
        वापस mfree(str);
}

अटल पूर्णांक strbuf_children_cmp(स्थिर काष्ठा strbuf_child_entry *n1, स्थिर काष्ठा strbuf_child_entry *n2) {
        निश्चित(n1);
        निश्चित(n2);

        वापस CMP(n1->c, n2->c);
}

अटल व्योम bubbleinsert(काष्ठा strbuf_गाँठ *गाँठ,
                         uint8_t c,
                         काष्ठा strbuf_गाँठ *गाँठ_child) {

        काष्ठा strbuf_child_entry new = {
                .c = c,
                .child = गाँठ_child,
        };
        पूर्णांक बायाँ = 0, दायाँ = ASSERT_PTR(गाँठ)->children_गिनती;

        जबतक (दायाँ > बायाँ) {
                पूर्णांक मध्य = (दायाँ + बायाँ) / 2 ;
                अगर (strbuf_children_cmp(&गाँठ->children[मध्य], &new) <= 0)
                        बायाँ = मध्य + 1;
                अन्यथा
                        दायाँ = मध्य;
        }

        स_हटाओ(गाँठ->children + बायाँ + 1, गाँठ->children + बायाँ,
                माप(काष्ठा strbuf_child_entry) * (गाँठ->children_गिनती - बायाँ));
        गाँठ->children[बायाँ] = new;

        गाँठ->children_गिनती++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_add_string_full(काष्ठा strbuf *str, स्थिर अक्षर *s, माप_प्रकार len) {
        uint8_t c;
        ssize_t off;

        निश्चित(str);
        निश्चित(s || len == 0);

        अगर (len == SIZE_MAX)
                len = म_माप(s);

        अगर (!str->मूल)
                वापस -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->in_गिनती++;
        अगर (len == 0) {
                str->dedup_गिनती++;
                वापस 0;
        }
        str->in_len += len;

        काष्ठा strbuf_गाँठ *गाँठ = str->मूल;
        क्रम (माप_प्रकार depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = गाँठ->मूल्य_off + गाँठ->मूल्य_len - len;
                अगर (depth == len || (गाँठ->मूल्य_len >= len && स_भेद(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_गिनती++;
                        वापस off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                काष्ठा strbuf_child_entry *child, खोज = { .c = c };
                child = typesafe_bsearch(&खोज, गाँठ->children, गाँठ->children_गिनती, strbuf_children_cmp);
                अगर (!child)
                        अवरोध;
                गाँठ = child->child;
        }

        /* add new string */
        अगर (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                वापस -ENOMEM;
        off = str->len;
        स_नकल(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ काष्ठा strbuf_गाँठ *गाँठ_child = शून्य;

        गाँठ_child = new(काष्ठा strbuf_गाँठ, 1);
        अगर (!गाँठ_child)
                वापस -ENOMEM;
        *गाँठ_child = (काष्ठा strbuf_गाँठ) {
                .मूल्य_off = off,
                .मूल्य_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        अगर (!GREEDY_REALLOC(गाँठ->children, गाँठ->children_गिनती + 1))
                वापस -ENOMEM;

        str->nodes_गिनती++;

        bubbleinsert(गाँठ, c, TAKE_PTR(गाँठ_child));

        वापस off;
}
