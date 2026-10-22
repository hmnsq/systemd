/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

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

#تعریف_کرو HASH_کلید_ناپ 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
تعريفنوع بنية Hashmap_ادھار Hashmap_ادھار;

/* Specific hashmap/set types */
تعريفنوع بنية Hashmap Hashmap;               /* Maps keys to values */
تعريفنوع بنية OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
تعريفنوع بنية سیٹ_2 سیٹ_2;                       /* Stores just keys */

تعريفنوع بنية IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

/* Macros for type checking */
#تعریف_کرو PTR_COMPATIBLE_WITH_HASHMAP_ادھار(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap_ادھار*) || \
         __builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*) || \
         __builtin_types_compatible_p(typeof(h), سیٹ_2*))

#تعریف_کرو PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*)) \

#تعریف_کرو HASHMAP_ادھار(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_HASHMAP_ادھار(h), \
                (Hashmap_ادھار*)(h), \
                (فراغ)0)

#تعریف_کرو PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (فراغ)0)

Hashmap* hashmap_new(ثابت بنية hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ثابت بنية hash_ops *hash_ops);

#تعریف_کرو hashmap_free_and_replace(a, b)                          \
        free_and_replace_بھریا(a, b, hashmap_free)
#تعریف_کرو ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_بھریا(a, b, ordered_hashmap_free)

Hashmap_ادھار* _hashmap_free(Hashmap_ادھار *h);
ساكن مضمن Hashmap* hashmap_free(Hashmap *h) {
        أرجع (فراغ*) _hashmap_free(HASHMAP_ادھار(h));
}
ساكن مضمن OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        أرجع (فراغ*) _hashmap_free(HASHMAP_ادھار(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_کلیدیں, ثابت فراغ ***res_اقدار, بنانشانی *res_n_entries);

Hashmap_ادھار* _hashmap_copy(Hashmap_ادھار *h);
ساكن مضمن Hashmap* hashmap_copy(Hashmap *h) {
        أرجع (Hashmap*) _hashmap_copy(HASHMAP_ادھار(h));
}
ساكن مضمن OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        أرجع (OrderedHashmap*) _hashmap_copy(HASHMAP_ادھار(h));
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops);
صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *قدر);
صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops);
صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *قدر);

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *قدر);
صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *قدر);

IteratedCache* _hashmap_iterated_cache_new(Hashmap_ادھار *h);
ساكن مضمن IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        أرجع _hashmap_iterated_cache_new(HASHMAP_ادھار(h));
}
ساكن مضمن IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        أرجع _hashmap_iterated_cache_new(HASHMAP_ادھار(h));
}

صحيح hashmap_put(Hashmap *h, ثابت فراغ *کلید, فراغ *قدر);
ساكن مضمن صحيح ordered_hashmap_put(OrderedHashmap *h, ثابت فراغ *کلید, فراغ *قدر) {
        أرجع hashmap_put(PLAIN_HASHMAP(h), کلید, قدر);
}

صحيح hashmap_put_strdup_بھریا(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v);
ساكن مضمن صحيح hashmap_put_strdup(Hashmap **h, ثابت حرف *k, ثابت حرف *v) {
        أرجع hashmap_put_strdup_بھریا(h, &string_hash_ops_free_free, k, v);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *کلید, فراغ *قدر);
ساكن مضمن صحيح ordered_hashmap_update(OrderedHashmap *h, ثابت فراغ *کلید, فراغ *قدر) {
        أرجع hashmap_update(PLAIN_HASHMAP(h), کلید, قدر);
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *کلید, فراغ *قدر);
ساكن مضمن صحيح ordered_hashmap_replace(OrderedHashmap *h, ثابت فراغ *کلید, فراغ *قدر) {
        أرجع hashmap_replace(PLAIN_HASHMAP(h), کلید, قدر);
}

فراغ* _hashmap_get(Hashmap_ادھار *h, ثابت فراغ *کلید);
ساكن مضمن فراغ *hashmap_get(Hashmap *h, ثابت فراغ *کلید) {
        أرجع _hashmap_get(HASHMAP_ادھار(h), کلید);
}
ساكن مضمن فراغ *ordered_hashmap_get(OrderedHashmap *h, ثابت فراغ *کلید) {
        أرجع _hashmap_get(HASHMAP_ادھار(h), کلید);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *کلید, فراغ **ret);
ساكن مضمن فراغ *ordered_hashmap_get2(OrderedHashmap *h, ثابت فراغ *کلید, فراغ **ret) {
        أرجع hashmap_get2(PLAIN_HASHMAP(h), کلید, ret);
}

bool hashmap_شامل_اے_2(Hashmap_ادھار *h, ثابت فراغ *کلید);
ساكن مضمن bool hashmap_شامل_اے(Hashmap *h, ثابت فراغ *کلید) {
        أرجع hashmap_شامل_اے_2(HASHMAP_ادھار(h), کلید);
}
ساكن مضمن bool ordered_hashmap_شامل_اے(OrderedHashmap *h, ثابت فراغ *کلید) {
        أرجع hashmap_شامل_اے_2(HASHMAP_ادھار(h), کلید);
}

فراغ* _hashmap_remove(Hashmap_ادھار *h, ثابت فراغ *کلید);
ساكن مضمن فراغ *hashmap_remove(Hashmap *h, ثابت فراغ *کلید) {
        أرجع _hashmap_remove(HASHMAP_ادھار(h), کلید);
}
ساكن مضمن فراغ *ordered_hashmap_remove(OrderedHashmap *h, ثابت فراغ *کلید) {
        أرجع _hashmap_remove(HASHMAP_ادھار(h), کلید);
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *کلید, فراغ **ret);
ساكن مضمن فراغ *ordered_hashmap_remove2(OrderedHashmap *h, ثابت فراغ *کلید, فراغ **ret) {
        أرجع hashmap_remove2(PLAIN_HASHMAP(h), کلید, ret);
}

فراغ* hashmap_remove_قدر(Hashmap_ادھار *h, ثابت فراغ *کلید, فراغ *قدر);
ساكن مضمن فراغ *hashmap_remove_قدر_2(Hashmap *h, ثابت فراغ *کلید, فراغ *قدر) {
        أرجع hashmap_remove_قدر(HASHMAP_ادھار(h), کلید, قدر);
}

ساكن مضمن فراغ* ordered_hashmap_remove_قدر(OrderedHashmap *h, ثابت فراغ *کلید, فراغ *قدر) {
        أرجع hashmap_remove_قدر_2(PLAIN_HASHMAP(h), کلید, قدر);
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *قدر);
ساكن مضمن صحيح ordered_hashmap_remove_and_put(OrderedHashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *قدر) {
        أرجع hashmap_remove_and_put(PLAIN_HASHMAP(h), old_کلید, new_کلید, قدر);
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *قدر);
ساكن مضمن صحيح ordered_hashmap_remove_and_replace(OrderedHashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *قدر) {
        أرجع hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_کلید, new_کلید, قدر);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
صحيح hashmap_ملاؤ(Hashmap *h, Hashmap *other);
#تعریف_کرو hashmap_ملاؤ_2(h, other) hashmap_ملاؤ(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#تعریف_کرو ordered_hashmap_ملاؤ(h, other) hashmap_ملاؤ_2(h, other)

صحيح _hashmap_reserve(Hashmap_ادھار *h, بنانشانی entries_جوڑو);
ساكن مضمن صحيح hashmap_reserve(Hashmap *h, بنانشانی entries_جوڑو) {
        أرجع _hashmap_reserve(HASHMAP_ادھار(h), entries_جوڑو);
}
ساكن مضمن صحيح ordered_hashmap_reserve(OrderedHashmap *h, بنانشانی entries_جوڑو) {
        أرجع _hashmap_reserve(HASHMAP_ادھار(h), entries_جوڑو);
}

صحيح _hashmap_move(Hashmap_ادھار *h, Hashmap_ادھار *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ساكن مضمن صحيح hashmap_move(Hashmap *h, Hashmap *other) {
        أرجع _hashmap_move(HASHMAP_ادھار(h), HASHMAP_ادھار(other));
}
ساكن مضمن صحيح ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        أرجع _hashmap_move(HASHMAP_ادھار(h), HASHMAP_ادھار(other));
}

صحيح _hashmap_move_one(Hashmap_ادھار *h, Hashmap_ادھار *other, ثابت فراغ *کلید);
ساكن مضمن صحيح hashmap_move_one(Hashmap *h, Hashmap *other, ثابت فراغ *کلید) {
        أرجع _hashmap_move_one(HASHMAP_ادھار(h), HASHMAP_ادھار(other), کلید);
}
ساكن مضمن صحيح ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ثابت فراغ *کلید) {
        أرجع _hashmap_move_one(HASHMAP_ادھار(h), HASHMAP_ادھار(other), کلید);
}

بنانشانی hashmap_ناپ(Hashmap_ادھار *h) خالص;
ساكن مضمن بنانشانی hashmap_ناپ_2(Hashmap *h) {
        أرجع hashmap_ناپ(HASHMAP_ادھار(h));
}
ساكن مضمن بنانشانی ordered_hashmap_ناپ(OrderedHashmap *h) {
        أرجع hashmap_ناپ(HASHMAP_ادھار(h));
}

ساكن مضمن bool hashmap_isempty(Hashmap *h) {
        أرجع hashmap_ناپ_2(h) == 0;
}
ساكن مضمن bool ordered_hashmap_isempty(OrderedHashmap *h) {
        أرجع ordered_hashmap_ناپ(h) == 0;
}

بنانشانی _hashmap_buckets(Hashmap_ادھار *h) خالص;
ساكن مضمن بنانشانی hashmap_buckets(Hashmap *h) {
        أرجع _hashmap_buckets(HASHMAP_ادھار(h));
}
ساكن مضمن بنانشانی ordered_hashmap_buckets(OrderedHashmap *h) {
        أرجع _hashmap_buckets(HASHMAP_ادھار(h));
}

bool _hashmap_iterate(Hashmap_ادھار *h, Iterator *i, فراغ **قدر, ثابت فراغ **کلید);
ساكن مضمن bool hashmap_iterate(Hashmap *h, Iterator *i, فراغ **قدر, ثابت فراغ **کلید) {
        أرجع _hashmap_iterate(HASHMAP_ادھار(h), i, قدر, کلید);
}
ساكن مضمن bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, فراغ **قدر, ثابت فراغ **کلید) {
        أرجع _hashmap_iterate(HASHMAP_ادھار(h), i, قدر, کلید);
}

فراغ _hashmap_clear(Hashmap_ادھار *h);
ساكن مضمن فراغ hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_ادھار(h));
}
ساكن مضمن فراغ ordered_hashmap_clear(OrderedHashmap *h) {
        _hashmap_clear(HASHMAP_ادھار(h));
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

فراغ *hashmap_پہلا_کلید_and_قدر(Hashmap_ادھار *h, bool remove, فراغ **ret_کلید);
ساكن مضمن فراغ *hashmap_steal_پہلا_کلید_and_قدر(Hashmap *h, فراغ **ret) {
        أرجع hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار(h), true, ret);
}
ساكن مضمن فراغ *ordered_hashmap_steal_پہلا_کلید_and_قدر(OrderedHashmap *h, فراغ **ret) {
        أرجع hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار(h), true, ret);
}
ساكن مضمن فراغ *hashmap_پہلا_کلید_and_قدر_2(Hashmap *h, فراغ **ret) {
        أرجع hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار(h), false, ret);
}
ساكن مضمن فراغ *ordered_hashmap_پہلا_کلید_and_قدر(OrderedHashmap *h, فراغ **ret) {
        أرجع hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار(h), false, ret);
}

ساكن مضمن فراغ *hashmap_steal_پہلا(Hashmap *h) {
        أرجع hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار(h), true, عدم);
}
ساكن مضمن فراغ *ordered_hashmap_steal_پہلا(OrderedHashmap *h) {
        أرجع hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار(h), true, عدم);
}
ساكن مضمن فراغ *hashmap_پہلا(Hashmap *h) {
        أرجع hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار(h), false, عدم);
}
ساكن مضمن فراغ *ordered_hashmap_پہلا(OrderedHashmap *h) {
        أرجع hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار(h), false, عدم);
}

ساكن مضمن فراغ *hashmap_پہلا_کلید(Hashmap_ادھار *h, bool remove) {
        فراغ *کلید = عدم;

        (فراغ) hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار(h), remove, &کلید);
        أرجع کلید;
}
ساكن مضمن فراغ *hashmap_steal_پہلا_کلید(Hashmap *h) {
        أرجع hashmap_پہلا_کلید(HASHMAP_ادھار(h), true);
}
ساكن مضمن فراغ *ordered_hashmap_steal_پہلا_کلید(OrderedHashmap *h) {
        أرجع hashmap_پہلا_کلید(HASHMAP_ادھار(h), true);
}
ساكن مضمن فراغ *hashmap_پہلا_کلید_2(Hashmap *h) {
        أرجع hashmap_پہلا_کلید(HASHMAP_ادھار(h), false);
}
ساكن مضمن فراغ *ordered_hashmap_پہلا_کلید(OrderedHashmap *h) {
        أرجع hashmap_پہلا_کلید(HASHMAP_ادھار(h), false);
}

/* no hashmap_next */
فراغ* ordered_hashmap_اگلا(OrderedHashmap *h, ثابت فراغ *کلید);

حرف** _hashmap_get_strv(Hashmap_ادھار *h);
ساكن مضمن حرف** hashmap_get_strv(Hashmap *h) {
        أرجع _hashmap_get_strv(HASHMAP_ادھار(h));
}
ساكن مضمن حرف** ordered_hashmap_get_strv(OrderedHashmap *h) {
        أرجع _hashmap_get_strv(HASHMAP_ادھار(h));
}

صحيح _hashmap_dump_sorted(Hashmap_ادھار *h, فراغ ***ret, size_t *ret_n);
ساكن مضمن صحيح hashmap_dump_sorted(Hashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_ادھار(h), ret, ret_n);
}
ساكن مضمن صحيح ordered_hashmap_dump_sorted(OrderedHashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_ادھار(h), ret, ret_n);
}
ساكن مضمن صحيح سیٹ_dump_sorted(سیٹ_2 *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_ادھار(h), ret, ret_n);
}

صحيح hashmap_dump_کلیدیں_sorted(Hashmap_ادھار *h, فراغ ***ret, size_t *ret_n);
ساكن مضمن صحيح hashmap_dump_کلیدیں_sorted_2(Hashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع hashmap_dump_کلیدیں_sorted(HASHMAP_ادھار(h), ret, ret_n);
}
ساكن مضمن صحيح ordered_hashmap_dump_کلیدیں_sorted(OrderedHashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع hashmap_dump_کلیدیں_sorted(HASHMAP_ادھار(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#تعریف_کرو HASHMAP_ادھار_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_پہلا; _hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#تعریف_کرو HASHMAP_ادھار_FOREACH_2(e, h) \
        HASHMAP_ادھار_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کرو _HASHMAP_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_پہلا; hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#تعریف_کرو HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کرو _ORDERED_HASHMAP_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_پہلا; ordered_hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#تعریف_کرو ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کرو HASHMAP_ادھار_FOREACH_کلید(e, k, h, i) \
        لكل (Iterator i = ITERATOR_پہلا; _hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#تعریف_کرو HASHMAP_ادھار_FOREACH_کلید_2(e, k, h) \
        HASHMAP_ادھار_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

#تعریف_کرو HASHMAP_FOREACH_کلید(e, k, h, i) \
        لكل (Iterator i = ITERATOR_پہلا; hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#تعریف_کرو HASHMAP_FOREACH_کلید_2(e, k, h) \
        HASHMAP_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

#تعریف_کرو ORDERED_HASHMAP_FOREACH_کلید(e, k, h, i) \
        لكل (Iterator i = ITERATOR_پہلا; ordered_hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#تعریف_کرو ORDERED_HASHMAP_FOREACH_کلید_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#تعریف_کرو _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#تعریف_کرو _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#تعریف_کرو _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

فراغ hashmap_trim_pools(فراغ);
