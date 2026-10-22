/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"
#شامل "hash-funcs.h"
#شامل "iterator.h"

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

#define HASH_کلید_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
تعریفنوع ساختار HashmapBase HashmapBase;

/* Specific hashmap/set types */
تعریفنوع ساختار Hashmap Hashmap;               /* Maps keys to values */
تعریفنوع ساختار OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
تعریفنوع ساختار Set Set;                       /* Stores just keys */

تعریفنوع ساختار IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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
                (تهی)0)

#define PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (تهی)0)

Hashmap* hashmap_new(ثابت ساختار hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ثابت ساختار hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
ایستا درونخطی Hashmap* hashmap_free(Hashmap *h) {
        بازگشت (تهی*) _hashmap_free(HASHMAP_BASE(h));
}
ایستا درونخطی OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        بازگشت (تهی*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
صحیح iterated_cache_get(IteratedCache *cache, ثابت تهی ***res_کلیدها, ثابت تهی ***res_مقادیر, unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
ایستا درونخطی Hashmap* hashmap_copy(Hashmap *h) {
        بازگشت (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
ایستا درونخطی OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        بازگشت (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

صحیح hashmap_ensure_allocated(Hashmap **h, ثابت ساختار hash_ops *hash_ops);
صحیح hashmap_ensure_put(Hashmap **h, ثابت ساختار hash_ops *hash_ops, ثابت تهی *کلید, تهی *مقدار);
صحیح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت ساختار hash_ops *hash_ops);
صحیح hashmap_ensure_replace(Hashmap **h, ثابت ساختار hash_ops *hash_ops, ثابت تهی *کلید, تهی *مقدار);

صحیح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت ساختار hash_ops *hash_ops, ثابت تهی *کلید, تهی *مقدار);
صحیح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت ساختار hash_ops *hash_ops, ثابت تهی *کلید, تهی *مقدار);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
ایستا درونخطی IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        بازگشت _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
ایستا درونخطی IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        بازگشت _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

صحیح hashmap_put(Hashmap *h, ثابت تهی *کلید, تهی *مقدار);
ایستا درونخطی صحیح ordered_hashmap_put(OrderedHashmap *h, ثابت تهی *کلید, تهی *مقدار) {
        بازگشت hashmap_put(PLAIN_HASHMAP(h), کلید, مقدار);
}

صحیح hashmap_put_strdup_full(Hashmap **h, ثابت ساختار hash_ops *hash_ops, ثابت نویسه *k, ثابت نویسه *v);
ایستا درونخطی صحیح hashmap_put_strdup(Hashmap **h, ثابت نویسه *k, ثابت نویسه *v) {
        بازگشت hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

صحیح hashmap_update(Hashmap *h, ثابت تهی *کلید, تهی *مقدار);
ایستا درونخطی صحیح ordered_hashmap_update(OrderedHashmap *h, ثابت تهی *کلید, تهی *مقدار) {
        بازگشت hashmap_update(PLAIN_HASHMAP(h), کلید, مقدار);
}

صحیح hashmap_replace(Hashmap *h, ثابت تهی *کلید, تهی *مقدار);
ایستا درونخطی صحیح ordered_hashmap_replace(OrderedHashmap *h, ثابت تهی *کلید, تهی *مقدار) {
        بازگشت hashmap_replace(PLAIN_HASHMAP(h), کلید, مقدار);
}

تهی* _hashmap_get(HashmapBase *h, ثابت تهی *کلید);
ایستا درونخطی تهی *hashmap_get(Hashmap *h, ثابت تهی *کلید) {
        بازگشت _hashmap_get(HASHMAP_BASE(h), کلید);
}
ایستا درونخطی تهی *ordered_hashmap_get(OrderedHashmap *h, ثابت تهی *کلید) {
        بازگشت _hashmap_get(HASHMAP_BASE(h), کلید);
}

تهی* hashmap_get2(Hashmap *h, ثابت تهی *کلید, تهی **ret);
ایستا درونخطی تهی *ordered_hashmap_get2(OrderedHashmap *h, ثابت تهی *کلید, تهی **ret) {
        بازگشت hashmap_get2(PLAIN_HASHMAP(h), کلید, ret);
}

bool _hashmap_contains(HashmapBase *h, ثابت تهی *کلید);
ایستا درونخطی bool hashmap_contains(Hashmap *h, ثابت تهی *کلید) {
        بازگشت _hashmap_contains(HASHMAP_BASE(h), کلید);
}
ایستا درونخطی bool ordered_hashmap_contains(OrderedHashmap *h, ثابت تهی *کلید) {
        بازگشت _hashmap_contains(HASHMAP_BASE(h), کلید);
}

تهی* _hashmap_remove(HashmapBase *h, ثابت تهی *کلید);
ایستا درونخطی تهی *hashmap_remove(Hashmap *h, ثابت تهی *کلید) {
        بازگشت _hashmap_remove(HASHMAP_BASE(h), کلید);
}
ایستا درونخطی تهی *ordered_hashmap_remove(OrderedHashmap *h, ثابت تهی *کلید) {
        بازگشت _hashmap_remove(HASHMAP_BASE(h), کلید);
}

تهی* hashmap_remove2(Hashmap *h, ثابت تهی *کلید, تهی **ret);
ایستا درونخطی تهی *ordered_hashmap_remove2(OrderedHashmap *h, ثابت تهی *کلید, تهی **ret) {
        بازگشت hashmap_remove2(PLAIN_HASHMAP(h), کلید, ret);
}

تهی* hashmap_remove_مقدار(HashmapBase *h, ثابت تهی *کلید, تهی *مقدار);
ایستا درونخطی تهی *hashmap_remove_مقدار_2(Hashmap *h, ثابت تهی *کلید, تهی *مقدار) {
        بازگشت hashmap_remove_مقدار(HASHMAP_BASE(h), کلید, مقدار);
}

ایستا درونخطی تهی* ordered_hashmap_remove_مقدار(OrderedHashmap *h, ثابت تهی *کلید, تهی *مقدار) {
        بازگشت hashmap_remove_مقدار_2(PLAIN_HASHMAP(h), کلید, مقدار);
}

صحیح hashmap_remove_and_put(Hashmap *h, ثابت تهی *old_کلید, ثابت تهی *new_کلید, تهی *مقدار);
ایستا درونخطی صحیح ordered_hashmap_remove_and_put(OrderedHashmap *h, ثابت تهی *old_کلید, ثابت تهی *new_کلید, تهی *مقدار) {
        بازگشت hashmap_remove_and_put(PLAIN_HASHMAP(h), old_کلید, new_کلید, مقدار);
}

صحیح hashmap_remove_and_replace(Hashmap *h, ثابت تهی *old_کلید, ثابت تهی *new_کلید, تهی *مقدار);
ایستا درونخطی صحیح ordered_hashmap_remove_and_replace(OrderedHashmap *h, ثابت تهی *old_کلید, ثابت تهی *new_کلید, تهی *مقدار) {
        بازگشت hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_کلید, new_کلید, مقدار);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
صحیح _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

صحیح _hashmap_reserve(HashmapBase *h, unsigned entries_add);
ایستا درونخطی صحیح hashmap_reserve(Hashmap *h, unsigned entries_add) {
        بازگشت _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
ایستا درونخطی صحیح ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        بازگشت _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

صحیح _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ایستا درونخطی صحیح hashmap_move(Hashmap *h, Hashmap *other) {
        بازگشت _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
ایستا درونخطی صحیح ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        بازگشت _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

صحیح _hashmap_move_one(HashmapBase *h, HashmapBase *other, ثابت تهی *کلید);
ایستا درونخطی صحیح hashmap_move_one(Hashmap *h, Hashmap *other, ثابت تهی *کلید) {
        بازگشت _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), کلید);
}
ایستا درونخطی صحیح ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ثابت تهی *کلید) {
        بازگشت _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), کلید);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
ایستا درونخطی unsigned hashmap_size(Hashmap *h) {
        بازگشت _hashmap_size(HASHMAP_BASE(h));
}
ایستا درونخطی unsigned ordered_hashmap_size(OrderedHashmap *h) {
        بازگشت _hashmap_size(HASHMAP_BASE(h));
}

ایستا درونخطی bool hashmap_isempty(Hashmap *h) {
        بازگشت hashmap_size(h) == 0;
}
ایستا درونخطی bool ordered_hashmap_isempty(OrderedHashmap *h) {
        بازگشت ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
ایستا درونخطی unsigned hashmap_buckets(Hashmap *h) {
        بازگشت _hashmap_buckets(HASHMAP_BASE(h));
}
ایستا درونخطی unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        بازگشت _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, تهی **مقدار, ثابت تهی **کلید);
ایستا درونخطی bool hashmap_iterate(Hashmap *h, Iterator *i, تهی **مقدار, ثابت تهی **کلید) {
        بازگشت _hashmap_iterate(HASHMAP_BASE(h), i, مقدار, کلید);
}
ایستا درونخطی bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, تهی **مقدار, ثابت تهی **کلید) {
        بازگشت _hashmap_iterate(HASHMAP_BASE(h), i, مقدار, کلید);
}

تهی _hashmap_clear(HashmapBase *h);
ایستا درونخطی تهی hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
ایستا درونخطی تهی ordered_hashmap_clear(OrderedHashmap *h) {
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

تهی *hashmap_first_کلید_and_مقدار(HashmapBase *h, bool remove, تهی **ret_کلید);
ایستا درونخطی تهی *hashmap_steal_first_کلید_and_مقدار(Hashmap *h, تهی **ret) {
        بازگشت hashmap_first_کلید_and_مقدار(HASHMAP_BASE(h), true, ret);
}
ایستا درونخطی تهی *ordered_hashmap_steal_first_کلید_and_مقدار(OrderedHashmap *h, تهی **ret) {
        بازگشت hashmap_first_کلید_and_مقدار(HASHMAP_BASE(h), true, ret);
}
ایستا درونخطی تهی *hashmap_first_کلید_and_مقدار_2(Hashmap *h, تهی **ret) {
        بازگشت hashmap_first_کلید_and_مقدار(HASHMAP_BASE(h), false, ret);
}
ایستا درونخطی تهی *ordered_hashmap_first_کلید_and_مقدار(OrderedHashmap *h, تهی **ret) {
        بازگشت hashmap_first_کلید_and_مقدار(HASHMAP_BASE(h), false, ret);
}

ایستا درونخطی تهی *hashmap_steal_first(Hashmap *h) {
        بازگشت hashmap_first_کلید_and_مقدار(HASHMAP_BASE(h), true, پوچ);
}
ایستا درونخطی تهی *ordered_hashmap_steal_first(OrderedHashmap *h) {
        بازگشت hashmap_first_کلید_and_مقدار(HASHMAP_BASE(h), true, پوچ);
}
ایستا درونخطی تهی *hashmap_first(Hashmap *h) {
        بازگشت hashmap_first_کلید_and_مقدار(HASHMAP_BASE(h), false, پوچ);
}
ایستا درونخطی تهی *ordered_hashmap_first(OrderedHashmap *h) {
        بازگشت hashmap_first_کلید_and_مقدار(HASHMAP_BASE(h), false, پوچ);
}

ایستا درونخطی تهی *hashmap_first_کلید(HashmapBase *h, bool remove) {
        تهی *کلید = پوچ;

        (تهی) hashmap_first_کلید_and_مقدار(HASHMAP_BASE(h), remove, &کلید);
        بازگشت کلید;
}
ایستا درونخطی تهی *hashmap_steal_first_کلید(Hashmap *h) {
        بازگشت hashmap_first_کلید(HASHMAP_BASE(h), true);
}
ایستا درونخطی تهی *ordered_hashmap_steal_first_کلید(OrderedHashmap *h) {
        بازگشت hashmap_first_کلید(HASHMAP_BASE(h), true);
}
ایستا درونخطی تهی *hashmap_first_کلید_2(Hashmap *h) {
        بازگشت hashmap_first_کلید(HASHMAP_BASE(h), false);
}
ایستا درونخطی تهی *ordered_hashmap_first_کلید(OrderedHashmap *h) {
        بازگشت hashmap_first_کلید(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
تهی* ordered_hashmap_next(OrderedHashmap *h, ثابت تهی *کلید);

نویسه** _hashmap_get_strv(HashmapBase *h);
ایستا درونخطی نویسه** hashmap_get_strv(Hashmap *h) {
        بازگشت _hashmap_get_strv(HASHMAP_BASE(h));
}
ایستا درونخطی نویسه** ordered_hashmap_get_strv(OrderedHashmap *h) {
        بازگشت _hashmap_get_strv(HASHMAP_BASE(h));
}

صحیح _hashmap_dump_sorted(HashmapBase *h, تهی ***ret, size_t *ret_n);
ایستا درونخطی صحیح hashmap_dump_sorted(Hashmap *h, تهی ***ret, size_t *ret_n) {
        بازگشت _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ایستا درونخطی صحیح ordered_hashmap_dump_sorted(OrderedHashmap *h, تهی ***ret, size_t *ret_n) {
        بازگشت _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ایستا درونخطی صحیح set_dump_sorted(Set *h, تهی ***ret, size_t *ret_n) {
        بازگشت _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

صحیح hashmap_dump_کلیدها_sorted(HashmapBase *h, تهی ***ret, size_t *ret_n);
ایستا درونخطی صحیح hashmap_dump_کلیدها_sorted_2(Hashmap *h, تهی ***ret, size_t *ret_n) {
        بازگشت hashmap_dump_کلیدها_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ایستا درونخطی صحیح ordered_hashmap_dump_کلیدها_sorted(OrderedHashmap *h, تهی ***ret, size_t *ret_n) {
        بازگشت hashmap_dump_کلیدها_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        برای (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (تهی**)&(e), پوچ); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        برای (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (تهی**)&(e), پوچ); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        برای (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (تهی**)&(e), پوچ); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define HASHMAP_BASE_FOREACH_کلید(e, k, h, i) \
        برای (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (تهی**)&(e), (ثابت تهی**) &(k)); )
#define HASHMAP_BASE_FOREACH_کلید_2(e, k, h) \
        HASHMAP_BASE_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

#define HASHMAP_FOREACH_کلید(e, k, h, i) \
        برای (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (تهی**)&(e), (ثابت تهی**) &(k)); )
#define HASHMAP_FOREACH_کلید_2(e, k, h) \
        HASHMAP_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

#define ORDERED_HASHMAP_FOREACH_کلید(e, k, h, i) \
        برای (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (تهی**)&(e), (ثابت تهی**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_کلید_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

تهی hashmap_trim_pools(تهی);
