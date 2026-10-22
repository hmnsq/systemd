/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"
#ضمن "hash-funcs.h"
#ضمن "iterator.h"

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

#define HASH_کلیل_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
تعريفنوع بنية HashmapBase HashmapBase;

/* Specific hashmap/set types */
تعريفنوع بنية Hashmap Hashmap;               /* Maps keys to values */
تعريفنوع بنية OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
تعريفنوع بنية Set Set;                       /* Stores just keys */

تعريفنوع بنية IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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
                (فراغ)0)

#define PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (فراغ)0)

Hashmap* hashmap_new(ثابت بنية hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ثابت بنية hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
ساكن مضمن Hashmap* hashmap_free(Hashmap *h) {
        أرجع (فراغ*) _hashmap_free(HASHMAP_BASE(h));
}
ساكن مضمن OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        أرجع (فراغ*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_کلیلەکان, ثابت فراغ ***res_بەهاکان, unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
ساكن مضمن Hashmap* hashmap_copy(Hashmap *h) {
        أرجع (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
ساكن مضمن OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        أرجع (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops);
صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلیل, فراغ *بەها);
صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops);
صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلیل, فراغ *بەها);

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلیل, فراغ *بەها);
صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلیل, فراغ *بەها);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
ساكن مضمن IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        أرجع _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
ساكن مضمن IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        أرجع _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

صحيح hashmap_put(Hashmap *h, ثابت فراغ *کلیل, فراغ *بەها);
ساكن مضمن صحيح ordered_hashmap_put(OrderedHashmap *h, ثابت فراغ *کلیل, فراغ *بەها) {
        أرجع hashmap_put(PLAIN_HASHMAP(h), کلیل, بەها);
}

صحيح hashmap_put_strdup_full(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v);
ساكن مضمن صحيح hashmap_put_strdup(Hashmap **h, ثابت حرف *k, ثابت حرف *v) {
        أرجع hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *کلیل, فراغ *بەها);
ساكن مضمن صحيح ordered_hashmap_update(OrderedHashmap *h, ثابت فراغ *کلیل, فراغ *بەها) {
        أرجع hashmap_update(PLAIN_HASHMAP(h), کلیل, بەها);
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *کلیل, فراغ *بەها);
ساكن مضمن صحيح ordered_hashmap_replace(OrderedHashmap *h, ثابت فراغ *کلیل, فراغ *بەها) {
        أرجع hashmap_replace(PLAIN_HASHMAP(h), کلیل, بەها);
}

فراغ* _hashmap_get(HashmapBase *h, ثابت فراغ *کلیل);
ساكن مضمن فراغ *hashmap_get(Hashmap *h, ثابت فراغ *کلیل) {
        أرجع _hashmap_get(HASHMAP_BASE(h), کلیل);
}
ساكن مضمن فراغ *ordered_hashmap_get(OrderedHashmap *h, ثابت فراغ *کلیل) {
        أرجع _hashmap_get(HASHMAP_BASE(h), کلیل);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *کلیل, فراغ **ret);
ساكن مضمن فراغ *ordered_hashmap_get2(OrderedHashmap *h, ثابت فراغ *کلیل, فراغ **ret) {
        أرجع hashmap_get2(PLAIN_HASHMAP(h), کلیل, ret);
}

bool _hashmap_contains(HashmapBase *h, ثابت فراغ *کلیل);
ساكن مضمن bool hashmap_contains(Hashmap *h, ثابت فراغ *کلیل) {
        أرجع _hashmap_contains(HASHMAP_BASE(h), کلیل);
}
ساكن مضمن bool ordered_hashmap_contains(OrderedHashmap *h, ثابت فراغ *کلیل) {
        أرجع _hashmap_contains(HASHMAP_BASE(h), کلیل);
}

فراغ* _hashmap_remove(HashmapBase *h, ثابت فراغ *کلیل);
ساكن مضمن فراغ *hashmap_remove(Hashmap *h, ثابت فراغ *کلیل) {
        أرجع _hashmap_remove(HASHMAP_BASE(h), کلیل);
}
ساكن مضمن فراغ *ordered_hashmap_remove(OrderedHashmap *h, ثابت فراغ *کلیل) {
        أرجع _hashmap_remove(HASHMAP_BASE(h), کلیل);
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *کلیل, فراغ **ret);
ساكن مضمن فراغ *ordered_hashmap_remove2(OrderedHashmap *h, ثابت فراغ *کلیل, فراغ **ret) {
        أرجع hashmap_remove2(PLAIN_HASHMAP(h), کلیل, ret);
}

فراغ* hashmap_remove_بەها(HashmapBase *h, ثابت فراغ *کلیل, فراغ *بەها);
ساكن مضمن فراغ *hashmap_remove_بەها_2(Hashmap *h, ثابت فراغ *کلیل, فراغ *بەها) {
        أرجع hashmap_remove_بەها(HASHMAP_BASE(h), کلیل, بەها);
}

ساكن مضمن فراغ* ordered_hashmap_remove_بەها(OrderedHashmap *h, ثابت فراغ *کلیل, فراغ *بەها) {
        أرجع hashmap_remove_بەها_2(PLAIN_HASHMAP(h), کلیل, بەها);
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_کلیل, ثابت فراغ *new_کلیل, فراغ *بەها);
ساكن مضمن صحيح ordered_hashmap_remove_and_put(OrderedHashmap *h, ثابت فراغ *old_کلیل, ثابت فراغ *new_کلیل, فراغ *بەها) {
        أرجع hashmap_remove_and_put(PLAIN_HASHMAP(h), old_کلیل, new_کلیل, بەها);
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_کلیل, ثابت فراغ *new_کلیل, فراغ *بەها);
ساكن مضمن صحيح ordered_hashmap_remove_and_replace(OrderedHashmap *h, ثابت فراغ *old_کلیل, ثابت فراغ *new_کلیل, فراغ *بەها) {
        أرجع hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_کلیل, new_کلیل, بەها);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
صحيح _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

صحيح _hashmap_reserve(HashmapBase *h, unsigned entries_add);
ساكن مضمن صحيح hashmap_reserve(Hashmap *h, unsigned entries_add) {
        أرجع _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
ساكن مضمن صحيح ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        أرجع _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

صحيح _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ساكن مضمن صحيح hashmap_move(Hashmap *h, Hashmap *other) {
        أرجع _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
ساكن مضمن صحيح ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        أرجع _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

صحيح _hashmap_move_one(HashmapBase *h, HashmapBase *other, ثابت فراغ *کلیل);
ساكن مضمن صحيح hashmap_move_one(Hashmap *h, Hashmap *other, ثابت فراغ *کلیل) {
        أرجع _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), کلیل);
}
ساكن مضمن صحيح ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ثابت فراغ *کلیل) {
        أرجع _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), کلیل);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
ساكن مضمن unsigned hashmap_size(Hashmap *h) {
        أرجع _hashmap_size(HASHMAP_BASE(h));
}
ساكن مضمن unsigned ordered_hashmap_size(OrderedHashmap *h) {
        أرجع _hashmap_size(HASHMAP_BASE(h));
}

ساكن مضمن bool hashmap_isempty(Hashmap *h) {
        أرجع hashmap_size(h) == 0;
}
ساكن مضمن bool ordered_hashmap_isempty(OrderedHashmap *h) {
        أرجع ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
ساكن مضمن unsigned hashmap_buckets(Hashmap *h) {
        أرجع _hashmap_buckets(HASHMAP_BASE(h));
}
ساكن مضمن unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        أرجع _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, فراغ **بەها, ثابت فراغ **کلیل);
ساكن مضمن bool hashmap_iterate(Hashmap *h, Iterator *i, فراغ **بەها, ثابت فراغ **کلیل) {
        أرجع _hashmap_iterate(HASHMAP_BASE(h), i, بەها, کلیل);
}
ساكن مضمن bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, فراغ **بەها, ثابت فراغ **کلیل) {
        أرجع _hashmap_iterate(HASHMAP_BASE(h), i, بەها, کلیل);
}

فراغ _hashmap_clear(HashmapBase *h);
ساكن مضمن فراغ hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
ساكن مضمن فراغ ordered_hashmap_clear(OrderedHashmap *h) {
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

فراغ *hashmap_first_کلیل_and_بەها(HashmapBase *h, bool remove, فراغ **ret_کلیل);
ساكن مضمن فراغ *hashmap_steal_first_کلیل_and_بەها(Hashmap *h, فراغ **ret) {
        أرجع hashmap_first_کلیل_and_بەها(HASHMAP_BASE(h), true, ret);
}
ساكن مضمن فراغ *ordered_hashmap_steal_first_کلیل_and_بەها(OrderedHashmap *h, فراغ **ret) {
        أرجع hashmap_first_کلیل_and_بەها(HASHMAP_BASE(h), true, ret);
}
ساكن مضمن فراغ *hashmap_first_کلیل_and_بەها_2(Hashmap *h, فراغ **ret) {
        أرجع hashmap_first_کلیل_and_بەها(HASHMAP_BASE(h), false, ret);
}
ساكن مضمن فراغ *ordered_hashmap_first_کلیل_and_بەها(OrderedHashmap *h, فراغ **ret) {
        أرجع hashmap_first_کلیل_and_بەها(HASHMAP_BASE(h), false, ret);
}

ساكن مضمن فراغ *hashmap_steal_first(Hashmap *h) {
        أرجع hashmap_first_کلیل_and_بەها(HASHMAP_BASE(h), true, عدم);
}
ساكن مضمن فراغ *ordered_hashmap_steal_first(OrderedHashmap *h) {
        أرجع hashmap_first_کلیل_and_بەها(HASHMAP_BASE(h), true, عدم);
}
ساكن مضمن فراغ *hashmap_first(Hashmap *h) {
        أرجع hashmap_first_کلیل_and_بەها(HASHMAP_BASE(h), false, عدم);
}
ساكن مضمن فراغ *ordered_hashmap_first(OrderedHashmap *h) {
        أرجع hashmap_first_کلیل_and_بەها(HASHMAP_BASE(h), false, عدم);
}

ساكن مضمن فراغ *hashmap_first_کلیل(HashmapBase *h, bool remove) {
        فراغ *کلیل = عدم;

        (فراغ) hashmap_first_کلیل_and_بەها(HASHMAP_BASE(h), remove, &کلیل);
        أرجع کلیل;
}
ساكن مضمن فراغ *hashmap_steal_first_کلیل(Hashmap *h) {
        أرجع hashmap_first_کلیل(HASHMAP_BASE(h), true);
}
ساكن مضمن فراغ *ordered_hashmap_steal_first_کلیل(OrderedHashmap *h) {
        أرجع hashmap_first_کلیل(HASHMAP_BASE(h), true);
}
ساكن مضمن فراغ *hashmap_first_کلیل_2(Hashmap *h) {
        أرجع hashmap_first_کلیل(HASHMAP_BASE(h), false);
}
ساكن مضمن فراغ *ordered_hashmap_first_کلیل(OrderedHashmap *h) {
        أرجع hashmap_first_کلیل(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
فراغ* ordered_hashmap_next(OrderedHashmap *h, ثابت فراغ *کلیل);

حرف** _hashmap_get_strv(HashmapBase *h);
ساكن مضمن حرف** hashmap_get_strv(Hashmap *h) {
        أرجع _hashmap_get_strv(HASHMAP_BASE(h));
}
ساكن مضمن حرف** ordered_hashmap_get_strv(OrderedHashmap *h) {
        أرجع _hashmap_get_strv(HASHMAP_BASE(h));
}

صحيح _hashmap_dump_sorted(HashmapBase *h, فراغ ***ret, size_t *ret_n);
ساكن مضمن صحيح hashmap_dump_sorted(Hashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ساكن مضمن صحيح ordered_hashmap_dump_sorted(OrderedHashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ساكن مضمن صحيح set_dump_sorted(Set *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

صحيح hashmap_dump_کلیلەکان_sorted(HashmapBase *h, فراغ ***ret, size_t *ret_n);
ساكن مضمن صحيح hashmap_dump_کلیلەکان_sorted_2(Hashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع hashmap_dump_کلیلەکان_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ساكن مضمن صحيح ordered_hashmap_dump_کلیلەکان_sorted(OrderedHashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع hashmap_dump_کلیلەکان_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define HASHMAP_BASE_FOREACH_کلیل(e, k, h, i) \
        لكل (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#define HASHMAP_BASE_FOREACH_کلیل_2(e, k, h) \
        HASHMAP_BASE_FOREACH_کلیل(e, k, h, UNIQ_T(i, UNIQ))

#define HASHMAP_FOREACH_کلیل(e, k, h, i) \
        لكل (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#define HASHMAP_FOREACH_کلیل_2(e, k, h) \
        HASHMAP_FOREACH_کلیل(e, k, h, UNIQ_T(i, UNIQ))

#define ORDERED_HASHMAP_FOREACH_کلیل(e, k, h, i) \
        لكل (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_کلیل_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_کلیل(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

فراغ hashmap_trim_pools(فراغ);
