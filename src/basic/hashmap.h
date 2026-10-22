/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

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

#وصف_ڪريو HASH_ڪنجي_سائيز 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
تعريفنوع بنية Hashmap_بنياد Hashmap_بنياد;

/* Specific hashmap/set types */
تعريفنوع بنية Hashmap Hashmap;               /* Maps keys to values */
تعريفنوع بنية OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
تعريفنوع بنية سيٽ_2 سيٽ_2;                       /* Stores just keys */

تعريفنوع بنية IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

/* Macros for type checking */
#وصف_ڪريو PTR_COMPATIBLE_WITH_HASHMAP_بنياد(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap_بنياد*) || \
         __builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*) || \
         __builtin_types_compatible_p(typeof(h), سيٽ_2*))

#وصف_ڪريو PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*)) \

#وصف_ڪريو HASHMAP_بنياد(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_HASHMAP_بنياد(h), \
                (Hashmap_بنياد*)(h), \
                (فراغ)0)

#وصف_ڪريو PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (فراغ)0)

Hashmap* hashmap_new(ثابت بنية hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ثابت بنية hash_ops *hash_ops);

#وصف_ڪريو hashmap_free_and_replace(a, b)                          \
        free_and_replace_ڀريل(a, b, hashmap_free)
#وصف_ڪريو ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_ڀريل(a, b, ordered_hashmap_free)

Hashmap_بنياد* _hashmap_free(Hashmap_بنياد *h);
ساكن مضمن Hashmap* hashmap_free(Hashmap *h) {
        أرجع (فراغ*) _hashmap_free(HASHMAP_بنياد(h));
}
ساكن مضمن OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        أرجع (فراغ*) _hashmap_free(HASHMAP_بنياد(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_ڪنجيون, ثابت فراغ ***res_قدرون, بنانشان *res_n_entries);

Hashmap_بنياد* _hashmap_copy(Hashmap_بنياد *h);
ساكن مضمن Hashmap* hashmap_copy(Hashmap *h) {
        أرجع (Hashmap*) _hashmap_copy(HASHMAP_بنياد(h));
}
ساكن مضمن OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        أرجع (OrderedHashmap*) _hashmap_copy(HASHMAP_بنياد(h));
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops);
صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ڪنجي, فراغ *قدر);
صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops);
صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ڪنجي, فراغ *قدر);

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ڪنجي, فراغ *قدر);
صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ڪنجي, فراغ *قدر);

IteratedCache* _hashmap_iterated_cache_new(Hashmap_بنياد *h);
ساكن مضمن IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        أرجع _hashmap_iterated_cache_new(HASHMAP_بنياد(h));
}
ساكن مضمن IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        أرجع _hashmap_iterated_cache_new(HASHMAP_بنياد(h));
}

صحيح hashmap_put(Hashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر);
ساكن مضمن صحيح ordered_hashmap_put(OrderedHashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر) {
        أرجع hashmap_put(PLAIN_HASHMAP(h), ڪنجي, قدر);
}

صحيح hashmap_put_strdup_ڀريل(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v);
ساكن مضمن صحيح hashmap_put_strdup(Hashmap **h, ثابت حرف *k, ثابت حرف *v) {
        أرجع hashmap_put_strdup_ڀريل(h, &string_hash_ops_free_free, k, v);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر);
ساكن مضمن صحيح ordered_hashmap_update(OrderedHashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر) {
        أرجع hashmap_update(PLAIN_HASHMAP(h), ڪنجي, قدر);
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر);
ساكن مضمن صحيح ordered_hashmap_replace(OrderedHashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر) {
        أرجع hashmap_replace(PLAIN_HASHMAP(h), ڪنجي, قدر);
}

فراغ* _hashmap_get(Hashmap_بنياد *h, ثابت فراغ *ڪنجي);
ساكن مضمن فراغ *hashmap_get(Hashmap *h, ثابت فراغ *ڪنجي) {
        أرجع _hashmap_get(HASHMAP_بنياد(h), ڪنجي);
}
ساكن مضمن فراغ *ordered_hashmap_get(OrderedHashmap *h, ثابت فراغ *ڪنجي) {
        أرجع _hashmap_get(HASHMAP_بنياد(h), ڪنجي);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *ڪنجي, فراغ **ret);
ساكن مضمن فراغ *ordered_hashmap_get2(OrderedHashmap *h, ثابت فراغ *ڪنجي, فراغ **ret) {
        أرجع hashmap_get2(PLAIN_HASHMAP(h), ڪنجي, ret);
}

bool hashmap_شامل_آهي_2(Hashmap_بنياد *h, ثابت فراغ *ڪنجي);
ساكن مضمن bool hashmap_شامل_آهي(Hashmap *h, ثابت فراغ *ڪنجي) {
        أرجع hashmap_شامل_آهي_2(HASHMAP_بنياد(h), ڪنجي);
}
ساكن مضمن bool ordered_hashmap_شامل_آهي(OrderedHashmap *h, ثابت فراغ *ڪنجي) {
        أرجع hashmap_شامل_آهي_2(HASHMAP_بنياد(h), ڪنجي);
}

فراغ* _hashmap_remove(Hashmap_بنياد *h, ثابت فراغ *ڪنجي);
ساكن مضمن فراغ *hashmap_remove(Hashmap *h, ثابت فراغ *ڪنجي) {
        أرجع _hashmap_remove(HASHMAP_بنياد(h), ڪنجي);
}
ساكن مضمن فراغ *ordered_hashmap_remove(OrderedHashmap *h, ثابت فراغ *ڪنجي) {
        أرجع _hashmap_remove(HASHMAP_بنياد(h), ڪنجي);
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *ڪنجي, فراغ **ret);
ساكن مضمن فراغ *ordered_hashmap_remove2(OrderedHashmap *h, ثابت فراغ *ڪنجي, فراغ **ret) {
        أرجع hashmap_remove2(PLAIN_HASHMAP(h), ڪنجي, ret);
}

فراغ* hashmap_remove_قدر(Hashmap_بنياد *h, ثابت فراغ *ڪنجي, فراغ *قدر);
ساكن مضمن فراغ *hashmap_remove_قدر_2(Hashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر) {
        أرجع hashmap_remove_قدر(HASHMAP_بنياد(h), ڪنجي, قدر);
}

ساكن مضمن فراغ* ordered_hashmap_remove_قدر(OrderedHashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر) {
        أرجع hashmap_remove_قدر_2(PLAIN_HASHMAP(h), ڪنجي, قدر);
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_ڪنجي, ثابت فراغ *new_ڪنجي, فراغ *قدر);
ساكن مضمن صحيح ordered_hashmap_remove_and_put(OrderedHashmap *h, ثابت فراغ *old_ڪنجي, ثابت فراغ *new_ڪنجي, فراغ *قدر) {
        أرجع hashmap_remove_and_put(PLAIN_HASHMAP(h), old_ڪنجي, new_ڪنجي, قدر);
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_ڪنجي, ثابت فراغ *new_ڪنجي, فراغ *قدر);
ساكن مضمن صحيح ordered_hashmap_remove_and_replace(OrderedHashmap *h, ثابت فراغ *old_ڪنجي, ثابت فراغ *new_ڪنجي, فراغ *قدر) {
        أرجع hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_ڪنجي, new_ڪنجي, قدر);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
صحيح hashmap_ملايو(Hashmap *h, Hashmap *other);
#وصف_ڪريو hashmap_ملايو_2(h, other) hashmap_ملايو(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#وصف_ڪريو ordered_hashmap_ملايو(h, other) hashmap_ملايو_2(h, other)

صحيح _hashmap_reserve(Hashmap_بنياد *h, بنانشان entries_جوڙيو);
ساكن مضمن صحيح hashmap_reserve(Hashmap *h, بنانشان entries_جوڙيو) {
        أرجع _hashmap_reserve(HASHMAP_بنياد(h), entries_جوڙيو);
}
ساكن مضمن صحيح ordered_hashmap_reserve(OrderedHashmap *h, بنانشان entries_جوڙيو) {
        أرجع _hashmap_reserve(HASHMAP_بنياد(h), entries_جوڙيو);
}

صحيح _hashmap_move(Hashmap_بنياد *h, Hashmap_بنياد *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ساكن مضمن صحيح hashmap_move(Hashmap *h, Hashmap *other) {
        أرجع _hashmap_move(HASHMAP_بنياد(h), HASHMAP_بنياد(other));
}
ساكن مضمن صحيح ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        أرجع _hashmap_move(HASHMAP_بنياد(h), HASHMAP_بنياد(other));
}

صحيح _hashmap_move_one(Hashmap_بنياد *h, Hashmap_بنياد *other, ثابت فراغ *ڪنجي);
ساكن مضمن صحيح hashmap_move_one(Hashmap *h, Hashmap *other, ثابت فراغ *ڪنجي) {
        أرجع _hashmap_move_one(HASHMAP_بنياد(h), HASHMAP_بنياد(other), ڪنجي);
}
ساكن مضمن صحيح ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ثابت فراغ *ڪنجي) {
        أرجع _hashmap_move_one(HASHMAP_بنياد(h), HASHMAP_بنياد(other), ڪنجي);
}

بنانشان hashmap_سائيز(Hashmap_بنياد *h) خالص;
ساكن مضمن بنانشان hashmap_سائيز_2(Hashmap *h) {
        أرجع hashmap_سائيز(HASHMAP_بنياد(h));
}
ساكن مضمن بنانشان ordered_hashmap_سائيز(OrderedHashmap *h) {
        أرجع hashmap_سائيز(HASHMAP_بنياد(h));
}

ساكن مضمن bool hashmap_isempty(Hashmap *h) {
        أرجع hashmap_سائيز_2(h) == 0;
}
ساكن مضمن bool ordered_hashmap_isempty(OrderedHashmap *h) {
        أرجع ordered_hashmap_سائيز(h) == 0;
}

بنانشان _hashmap_buckets(Hashmap_بنياد *h) خالص;
ساكن مضمن بنانشان hashmap_buckets(Hashmap *h) {
        أرجع _hashmap_buckets(HASHMAP_بنياد(h));
}
ساكن مضمن بنانشان ordered_hashmap_buckets(OrderedHashmap *h) {
        أرجع _hashmap_buckets(HASHMAP_بنياد(h));
}

bool _hashmap_iterate(Hashmap_بنياد *h, Iterator *i, فراغ **قدر, ثابت فراغ **ڪنجي);
ساكن مضمن bool hashmap_iterate(Hashmap *h, Iterator *i, فراغ **قدر, ثابت فراغ **ڪنجي) {
        أرجع _hashmap_iterate(HASHMAP_بنياد(h), i, قدر, ڪنجي);
}
ساكن مضمن bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, فراغ **قدر, ثابت فراغ **ڪنجي) {
        أرجع _hashmap_iterate(HASHMAP_بنياد(h), i, قدر, ڪنجي);
}

فراغ _hashmap_clear(Hashmap_بنياد *h);
ساكن مضمن فراغ hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_بنياد(h));
}
ساكن مضمن فراغ ordered_hashmap_clear(OrderedHashmap *h) {
        _hashmap_clear(HASHMAP_بنياد(h));
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

فراغ *hashmap_پهريون_ڪنجي_and_قدر(Hashmap_بنياد *h, bool remove, فراغ **ret_ڪنجي);
ساكن مضمن فراغ *hashmap_steal_پهريون_ڪنجي_and_قدر(Hashmap *h, فراغ **ret) {
        أرجع hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد(h), true, ret);
}
ساكن مضمن فراغ *ordered_hashmap_steal_پهريون_ڪنجي_and_قدر(OrderedHashmap *h, فراغ **ret) {
        أرجع hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد(h), true, ret);
}
ساكن مضمن فراغ *hashmap_پهريون_ڪنجي_and_قدر_2(Hashmap *h, فراغ **ret) {
        أرجع hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد(h), false, ret);
}
ساكن مضمن فراغ *ordered_hashmap_پهريون_ڪنجي_and_قدر(OrderedHashmap *h, فراغ **ret) {
        أرجع hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد(h), false, ret);
}

ساكن مضمن فراغ *hashmap_steal_پهريون(Hashmap *h) {
        أرجع hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد(h), true, عدم);
}
ساكن مضمن فراغ *ordered_hashmap_steal_پهريون(OrderedHashmap *h) {
        أرجع hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد(h), true, عدم);
}
ساكن مضمن فراغ *hashmap_پهريون(Hashmap *h) {
        أرجع hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد(h), false, عدم);
}
ساكن مضمن فراغ *ordered_hashmap_پهريون(OrderedHashmap *h) {
        أرجع hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد(h), false, عدم);
}

ساكن مضمن فراغ *hashmap_پهريون_ڪنجي(Hashmap_بنياد *h, bool remove) {
        فراغ *ڪنجي = عدم;

        (فراغ) hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد(h), remove, &ڪنجي);
        أرجع ڪنجي;
}
ساكن مضمن فراغ *hashmap_steal_پهريون_ڪنجي(Hashmap *h) {
        أرجع hashmap_پهريون_ڪنجي(HASHMAP_بنياد(h), true);
}
ساكن مضمن فراغ *ordered_hashmap_steal_پهريون_ڪنجي(OrderedHashmap *h) {
        أرجع hashmap_پهريون_ڪنجي(HASHMAP_بنياد(h), true);
}
ساكن مضمن فراغ *hashmap_پهريون_ڪنجي_2(Hashmap *h) {
        أرجع hashmap_پهريون_ڪنجي(HASHMAP_بنياد(h), false);
}
ساكن مضمن فراغ *ordered_hashmap_پهريون_ڪنجي(OrderedHashmap *h) {
        أرجع hashmap_پهريون_ڪنجي(HASHMAP_بنياد(h), false);
}

/* no hashmap_next */
فراغ* ordered_hashmap_ايندڙ(OrderedHashmap *h, ثابت فراغ *ڪنجي);

حرف** _hashmap_get_strv(Hashmap_بنياد *h);
ساكن مضمن حرف** hashmap_get_strv(Hashmap *h) {
        أرجع _hashmap_get_strv(HASHMAP_بنياد(h));
}
ساكن مضمن حرف** ordered_hashmap_get_strv(OrderedHashmap *h) {
        أرجع _hashmap_get_strv(HASHMAP_بنياد(h));
}

صحيح _hashmap_dump_sorted(Hashmap_بنياد *h, فراغ ***ret, size_t *ret_n);
ساكن مضمن صحيح hashmap_dump_sorted(Hashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_بنياد(h), ret, ret_n);
}
ساكن مضمن صحيح ordered_hashmap_dump_sorted(OrderedHashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_بنياد(h), ret, ret_n);
}
ساكن مضمن صحيح سيٽ_dump_sorted(سيٽ_2 *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_بنياد(h), ret, ret_n);
}

صحيح hashmap_dump_ڪنجيون_sorted(Hashmap_بنياد *h, فراغ ***ret, size_t *ret_n);
ساكن مضمن صحيح hashmap_dump_ڪنجيون_sorted_2(Hashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع hashmap_dump_ڪنجيون_sorted(HASHMAP_بنياد(h), ret, ret_n);
}
ساكن مضمن صحيح ordered_hashmap_dump_ڪنجيون_sorted(OrderedHashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع hashmap_dump_ڪنجيون_sorted(HASHMAP_بنياد(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#وصف_ڪريو HASHMAP_بنياد_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_پهريون; _hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#وصف_ڪريو HASHMAP_بنياد_FOREACH_2(e, h) \
        HASHMAP_بنياد_FOREACH(e, h, UNIQ_T(i, UNIQ))

#وصف_ڪريو _HASHMAP_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_پهريون; hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#وصف_ڪريو HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#وصف_ڪريو _ORDERED_HASHMAP_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_پهريون; ordered_hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#وصف_ڪريو ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#وصف_ڪريو HASHMAP_بنياد_FOREACH_ڪنجي(e, k, h, i) \
        لكل (Iterator i = ITERATOR_پهريون; _hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#وصف_ڪريو HASHMAP_بنياد_FOREACH_ڪنجي_2(e, k, h) \
        HASHMAP_بنياد_FOREACH_ڪنجي(e, k, h, UNIQ_T(i, UNIQ))

#وصف_ڪريو HASHMAP_FOREACH_ڪنجي(e, k, h, i) \
        لكل (Iterator i = ITERATOR_پهريون; hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#وصف_ڪريو HASHMAP_FOREACH_ڪنجي_2(e, k, h) \
        HASHMAP_FOREACH_ڪنجي(e, k, h, UNIQ_T(i, UNIQ))

#وصف_ڪريو ORDERED_HASHMAP_FOREACH_ڪنجي(e, k, h, i) \
        لكل (Iterator i = ITERATOR_پهريون; ordered_hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#وصف_ڪريو ORDERED_HASHMAP_FOREACH_ڪنجي_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_ڪنجي(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#وصف_ڪريو _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#وصف_ڪريو _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#وصف_ڪريو _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

فراغ hashmap_trim_pools(فراغ);
