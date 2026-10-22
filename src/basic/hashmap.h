/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

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

#تعریف_کړه HASH_کلی_اندازه 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
تعريفنوع بنية Hashmap_بنسټ Hashmap_بنسټ;

/* Specific hashmap/set types */
تعريفنوع بنية Hashmap Hashmap;               /* Maps keys to values */
تعريفنوع بنية OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
تعريفنوع بنية سېټ_2 سېټ_2;                       /* Stores just keys */

تعريفنوع بنية IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

/* Macros for type checking */
#تعریف_کړه PTR_COMPATIBLE_WITH_HASHMAP_بنسټ(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap_بنسټ*) || \
         __builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*) || \
         __builtin_types_compatible_p(typeof(h), سېټ_2*))

#تعریف_کړه PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*)) \

#تعریف_کړه HASHMAP_بنسټ(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_HASHMAP_بنسټ(h), \
                (Hashmap_بنسټ*)(h), \
                (فراغ)0)

#تعریف_کړه PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (فراغ)0)

Hashmap* hashmap_new(ثابت بنية hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ثابت بنية hash_ops *hash_ops);

#تعریف_کړه hashmap_free_and_replace(a, b)                          \
        free_and_replace_ډک(a, b, hashmap_free)
#تعریف_کړه ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_ډک(a, b, ordered_hashmap_free)

Hashmap_بنسټ* _hashmap_free(Hashmap_بنسټ *h);
ساكن مضمن Hashmap* hashmap_free(Hashmap *h) {
        أرجع (فراغ*) _hashmap_free(HASHMAP_بنسټ(h));
}
ساكن مضمن OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        أرجع (فراغ*) _hashmap_free(HASHMAP_بنسټ(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_کلیګانې, ثابت فراغ ***res_ارزښتونه, بینښه *res_n_entries);

Hashmap_بنسټ* _hashmap_copy(Hashmap_بنسټ *h);
ساكن مضمن Hashmap* hashmap_copy(Hashmap *h) {
        أرجع (Hashmap*) _hashmap_copy(HASHMAP_بنسټ(h));
}
ساكن مضمن OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        أرجع (OrderedHashmap*) _hashmap_copy(HASHMAP_بنسټ(h));
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops);
صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلی, فراغ *ارزښت);
صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops);
صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلی, فراغ *ارزښت);

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلی, فراغ *ارزښت);
صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلی, فراغ *ارزښت);

IteratedCache* _hashmap_iterated_cache_new(Hashmap_بنسټ *h);
ساكن مضمن IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        أرجع _hashmap_iterated_cache_new(HASHMAP_بنسټ(h));
}
ساكن مضمن IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        أرجع _hashmap_iterated_cache_new(HASHMAP_بنسټ(h));
}

صحيح hashmap_put(Hashmap *h, ثابت فراغ *کلی, فراغ *ارزښت);
ساكن مضمن صحيح ordered_hashmap_put(OrderedHashmap *h, ثابت فراغ *کلی, فراغ *ارزښت) {
        أرجع hashmap_put(PLAIN_HASHMAP(h), کلی, ارزښت);
}

صحيح hashmap_put_strdup_ډک(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v);
ساكن مضمن صحيح hashmap_put_strdup(Hashmap **h, ثابت حرف *k, ثابت حرف *v) {
        أرجع hashmap_put_strdup_ډک(h, &string_hash_ops_free_free, k, v);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *کلی, فراغ *ارزښت);
ساكن مضمن صحيح ordered_hashmap_update(OrderedHashmap *h, ثابت فراغ *کلی, فراغ *ارزښت) {
        أرجع hashmap_update(PLAIN_HASHMAP(h), کلی, ارزښت);
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *کلی, فراغ *ارزښت);
ساكن مضمن صحيح ordered_hashmap_replace(OrderedHashmap *h, ثابت فراغ *کلی, فراغ *ارزښت) {
        أرجع hashmap_replace(PLAIN_HASHMAP(h), کلی, ارزښت);
}

فراغ* _hashmap_get(Hashmap_بنسټ *h, ثابت فراغ *کلی);
ساكن مضمن فراغ *hashmap_get(Hashmap *h, ثابت فراغ *کلی) {
        أرجع _hashmap_get(HASHMAP_بنسټ(h), کلی);
}
ساكن مضمن فراغ *ordered_hashmap_get(OrderedHashmap *h, ثابت فراغ *کلی) {
        أرجع _hashmap_get(HASHMAP_بنسټ(h), کلی);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *کلی, فراغ **ret);
ساكن مضمن فراغ *ordered_hashmap_get2(OrderedHashmap *h, ثابت فراغ *کلی, فراغ **ret) {
        أرجع hashmap_get2(PLAIN_HASHMAP(h), کلی, ret);
}

bool hashmap_لري_2(Hashmap_بنسټ *h, ثابت فراغ *کلی);
ساكن مضمن bool hashmap_لري(Hashmap *h, ثابت فراغ *کلی) {
        أرجع hashmap_لري_2(HASHMAP_بنسټ(h), کلی);
}
ساكن مضمن bool ordered_hashmap_لري(OrderedHashmap *h, ثابت فراغ *کلی) {
        أرجع hashmap_لري_2(HASHMAP_بنسټ(h), کلی);
}

فراغ* _hashmap_remove(Hashmap_بنسټ *h, ثابت فراغ *کلی);
ساكن مضمن فراغ *hashmap_remove(Hashmap *h, ثابت فراغ *کلی) {
        أرجع _hashmap_remove(HASHMAP_بنسټ(h), کلی);
}
ساكن مضمن فراغ *ordered_hashmap_remove(OrderedHashmap *h, ثابت فراغ *کلی) {
        أرجع _hashmap_remove(HASHMAP_بنسټ(h), کلی);
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *کلی, فراغ **ret);
ساكن مضمن فراغ *ordered_hashmap_remove2(OrderedHashmap *h, ثابت فراغ *کلی, فراغ **ret) {
        أرجع hashmap_remove2(PLAIN_HASHMAP(h), کلی, ret);
}

فراغ* hashmap_remove_ارزښت(Hashmap_بنسټ *h, ثابت فراغ *کلی, فراغ *ارزښت);
ساكن مضمن فراغ *hashmap_remove_ارزښت_2(Hashmap *h, ثابت فراغ *کلی, فراغ *ارزښت) {
        أرجع hashmap_remove_ارزښت(HASHMAP_بنسټ(h), کلی, ارزښت);
}

ساكن مضمن فراغ* ordered_hashmap_remove_ارزښت(OrderedHashmap *h, ثابت فراغ *کلی, فراغ *ارزښت) {
        أرجع hashmap_remove_ارزښت_2(PLAIN_HASHMAP(h), کلی, ارزښت);
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_کلی, ثابت فراغ *new_کلی, فراغ *ارزښت);
ساكن مضمن صحيح ordered_hashmap_remove_and_put(OrderedHashmap *h, ثابت فراغ *old_کلی, ثابت فراغ *new_کلی, فراغ *ارزښت) {
        أرجع hashmap_remove_and_put(PLAIN_HASHMAP(h), old_کلی, new_کلی, ارزښت);
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_کلی, ثابت فراغ *new_کلی, فراغ *ارزښت);
ساكن مضمن صحيح ordered_hashmap_remove_and_replace(OrderedHashmap *h, ثابت فراغ *old_کلی, ثابت فراغ *new_کلی, فراغ *ارزښت) {
        أرجع hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_کلی, new_کلی, ارزښت);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
صحيح hashmap_یوځای(Hashmap *h, Hashmap *other);
#تعریف_کړه hashmap_یوځای_2(h, other) hashmap_یوځای(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#تعریف_کړه ordered_hashmap_یوځای(h, other) hashmap_یوځای_2(h, other)

صحيح _hashmap_reserve(Hashmap_بنسټ *h, بینښه entries_جمع_کړه);
ساكن مضمن صحيح hashmap_reserve(Hashmap *h, بینښه entries_جمع_کړه) {
        أرجع _hashmap_reserve(HASHMAP_بنسټ(h), entries_جمع_کړه);
}
ساكن مضمن صحيح ordered_hashmap_reserve(OrderedHashmap *h, بینښه entries_جمع_کړه) {
        أرجع _hashmap_reserve(HASHMAP_بنسټ(h), entries_جمع_کړه);
}

صحيح _hashmap_move(Hashmap_بنسټ *h, Hashmap_بنسټ *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ساكن مضمن صحيح hashmap_move(Hashmap *h, Hashmap *other) {
        أرجع _hashmap_move(HASHMAP_بنسټ(h), HASHMAP_بنسټ(other));
}
ساكن مضمن صحيح ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        أرجع _hashmap_move(HASHMAP_بنسټ(h), HASHMAP_بنسټ(other));
}

صحيح _hashmap_move_one(Hashmap_بنسټ *h, Hashmap_بنسټ *other, ثابت فراغ *کلی);
ساكن مضمن صحيح hashmap_move_one(Hashmap *h, Hashmap *other, ثابت فراغ *کلی) {
        أرجع _hashmap_move_one(HASHMAP_بنسټ(h), HASHMAP_بنسټ(other), کلی);
}
ساكن مضمن صحيح ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ثابت فراغ *کلی) {
        أرجع _hashmap_move_one(HASHMAP_بنسټ(h), HASHMAP_بنسټ(other), کلی);
}

بینښه hashmap_اندازه(Hashmap_بنسټ *h) خالص;
ساكن مضمن بینښه hashmap_اندازه_2(Hashmap *h) {
        أرجع hashmap_اندازه(HASHMAP_بنسټ(h));
}
ساكن مضمن بینښه ordered_hashmap_اندازه(OrderedHashmap *h) {
        أرجع hashmap_اندازه(HASHMAP_بنسټ(h));
}

ساكن مضمن bool hashmap_isempty(Hashmap *h) {
        أرجع hashmap_اندازه_2(h) == 0;
}
ساكن مضمن bool ordered_hashmap_isempty(OrderedHashmap *h) {
        أرجع ordered_hashmap_اندازه(h) == 0;
}

بینښه _hashmap_buckets(Hashmap_بنسټ *h) خالص;
ساكن مضمن بینښه hashmap_buckets(Hashmap *h) {
        أرجع _hashmap_buckets(HASHMAP_بنسټ(h));
}
ساكن مضمن بینښه ordered_hashmap_buckets(OrderedHashmap *h) {
        أرجع _hashmap_buckets(HASHMAP_بنسټ(h));
}

bool _hashmap_iterate(Hashmap_بنسټ *h, Iterator *i, فراغ **ارزښت, ثابت فراغ **کلی);
ساكن مضمن bool hashmap_iterate(Hashmap *h, Iterator *i, فراغ **ارزښت, ثابت فراغ **کلی) {
        أرجع _hashmap_iterate(HASHMAP_بنسټ(h), i, ارزښت, کلی);
}
ساكن مضمن bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, فراغ **ارزښت, ثابت فراغ **کلی) {
        أرجع _hashmap_iterate(HASHMAP_بنسټ(h), i, ارزښت, کلی);
}

فراغ _hashmap_clear(Hashmap_بنسټ *h);
ساكن مضمن فراغ hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_بنسټ(h));
}
ساكن مضمن فراغ ordered_hashmap_clear(OrderedHashmap *h) {
        _hashmap_clear(HASHMAP_بنسټ(h));
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

فراغ *hashmap_لومړی_کلی_and_ارزښت(Hashmap_بنسټ *h, bool remove, فراغ **ret_کلی);
ساكن مضمن فراغ *hashmap_steal_لومړی_کلی_and_ارزښت(Hashmap *h, فراغ **ret) {
        أرجع hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ(h), true, ret);
}
ساكن مضمن فراغ *ordered_hashmap_steal_لومړی_کلی_and_ارزښت(OrderedHashmap *h, فراغ **ret) {
        أرجع hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ(h), true, ret);
}
ساكن مضمن فراغ *hashmap_لومړی_کلی_and_ارزښت_2(Hashmap *h, فراغ **ret) {
        أرجع hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ(h), false, ret);
}
ساكن مضمن فراغ *ordered_hashmap_لومړی_کلی_and_ارزښت(OrderedHashmap *h, فراغ **ret) {
        أرجع hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ(h), false, ret);
}

ساكن مضمن فراغ *hashmap_steal_لومړی(Hashmap *h) {
        أرجع hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ(h), true, عدم);
}
ساكن مضمن فراغ *ordered_hashmap_steal_لومړی(OrderedHashmap *h) {
        أرجع hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ(h), true, عدم);
}
ساكن مضمن فراغ *hashmap_لومړی(Hashmap *h) {
        أرجع hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ(h), false, عدم);
}
ساكن مضمن فراغ *ordered_hashmap_لومړی(OrderedHashmap *h) {
        أرجع hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ(h), false, عدم);
}

ساكن مضمن فراغ *hashmap_لومړی_کلی(Hashmap_بنسټ *h, bool remove) {
        فراغ *کلی = عدم;

        (فراغ) hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ(h), remove, &کلی);
        أرجع کلی;
}
ساكن مضمن فراغ *hashmap_steal_لومړی_کلی(Hashmap *h) {
        أرجع hashmap_لومړی_کلی(HASHMAP_بنسټ(h), true);
}
ساكن مضمن فراغ *ordered_hashmap_steal_لومړی_کلی(OrderedHashmap *h) {
        أرجع hashmap_لومړی_کلی(HASHMAP_بنسټ(h), true);
}
ساكن مضمن فراغ *hashmap_لومړی_کلی_2(Hashmap *h) {
        أرجع hashmap_لومړی_کلی(HASHMAP_بنسټ(h), false);
}
ساكن مضمن فراغ *ordered_hashmap_لومړی_کلی(OrderedHashmap *h) {
        أرجع hashmap_لومړی_کلی(HASHMAP_بنسټ(h), false);
}

/* no hashmap_next */
فراغ* ordered_hashmap_بل(OrderedHashmap *h, ثابت فراغ *کلی);

حرف** _hashmap_get_strv(Hashmap_بنسټ *h);
ساكن مضمن حرف** hashmap_get_strv(Hashmap *h) {
        أرجع _hashmap_get_strv(HASHMAP_بنسټ(h));
}
ساكن مضمن حرف** ordered_hashmap_get_strv(OrderedHashmap *h) {
        أرجع _hashmap_get_strv(HASHMAP_بنسټ(h));
}

صحيح _hashmap_dump_sorted(Hashmap_بنسټ *h, فراغ ***ret, size_t *ret_n);
ساكن مضمن صحيح hashmap_dump_sorted(Hashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_بنسټ(h), ret, ret_n);
}
ساكن مضمن صحيح ordered_hashmap_dump_sorted(OrderedHashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_بنسټ(h), ret, ret_n);
}
ساكن مضمن صحيح سېټ_dump_sorted(سېټ_2 *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_بنسټ(h), ret, ret_n);
}

صحيح hashmap_dump_کلیګانې_sorted(Hashmap_بنسټ *h, فراغ ***ret, size_t *ret_n);
ساكن مضمن صحيح hashmap_dump_کلیګانې_sorted_2(Hashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع hashmap_dump_کلیګانې_sorted(HASHMAP_بنسټ(h), ret, ret_n);
}
ساكن مضمن صحيح ordered_hashmap_dump_کلیګانې_sorted(OrderedHashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع hashmap_dump_کلیګانې_sorted(HASHMAP_بنسټ(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#تعریف_کړه HASHMAP_بنسټ_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_لومړی; _hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#تعریف_کړه HASHMAP_بنسټ_FOREACH_2(e, h) \
        HASHMAP_بنسټ_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کړه _HASHMAP_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_لومړی; hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#تعریف_کړه HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کړه _ORDERED_HASHMAP_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_لومړی; ordered_hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#تعریف_کړه ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کړه HASHMAP_بنسټ_FOREACH_کلی(e, k, h, i) \
        لكل (Iterator i = ITERATOR_لومړی; _hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#تعریف_کړه HASHMAP_بنسټ_FOREACH_کلی_2(e, k, h) \
        HASHMAP_بنسټ_FOREACH_کلی(e, k, h, UNIQ_T(i, UNIQ))

#تعریف_کړه HASHMAP_FOREACH_کلی(e, k, h, i) \
        لكل (Iterator i = ITERATOR_لومړی; hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#تعریف_کړه HASHMAP_FOREACH_کلی_2(e, k, h) \
        HASHMAP_FOREACH_کلی(e, k, h, UNIQ_T(i, UNIQ))

#تعریف_کړه ORDERED_HASHMAP_FOREACH_کلی(e, k, h, i) \
        لكل (Iterator i = ITERATOR_لومړی; ordered_hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#تعریف_کړه ORDERED_HASHMAP_FOREACH_کلی_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_کلی(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#تعریف_کړه _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#تعریف_کړه _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#تعریف_کړه _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

فراغ hashmap_trim_pools(فراغ);
