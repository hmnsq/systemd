/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "hash-funcs.h"
#અન્તર્ભાવ "iterator.h"

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

#define HASH_ચાવી_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
પ્રકારવ્યાખ્યા સંરચના HashmapBase HashmapBase;

/* Specific hashmap/set types */
પ્રકારવ્યાખ્યા સંરચના Hashmap Hashmap;               /* Maps keys to values */
પ્રકારવ્યાખ્યા સંરચના OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
પ્રકારવ્યાખ્યા સંરચના Set Set;                       /* Stores just keys */

પ્રકારવ્યાખ્યા સંરચના IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

/* Macros for type checking */
#define PTR_COMPATIBLE_WITH_HASHMAP_BASE(h) \
        (__builtin_types_compatible_p(typeof(h), HashmapBase*) || \
         __builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*) || \
         __builtin_types_compatible_p(typeof(h), Set*))

#define PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*)) \

#define HASHMAP_BASE(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_HASHMAP_BASE(h), \
                (HashmapBase*)(h), \
                (શૂન્ય)0)

#define PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (શૂન્ય)0)

Hashmap* hashmap_new(અચલ સંરચના hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(અચલ સંરચના hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
સ્થિર અંતઃરેખ Hashmap* hashmap_free(Hashmap *h) {
        પાછા (શૂન્ય*) _hashmap_free(HASHMAP_BASE(h));
}
સ્થિર અંતઃરેખ OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        પાછા (શૂન્ય*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
પૂર્ણાંક iterated_cache_get(IteratedCache *cache, અચલ શૂન્ય ***res_ચાવીઓ, અચલ શૂન્ય ***res_કિંમતો, unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
સ્થિર અંતઃરેખ Hashmap* hashmap_copy(Hashmap *h) {
        પાછા (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
સ્થિર અંતઃરેખ OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        પાછા (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

પૂર્ણાંક hashmap_ensure_allocated(Hashmap **h, અચલ સંરચના hash_ops *hash_ops);
પૂર્ણાંક hashmap_ensure_put(Hashmap **h, અચલ સંરચના hash_ops *hash_ops, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત);
પૂર્ણાંક ordered_hashmap_ensure_allocated(OrderedHashmap **h, અચલ સંરચના hash_ops *hash_ops);
પૂર્ણાંક hashmap_ensure_replace(Hashmap **h, અચલ સંરચના hash_ops *hash_ops, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત);

પૂર્ણાંક ordered_hashmap_ensure_put(OrderedHashmap **h, અચલ સંરચના hash_ops *hash_ops, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત);
પૂર્ણાંક ordered_hashmap_ensure_replace(OrderedHashmap **h, અચલ સંરચના hash_ops *hash_ops, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
સ્થિર અંતઃરેખ IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        પાછા _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
સ્થિર અંતઃરેખ IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        પાછા _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

પૂર્ણાંક hashmap_put(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત);
સ્થિર અંતઃરેખ પૂર્ણાંક ordered_hashmap_put(OrderedHashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        પાછા hashmap_put(PLAIN_HASHMAP(h), ચાવી, કિંમત);
}

પૂર્ણાંક hashmap_put_strdup_full(Hashmap **h, અચલ સંરચના hash_ops *hash_ops, અચલ અક્ષર *k, અચલ અક્ષર *v);
સ્થિર અંતઃરેખ પૂર્ણાંક hashmap_put_strdup(Hashmap **h, અચલ અક્ષર *k, અચલ અક્ષર *v) {
        પાછા hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

પૂર્ણાંક hashmap_update(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત);
સ્થિર અંતઃરેખ પૂર્ણાંક ordered_hashmap_update(OrderedHashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        પાછા hashmap_update(PLAIN_HASHMAP(h), ચાવી, કિંમત);
}

પૂર્ણાંક hashmap_replace(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત);
સ્થિર અંતઃરેખ પૂર્ણાંક ordered_hashmap_replace(OrderedHashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        પાછા hashmap_replace(PLAIN_HASHMAP(h), ચાવી, કિંમત);
}

શૂન્ય* _hashmap_get(HashmapBase *h, અચલ શૂન્ય *ચાવી);
સ્થિર અંતઃરેખ શૂન્ય *hashmap_get(Hashmap *h, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_get(HASHMAP_BASE(h), ચાવી);
}
સ્થિર અંતઃરેખ શૂન્ય *ordered_hashmap_get(OrderedHashmap *h, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_get(HASHMAP_BASE(h), ચાવી);
}

શૂન્ય* hashmap_get2(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય **ret);
સ્થિર અંતઃરેખ શૂન્ય *ordered_hashmap_get2(OrderedHashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય **ret) {
        પાછા hashmap_get2(PLAIN_HASHMAP(h), ચાવી, ret);
}

bool _hashmap_contains(HashmapBase *h, અચલ શૂન્ય *ચાવી);
સ્થિર અંતઃરેખ bool hashmap_contains(Hashmap *h, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_contains(HASHMAP_BASE(h), ચાવી);
}
સ્થિર અંતઃરેખ bool ordered_hashmap_contains(OrderedHashmap *h, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_contains(HASHMAP_BASE(h), ચાવી);
}

શૂન્ય* _hashmap_remove(HashmapBase *h, અચલ શૂન્ય *ચાવી);
સ્થિર અંતઃરેખ શૂન્ય *hashmap_remove(Hashmap *h, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_remove(HASHMAP_BASE(h), ચાવી);
}
સ્થિર અંતઃરેખ શૂન્ય *ordered_hashmap_remove(OrderedHashmap *h, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_remove(HASHMAP_BASE(h), ચાવી);
}

શૂન્ય* hashmap_remove2(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય **ret);
સ્થિર અંતઃરેખ શૂન્ય *ordered_hashmap_remove2(OrderedHashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય **ret) {
        પાછા hashmap_remove2(PLAIN_HASHMAP(h), ચાવી, ret);
}

શૂન્ય* hashmap_remove_કિંમત(HashmapBase *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત);
સ્થિર અંતઃરેખ શૂન્ય *hashmap_remove_કિંમત_2(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        પાછા hashmap_remove_કિંમત(HASHMAP_BASE(h), ચાવી, કિંમત);
}

સ્થિર અંતઃરેખ શૂન્ય* ordered_hashmap_remove_કિંમત(OrderedHashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        પાછા hashmap_remove_કિંમત_2(PLAIN_HASHMAP(h), ચાવી, કિંમત);
}

પૂર્ણાંક hashmap_remove_and_put(Hashmap *h, અચલ શૂન્ય *old_ચાવી, અચલ શૂન્ય *new_ચાવી, શૂન્ય *કિંમત);
સ્થિર અંતઃરેખ પૂર્ણાંક ordered_hashmap_remove_and_put(OrderedHashmap *h, અચલ શૂન્ય *old_ચાવી, અચલ શૂન્ય *new_ચાવી, શૂન્ય *કિંમત) {
        પાછા hashmap_remove_and_put(PLAIN_HASHMAP(h), old_ચાવી, new_ચાવી, કિંમત);
}

પૂર્ણાંક hashmap_remove_and_replace(Hashmap *h, અચલ શૂન્ય *old_ચાવી, અચલ શૂન્ય *new_ચાવી, શૂન્ય *કિંમત);
સ્થિર અંતઃરેખ પૂર્ણાંક ordered_hashmap_remove_and_replace(OrderedHashmap *h, અચલ શૂન્ય *old_ચાવી, અચલ શૂન્ય *new_ચાવી, શૂન્ય *કિંમત) {
        પાછા hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_ચાવી, new_ચાવી, કિંમત);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
પૂર્ણાંક _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

પૂર્ણાંક _hashmap_reserve(HashmapBase *h, unsigned entries_add);
સ્થિર અંતઃરેખ પૂર્ણાંક hashmap_reserve(Hashmap *h, unsigned entries_add) {
        પાછા _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
સ્થિર અંતઃરેખ પૂર્ણાંક ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        પાછા _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

પૂર્ણાંક _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
સ્થિર અંતઃરેખ પૂર્ણાંક hashmap_move(Hashmap *h, Hashmap *other) {
        પાછા _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
સ્થિર અંતઃરેખ પૂર્ણાંક ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        પાછા _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

પૂર્ણાંક _hashmap_move_one(HashmapBase *h, HashmapBase *other, અચલ શૂન્ય *ચાવી);
સ્થિર અંતઃરેખ પૂર્ણાંક hashmap_move_one(Hashmap *h, Hashmap *other, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), ચાવી);
}
સ્થિર અંતઃરેખ પૂર્ણાંક ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), ચાવી);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
સ્થિર અંતઃરેખ unsigned hashmap_size(Hashmap *h) {
        પાછા _hashmap_size(HASHMAP_BASE(h));
}
સ્થિર અંતઃરેખ unsigned ordered_hashmap_size(OrderedHashmap *h) {
        પાછા _hashmap_size(HASHMAP_BASE(h));
}

સ્થિર અંતઃરેખ bool hashmap_isempty(Hashmap *h) {
        પાછા hashmap_size(h) == 0;
}
સ્થિર અંતઃરેખ bool ordered_hashmap_isempty(OrderedHashmap *h) {
        પાછા ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
સ્થિર અંતઃરેખ unsigned hashmap_buckets(Hashmap *h) {
        પાછા _hashmap_buckets(HASHMAP_BASE(h));
}
સ્થિર અંતઃરેખ unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        પાછા _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, શૂન્ય **કિંમત, અચલ શૂન્ય **ચાવી);
સ્થિર અંતઃરેખ bool hashmap_iterate(Hashmap *h, Iterator *i, શૂન્ય **કિંમત, અચલ શૂન્ય **ચાવી) {
        પાછા _hashmap_iterate(HASHMAP_BASE(h), i, કિંમત, ચાવી);
}
સ્થિર અંતઃરેખ bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, શૂન્ય **કિંમત, અચલ શૂન્ય **ચાવી) {
        પાછા _hashmap_iterate(HASHMAP_BASE(h), i, કિંમત, ચાવી);
}

શૂન્ય _hashmap_clear(HashmapBase *h);
સ્થિર અંતઃરેખ શૂન્ય hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
સ્થિર અંતઃરેખ શૂન્ય ordered_hashmap_clear(OrderedHashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
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

શૂન્ય *hashmap_first_ચાવી_and_કિંમત(HashmapBase *h, bool remove, શૂન્ય **ret_ચાવી);
સ્થિર અંતઃરેખ શૂન્ય *hashmap_steal_first_ચાવી_and_કિંમત(Hashmap *h, શૂન્ય **ret) {
        પાછા hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE(h), true, ret);
}
સ્થિર અંતઃરેખ શૂન્ય *ordered_hashmap_steal_first_ચાવી_and_કિંમત(OrderedHashmap *h, શૂન્ય **ret) {
        પાછા hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE(h), true, ret);
}
સ્થિર અંતઃરેખ શૂન્ય *hashmap_first_ચાવી_and_કિંમત_2(Hashmap *h, શૂન્ય **ret) {
        પાછા hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE(h), false, ret);
}
સ્થિર અંતઃરેખ શૂન્ય *ordered_hashmap_first_ચાવી_and_કિંમત(OrderedHashmap *h, શૂન્ય **ret) {
        પાછા hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE(h), false, ret);
}

સ્થિર અંતઃરેખ શૂન્ય *hashmap_steal_first(Hashmap *h) {
        પાછા hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE(h), true, ખાલી);
}
સ્થિર અંતઃરેખ શૂન્ય *ordered_hashmap_steal_first(OrderedHashmap *h) {
        પાછા hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE(h), true, ખાલી);
}
સ્થિર અંતઃરેખ શૂન્ય *hashmap_first(Hashmap *h) {
        પાછા hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE(h), false, ખાલી);
}
સ્થિર અંતઃરેખ શૂન્ય *ordered_hashmap_first(OrderedHashmap *h) {
        પાછા hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE(h), false, ખાલી);
}

સ્થિર અંતઃરેખ શૂન્ય *hashmap_first_ચાવી(HashmapBase *h, bool remove) {
        શૂન્ય *ચાવી = ખાલી;

        (શૂન્ય) hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE(h), remove, &ચાવી);
        પાછા ચાવી;
}
સ્થિર અંતઃરેખ શૂન્ય *hashmap_steal_first_ચાવી(Hashmap *h) {
        પાછા hashmap_first_ચાવી(HASHMAP_BASE(h), true);
}
સ્થિર અંતઃરેખ શૂન્ય *ordered_hashmap_steal_first_ચાવી(OrderedHashmap *h) {
        પાછા hashmap_first_ચાવી(HASHMAP_BASE(h), true);
}
સ્થિર અંતઃરેખ શૂન્ય *hashmap_first_ચાવી_2(Hashmap *h) {
        પાછા hashmap_first_ચાવી(HASHMAP_BASE(h), false);
}
સ્થિર અંતઃરેખ શૂન્ય *ordered_hashmap_first_ચાવી(OrderedHashmap *h) {
        પાછા hashmap_first_ચાવી(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
શૂન્ય* ordered_hashmap_next(OrderedHashmap *h, અચલ શૂન્ય *ચાવી);

અક્ષર** _hashmap_get_strv(HashmapBase *h);
સ્થિર અંતઃરેખ અક્ષર** hashmap_get_strv(Hashmap *h) {
        પાછા _hashmap_get_strv(HASHMAP_BASE(h));
}
સ્થિર અંતઃરેખ અક્ષર** ordered_hashmap_get_strv(OrderedHashmap *h) {
        પાછા _hashmap_get_strv(HASHMAP_BASE(h));
}

પૂર્ણાંક _hashmap_dump_sorted(HashmapBase *h, શૂન્ય ***ret, size_t *ret_n);
સ્થિર અંતઃરેખ પૂર્ણાંક hashmap_dump_sorted(Hashmap *h, શૂન્ય ***ret, size_t *ret_n) {
        પાછા _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
સ્થિર અંતઃરેખ પૂર્ણાંક ordered_hashmap_dump_sorted(OrderedHashmap *h, શૂન્ય ***ret, size_t *ret_n) {
        પાછા _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
સ્થિર અંતઃરેખ પૂર્ણાંક set_dump_sorted(Set *h, શૂન્ય ***ret, size_t *ret_n) {
        પાછા _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

પૂર્ણાંક hashmap_dump_ચાવીઓ_sorted(HashmapBase *h, શૂન્ય ***ret, size_t *ret_n);
સ્થિર અંતઃરેખ પૂર્ણાંક hashmap_dump_ચાવીઓ_sorted_2(Hashmap *h, શૂન્ય ***ret, size_t *ret_n) {
        પાછા hashmap_dump_ચાવીઓ_sorted(HASHMAP_BASE(h), ret, ret_n);
}
સ્થિર અંતઃરેખ પૂર્ણાંક ordered_hashmap_dump_ચાવીઓ_sorted(OrderedHashmap *h, શૂન્ય ***ret, size_t *ret_n) {
        પાછા hashmap_dump_ચાવીઓ_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        માટે (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (શૂન્ય**)&(e), ખાલી); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        માટે (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (શૂન્ય**)&(e), ખાલી); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        માટે (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (શૂન્ય**)&(e), ખાલી); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define HASHMAP_BASE_FOREACH_ચાવી(e, k, h, i) \
        માટે (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (શૂન્ય**)&(e), (અચલ શૂન્ય**) &(k)); )
#define HASHMAP_BASE_FOREACH_ચાવી_2(e, k, h) \
        HASHMAP_BASE_FOREACH_ચાવી(e, k, h, UNIQ_T(i, UNIQ))

#define HASHMAP_FOREACH_ચાવી(e, k, h, i) \
        માટે (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (શૂન્ય**)&(e), (અચલ શૂન્ય**) &(k)); )
#define HASHMAP_FOREACH_ચાવી_2(e, k, h) \
        HASHMAP_FOREACH_ચાવી(e, k, h, UNIQ_T(i, UNIQ))

#define ORDERED_HASHMAP_FOREACH_ચાવી(e, k, h, i) \
        માટે (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (શૂન્ય**)&(e), (અચલ શૂન્ય**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_ચાવી_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_ચાવી(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

શૂન્ય hashmap_trim_pools(શૂન્ય);
