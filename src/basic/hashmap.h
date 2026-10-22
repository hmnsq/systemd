/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

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

#تعریف_کن HASH_کلید_اندازه 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
تعريفنوع بنية Hashmap_پایه Hashmap_پایه;

/* Specific hashmap/set types */
تعريفنوع بنية Hashmap Hashmap;               /* Maps keys to values */
تعريفنوع بنية OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
تعريفنوع بنية مجموعه_2 مجموعه_2;                       /* Stores just keys */

تعريفنوع بنية IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

/* Macros for type checking */
#تعریف_کن PTR_COMPATIBLE_WITH_HASHMAP_پایه(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap_پایه*) || \
         __builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*) || \
         __builtin_types_compatible_p(typeof(h), مجموعه_2*))

#تعریف_کن PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*)) \

#تعریف_کن HASHMAP_پایه(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_HASHMAP_پایه(h), \
                (Hashmap_پایه*)(h), \
                (فراغ)0)

#تعریف_کن PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (فراغ)0)

Hashmap* hashmap_new(ثابت بنية hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ثابت بنية hash_ops *hash_ops);

#تعریف_کن hashmap_free_and_replace(a, b)                          \
        free_and_replace_پر(a, b, hashmap_free)
#تعریف_کن ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_پر(a, b, ordered_hashmap_free)

Hashmap_پایه* _hashmap_free(Hashmap_پایه *h);
ساكن مضمن Hashmap* hashmap_free(Hashmap *h) {
        أرجع (فراغ*) _hashmap_free(HASHMAP_پایه(h));
}
ساكن مضمن OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        أرجع (فراغ*) _hashmap_free(HASHMAP_پایه(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_کلیدها, ثابت فراغ ***res_مقادیر, بیعلامت *res_n_entries);

Hashmap_پایه* _hashmap_copy(Hashmap_پایه *h);
ساكن مضمن Hashmap* hashmap_copy(Hashmap *h) {
        أرجع (Hashmap*) _hashmap_copy(HASHMAP_پایه(h));
}
ساكن مضمن OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        أرجع (OrderedHashmap*) _hashmap_copy(HASHMAP_پایه(h));
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops);
صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *مقدار);
صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops);
صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *مقدار);

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *مقدار);
صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *مقدار);

IteratedCache* _hashmap_iterated_cache_new(Hashmap_پایه *h);
ساكن مضمن IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        أرجع _hashmap_iterated_cache_new(HASHMAP_پایه(h));
}
ساكن مضمن IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        أرجع _hashmap_iterated_cache_new(HASHMAP_پایه(h));
}

صحيح hashmap_put(Hashmap *h, ثابت فراغ *کلید, فراغ *مقدار);
ساكن مضمن صحيح ordered_hashmap_put(OrderedHashmap *h, ثابت فراغ *کلید, فراغ *مقدار) {
        أرجع hashmap_put(PLAIN_HASHMAP(h), کلید, مقدار);
}

صحيح hashmap_put_strdup_پر(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v);
ساكن مضمن صحيح hashmap_put_strdup(Hashmap **h, ثابت حرف *k, ثابت حرف *v) {
        أرجع hashmap_put_strdup_پر(h, &string_hash_ops_free_free, k, v);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *کلید, فراغ *مقدار);
ساكن مضمن صحيح ordered_hashmap_update(OrderedHashmap *h, ثابت فراغ *کلید, فراغ *مقدار) {
        أرجع hashmap_update(PLAIN_HASHMAP(h), کلید, مقدار);
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *کلید, فراغ *مقدار);
ساكن مضمن صحيح ordered_hashmap_replace(OrderedHashmap *h, ثابت فراغ *کلید, فراغ *مقدار) {
        أرجع hashmap_replace(PLAIN_HASHMAP(h), کلید, مقدار);
}

فراغ* _hashmap_get(Hashmap_پایه *h, ثابت فراغ *کلید);
ساكن مضمن فراغ *hashmap_get(Hashmap *h, ثابت فراغ *کلید) {
        أرجع _hashmap_get(HASHMAP_پایه(h), کلید);
}
ساكن مضمن فراغ *ordered_hashmap_get(OrderedHashmap *h, ثابت فراغ *کلید) {
        أرجع _hashmap_get(HASHMAP_پایه(h), کلید);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *کلید, فراغ **ret);
ساكن مضمن فراغ *ordered_hashmap_get2(OrderedHashmap *h, ثابت فراغ *کلید, فراغ **ret) {
        أرجع hashmap_get2(PLAIN_HASHMAP(h), کلید, ret);
}

bool hashmap_شامل_است_2(Hashmap_پایه *h, ثابت فراغ *کلید);
ساكن مضمن bool hashmap_شامل_است(Hashmap *h, ثابت فراغ *کلید) {
        أرجع hashmap_شامل_است_2(HASHMAP_پایه(h), کلید);
}
ساكن مضمن bool ordered_hashmap_شامل_است(OrderedHashmap *h, ثابت فراغ *کلید) {
        أرجع hashmap_شامل_است_2(HASHMAP_پایه(h), کلید);
}

فراغ* _hashmap_remove(Hashmap_پایه *h, ثابت فراغ *کلید);
ساكن مضمن فراغ *hashmap_remove(Hashmap *h, ثابت فراغ *کلید) {
        أرجع _hashmap_remove(HASHMAP_پایه(h), کلید);
}
ساكن مضمن فراغ *ordered_hashmap_remove(OrderedHashmap *h, ثابت فراغ *کلید) {
        أرجع _hashmap_remove(HASHMAP_پایه(h), کلید);
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *کلید, فراغ **ret);
ساكن مضمن فراغ *ordered_hashmap_remove2(OrderedHashmap *h, ثابت فراغ *کلید, فراغ **ret) {
        أرجع hashmap_remove2(PLAIN_HASHMAP(h), کلید, ret);
}

فراغ* hashmap_remove_مقدار(Hashmap_پایه *h, ثابت فراغ *کلید, فراغ *مقدار);
ساكن مضمن فراغ *hashmap_remove_مقدار_2(Hashmap *h, ثابت فراغ *کلید, فراغ *مقدار) {
        أرجع hashmap_remove_مقدار(HASHMAP_پایه(h), کلید, مقدار);
}

ساكن مضمن فراغ* ordered_hashmap_remove_مقدار(OrderedHashmap *h, ثابت فراغ *کلید, فراغ *مقدار) {
        أرجع hashmap_remove_مقدار_2(PLAIN_HASHMAP(h), کلید, مقدار);
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *مقدار);
ساكن مضمن صحيح ordered_hashmap_remove_and_put(OrderedHashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *مقدار) {
        أرجع hashmap_remove_and_put(PLAIN_HASHMAP(h), old_کلید, new_کلید, مقدار);
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *مقدار);
ساكن مضمن صحيح ordered_hashmap_remove_and_replace(OrderedHashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *مقدار) {
        أرجع hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_کلید, new_کلید, مقدار);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
صحيح hashmap_ادغام(Hashmap *h, Hashmap *other);
#تعریف_کن hashmap_ادغام_2(h, other) hashmap_ادغام(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#تعریف_کن ordered_hashmap_ادغام(h, other) hashmap_ادغام_2(h, other)

صحيح _hashmap_reserve(Hashmap_پایه *h, بیعلامت entries_جمع_کن);
ساكن مضمن صحيح hashmap_reserve(Hashmap *h, بیعلامت entries_جمع_کن) {
        أرجع _hashmap_reserve(HASHMAP_پایه(h), entries_جمع_کن);
}
ساكن مضمن صحيح ordered_hashmap_reserve(OrderedHashmap *h, بیعلامت entries_جمع_کن) {
        أرجع _hashmap_reserve(HASHMAP_پایه(h), entries_جمع_کن);
}

صحيح _hashmap_move(Hashmap_پایه *h, Hashmap_پایه *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ساكن مضمن صحيح hashmap_move(Hashmap *h, Hashmap *other) {
        أرجع _hashmap_move(HASHMAP_پایه(h), HASHMAP_پایه(other));
}
ساكن مضمن صحيح ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        أرجع _hashmap_move(HASHMAP_پایه(h), HASHMAP_پایه(other));
}

صحيح _hashmap_move_one(Hashmap_پایه *h, Hashmap_پایه *other, ثابت فراغ *کلید);
ساكن مضمن صحيح hashmap_move_one(Hashmap *h, Hashmap *other, ثابت فراغ *کلید) {
        أرجع _hashmap_move_one(HASHMAP_پایه(h), HASHMAP_پایه(other), کلید);
}
ساكن مضمن صحيح ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ثابت فراغ *کلید) {
        أرجع _hashmap_move_one(HASHMAP_پایه(h), HASHMAP_پایه(other), کلید);
}

بیعلامت hashmap_اندازه(Hashmap_پایه *h) خالص;
ساكن مضمن بیعلامت hashmap_اندازه_2(Hashmap *h) {
        أرجع hashmap_اندازه(HASHMAP_پایه(h));
}
ساكن مضمن بیعلامت ordered_hashmap_اندازه(OrderedHashmap *h) {
        أرجع hashmap_اندازه(HASHMAP_پایه(h));
}

ساكن مضمن bool hashmap_isempty(Hashmap *h) {
        أرجع hashmap_اندازه_2(h) == 0;
}
ساكن مضمن bool ordered_hashmap_isempty(OrderedHashmap *h) {
        أرجع ordered_hashmap_اندازه(h) == 0;
}

بیعلامت _hashmap_buckets(Hashmap_پایه *h) خالص;
ساكن مضمن بیعلامت hashmap_buckets(Hashmap *h) {
        أرجع _hashmap_buckets(HASHMAP_پایه(h));
}
ساكن مضمن بیعلامت ordered_hashmap_buckets(OrderedHashmap *h) {
        أرجع _hashmap_buckets(HASHMAP_پایه(h));
}

bool _hashmap_iterate(Hashmap_پایه *h, Iterator *i, فراغ **مقدار, ثابت فراغ **کلید);
ساكن مضمن bool hashmap_iterate(Hashmap *h, Iterator *i, فراغ **مقدار, ثابت فراغ **کلید) {
        أرجع _hashmap_iterate(HASHMAP_پایه(h), i, مقدار, کلید);
}
ساكن مضمن bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, فراغ **مقدار, ثابت فراغ **کلید) {
        أرجع _hashmap_iterate(HASHMAP_پایه(h), i, مقدار, کلید);
}

فراغ _hashmap_clear(Hashmap_پایه *h);
ساكن مضمن فراغ hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_پایه(h));
}
ساكن مضمن فراغ ordered_hashmap_clear(OrderedHashmap *h) {
        _hashmap_clear(HASHMAP_پایه(h));
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

فراغ *hashmap_اولی_کلید_and_مقدار(Hashmap_پایه *h, bool remove, فراغ **ret_کلید);
ساكن مضمن فراغ *hashmap_steal_اولی_کلید_and_مقدار(Hashmap *h, فراغ **ret) {
        أرجع hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه(h), true, ret);
}
ساكن مضمن فراغ *ordered_hashmap_steal_اولی_کلید_and_مقدار(OrderedHashmap *h, فراغ **ret) {
        أرجع hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه(h), true, ret);
}
ساكن مضمن فراغ *hashmap_اولی_کلید_and_مقدار_2(Hashmap *h, فراغ **ret) {
        أرجع hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه(h), false, ret);
}
ساكن مضمن فراغ *ordered_hashmap_اولی_کلید_and_مقدار(OrderedHashmap *h, فراغ **ret) {
        أرجع hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه(h), false, ret);
}

ساكن مضمن فراغ *hashmap_steal_اولی(Hashmap *h) {
        أرجع hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه(h), true, عدم);
}
ساكن مضمن فراغ *ordered_hashmap_steal_اولی(OrderedHashmap *h) {
        أرجع hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه(h), true, عدم);
}
ساكن مضمن فراغ *hashmap_اولی(Hashmap *h) {
        أرجع hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه(h), false, عدم);
}
ساكن مضمن فراغ *ordered_hashmap_اولی(OrderedHashmap *h) {
        أرجع hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه(h), false, عدم);
}

ساكن مضمن فراغ *hashmap_اولی_کلید(Hashmap_پایه *h, bool remove) {
        فراغ *کلید = عدم;

        (فراغ) hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه(h), remove, &کلید);
        أرجع کلید;
}
ساكن مضمن فراغ *hashmap_steal_اولی_کلید(Hashmap *h) {
        أرجع hashmap_اولی_کلید(HASHMAP_پایه(h), true);
}
ساكن مضمن فراغ *ordered_hashmap_steal_اولی_کلید(OrderedHashmap *h) {
        أرجع hashmap_اولی_کلید(HASHMAP_پایه(h), true);
}
ساكن مضمن فراغ *hashmap_اولی_کلید_2(Hashmap *h) {
        أرجع hashmap_اولی_کلید(HASHMAP_پایه(h), false);
}
ساكن مضمن فراغ *ordered_hashmap_اولی_کلید(OrderedHashmap *h) {
        أرجع hashmap_اولی_کلید(HASHMAP_پایه(h), false);
}

/* no hashmap_next */
فراغ* ordered_hashmap_بعدی(OrderedHashmap *h, ثابت فراغ *کلید);

حرف** _hashmap_get_strv(Hashmap_پایه *h);
ساكن مضمن حرف** hashmap_get_strv(Hashmap *h) {
        أرجع _hashmap_get_strv(HASHMAP_پایه(h));
}
ساكن مضمن حرف** ordered_hashmap_get_strv(OrderedHashmap *h) {
        أرجع _hashmap_get_strv(HASHMAP_پایه(h));
}

صحيح _hashmap_dump_sorted(Hashmap_پایه *h, فراغ ***ret, size_t *ret_n);
ساكن مضمن صحيح hashmap_dump_sorted(Hashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_پایه(h), ret, ret_n);
}
ساكن مضمن صحيح ordered_hashmap_dump_sorted(OrderedHashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_پایه(h), ret, ret_n);
}
ساكن مضمن صحيح مجموعه_dump_sorted(مجموعه_2 *h, فراغ ***ret, size_t *ret_n) {
        أرجع _hashmap_dump_sorted(HASHMAP_پایه(h), ret, ret_n);
}

صحيح hashmap_dump_کلیدها_sorted(Hashmap_پایه *h, فراغ ***ret, size_t *ret_n);
ساكن مضمن صحيح hashmap_dump_کلیدها_sorted_2(Hashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع hashmap_dump_کلیدها_sorted(HASHMAP_پایه(h), ret, ret_n);
}
ساكن مضمن صحيح ordered_hashmap_dump_کلیدها_sorted(OrderedHashmap *h, فراغ ***ret, size_t *ret_n) {
        أرجع hashmap_dump_کلیدها_sorted(HASHMAP_پایه(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#تعریف_کن HASHMAP_پایه_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_اولی; _hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#تعریف_کن HASHMAP_پایه_FOREACH_2(e, h) \
        HASHMAP_پایه_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کن _HASHMAP_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_اولی; hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#تعریف_کن HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کن _ORDERED_HASHMAP_FOREACH(e, h, i) \
        لكل (Iterator i = ITERATOR_اولی; ordered_hashmap_iterate((h), &i, (فراغ**)&(e), عدم); )
#تعریف_کن ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کن HASHMAP_پایه_FOREACH_کلید(e, k, h, i) \
        لكل (Iterator i = ITERATOR_اولی; _hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#تعریف_کن HASHMAP_پایه_FOREACH_کلید_2(e, k, h) \
        HASHMAP_پایه_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

#تعریف_کن HASHMAP_FOREACH_کلید(e, k, h, i) \
        لكل (Iterator i = ITERATOR_اولی; hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#تعریف_کن HASHMAP_FOREACH_کلید_2(e, k, h) \
        HASHMAP_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

#تعریف_کن ORDERED_HASHMAP_FOREACH_کلید(e, k, h, i) \
        لكل (Iterator i = ITERATOR_اولی; ordered_hashmap_iterate((h), &i, (فراغ**)&(e), (ثابت فراغ**) &(k)); )
#تعریف_کن ORDERED_HASHMAP_FOREACH_کلید_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#تعریف_کن _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#تعریف_کن _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#تعریف_کن _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

فراغ hashmap_trim_pools(فراغ);
