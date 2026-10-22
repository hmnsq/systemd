/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <string.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "sort-util.h"
#అన్తర్భావ "strbuf.h"

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

నిర్మాణం strbuf* strbuf_new(శూన్య) {
        _cleanup_(strbuf_freep) నిర్మాణం strbuf *str = ఖాళీ;

        str = new(నిర్మాణం strbuf, 1);
        ఒకవేళ (!str)
                తిరిగి ఖాళీ;

        *str = (నిర్మాణం strbuf) {
                .buf = new0(అక్షరం, 1),
                .మూలం = new0(నిర్మాణం strbuf_కణుపు, 1),
                .len = 1,
                .nodes_లెక్క = 1,
        };
        ఒకవేళ (!str->buf || !str->మూలం)
                తిరిగి ఖాళీ;

        తిరిగి TAKE_PTR(str);
}

స్థిర నిర్మాణం strbuf_కణుపు* strbuf_కణుపు_cleanup(నిర్మాణం strbuf_కణుపు *కణుపు) {
        ప్రతిజ్ఞా(కణుపు);

        FOREACH_ARRAY(బిడ్డ, కణుపు->children, కణుపు->children_లెక్క)
                strbuf_కణుపు_cleanup(బిడ్డ->బిడ్డ);

        విడుదల(కణుపు->children);
        తిరిగి mfree(కణుపు);
}

/* clean up trie data, leave only the string buffer */
శూన్య strbuf_complete(నిర్మాణం strbuf *str) {
        ఒకవేళ (!str || !str->మూలం)
                తిరిగి;

        str->మూలం = strbuf_కణుపు_cleanup(str->మూలం);
}

/* clean up everything */
నిర్మాణం strbuf* strbuf_free(నిర్మాణం strbuf *str) {
        ఒకవేళ (!str)
                తిరిగి ఖాళీ;

        strbuf_complete(str);
        విడుదల(str->buf);
        తిరిగి mfree(str);
}

స్థిర పూర్ణాంకం strbuf_children_cmp(స్థిరరాశి నిర్మాణం strbuf_బిడ్డ_నమోదు *n1, స్థిరరాశి నిర్మాణం strbuf_బిడ్డ_నమోదు *n2) {
        ప్రతిజ్ఞా(n1);
        ప్రతిజ్ఞా(n2);

        తిరిగి CMP(n1->c, n2->c);
}

స్థిర శూన్య bubbleinsert(నిర్మాణం strbuf_కణుపు *కణుపు,
                         uint8_t c,
                         నిర్మాణం strbuf_కణుపు *కణుపు_బిడ్డ) {

        నిర్మాణం strbuf_బిడ్డ_నమోదు new = {
                .c = c,
                .బిడ్డ = కణుపు_బిడ్డ,
        };
        పూర్ణాంకం ఎడమ = 0, కుడి = ASSERT_PTR(కణుపు)->children_లెక్క;

        అయితే (కుడి > ఎడమ) {
                పూర్ణాంకం మధ్య = (కుడి + ఎడమ) / 2 ;
                ఒకవేళ (strbuf_children_cmp(&కణుపు->children[మధ్య], &new) <= 0)
                        ఎడమ = మధ్య + 1;
                లేకపోతే
                        కుడి = మధ్య;
        }

        memmove(కణుపు->children + ఎడమ + 1, కణుపు->children + ఎడమ,
                పరిమాణం(నిర్మాణం strbuf_బిడ్డ_నమోదు) * (కణుపు->children_లెక్క - ఎడమ));
        కణుపు->children[ఎడమ] = new;

        కణుపు->children_లెక్క++;
}

/* add string, return the index/offset into the buffer */
ssize_t strbuf_కూడు_string_నిండిన(నిర్మాణం strbuf *str, స్థిరరాశి అక్షరం *s, size_t len) {
        uint8_t c;
        ssize_t off;

        ప్రతిజ్ఞా(str);
        ప్రతిజ్ఞా(s || len == 0);

        ఒకవేళ (len == SIZE_MAX)
                len = అక్షరశ్రేణి_పొడవు(s);

        ఒకవేళ (!str->మూలం)
                తిరిగి -EINVAL;

        /* search string; start from last character to find possibly matching tails */

        str->లోపల_లెక్క++;
        ఒకవేళ (len == 0) {
                str->dedup_లెక్క++;
                తిరిగి 0;
        }
        str->లోపల_len += len;

        నిర్మాణం strbuf_కణుపు *కణుపు = str->మూలం;
        కొరకు (size_t depth = 0; depth <= len; depth++) {
                /* match against current node */
                off = కణుపు->విలువ_off + కణుపు->విలువ_len - len;
                ఒకవేళ (depth == len || (కణుపు->విలువ_len >= len && memcmp(str->buf + off, s, len) == 0)) {
                        str->dedup_len += len;
                        str->dedup_లెక్క++;
                        తిరిగి off;
                }

                c = s[len - 1 - depth];

                /* lookup child node */
                నిర్మాణం strbuf_బిడ్డ_నమోదు *బిడ్డ, శోధన = { .c = c };
                బిడ్డ = typesafe_bsearch(&శోధన, కణుపు->children, కణుపు->children_లెక్క, strbuf_children_cmp);
                ఒకవేళ (!బిడ్డ)
                        ఆపు;
                కణుపు = బిడ్డ->బిడ్డ;
        }

        /* add new string */
        ఒకవేళ (!GREEDY_REALLOC(str->buf, str->len + len + 1))
                తిరిగి -ENOMEM;
        off = str->len;
        జ్ఞాపకం_నకలు(str->buf + off, s, len);
        str->len += len;
        str->buf[str->len++] = '\0';

        /* new node */
        _cleanup_free_ నిర్మాణం strbuf_కణుపు *కణుపు_బిడ్డ = ఖాళీ;

        కణుపు_బిడ్డ = new(నిర్మాణం strbuf_కణుపు, 1);
        ఒకవేళ (!కణుపు_బిడ్డ)
                తిరిగి -ENOMEM;
        *కణుపు_బిడ్డ = (నిర్మాణం strbuf_కణుపు) {
                .విలువ_off = off,
                .విలువ_len = len,
        };

        /* extend array, add new entry, sort for bisection */
        ఒకవేళ (!GREEDY_REALLOC(కణుపు->children, కణుపు->children_లెక్క + 1))
                తిరిగి -ENOMEM;

        str->nodes_లెక్క++;

        bubbleinsert(కణుపు, c, TAKE_PTR(కణుపు_బిడ్డ));

        తిరిగి off;
}
