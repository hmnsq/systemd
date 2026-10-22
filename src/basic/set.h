/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "hashmap.h"

#నిర్వచించు సమితి_free_and_replace(a, b)              \
        free_and_replace_నిండిన(a, b, సమితి_free)

సమితి_2* సమితి_new(స్థిరరాశి నిర్మాణం hash_ops *hash_ops);

స్థిర అంతఃవరుస సమితి_2* సమితి_free(సమితి_2 *s) {
        తిరిగి (సమితి_2*) _hashmap_free(HASHMAP_పునాది(s));
}

#నిర్వచించు సమితి_copy(s) ((సమితి_2*) _hashmap_copy(HASHMAP_పునాది(s)))

పూర్ణాంకం సమితి_ensure_allocated(సమితి_2 **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops);

పూర్ణాంకం సమితి_put(సమితి_2 *s, స్థిరరాశి శూన్య *కీలకం);
/* no set_update */
/* no set_replace */
స్థిర అంతఃవరుస శూన్య *సమితి_get(స్థిరరాశి సమితి_2 *s, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి _hashmap_get(HASHMAP_పునాది((సమితి_2 *) s), కీలకం);
}
/* no set_get2 */

స్థిర అంతఃవరుస bool సమితి_కలిగియుంది(స్థిరరాశి సమితి_2 *s, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి hashmap_కలిగియుంది_2(HASHMAP_పునాది((సమితి_2 *) s), కీలకం);
}

స్థిర అంతఃవరుస శూన్య *సమితి_remove(సమితి_2 *s, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి _hashmap_remove(HASHMAP_పునాది(s), కీలకం);
}

/* no set_remove2 */
/* no set_remove_value */
పూర్ణాంకం సమితి_remove_and_put(సమితి_2 *s, స్థిరరాశి శూన్య *old_కీలకం, స్థిరరాశి శూన్య *new_కీలకం);
/* no set_remove_and_replace */
పూర్ణాంకం సమితి_విలీనం(సమితి_2 *s, సమితి_2 *other);

స్థిర అంతఃవరుస పూర్ణాంకం సమితి_reserve(సమితి_2 *h, గుర్తులేని entries_కూడు) {
        తిరిగి _hashmap_reserve(HASHMAP_పునాది(h), entries_కూడు);
}

స్థిర అంతఃవరుస పూర్ణాంకం సమితి_move(సమితి_2 *s, సమితి_2 *other) {
        తిరిగి _hashmap_move(HASHMAP_పునాది(s), HASHMAP_పునాది(other));
}

స్థిర అంతఃవరుస పూర్ణాంకం సమితి_move_one(సమితి_2 *s, సమితి_2 *other, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి _hashmap_move_one(HASHMAP_పునాది(s), HASHMAP_పునాది(other), కీలకం);
}

స్థిర అంతఃవరుస గుర్తులేని సమితి_కొలత(స్థిరరాశి సమితి_2 *s) {
        తిరిగి hashmap_కొలత(HASHMAP_పునాది((సమితి_2 *) s));
}

స్థిర అంతఃవరుస bool సమితి_isempty(స్థిరరాశి సమితి_2 *s) {
        తిరిగి సమితి_కొలత(s) == 0;
}

స్థిర అంతఃవరుస గుర్తులేని సమితి_buckets(స్థిరరాశి సమితి_2 *s) {
        తిరిగి _hashmap_buckets(HASHMAP_పునాది((సమితి_2 *) s));
}

స్థిర అంతఃవరుస bool సమితి_iterate(స్థిరరాశి సమితి_2 *s, Iterator *i, శూన్య **విలువ) {
        తిరిగి _hashmap_iterate(HASHMAP_పునాది((సమితి_2*) s), i, విలువ, ఖాళీ);
}

స్థిర అంతఃవరుస శూన్య సమితి_clear(సమితి_2 *s) {
        _hashmap_clear(HASHMAP_పునాది(s));
}

స్థిర అంతఃవరుస శూన్య *సమితి_steal_మొదటి(సమితి_2 *s) {
        తిరిగి hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది(s), true, ఖాళీ);
}

/* no set_steal_first_key */
/* no set_first_key */

స్థిర అంతఃవరుస శూన్య *సమితి_మొదటి(స్థిరరాశి సమితి_2 *s) {
        తిరిగి hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది((సమితి_2 *) s), false, ఖాళీ);
}

/* no set_next */

స్థిర అంతఃవరుస అక్షరం **సమితి_get_strv(సమితి_2 *s) {
        తిరిగి _hashmap_get_strv(HASHMAP_పునాది(s));
}

అక్షరం** సమితి_గమ్యం_strv(సమితి_2 **s);

పూర్ణాంకం సమితి_ensure_put(సమితి_2 **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి శూన్య *కీలకం);

పూర్ణాంకం సమితి_ensure_consume(సమితి_2 **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, శూన్య *కీలకం);

పూర్ణాంకం సమితి_consume(సమితి_2 *s, శూన్య *విలువ);

పూర్ణాంకం సమితి_put_strndup_నిండిన(సమితి_2 **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి అక్షరం *p, size_t n);
#నిర్వచించు సమితి_put_strdup_నిండిన(s, hash_ops, p) సమితి_put_strndup_నిండిన(s, hash_ops, p, SIZE_MAX)
#నిర్వచించు సమితి_put_strndup(s, p, n) సమితి_put_strndup_నిండిన(s, &string_hash_ops_free, p, n)
#నిర్వచించు సమితి_put_strdup(s, p) సమితి_put_strndup(s, p, SIZE_MAX)

పూర్ణాంకం సమితి_put_strdupv_నిండిన(సమితి_2 **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, అక్షరం **l);
#నిర్వచించు సమితి_put_strdupv(s, l) సమితి_put_strdupv_నిండిన(s, &string_hash_ops_free, l)

పూర్ణాంకం సమితి_put_strsplit(సమితి_2 *s, స్థిరరాశి అక్షరం *v, స్థిరరాశి అక్షరం *separators, వెలికితీయి_Flags flags);

#నిర్వచించు సమితి_FOREACH_2(e, s, i) \
        కొరకు (Iterator i = ITERATOR_మొదటి; సమితి_iterate((s), &i, (శూన్య**)&(e)); )
#నిర్వచించు సమితి_FOREACH(e, s) \
        సమితి_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

#నిర్వచించు సమితి_FOREACH_MOVE(e, d, s)                                       \
        కొరకు (; ({ e = సమితి_మొదటి(s); assert_se(!e || సమితి_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(సమితి_2*, సమితి_free);

#నిర్వచించు cleanup_సమితి_free _cleanup_(సమితి_freep)

పూర్ణాంకం సమితి_strjoin(సమితి_2 *s, స్థిరరాశి అక్షరం *separator, bool wrap_with_separator, అక్షరం **ret);

bool సమితి_equal(సమితి_2 *a, సమితి_2 *b);

bool సమితి_fnmatch(సమితి_2 *include_patterns, సమితి_2 *exclude_patterns, స్థిరరాశి అక్షరం *needle);
