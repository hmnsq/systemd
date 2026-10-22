/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash-funcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "iterator.h"

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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASH_ਕੁੰਜੀ_ਮਾਪ 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Hashmap_ਅਧਾਰ Hashmap_ਅਧਾਰ;

/* Specific hashmap/set types */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Hashmap Hashmap;               /* Maps keys to values */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਸੈੱਟ_2 ਸੈੱਟ_2;                       /* Stores just keys */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

/* Macros for type checking */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_COMPATIBLE_WITH_HASHMAP_ਅਧਾਰ(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap_ਅਧਾਰ*) || \
         __builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*) || \
         __builtin_types_compatible_p(typeof(h), ਸੈੱਟ_2*))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*)) \

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHMAP_ਅਧਾਰ(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_HASHMAP_ਅਧਾਰ(h), \
                (Hashmap_ਅਧਾਰ*)(h), \
                (ਖਾਲੀ)0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (ਖਾਲੀ)0)

Hashmap* hashmap_new(ਅਚਲ ਬਣਤਰ hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ਅਚਲ ਬਣਤਰ hash_ops *hash_ops);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hashmap_free_and_replace(a, b)                          \
        free_and_replace_ਭਰਿਆ(a, b, hashmap_free)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_ਭਰਿਆ(a, b, ordered_hashmap_free)

Hashmap_ਅਧਾਰ* _hashmap_free(Hashmap_ਅਧਾਰ *h);
ਸਥਿਰ ਸਤਰਅੰਦਰ Hashmap* hashmap_free(Hashmap *h) {
        ਵਾਪਸ (ਖਾਲੀ*) _hashmap_free(HASHMAP_ਅਧਾਰ(h));
}
ਸਥਿਰ ਸਤਰਅੰਦਰ OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        ਵਾਪਸ (ਖਾਲੀ*) _hashmap_free(HASHMAP_ਅਧਾਰ(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
ਪੂਰਨ iterated_cache_get(IteratedCache *cache, ਅਚਲ ਖਾਲੀ ***res_ਕੁੰਜੀਆਂ, ਅਚਲ ਖਾਲੀ ***res_ਮੁੱਲਾਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ *res_n_entries);

Hashmap_ਅਧਾਰ* _hashmap_copy(Hashmap_ਅਧਾਰ *h);
ਸਥਿਰ ਸਤਰਅੰਦਰ Hashmap* hashmap_copy(Hashmap *h) {
        ਵਾਪਸ (Hashmap*) _hashmap_copy(HASHMAP_ਅਧਾਰ(h));
}
ਸਥਿਰ ਸਤਰਅੰਦਰ OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        ਵਾਪਸ (OrderedHashmap*) _hashmap_copy(HASHMAP_ਅਧਾਰ(h));
}

ਪੂਰਨ hashmap_ensure_allocated(Hashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops);
ਪੂਰਨ hashmap_ensure_put(Hashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ);
ਪੂਰਨ ordered_hashmap_ensure_allocated(OrderedHashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops);
ਪੂਰਨ hashmap_ensure_replace(Hashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ);

ਪੂਰਨ ordered_hashmap_ensure_put(OrderedHashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ);
ਪੂਰਨ ordered_hashmap_ensure_replace(OrderedHashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ);

IteratedCache* _hashmap_iterated_cache_new(Hashmap_ਅਧਾਰ *h);
ਸਥਿਰ ਸਤਰਅੰਦਰ IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        ਵਾਪਸ _hashmap_iterated_cache_new(HASHMAP_ਅਧਾਰ(h));
}
ਸਥਿਰ ਸਤਰਅੰਦਰ IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        ਵਾਪਸ _hashmap_iterated_cache_new(HASHMAP_ਅਧਾਰ(h));
}

ਪੂਰਨ hashmap_put(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_hashmap_put(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਵਾਪਸ hashmap_put(PLAIN_HASHMAP(h), ਕੁੰਜੀ, ਮੁੱਲ);
}

ਪੂਰਨ hashmap_put_strdup_ਭਰਿਆ(Hashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਅੱਖਰ *k, ਅਚਲ ਅੱਖਰ *v);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ hashmap_put_strdup(Hashmap **h, ਅਚਲ ਅੱਖਰ *k, ਅਚਲ ਅੱਖਰ *v) {
        ਵਾਪਸ hashmap_put_strdup_ਭਰਿਆ(h, &string_hash_ops_free_free, k, v);
}

ਪੂਰਨ hashmap_update(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_hashmap_update(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਵਾਪਸ hashmap_update(PLAIN_HASHMAP(h), ਕੁੰਜੀ, ਮੁੱਲ);
}

ਪੂਰਨ hashmap_replace(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_hashmap_replace(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਵਾਪਸ hashmap_replace(PLAIN_HASHMAP(h), ਕੁੰਜੀ, ਮੁੱਲ);
}

ਖਾਲੀ* _hashmap_get(Hashmap_ਅਧਾਰ *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hashmap_get(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_get(HASHMAP_ਅਧਾਰ(h), ਕੁੰਜੀ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_hashmap_get(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_get(HASHMAP_ਅਧਾਰ(h), ਕੁੰਜੀ);
}

ਖਾਲੀ* hashmap_get2(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_hashmap_get2(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ **ret) {
        ਵਾਪਸ hashmap_get2(PLAIN_HASHMAP(h), ਕੁੰਜੀ, ret);
}

bool hashmap_ਸ਼ਾਮਲ_ਹੈ_2(Hashmap_ਅਧਾਰ *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool hashmap_ਸ਼ਾਮਲ_ਹੈ(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ hashmap_ਸ਼ਾਮਲ_ਹੈ_2(HASHMAP_ਅਧਾਰ(h), ਕੁੰਜੀ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ordered_hashmap_ਸ਼ਾਮਲ_ਹੈ(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ hashmap_ਸ਼ਾਮਲ_ਹੈ_2(HASHMAP_ਅਧਾਰ(h), ਕੁੰਜੀ);
}

ਖਾਲੀ* _hashmap_remove(Hashmap_ਅਧਾਰ *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hashmap_remove(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_remove(HASHMAP_ਅਧਾਰ(h), ਕੁੰਜੀ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_hashmap_remove(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_remove(HASHMAP_ਅਧਾਰ(h), ਕੁੰਜੀ);
}

ਖਾਲੀ* hashmap_remove2(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_hashmap_remove2(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ **ret) {
        ਵਾਪਸ hashmap_remove2(PLAIN_HASHMAP(h), ਕੁੰਜੀ, ret);
}

ਖਾਲੀ* hashmap_remove_ਮੁੱਲ(Hashmap_ਅਧਾਰ *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hashmap_remove_ਮੁੱਲ_2(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਵਾਪਸ hashmap_remove_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(h), ਕੁੰਜੀ, ਮੁੱਲ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* ordered_hashmap_remove_ਮੁੱਲ(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਵਾਪਸ hashmap_remove_ਮੁੱਲ_2(PLAIN_HASHMAP(h), ਕੁੰਜੀ, ਮੁੱਲ);
}

ਪੂਰਨ hashmap_remove_and_put(Hashmap *h, ਅਚਲ ਖਾਲੀ *old_ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *new_ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_hashmap_remove_and_put(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *old_ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *new_ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਵਾਪਸ hashmap_remove_and_put(PLAIN_HASHMAP(h), old_ਕੁੰਜੀ, new_ਕੁੰਜੀ, ਮੁੱਲ);
}

ਪੂਰਨ hashmap_remove_and_replace(Hashmap *h, ਅਚਲ ਖਾਲੀ *old_ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *new_ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_hashmap_remove_and_replace(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *old_ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *new_ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਵਾਪਸ hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_ਕੁੰਜੀ, new_ਕੁੰਜੀ, ਮੁੱਲ);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
ਪੂਰਨ hashmap_ਮਿਲਾਓ(Hashmap *h, Hashmap *other);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hashmap_ਮਿਲਾਓ_2(h, other) hashmap_ਮਿਲਾਓ(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ordered_hashmap_ਮਿਲਾਓ(h, other) hashmap_ਮਿਲਾਓ_2(h, other)

ਪੂਰਨ _hashmap_reserve(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ entries_ਜੋੜੋ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ hashmap_reserve(Hashmap *h, ਬਿਨਾਨਿਸ਼ਾਨੀ entries_ਜੋੜੋ) {
        ਵਾਪਸ _hashmap_reserve(HASHMAP_ਅਧਾਰ(h), entries_ਜੋੜੋ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_hashmap_reserve(OrderedHashmap *h, ਬਿਨਾਨਿਸ਼ਾਨੀ entries_ਜੋੜੋ) {
        ਵਾਪਸ _hashmap_reserve(HASHMAP_ਅਧਾਰ(h), entries_ਜੋੜੋ);
}

ਪੂਰਨ _hashmap_move(Hashmap_ਅਧਾਰ *h, Hashmap_ਅਧਾਰ *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ hashmap_move(Hashmap *h, Hashmap *other) {
        ਵਾਪਸ _hashmap_move(HASHMAP_ਅਧਾਰ(h), HASHMAP_ਅਧਾਰ(other));
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        ਵਾਪਸ _hashmap_move(HASHMAP_ਅਧਾਰ(h), HASHMAP_ਅਧਾਰ(other));
}

ਪੂਰਨ _hashmap_move_one(Hashmap_ਅਧਾਰ *h, Hashmap_ਅਧਾਰ *other, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ hashmap_move_one(Hashmap *h, Hashmap *other, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_move_one(HASHMAP_ਅਧਾਰ(h), HASHMAP_ਅਧਾਰ(other), ਕੁੰਜੀ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_move_one(HASHMAP_ਅਧਾਰ(h), HASHMAP_ਅਧਾਰ(other), ਕੁੰਜੀ);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ hashmap_ਮਾਪ(Hashmap_ਅਧਾਰ *h) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ hashmap_ਮਾਪ_2(Hashmap *h) {
        ਵਾਪਸ hashmap_ਮਾਪ(HASHMAP_ਅਧਾਰ(h));
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ordered_hashmap_ਮਾਪ(OrderedHashmap *h) {
        ਵਾਪਸ hashmap_ਮਾਪ(HASHMAP_ਅਧਾਰ(h));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool hashmap_isempty(Hashmap *h) {
        ਵਾਪਸ hashmap_ਮਾਪ_2(h) == 0;
}
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ordered_hashmap_isempty(OrderedHashmap *h) {
        ਵਾਪਸ ordered_hashmap_ਮਾਪ(h) == 0;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ _hashmap_buckets(Hashmap_ਅਧਾਰ *h) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ hashmap_buckets(Hashmap *h) {
        ਵਾਪਸ _hashmap_buckets(HASHMAP_ਅਧਾਰ(h));
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ordered_hashmap_buckets(OrderedHashmap *h) {
        ਵਾਪਸ _hashmap_buckets(HASHMAP_ਅਧਾਰ(h));
}

bool _hashmap_iterate(Hashmap_ਅਧਾਰ *h, Iterator *i, ਖਾਲੀ **ਮੁੱਲ, ਅਚਲ ਖਾਲੀ **ਕੁੰਜੀ);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool hashmap_iterate(Hashmap *h, Iterator *i, ਖਾਲੀ **ਮੁੱਲ, ਅਚਲ ਖਾਲੀ **ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_iterate(HASHMAP_ਅਧਾਰ(h), i, ਮੁੱਲ, ਕੁੰਜੀ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, ਖਾਲੀ **ਮੁੱਲ, ਅਚਲ ਖਾਲੀ **ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_iterate(HASHMAP_ਅਧਾਰ(h), i, ਮੁੱਲ, ਕੁੰਜੀ);
}

ਖਾਲੀ _hashmap_clear(Hashmap_ਅਧਾਰ *h);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_ਅਧਾਰ(h));
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ordered_hashmap_clear(OrderedHashmap *h) {
        _hashmap_clear(HASHMAP_ਅਧਾਰ(h));
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

ਖਾਲੀ *hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(Hashmap_ਅਧਾਰ *h, bool remove, ਖਾਲੀ **ret_ਕੁੰਜੀ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hashmap_steal_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(Hashmap *h, ਖਾਲੀ **ret) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(h), true, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_hashmap_steal_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(OrderedHashmap *h, ਖਾਲੀ **ret) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(h), true, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ_2(Hashmap *h, ਖਾਲੀ **ret) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(h), false, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(OrderedHashmap *h, ਖਾਲੀ **ret) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(h), false, ret);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hashmap_steal_ਪਹਿਲਾ(Hashmap *h) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(h), true, ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_hashmap_steal_ਪਹਿਲਾ(OrderedHashmap *h) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(h), true, ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hashmap_ਪਹਿਲਾ(Hashmap *h) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(h), false, ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_hashmap_ਪਹਿਲਾ(OrderedHashmap *h) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(h), false, ਨਿਰਾ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ(Hashmap_ਅਧਾਰ *h, bool remove) {
        ਖਾਲੀ *ਕੁੰਜੀ = ਨਿਰਾ;

        (ਖਾਲੀ) hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(h), remove, &ਕੁੰਜੀ);
        ਵਾਪਸ ਕੁੰਜੀ;
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hashmap_steal_ਪਹਿਲਾ_ਕੁੰਜੀ(Hashmap *h) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ(HASHMAP_ਅਧਾਰ(h), true);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_hashmap_steal_ਪਹਿਲਾ_ਕੁੰਜੀ(OrderedHashmap *h) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ(HASHMAP_ਅਧਾਰ(h), true);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_2(Hashmap *h) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ(HASHMAP_ਅਧਾਰ(h), false);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ(OrderedHashmap *h) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ(HASHMAP_ਅਧਾਰ(h), false);
}

/* no hashmap_next */
ਖਾਲੀ* ordered_hashmap_ਅਗਲਾ(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ);

ਅੱਖਰ** _hashmap_get_strv(Hashmap_ਅਧਾਰ *h);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ** hashmap_get_strv(Hashmap *h) {
        ਵਾਪਸ _hashmap_get_strv(HASHMAP_ਅਧਾਰ(h));
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ** ordered_hashmap_get_strv(OrderedHashmap *h) {
        ਵਾਪਸ _hashmap_get_strv(HASHMAP_ਅਧਾਰ(h));
}

ਪੂਰਨ _hashmap_dump_sorted(Hashmap_ਅਧਾਰ *h, ਖਾਲੀ ***ret, size_t *ret_n);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ hashmap_dump_sorted(Hashmap *h, ਖਾਲੀ ***ret, size_t *ret_n) {
        ਵਾਪਸ _hashmap_dump_sorted(HASHMAP_ਅਧਾਰ(h), ret, ret_n);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_hashmap_dump_sorted(OrderedHashmap *h, ਖਾਲੀ ***ret, size_t *ret_n) {
        ਵਾਪਸ _hashmap_dump_sorted(HASHMAP_ਅਧਾਰ(h), ret, ret_n);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੈੱਟ_dump_sorted(ਸੈੱਟ_2 *h, ਖਾਲੀ ***ret, size_t *ret_n) {
        ਵਾਪਸ _hashmap_dump_sorted(HASHMAP_ਅਧਾਰ(h), ret, ret_n);
}

ਪੂਰਨ hashmap_dump_ਕੁੰਜੀਆਂ_sorted(Hashmap_ਅਧਾਰ *h, ਖਾਲੀ ***ret, size_t *ret_n);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ hashmap_dump_ਕੁੰਜੀਆਂ_sorted_2(Hashmap *h, ਖਾਲੀ ***ret, size_t *ret_n) {
        ਵਾਪਸ hashmap_dump_ਕੁੰਜੀਆਂ_sorted(HASHMAP_ਅਧਾਰ(h), ret, ret_n);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_hashmap_dump_ਕੁੰਜੀਆਂ_sorted(OrderedHashmap *h, ਖਾਲੀ ***ret, size_t *ret_n) {
        ਵਾਪਸ hashmap_dump_ਕੁੰਜੀਆਂ_sorted(HASHMAP_ਅਧਾਰ(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHMAP_ਅਧਾਰ_FOREACH(e, h, i) \
        ਲਈ (Iterator i = ITERATOR_ਪਹਿਲਾ; _hashmap_iterate((h), &i, (ਖਾਲੀ**)&(e), ਨਿਰਾ); )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHMAP_ਅਧਾਰ_FOREACH_2(e, h) \
        HASHMAP_ਅਧਾਰ_FOREACH(e, h, UNIQ_T(i, UNIQ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _HASHMAP_FOREACH(e, h, i) \
        ਲਈ (Iterator i = ITERATOR_ਪਹਿਲਾ; hashmap_iterate((h), &i, (ਖਾਲੀ**)&(e), ਨਿਰਾ); )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _ORDERED_HASHMAP_FOREACH(e, h, i) \
        ਲਈ (Iterator i = ITERATOR_ਪਹਿਲਾ; ordered_hashmap_iterate((h), &i, (ਖਾਲੀ**)&(e), ਨਿਰਾ); )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHMAP_ਅਧਾਰ_FOREACH_ਕੁੰਜੀ(e, k, h, i) \
        ਲਈ (Iterator i = ITERATOR_ਪਹਿਲਾ; _hashmap_iterate((h), &i, (ਖਾਲੀ**)&(e), (ਅਚਲ ਖਾਲੀ**) &(k)); )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHMAP_ਅਧਾਰ_FOREACH_ਕੁੰਜੀ_2(e, k, h) \
        HASHMAP_ਅਧਾਰ_FOREACH_ਕੁੰਜੀ(e, k, h, UNIQ_T(i, UNIQ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHMAP_FOREACH_ਕੁੰਜੀ(e, k, h, i) \
        ਲਈ (Iterator i = ITERATOR_ਪਹਿਲਾ; hashmap_iterate((h), &i, (ਖਾਲੀ**)&(e), (ਅਚਲ ਖਾਲੀ**) &(k)); )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHMAP_FOREACH_ਕੁੰਜੀ_2(e, k, h) \
        HASHMAP_FOREACH_ਕੁੰਜੀ(e, k, h, UNIQ_T(i, UNIQ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ORDERED_HASHMAP_FOREACH_ਕੁੰਜੀ(e, k, h, i) \
        ਲਈ (Iterator i = ITERATOR_ਪਹਿਲਾ; ordered_hashmap_iterate((h), &i, (ਖਾਲੀ**)&(e), (ਅਚਲ ਖਾਲੀ**) &(k)); )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ORDERED_HASHMAP_FOREACH_ਕੁੰਜੀ_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_ਕੁੰਜੀ(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

ਖਾਲੀ hashmap_trim_pools(ਖਾਲੀ);
