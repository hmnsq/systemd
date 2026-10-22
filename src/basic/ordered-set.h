/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "hashmap.h"

రకనిర్వచనం నిర్మాణం Ordered_సమితి Ordered_సమితి;

స్థిర అంతఃవరుస Ordered_సమితి* ordered_సమితి_new(స్థిరరాశి నిర్మాణం hash_ops *ops) {
        తిరిగి (Ordered_సమితి*) ordered_hashmap_new(ops);
}

పూర్ణాంకం ordered_సమితి_ensure_allocated(Ordered_సమితి **s, స్థిరరాశి నిర్మాణం hash_ops *ops);

పూర్ణాంకం ordered_సమితి_ensure_put(Ordered_సమితి **s, స్థిరరాశి నిర్మాణం hash_ops *ops, శూన్య *p);

స్థిర అంతఃవరుస శూన్య ordered_సమితి_clear(Ordered_సమితి *s) {
        తిరిగి ordered_hashmap_clear((OrderedHashmap*) s);
}

స్థిర అంతఃవరుస Ordered_సమితి* ordered_సమితి_free(Ordered_సమితి *s) {
        తిరిగి (Ordered_సమితి*) ordered_hashmap_free((OrderedHashmap*) s);
}

స్థిర అంతఃవరుస పూర్ణాంకం ordered_సమితి_కలిగియుంది(Ordered_సమితి *s, స్థిరరాశి శూన్య *p) {
        తిరిగి ordered_hashmap_కలిగియుంది((OrderedHashmap*) s, p);
}

స్థిర అంతఃవరుస పూర్ణాంకం ordered_సమితి_put(Ordered_సమితి *s, శూన్య *p) {
        తిరిగి ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

స్థిర అంతఃవరుస శూన్య *ordered_సమితి_get(Ordered_సమితి *s, స్థిరరాశి శూన్య *p) {
        తిరిగి ordered_hashmap_get((OrderedHashmap*) s, p);
}

స్థిర అంతఃవరుస గుర్తులేని ordered_సమితి_కొలత(Ordered_సమితి *s) {
        తిరిగి ordered_hashmap_కొలత((OrderedHashmap*) s);
}

స్థిర అంతఃవరుస bool ordered_సమితి_isempty(Ordered_సమితి *s) {
        తిరిగి ordered_hashmap_isempty((OrderedHashmap*) s);
}

స్థిర అంతఃవరుస bool ordered_సమితి_iterate(Ordered_సమితి *s, Iterator *i, శూన్య **విలువ) {
        తిరిగి ordered_hashmap_iterate((OrderedHashmap*) s, i, విలువ, ఖాళీ);
}

స్థిర అంతఃవరుస శూన్య* ordered_సమితి_remove(Ordered_సమితి *s, శూన్య *p) {
        తిరిగి ordered_hashmap_remove((OrderedHashmap*) s, p);
}

స్థిర అంతఃవరుస శూన్య* ordered_సమితి_మొదటి(Ordered_సమితి *s) {
        తిరిగి ordered_hashmap_మొదటి((OrderedHashmap*) s);
}

స్థిర అంతఃవరుస శూన్య* ordered_సమితి_steal_మొదటి(Ordered_సమితి *s) {
        తిరిగి ordered_hashmap_steal_మొదటి((OrderedHashmap*) s);
}

స్థిర అంతఃవరుస అక్షరం** ordered_సమితి_get_strv(Ordered_సమితి *s) {
        తిరిగి _hashmap_get_strv(HASHMAP_పునాది((OrderedHashmap*) s));
}

స్థిర అంతఃవరుస పూర్ణాంకం ordered_సమితి_reserve(Ordered_సమితి *s, గుర్తులేని entries_కూడు) {
        తిరిగి ordered_hashmap_reserve((OrderedHashmap*) s, entries_కూడు);
}

పూర్ణాంకం ordered_సమితి_consume(Ordered_సమితి *s, శూన్య *p);

పూర్ణాంకం ordered_సమితి_put_strdup_నిండిన(Ordered_సమితి **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి అక్షరం *p);
#నిర్వచించు ordered_సమితి_put_strdup(s, p) ordered_సమితి_put_strdup_నిండిన(s, &string_hash_ops_free, p)
పూర్ణాంకం ordered_సమితి_put_strdupv_నిండిన(Ordered_సమితి **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, అక్షరం **l);
#నిర్వచించు ordered_సమితి_put_strdupv(s, l) ordered_సమితి_put_strdupv_నిండిన(s, &string_hash_ops_free, l)
పూర్ణాంకం ordered_సమితి_put_string_సమితి_నిండిన(Ordered_సమితి **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, Ordered_సమితి *l);
#నిర్వచించు ordered_సమితి_put_string_సమితి(s, l) ordered_సమితి_put_string_సమితి_నిండిన(s, &string_hash_ops_free, l)

శూన్య ordered_సమితి_print(FILE *f, స్థిరరాశి అక్షరం *క్షేత్రం, Ordered_సమితి *s);

#నిర్వచించు ORDERED_సమితి_FOREACH_2(e, s, i) \
        కొరకు (Iterator i = ITERATOR_మొదటి; ordered_సమితి_iterate((s), &i, (శూన్య**)&(e)); )
#నిర్వచించు ORDERED_సమితి_FOREACH(e, s) \
        ORDERED_సమితి_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Ordered_సమితి*, ordered_సమితి_free);

#నిర్వచించు cleanup_ordered_సమితి_free _cleanup_(ordered_సమితి_freep)
