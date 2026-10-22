/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

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

#تعریف_کرو HASH_کلید_سائز 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
قسمتعریف ساخت Hashmap_بنیاد Hashmap_بنیاد;

/* Specific hashmap/set types */
قسمتعریف ساخت Hashmap Hashmap;               /* Maps keys to values */
قسمتعریف ساخت OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
قسمتعریف ساخت سیٹ_2 سیٹ_2;                       /* Stores just keys */

قسمتعریف ساخت IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

/* Macros for type checking */
#تعریف_کرو PTR_COMPATIBLE_WITH_HASHMAP_بنیاد(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap_بنیاد*) || \
         __builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*) || \
         __builtin_types_compatible_p(typeof(h), سیٹ_2*))

#تعریف_کرو PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*)) \

#تعریف_کرو HASHMAP_بنیاد(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_HASHMAP_بنیاد(h), \
                (Hashmap_بنیاد*)(h), \
                (خالی)0)

#تعریف_کرو PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (خالی)0)

Hashmap* hashmap_new(ثابت ساخت hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ثابت ساخت hash_ops *hash_ops);

#تعریف_کرو hashmap_free_and_replace(a, b)                          \
        free_and_replace_بھرا(a, b, hashmap_free)
#تعریف_کرو ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_بھرا(a, b, ordered_hashmap_free)

Hashmap_بنیاد* _hashmap_free(Hashmap_بنیاد *h);
ساکن اندرونی Hashmap* hashmap_free(Hashmap *h) {
        واپس (خالی*) _hashmap_free(HASHMAP_بنیاد(h));
}
ساکن اندرونی OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        واپس (خالی*) _hashmap_free(HASHMAP_بنیاد(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
صحیح iterated_cache_get(IteratedCache *cache, ثابت خالی ***res_کلیدیں, ثابت خالی ***res_اقدار, غیرعلامتی *res_n_entries);

Hashmap_بنیاد* _hashmap_copy(Hashmap_بنیاد *h);
ساکن اندرونی Hashmap* hashmap_copy(Hashmap *h) {
        واپس (Hashmap*) _hashmap_copy(HASHMAP_بنیاد(h));
}
ساکن اندرونی OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        واپس (OrderedHashmap*) _hashmap_copy(HASHMAP_بنیاد(h));
}

صحیح hashmap_ensure_allocated(Hashmap **h, ثابت ساخت hash_ops *hash_ops);
صحیح hashmap_ensure_put(Hashmap **h, ثابت ساخت hash_ops *hash_ops, ثابت خالی *کلید, خالی *قدر);
صحیح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت ساخت hash_ops *hash_ops);
صحیح hashmap_ensure_replace(Hashmap **h, ثابت ساخت hash_ops *hash_ops, ثابت خالی *کلید, خالی *قدر);

صحیح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت ساخت hash_ops *hash_ops, ثابت خالی *کلید, خالی *قدر);
صحیح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت ساخت hash_ops *hash_ops, ثابت خالی *کلید, خالی *قدر);

IteratedCache* _hashmap_iterated_cache_new(Hashmap_بنیاد *h);
ساکن اندرونی IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        واپس _hashmap_iterated_cache_new(HASHMAP_بنیاد(h));
}
ساکن اندرونی IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        واپس _hashmap_iterated_cache_new(HASHMAP_بنیاد(h));
}

صحیح hashmap_put(Hashmap *h, ثابت خالی *کلید, خالی *قدر);
ساکن اندرونی صحیح ordered_hashmap_put(OrderedHashmap *h, ثابت خالی *کلید, خالی *قدر) {
        واپس hashmap_put(PLAIN_HASHMAP(h), کلید, قدر);
}

صحیح hashmap_put_strdup_بھرا(Hashmap **h, ثابت ساخت hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v);
ساکن اندرونی صحیح hashmap_put_strdup(Hashmap **h, ثابت حرف *k, ثابت حرف *v) {
        واپس hashmap_put_strdup_بھرا(h, &string_hash_ops_free_free, k, v);
}

صحیح hashmap_update(Hashmap *h, ثابت خالی *کلید, خالی *قدر);
ساکن اندرونی صحیح ordered_hashmap_update(OrderedHashmap *h, ثابت خالی *کلید, خالی *قدر) {
        واپس hashmap_update(PLAIN_HASHMAP(h), کلید, قدر);
}

صحیح hashmap_replace(Hashmap *h, ثابت خالی *کلید, خالی *قدر);
ساکن اندرونی صحیح ordered_hashmap_replace(OrderedHashmap *h, ثابت خالی *کلید, خالی *قدر) {
        واپس hashmap_replace(PLAIN_HASHMAP(h), کلید, قدر);
}

خالی* _hashmap_get(Hashmap_بنیاد *h, ثابت خالی *کلید);
ساکن اندرونی خالی *hashmap_get(Hashmap *h, ثابت خالی *کلید) {
        واپس _hashmap_get(HASHMAP_بنیاد(h), کلید);
}
ساکن اندرونی خالی *ordered_hashmap_get(OrderedHashmap *h, ثابت خالی *کلید) {
        واپس _hashmap_get(HASHMAP_بنیاد(h), کلید);
}

خالی* hashmap_get2(Hashmap *h, ثابت خالی *کلید, خالی **ret);
ساکن اندرونی خالی *ordered_hashmap_get2(OrderedHashmap *h, ثابت خالی *کلید, خالی **ret) {
        واپس hashmap_get2(PLAIN_HASHMAP(h), کلید, ret);
}

bool hashmap_شامل_ہے_2(Hashmap_بنیاد *h, ثابت خالی *کلید);
ساکن اندرونی bool hashmap_شامل_ہے(Hashmap *h, ثابت خالی *کلید) {
        واپس hashmap_شامل_ہے_2(HASHMAP_بنیاد(h), کلید);
}
ساکن اندرونی bool ordered_hashmap_شامل_ہے(OrderedHashmap *h, ثابت خالی *کلید) {
        واپس hashmap_شامل_ہے_2(HASHMAP_بنیاد(h), کلید);
}

خالی* _hashmap_remove(Hashmap_بنیاد *h, ثابت خالی *کلید);
ساکن اندرونی خالی *hashmap_remove(Hashmap *h, ثابت خالی *کلید) {
        واپس _hashmap_remove(HASHMAP_بنیاد(h), کلید);
}
ساکن اندرونی خالی *ordered_hashmap_remove(OrderedHashmap *h, ثابت خالی *کلید) {
        واپس _hashmap_remove(HASHMAP_بنیاد(h), کلید);
}

خالی* hashmap_remove2(Hashmap *h, ثابت خالی *کلید, خالی **ret);
ساکن اندرونی خالی *ordered_hashmap_remove2(OrderedHashmap *h, ثابت خالی *کلید, خالی **ret) {
        واپس hashmap_remove2(PLAIN_HASHMAP(h), کلید, ret);
}

خالی* hashmap_remove_قدر(Hashmap_بنیاد *h, ثابت خالی *کلید, خالی *قدر);
ساکن اندرونی خالی *hashmap_remove_قدر_2(Hashmap *h, ثابت خالی *کلید, خالی *قدر) {
        واپس hashmap_remove_قدر(HASHMAP_بنیاد(h), کلید, قدر);
}

ساکن اندرونی خالی* ordered_hashmap_remove_قدر(OrderedHashmap *h, ثابت خالی *کلید, خالی *قدر) {
        واپس hashmap_remove_قدر_2(PLAIN_HASHMAP(h), کلید, قدر);
}

صحیح hashmap_remove_and_put(Hashmap *h, ثابت خالی *old_کلید, ثابت خالی *new_کلید, خالی *قدر);
ساکن اندرونی صحیح ordered_hashmap_remove_and_put(OrderedHashmap *h, ثابت خالی *old_کلید, ثابت خالی *new_کلید, خالی *قدر) {
        واپس hashmap_remove_and_put(PLAIN_HASHMAP(h), old_کلید, new_کلید, قدر);
}

صحیح hashmap_remove_and_replace(Hashmap *h, ثابت خالی *old_کلید, ثابت خالی *new_کلید, خالی *قدر);
ساکن اندرونی صحیح ordered_hashmap_remove_and_replace(OrderedHashmap *h, ثابت خالی *old_کلید, ثابت خالی *new_کلید, خالی *قدر) {
        واپس hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_کلید, new_کلید, قدر);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
صحیح hashmap_ملاؤ(Hashmap *h, Hashmap *other);
#تعریف_کرو hashmap_ملاؤ_2(h, other) hashmap_ملاؤ(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#تعریف_کرو ordered_hashmap_ملاؤ(h, other) hashmap_ملاؤ_2(h, other)

صحیح _hashmap_reserve(Hashmap_بنیاد *h, غیرعلامتی entries_جوڑو);
ساکن اندرونی صحیح hashmap_reserve(Hashmap *h, غیرعلامتی entries_جوڑو) {
        واپس _hashmap_reserve(HASHMAP_بنیاد(h), entries_جوڑو);
}
ساکن اندرونی صحیح ordered_hashmap_reserve(OrderedHashmap *h, غیرعلامتی entries_جوڑو) {
        واپس _hashmap_reserve(HASHMAP_بنیاد(h), entries_جوڑو);
}

صحیح _hashmap_move(Hashmap_بنیاد *h, Hashmap_بنیاد *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ساکن اندرونی صحیح hashmap_move(Hashmap *h, Hashmap *other) {
        واپس _hashmap_move(HASHMAP_بنیاد(h), HASHMAP_بنیاد(other));
}
ساکن اندرونی صحیح ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        واپس _hashmap_move(HASHMAP_بنیاد(h), HASHMAP_بنیاد(other));
}

صحیح _hashmap_move_one(Hashmap_بنیاد *h, Hashmap_بنیاد *other, ثابت خالی *کلید);
ساکن اندرونی صحیح hashmap_move_one(Hashmap *h, Hashmap *other, ثابت خالی *کلید) {
        واپس _hashmap_move_one(HASHMAP_بنیاد(h), HASHMAP_بنیاد(other), کلید);
}
ساکن اندرونی صحیح ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ثابت خالی *کلید) {
        واپس _hashmap_move_one(HASHMAP_بنیاد(h), HASHMAP_بنیاد(other), کلید);
}

غیرعلامتی hashmap_سائز(Hashmap_بنیاد *h) خالص;
ساکن اندرونی غیرعلامتی hashmap_سائز_2(Hashmap *h) {
        واپس hashmap_سائز(HASHMAP_بنیاد(h));
}
ساکن اندرونی غیرعلامتی ordered_hashmap_سائز(OrderedHashmap *h) {
        واپس hashmap_سائز(HASHMAP_بنیاد(h));
}

ساکن اندرونی bool hashmap_isempty(Hashmap *h) {
        واپس hashmap_سائز_2(h) == 0;
}
ساکن اندرونی bool ordered_hashmap_isempty(OrderedHashmap *h) {
        واپس ordered_hashmap_سائز(h) == 0;
}

غیرعلامتی _hashmap_buckets(Hashmap_بنیاد *h) خالص;
ساکن اندرونی غیرعلامتی hashmap_buckets(Hashmap *h) {
        واپس _hashmap_buckets(HASHMAP_بنیاد(h));
}
ساکن اندرونی غیرعلامتی ordered_hashmap_buckets(OrderedHashmap *h) {
        واپس _hashmap_buckets(HASHMAP_بنیاد(h));
}

bool _hashmap_iterate(Hashmap_بنیاد *h, Iterator *i, خالی **قدر, ثابت خالی **کلید);
ساکن اندرونی bool hashmap_iterate(Hashmap *h, Iterator *i, خالی **قدر, ثابت خالی **کلید) {
        واپس _hashmap_iterate(HASHMAP_بنیاد(h), i, قدر, کلید);
}
ساکن اندرونی bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, خالی **قدر, ثابت خالی **کلید) {
        واپس _hashmap_iterate(HASHMAP_بنیاد(h), i, قدر, کلید);
}

خالی _hashmap_clear(Hashmap_بنیاد *h);
ساکن اندرونی خالی hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_بنیاد(h));
}
ساکن اندرونی خالی ordered_hashmap_clear(OrderedHashmap *h) {
        _hashmap_clear(HASHMAP_بنیاد(h));
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

خالی *hashmap_پہلا_کلید_and_قدر(Hashmap_بنیاد *h, bool remove, خالی **ret_کلید);
ساکن اندرونی خالی *hashmap_steal_پہلا_کلید_and_قدر(Hashmap *h, خالی **ret) {
        واپس hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد(h), true, ret);
}
ساکن اندرونی خالی *ordered_hashmap_steal_پہلا_کلید_and_قدر(OrderedHashmap *h, خالی **ret) {
        واپس hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد(h), true, ret);
}
ساکن اندرونی خالی *hashmap_پہلا_کلید_and_قدر_2(Hashmap *h, خالی **ret) {
        واپس hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد(h), false, ret);
}
ساکن اندرونی خالی *ordered_hashmap_پہلا_کلید_and_قدر(OrderedHashmap *h, خالی **ret) {
        واپس hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد(h), false, ret);
}

ساکن اندرونی خالی *hashmap_steal_پہلا(Hashmap *h) {
        واپس hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد(h), true, تہی);
}
ساکن اندرونی خالی *ordered_hashmap_steal_پہلا(OrderedHashmap *h) {
        واپس hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد(h), true, تہی);
}
ساکن اندرونی خالی *hashmap_پہلا(Hashmap *h) {
        واپس hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد(h), false, تہی);
}
ساکن اندرونی خالی *ordered_hashmap_پہلا(OrderedHashmap *h) {
        واپس hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد(h), false, تہی);
}

ساکن اندرونی خالی *hashmap_پہلا_کلید(Hashmap_بنیاد *h, bool remove) {
        خالی *کلید = تہی;

        (خالی) hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد(h), remove, &کلید);
        واپس کلید;
}
ساکن اندرونی خالی *hashmap_steal_پہلا_کلید(Hashmap *h) {
        واپس hashmap_پہلا_کلید(HASHMAP_بنیاد(h), true);
}
ساکن اندرونی خالی *ordered_hashmap_steal_پہلا_کلید(OrderedHashmap *h) {
        واپس hashmap_پہلا_کلید(HASHMAP_بنیاد(h), true);
}
ساکن اندرونی خالی *hashmap_پہلا_کلید_2(Hashmap *h) {
        واپس hashmap_پہلا_کلید(HASHMAP_بنیاد(h), false);
}
ساکن اندرونی خالی *ordered_hashmap_پہلا_کلید(OrderedHashmap *h) {
        واپس hashmap_پہلا_کلید(HASHMAP_بنیاد(h), false);
}

/* no hashmap_next */
خالی* ordered_hashmap_اگلا(OrderedHashmap *h, ثابت خالی *کلید);

حرف** _hashmap_get_strv(Hashmap_بنیاد *h);
ساکن اندرونی حرف** hashmap_get_strv(Hashmap *h) {
        واپس _hashmap_get_strv(HASHMAP_بنیاد(h));
}
ساکن اندرونی حرف** ordered_hashmap_get_strv(OrderedHashmap *h) {
        واپس _hashmap_get_strv(HASHMAP_بنیاد(h));
}

صحیح _hashmap_dump_sorted(Hashmap_بنیاد *h, خالی ***ret, size_t *ret_n);
ساکن اندرونی صحیح hashmap_dump_sorted(Hashmap *h, خالی ***ret, size_t *ret_n) {
        واپس _hashmap_dump_sorted(HASHMAP_بنیاد(h), ret, ret_n);
}
ساکن اندرونی صحیح ordered_hashmap_dump_sorted(OrderedHashmap *h, خالی ***ret, size_t *ret_n) {
        واپس _hashmap_dump_sorted(HASHMAP_بنیاد(h), ret, ret_n);
}
ساکن اندرونی صحیح سیٹ_dump_sorted(سیٹ_2 *h, خالی ***ret, size_t *ret_n) {
        واپس _hashmap_dump_sorted(HASHMAP_بنیاد(h), ret, ret_n);
}

صحیح hashmap_dump_کلیدیں_sorted(Hashmap_بنیاد *h, خالی ***ret, size_t *ret_n);
ساکن اندرونی صحیح hashmap_dump_کلیدیں_sorted_2(Hashmap *h, خالی ***ret, size_t *ret_n) {
        واپس hashmap_dump_کلیدیں_sorted(HASHMAP_بنیاد(h), ret, ret_n);
}
ساکن اندرونی صحیح ordered_hashmap_dump_کلیدیں_sorted(OrderedHashmap *h, خالی ***ret, size_t *ret_n) {
        واپس hashmap_dump_کلیدیں_sorted(HASHMAP_بنیاد(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#تعریف_کرو HASHMAP_بنیاد_FOREACH(e, h, i) \
        برائے (Iterator i = ITERATOR_پہلا; _hashmap_iterate((h), &i, (خالی**)&(e), تہی); )
#تعریف_کرو HASHMAP_بنیاد_FOREACH_2(e, h) \
        HASHMAP_بنیاد_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کرو _HASHMAP_FOREACH(e, h, i) \
        برائے (Iterator i = ITERATOR_پہلا; hashmap_iterate((h), &i, (خالی**)&(e), تہی); )
#تعریف_کرو HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کرو _ORDERED_HASHMAP_FOREACH(e, h, i) \
        برائے (Iterator i = ITERATOR_پہلا; ordered_hashmap_iterate((h), &i, (خالی**)&(e), تہی); )
#تعریف_کرو ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#تعریف_کرو HASHMAP_بنیاد_FOREACH_کلید(e, k, h, i) \
        برائے (Iterator i = ITERATOR_پہلا; _hashmap_iterate((h), &i, (خالی**)&(e), (ثابت خالی**) &(k)); )
#تعریف_کرو HASHMAP_بنیاد_FOREACH_کلید_2(e, k, h) \
        HASHMAP_بنیاد_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

#تعریف_کرو HASHMAP_FOREACH_کلید(e, k, h, i) \
        برائے (Iterator i = ITERATOR_پہلا; hashmap_iterate((h), &i, (خالی**)&(e), (ثابت خالی**) &(k)); )
#تعریف_کرو HASHMAP_FOREACH_کلید_2(e, k, h) \
        HASHMAP_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

#تعریف_کرو ORDERED_HASHMAP_FOREACH_کلید(e, k, h, i) \
        برائے (Iterator i = ITERATOR_پہلا; ordered_hashmap_iterate((h), &i, (خالی**)&(e), (ثابت خالی**) &(k)); )
#تعریف_کرو ORDERED_HASHMAP_FOREACH_کلید_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_کلید(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#تعریف_کرو _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#تعریف_کرو _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#تعریف_کرو _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

خالی hashmap_trim_pools(خالی);
