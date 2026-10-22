/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "hash-funcs.h"
#అన్తర్భావ "iterator.h"

/*
 * A hash table implementation. As a minor optimization a NULL hashmap object
 * will be treated as empty hashmap for all read operations. That way it is not
 * necessary to instantiate an object for each Hashmap use.
 *
 * If ENABLE_DEBUG_HASHMAP is defined (by configuring with -Ddebug-extra=hashmap),
 * the implementation will:
 * - store extra data for debugging and statistics (see tools/gdb-sd_dump_hashmaps.py)
 * - perform extra checks for invalid use of iterators
 */

#నిర్వచించు HASH_కీలకం_కొలత 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
రకనిర్వచనం నిర్మాణం Hashmap_పునాది Hashmap_పునాది;

/* Specific hashmap/set types */
రకనిర్వచనం నిర్మాణం Hashmap Hashmap;               /* Maps keys to values */
రకనిర్వచనం నిర్మాణం OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
రకనిర్వచనం నిర్మాణం సమితి_2 సమితి_2;                       /* Stores just keys */

రకనిర్వచనం నిర్మాణం IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

/* Macros for type checking */
#నిర్వచించు PTR_COMPATIBLE_WITH_HASHMAP_పునాది(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap_పునాది*) || \
         __builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*) || \
         __builtin_types_compatible_p(typeof(h), సమితి_2*))

#నిర్వచించు PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*)) \

#నిర్వచించు HASHMAP_పునాది(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_HASHMAP_పునాది(h), \
                (Hashmap_పునాది*)(h), \
                (శూన్య)0)

#నిర్వచించు PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (శూన్య)0)

Hashmap* hashmap_new(స్థిరరాశి నిర్మాణం hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(స్థిరరాశి నిర్మాణం hash_ops *hash_ops);

#నిర్వచించు hashmap_free_and_replace(a, b)                          \
        free_and_replace_నిండిన(a, b, hashmap_free)
#నిర్వచించు ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_నిండిన(a, b, ordered_hashmap_free)

Hashmap_పునాది* _hashmap_free(Hashmap_పునాది *h);
స్థిర అంతఃవరుస Hashmap* hashmap_free(Hashmap *h) {
        తిరిగి (శూన్య*) _hashmap_free(HASHMAP_పునాది(h));
}
స్థిర అంతఃవరుస OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        తిరిగి (శూన్య*) _hashmap_free(HASHMAP_పునాది(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
పూర్ణాంకం iterated_cache_get(IteratedCache *cache, స్థిరరాశి శూన్య ***res_కీలకాలు, స్థిరరాశి శూన్య ***res_విలువలు, గుర్తులేని *res_n_entries);

Hashmap_పునాది* _hashmap_copy(Hashmap_పునాది *h);
స్థిర అంతఃవరుస Hashmap* hashmap_copy(Hashmap *h) {
        తిరిగి (Hashmap*) _hashmap_copy(HASHMAP_పునాది(h));
}
స్థిర అంతఃవరుస OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        తిరిగి (OrderedHashmap*) _hashmap_copy(HASHMAP_పునాది(h));
}

పూర్ణాంకం hashmap_ensure_allocated(Hashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops);
పూర్ణాంకం hashmap_ensure_put(Hashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ);
పూర్ణాంకం ordered_hashmap_ensure_allocated(OrderedHashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops);
పూర్ణాంకం hashmap_ensure_replace(Hashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ);

పూర్ణాంకం ordered_hashmap_ensure_put(OrderedHashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ);
పూర్ణాంకం ordered_hashmap_ensure_replace(OrderedHashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ);

IteratedCache* _hashmap_iterated_cache_new(Hashmap_పునాది *h);
స్థిర అంతఃవరుస IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        తిరిగి _hashmap_iterated_cache_new(HASHMAP_పునాది(h));
}
స్థిర అంతఃవరుస IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        తిరిగి _hashmap_iterated_cache_new(HASHMAP_పునాది(h));
}

పూర్ణాంకం hashmap_put(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ);
స్థిర అంతఃవరుస పూర్ణాంకం ordered_hashmap_put(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        తిరిగి hashmap_put(PLAIN_HASHMAP(h), కీలకం, విలువ);
}

పూర్ణాంకం hashmap_put_strdup_నిండిన(Hashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి అక్షరం *k, స్థిరరాశి అక్షరం *v);
స్థిర అంతఃవరుస పూర్ణాంకం hashmap_put_strdup(Hashmap **h, స్థిరరాశి అక్షరం *k, స్థిరరాశి అక్షరం *v) {
        తిరిగి hashmap_put_strdup_నిండిన(h, &string_hash_ops_free_free, k, v);
}

పూర్ణాంకం hashmap_update(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ);
స్థిర అంతఃవరుస పూర్ణాంకం ordered_hashmap_update(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        తిరిగి hashmap_update(PLAIN_HASHMAP(h), కీలకం, విలువ);
}

పూర్ణాంకం hashmap_replace(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ);
స్థిర అంతఃవరుస పూర్ణాంకం ordered_hashmap_replace(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        తిరిగి hashmap_replace(PLAIN_HASHMAP(h), కీలకం, విలువ);
}

శూన్య* _hashmap_get(Hashmap_పునాది *h, స్థిరరాశి శూన్య *కీలకం);
స్థిర అంతఃవరుస శూన్య *hashmap_get(Hashmap *h, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి _hashmap_get(HASHMAP_పునాది(h), కీలకం);
}
స్థిర అంతఃవరుస శూన్య *ordered_hashmap_get(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి _hashmap_get(HASHMAP_పునాది(h), కీలకం);
}

శూన్య* hashmap_get2(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య **ret);
స్థిర అంతఃవరుస శూన్య *ordered_hashmap_get2(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య **ret) {
        తిరిగి hashmap_get2(PLAIN_HASHMAP(h), కీలకం, ret);
}

bool hashmap_కలిగియుంది_2(Hashmap_పునాది *h, స్థిరరాశి శూన్య *కీలకం);
స్థిర అంతఃవరుస bool hashmap_కలిగియుంది(Hashmap *h, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి hashmap_కలిగియుంది_2(HASHMAP_పునాది(h), కీలకం);
}
స్థిర అంతఃవరుస bool ordered_hashmap_కలిగియుంది(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి hashmap_కలిగియుంది_2(HASHMAP_పునాది(h), కీలకం);
}

శూన్య* _hashmap_remove(Hashmap_పునాది *h, స్థిరరాశి శూన్య *కీలకం);
స్థిర అంతఃవరుస శూన్య *hashmap_remove(Hashmap *h, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి _hashmap_remove(HASHMAP_పునాది(h), కీలకం);
}
స్థిర అంతఃవరుస శూన్య *ordered_hashmap_remove(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి _hashmap_remove(HASHMAP_పునాది(h), కీలకం);
}

శూన్య* hashmap_remove2(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య **ret);
స్థిర అంతఃవరుస శూన్య *ordered_hashmap_remove2(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య **ret) {
        తిరిగి hashmap_remove2(PLAIN_HASHMAP(h), కీలకం, ret);
}

శూన్య* hashmap_remove_విలువ(Hashmap_పునాది *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ);
స్థిర అంతఃవరుస శూన్య *hashmap_remove_విలువ_2(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        తిరిగి hashmap_remove_విలువ(HASHMAP_పునాది(h), కీలకం, విలువ);
}

స్థిర అంతఃవరుస శూన్య* ordered_hashmap_remove_విలువ(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        తిరిగి hashmap_remove_విలువ_2(PLAIN_HASHMAP(h), కీలకం, విలువ);
}

పూర్ణాంకం hashmap_remove_and_put(Hashmap *h, స్థిరరాశి శూన్య *old_కీలకం, స్థిరరాశి శూన్య *new_కీలకం, శూన్య *విలువ);
స్థిర అంతఃవరుస పూర్ణాంకం ordered_hashmap_remove_and_put(OrderedHashmap *h, స్థిరరాశి శూన్య *old_కీలకం, స్థిరరాశి శూన్య *new_కీలకం, శూన్య *విలువ) {
        తిరిగి hashmap_remove_and_put(PLAIN_HASHMAP(h), old_కీలకం, new_కీలకం, విలువ);
}

పూర్ణాంకం hashmap_remove_and_replace(Hashmap *h, స్థిరరాశి శూన్య *old_కీలకం, స్థిరరాశి శూన్య *new_కీలకం, శూన్య *విలువ);
స్థిర అంతఃవరుస పూర్ణాంకం ordered_hashmap_remove_and_replace(OrderedHashmap *h, స్థిరరాశి శూన్య *old_కీలకం, స్థిరరాశి శూన్య *new_కీలకం, శూన్య *విలువ) {
        తిరిగి hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_కీలకం, new_కీలకం, విలువ);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
పూర్ణాంకం hashmap_విలీనం(Hashmap *h, Hashmap *other);
#నిర్వచించు hashmap_విలీనం_2(h, other) hashmap_విలీనం(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#నిర్వచించు ordered_hashmap_విలీనం(h, other) hashmap_విలీనం_2(h, other)

పూర్ణాంకం _hashmap_reserve(Hashmap_పునాది *h, గుర్తులేని entries_కూడు);
స్థిర అంతఃవరుస పూర్ణాంకం hashmap_reserve(Hashmap *h, గుర్తులేని entries_కూడు) {
        తిరిగి _hashmap_reserve(HASHMAP_పునాది(h), entries_కూడు);
}
స్థిర అంతఃవరుస పూర్ణాంకం ordered_hashmap_reserve(OrderedHashmap *h, గుర్తులేని entries_కూడు) {
        తిరిగి _hashmap_reserve(HASHMAP_పునాది(h), entries_కూడు);
}

పూర్ణాంకం _hashmap_move(Hashmap_పునాది *h, Hashmap_పునాది *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
స్థిర అంతఃవరుస పూర్ణాంకం hashmap_move(Hashmap *h, Hashmap *other) {
        తిరిగి _hashmap_move(HASHMAP_పునాది(h), HASHMAP_పునాది(other));
}
స్థిర అంతఃవరుస పూర్ణాంకం ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        తిరిగి _hashmap_move(HASHMAP_పునాది(h), HASHMAP_పునాది(other));
}

పూర్ణాంకం _hashmap_move_one(Hashmap_పునాది *h, Hashmap_పునాది *other, స్థిరరాశి శూన్య *కీలకం);
స్థిర అంతఃవరుస పూర్ణాంకం hashmap_move_one(Hashmap *h, Hashmap *other, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి _hashmap_move_one(HASHMAP_పునాది(h), HASHMAP_పునాది(other), కీలకం);
}
స్థిర అంతఃవరుస పూర్ణాంకం ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, స్థిరరాశి శూన్య *కీలకం) {
        తిరిగి _hashmap_move_one(HASHMAP_పునాది(h), HASHMAP_పునాది(other), కీలకం);
}

గుర్తులేని hashmap_కొలత(Hashmap_పునాది *h) శుద్ధ;
స్థిర అంతఃవరుస గుర్తులేని hashmap_కొలత_2(Hashmap *h) {
        తిరిగి hashmap_కొలత(HASHMAP_పునాది(h));
}
స్థిర అంతఃవరుస గుర్తులేని ordered_hashmap_కొలత(OrderedHashmap *h) {
        తిరిగి hashmap_కొలత(HASHMAP_పునాది(h));
}

స్థిర అంతఃవరుస bool hashmap_isempty(Hashmap *h) {
        తిరిగి hashmap_కొలత_2(h) == 0;
}
స్థిర అంతఃవరుస bool ordered_hashmap_isempty(OrderedHashmap *h) {
        తిరిగి ordered_hashmap_కొలత(h) == 0;
}

గుర్తులేని _hashmap_buckets(Hashmap_పునాది *h) శుద్ధ;
స్థిర అంతఃవరుస గుర్తులేని hashmap_buckets(Hashmap *h) {
        తిరిగి _hashmap_buckets(HASHMAP_పునాది(h));
}
స్థిర అంతఃవరుస గుర్తులేని ordered_hashmap_buckets(OrderedHashmap *h) {
        తిరిగి _hashmap_buckets(HASHMAP_పునాది(h));
}

bool _hashmap_iterate(Hashmap_పునాది *h, Iterator *i, శూన్య **విలువ, స్థిరరాశి శూన్య **కీలకం);
స్థిర అంతఃవరుస bool hashmap_iterate(Hashmap *h, Iterator *i, శూన్య **విలువ, స్థిరరాశి శూన్య **కీలకం) {
        తిరిగి _hashmap_iterate(HASHMAP_పునాది(h), i, విలువ, కీలకం);
}
స్థిర అంతఃవరుస bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, శూన్య **విలువ, స్థిరరాశి శూన్య **కీలకం) {
        తిరిగి _hashmap_iterate(HASHMAP_పునాది(h), i, విలువ, కీలకం);
}

శూన్య _hashmap_clear(Hashmap_పునాది *h);
స్థిర అంతఃవరుస శూన్య hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_పునాది(h));
}
స్థిర అంతఃవరుస శూన్య ordered_hashmap_clear(OrderedHashmap *h) {
        _hashmap_clear(HASHMAP_పునాది(h));
}

/*
 * Note about all *_first*() functions
 *
 * For plain Hashmaps and Sets the order of entries is undefined.
 * The functions find whatever entry is first in the implementation
 * internal order.
 *
 * Only for OrderedHashmaps the order is well defined and finding
 * the first entry is O(1).
 */

శూన్య *hashmap_మొదటి_కీలకం_and_విలువ(Hashmap_పునాది *h, bool remove, శూన్య **ret_కీలకం);
స్థిర అంతఃవరుస శూన్య *hashmap_steal_మొదటి_కీలకం_and_విలువ(Hashmap *h, శూన్య **ret) {
        తిరిగి hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది(h), true, ret);
}
స్థిర అంతఃవరుస శూన్య *ordered_hashmap_steal_మొదటి_కీలకం_and_విలువ(OrderedHashmap *h, శూన్య **ret) {
        తిరిగి hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది(h), true, ret);
}
స్థిర అంతఃవరుస శూన్య *hashmap_మొదటి_కీలకం_and_విలువ_2(Hashmap *h, శూన్య **ret) {
        తిరిగి hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది(h), false, ret);
}
స్థిర అంతఃవరుస శూన్య *ordered_hashmap_మొదటి_కీలకం_and_విలువ(OrderedHashmap *h, శూన్య **ret) {
        తిరిగి hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది(h), false, ret);
}

స్థిర అంతఃవరుస శూన్య *hashmap_steal_మొదటి(Hashmap *h) {
        తిరిగి hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది(h), true, ఖాళీ);
}
స్థిర అంతఃవరుస శూన్య *ordered_hashmap_steal_మొదటి(OrderedHashmap *h) {
        తిరిగి hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది(h), true, ఖాళీ);
}
స్థిర అంతఃవరుస శూన్య *hashmap_మొదటి(Hashmap *h) {
        తిరిగి hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది(h), false, ఖాళీ);
}
స్థిర అంతఃవరుస శూన్య *ordered_hashmap_మొదటి(OrderedHashmap *h) {
        తిరిగి hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది(h), false, ఖాళీ);
}

స్థిర అంతఃవరుస శూన్య *hashmap_మొదటి_కీలకం(Hashmap_పునాది *h, bool remove) {
        శూన్య *కీలకం = ఖాళీ;

        (శూన్య) hashmap_మొదటి_కీలకం_and_విలువ(HASHMAP_పునాది(h), remove, &కీలకం);
        తిరిగి కీలకం;
}
స్థిర అంతఃవరుస శూన్య *hashmap_steal_మొదటి_కీలకం(Hashmap *h) {
        తిరిగి hashmap_మొదటి_కీలకం(HASHMAP_పునాది(h), true);
}
స్థిర అంతఃవరుస శూన్య *ordered_hashmap_steal_మొదటి_కీలకం(OrderedHashmap *h) {
        తిరిగి hashmap_మొదటి_కీలకం(HASHMAP_పునాది(h), true);
}
స్థిర అంతఃవరుస శూన్య *hashmap_మొదటి_కీలకం_2(Hashmap *h) {
        తిరిగి hashmap_మొదటి_కీలకం(HASHMAP_పునాది(h), false);
}
స్థిర అంతఃవరుస శూన్య *ordered_hashmap_మొదటి_కీలకం(OrderedHashmap *h) {
        తిరిగి hashmap_మొదటి_కీలకం(HASHMAP_పునాది(h), false);
}

/* no hashmap_next */
శూన్య* ordered_hashmap_తదుపరి(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం);

అక్షరం** _hashmap_get_strv(Hashmap_పునాది *h);
స్థిర అంతఃవరుస అక్షరం** hashmap_get_strv(Hashmap *h) {
        తిరిగి _hashmap_get_strv(HASHMAP_పునాది(h));
}
స్థిర అంతఃవరుస అక్షరం** ordered_hashmap_get_strv(OrderedHashmap *h) {
        తిరిగి _hashmap_get_strv(HASHMAP_పునాది(h));
}

పూర్ణాంకం _hashmap_dump_sorted(Hashmap_పునాది *h, శూన్య ***ret, size_t *ret_n);
స్థిర అంతఃవరుస పూర్ణాంకం hashmap_dump_sorted(Hashmap *h, శూన్య ***ret, size_t *ret_n) {
        తిరిగి _hashmap_dump_sorted(HASHMAP_పునాది(h), ret, ret_n);
}
స్థిర అంతఃవరుస పూర్ణాంకం ordered_hashmap_dump_sorted(OrderedHashmap *h, శూన్య ***ret, size_t *ret_n) {
        తిరిగి _hashmap_dump_sorted(HASHMAP_పునాది(h), ret, ret_n);
}
స్థిర అంతఃవరుస పూర్ణాంకం సమితి_dump_sorted(సమితి_2 *h, శూన్య ***ret, size_t *ret_n) {
        తిరిగి _hashmap_dump_sorted(HASHMAP_పునాది(h), ret, ret_n);
}

పూర్ణాంకం hashmap_dump_కీలకాలు_sorted(Hashmap_పునాది *h, శూన్య ***ret, size_t *ret_n);
స్థిర అంతఃవరుస పూర్ణాంకం hashmap_dump_కీలకాలు_sorted_2(Hashmap *h, శూన్య ***ret, size_t *ret_n) {
        తిరిగి hashmap_dump_కీలకాలు_sorted(HASHMAP_పునాది(h), ret, ret_n);
}
స్థిర అంతఃవరుస పూర్ణాంకం ordered_hashmap_dump_కీలకాలు_sorted(OrderedHashmap *h, శూన్య ***ret, size_t *ret_n) {
        తిరిగి hashmap_dump_కీలకాలు_sorted(HASHMAP_పునాది(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#నిర్వచించు HASHMAP_పునాది_FOREACH(e, h, i) \
        కొరకు (Iterator i = ITERATOR_మొదటి; _hashmap_iterate((h), &i, (శూన్య**)&(e), ఖాళీ); )
#నిర్వచించు HASHMAP_పునాది_FOREACH_2(e, h) \
        HASHMAP_పునాది_FOREACH(e, h, UNIQ_T(i, UNIQ))

#నిర్వచించు _HASHMAP_FOREACH(e, h, i) \
        కొరకు (Iterator i = ITERATOR_మొదటి; hashmap_iterate((h), &i, (శూన్య**)&(e), ఖాళీ); )
#నిర్వచించు HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#నిర్వచించు _ORDERED_HASHMAP_FOREACH(e, h, i) \
        కొరకు (Iterator i = ITERATOR_మొదటి; ordered_hashmap_iterate((h), &i, (శూన్య**)&(e), ఖాళీ); )
#నిర్వచించు ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#నిర్వచించు HASHMAP_పునాది_FOREACH_కీలకం(e, k, h, i) \
        కొరకు (Iterator i = ITERATOR_మొదటి; _hashmap_iterate((h), &i, (శూన్య**)&(e), (స్థిరరాశి శూన్య**) &(k)); )
#నిర్వచించు HASHMAP_పునాది_FOREACH_కీలకం_2(e, k, h) \
        HASHMAP_పునాది_FOREACH_కీలకం(e, k, h, UNIQ_T(i, UNIQ))

#నిర్వచించు HASHMAP_FOREACH_కీలకం(e, k, h, i) \
        కొరకు (Iterator i = ITERATOR_మొదటి; hashmap_iterate((h), &i, (శూన్య**)&(e), (స్థిరరాశి శూన్య**) &(k)); )
#నిర్వచించు HASHMAP_FOREACH_కీలకం_2(e, k, h) \
        HASHMAP_FOREACH_కీలకం(e, k, h, UNIQ_T(i, UNIQ))

#నిర్వచించు ORDERED_HASHMAP_FOREACH_కీలకం(e, k, h, i) \
        కొరకు (Iterator i = ITERATOR_మొదటి; ordered_hashmap_iterate((h), &i, (శూన్య**)&(e), (స్థిరరాశి శూన్య**) &(k)); )
#నిర్వచించు ORDERED_HASHMAP_FOREACH_కీలకం_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_కీలకం(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#నిర్వచించు _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#నిర్వచించు _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#నిర్వచించు _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

శూన్య hashmap_trim_pools(శూన్య);
