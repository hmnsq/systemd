/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "hash-funcs.h"
#অন্তর্ভাব "iterator.h"

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

#define HASH_চাবি_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
প্রকারনির্দেশ সংরচনা HashmapBase HashmapBase;

/* Specific hashmap/set types */
প্রকারনির্দেশ সংরচনা Hashmap Hashmap;               /* Maps keys to values */
প্রকারনির্দেশ সংরচনা OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
প্রকারনির্দেশ সংরচনা Set Set;                       /* Stores just keys */

প্রকারনির্দেশ সংরচনা IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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
                (শূন্য)0)

#define PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (শূন্য)0)

Hashmap* hashmap_new(ধ্রুব সংরচনা hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ধ্রুব সংরচনা hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
স্থির অন্তঃস্থ Hashmap* hashmap_free(Hashmap *h) {
        প্রত্যাগম (শূন্য*) _hashmap_free(HASHMAP_BASE(h));
}
স্থির অন্তঃস্থ OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        প্রত্যাগম (শূন্য*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
পূর্ণাঙ্ক iterated_cache_get(IteratedCache *cache, ধ্রুব শূন্য ***res_চাবিগুলি, ধ্রুব শূন্য ***res_মানসমূহ, unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
স্থির অন্তঃস্থ Hashmap* hashmap_copy(Hashmap *h) {
        প্রত্যাগম (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
স্থির অন্তঃস্থ OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        প্রত্যাগম (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

পূর্ণাঙ্ক hashmap_ensure_allocated(Hashmap **h, ধ্রুব সংরচনা hash_ops *hash_ops);
পূর্ণাঙ্ক hashmap_ensure_put(Hashmap **h, ধ্রুব সংরচনা hash_ops *hash_ops, ধ্রুব শূন্য *চাবি, শূন্য *মান);
পূর্ণাঙ্ক ordered_hashmap_ensure_allocated(OrderedHashmap **h, ধ্রুব সংরচনা hash_ops *hash_ops);
পূর্ণাঙ্ক hashmap_ensure_replace(Hashmap **h, ধ্রুব সংরচনা hash_ops *hash_ops, ধ্রুব শূন্য *চাবি, শূন্য *মান);

পূর্ণাঙ্ক ordered_hashmap_ensure_put(OrderedHashmap **h, ধ্রুব সংরচনা hash_ops *hash_ops, ধ্রুব শূন্য *চাবি, শূন্য *মান);
পূর্ণাঙ্ক ordered_hashmap_ensure_replace(OrderedHashmap **h, ধ্রুব সংরচনা hash_ops *hash_ops, ধ্রুব শূন্য *চাবি, শূন্য *মান);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
স্থির অন্তঃস্থ IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        প্রত্যাগম _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
স্থির অন্তঃস্থ IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        প্রত্যাগম _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

পূর্ণাঙ্ক hashmap_put(Hashmap *h, ধ্রুব শূন্য *চাবি, শূন্য *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_hashmap_put(OrderedHashmap *h, ধ্রুব শূন্য *চাবি, শূন্য *মান) {
        প্রত্যাগম hashmap_put(PLAIN_HASHMAP(h), চাবি, মান);
}

পূর্ণাঙ্ক hashmap_put_strdup_full(Hashmap **h, ধ্রুব সংরচনা hash_ops *hash_ops, ধ্রুব বর্ণ *k, ধ্রুব বর্ণ *v);
স্থির অন্তঃস্থ পূর্ণাঙ্ক hashmap_put_strdup(Hashmap **h, ধ্রুব বর্ণ *k, ধ্রুব বর্ণ *v) {
        প্রত্যাগম hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

পূর্ণাঙ্ক hashmap_update(Hashmap *h, ধ্রুব শূন্য *চাবি, শূন্য *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_hashmap_update(OrderedHashmap *h, ধ্রুব শূন্য *চাবি, শূন্য *মান) {
        প্রত্যাগম hashmap_update(PLAIN_HASHMAP(h), চাবি, মান);
}

পূর্ণাঙ্ক hashmap_replace(Hashmap *h, ধ্রুব শূন্য *চাবি, শূন্য *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_hashmap_replace(OrderedHashmap *h, ধ্রুব শূন্য *চাবি, শূন্য *মান) {
        প্রত্যাগম hashmap_replace(PLAIN_HASHMAP(h), চাবি, মান);
}

শূন্য* _hashmap_get(HashmapBase *h, ধ্রুব শূন্য *চাবি);
স্থির অন্তঃস্থ শূন্য *hashmap_get(Hashmap *h, ধ্রুব শূন্য *চাবি) {
        প্রত্যাগম _hashmap_get(HASHMAP_BASE(h), চাবি);
}
স্থির অন্তঃস্থ শূন্য *ordered_hashmap_get(OrderedHashmap *h, ধ্রুব শূন্য *চাবি) {
        প্রত্যাগম _hashmap_get(HASHMAP_BASE(h), চাবি);
}

শূন্য* hashmap_get2(Hashmap *h, ধ্রুব শূন্য *চাবি, শূন্য **ret);
স্থির অন্তঃস্থ শূন্য *ordered_hashmap_get2(OrderedHashmap *h, ধ্রুব শূন্য *চাবি, শূন্য **ret) {
        প্রত্যাগম hashmap_get2(PLAIN_HASHMAP(h), চাবি, ret);
}

bool _hashmap_contains(HashmapBase *h, ধ্রুব শূন্য *চাবি);
স্থির অন্তঃস্থ bool hashmap_contains(Hashmap *h, ধ্রুব শূন্য *চাবি) {
        প্রত্যাগম _hashmap_contains(HASHMAP_BASE(h), চাবি);
}
স্থির অন্তঃস্থ bool ordered_hashmap_contains(OrderedHashmap *h, ধ্রুব শূন্য *চাবি) {
        প্রত্যাগম _hashmap_contains(HASHMAP_BASE(h), চাবি);
}

শূন্য* _hashmap_remove(HashmapBase *h, ধ্রুব শূন্য *চাবি);
স্থির অন্তঃস্থ শূন্য *hashmap_remove(Hashmap *h, ধ্রুব শূন্য *চাবি) {
        প্রত্যাগম _hashmap_remove(HASHMAP_BASE(h), চাবি);
}
স্থির অন্তঃস্থ শূন্য *ordered_hashmap_remove(OrderedHashmap *h, ধ্রুব শূন্য *চাবি) {
        প্রত্যাগম _hashmap_remove(HASHMAP_BASE(h), চাবি);
}

শূন্য* hashmap_remove2(Hashmap *h, ধ্রুব শূন্য *চাবি, শূন্য **ret);
স্থির অন্তঃস্থ শূন্য *ordered_hashmap_remove2(OrderedHashmap *h, ধ্রুব শূন্য *চাবি, শূন্য **ret) {
        প্রত্যাগম hashmap_remove2(PLAIN_HASHMAP(h), চাবি, ret);
}

শূন্য* hashmap_remove_মান(HashmapBase *h, ধ্রুব শূন্য *চাবি, শূন্য *মান);
স্থির অন্তঃস্থ শূন্য *hashmap_remove_মান_2(Hashmap *h, ধ্রুব শূন্য *চাবি, শূন্য *মান) {
        প্রত্যাগম hashmap_remove_মান(HASHMAP_BASE(h), চাবি, মান);
}

স্থির অন্তঃস্থ শূন্য* ordered_hashmap_remove_মান(OrderedHashmap *h, ধ্রুব শূন্য *চাবি, শূন্য *মান) {
        প্রত্যাগম hashmap_remove_মান_2(PLAIN_HASHMAP(h), চাবি, মান);
}

পূর্ণাঙ্ক hashmap_remove_and_put(Hashmap *h, ধ্রুব শূন্য *old_চাবি, ধ্রুব শূন্য *new_চাবি, শূন্য *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_hashmap_remove_and_put(OrderedHashmap *h, ধ্রুব শূন্য *old_চাবি, ধ্রুব শূন্য *new_চাবি, শূন্য *মান) {
        প্রত্যাগম hashmap_remove_and_put(PLAIN_HASHMAP(h), old_চাবি, new_চাবি, মান);
}

পূর্ণাঙ্ক hashmap_remove_and_replace(Hashmap *h, ধ্রুব শূন্য *old_চাবি, ধ্রুব শূন্য *new_চাবি, শূন্য *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_hashmap_remove_and_replace(OrderedHashmap *h, ধ্রুব শূন্য *old_চাবি, ধ্রুব শূন্য *new_চাবি, শূন্য *মান) {
        প্রত্যাগম hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_চাবি, new_চাবি, মান);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
পূর্ণাঙ্ক _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

পূর্ণাঙ্ক _hashmap_reserve(HashmapBase *h, unsigned entries_add);
স্থির অন্তঃস্থ পূর্ণাঙ্ক hashmap_reserve(Hashmap *h, unsigned entries_add) {
        প্রত্যাগম _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        প্রত্যাগম _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

পূর্ণাঙ্ক _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
স্থির অন্তঃস্থ পূর্ণাঙ্ক hashmap_move(Hashmap *h, Hashmap *other) {
        প্রত্যাগম _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        প্রত্যাগম _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

পূর্ণাঙ্ক _hashmap_move_one(HashmapBase *h, HashmapBase *other, ধ্রুব শূন্য *চাবি);
স্থির অন্তঃস্থ পূর্ণাঙ্ক hashmap_move_one(Hashmap *h, Hashmap *other, ধ্রুব শূন্য *চাবি) {
        প্রত্যাগম _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), চাবি);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ধ্রুব শূন্য *চাবি) {
        প্রত্যাগম _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), চাবি);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
স্থির অন্তঃস্থ unsigned hashmap_size(Hashmap *h) {
        প্রত্যাগম _hashmap_size(HASHMAP_BASE(h));
}
স্থির অন্তঃস্থ unsigned ordered_hashmap_size(OrderedHashmap *h) {
        প্রত্যাগম _hashmap_size(HASHMAP_BASE(h));
}

স্থির অন্তঃস্থ bool hashmap_isempty(Hashmap *h) {
        প্রত্যাগম hashmap_size(h) == 0;
}
স্থির অন্তঃস্থ bool ordered_hashmap_isempty(OrderedHashmap *h) {
        প্রত্যাগম ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
স্থির অন্তঃস্থ unsigned hashmap_buckets(Hashmap *h) {
        প্রত্যাগম _hashmap_buckets(HASHMAP_BASE(h));
}
স্থির অন্তঃস্থ unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        প্রত্যাগম _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, শূন্য **মান, ধ্রুব শূন্য **চাবি);
স্থির অন্তঃস্থ bool hashmap_iterate(Hashmap *h, Iterator *i, শূন্য **মান, ধ্রুব শূন্য **চাবি) {
        প্রত্যাগম _hashmap_iterate(HASHMAP_BASE(h), i, মান, চাবি);
}
স্থির অন্তঃস্থ bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, শূন্য **মান, ধ্রুব শূন্য **চাবি) {
        প্রত্যাগম _hashmap_iterate(HASHMAP_BASE(h), i, মান, চাবি);
}

শূন্য _hashmap_clear(HashmapBase *h);
স্থির অন্তঃস্থ শূন্য hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
স্থির অন্তঃস্থ শূন্য ordered_hashmap_clear(OrderedHashmap *h) {
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

শূন্য *hashmap_first_চাবি_and_মান(HashmapBase *h, bool remove, শূন্য **ret_চাবি);
স্থির অন্তঃস্থ শূন্য *hashmap_steal_first_চাবি_and_মান(Hashmap *h, শূন্য **ret) {
        প্রত্যাগম hashmap_first_চাবি_and_মান(HASHMAP_BASE(h), true, ret);
}
স্থির অন্তঃস্থ শূন্য *ordered_hashmap_steal_first_চাবি_and_মান(OrderedHashmap *h, শূন্য **ret) {
        প্রত্যাগম hashmap_first_চাবি_and_মান(HASHMAP_BASE(h), true, ret);
}
স্থির অন্তঃস্থ শূন্য *hashmap_first_চাবি_and_মান_2(Hashmap *h, শূন্য **ret) {
        প্রত্যাগম hashmap_first_চাবি_and_মান(HASHMAP_BASE(h), false, ret);
}
স্থির অন্তঃস্থ শূন্য *ordered_hashmap_first_চাবি_and_মান(OrderedHashmap *h, শূন্য **ret) {
        প্রত্যাগম hashmap_first_চাবি_and_মান(HASHMAP_BASE(h), false, ret);
}

স্থির অন্তঃস্থ শূন্য *hashmap_steal_first(Hashmap *h) {
        প্রত্যাগম hashmap_first_চাবি_and_মান(HASHMAP_BASE(h), true, রিক্ত);
}
স্থির অন্তঃস্থ শূন্য *ordered_hashmap_steal_first(OrderedHashmap *h) {
        প্রত্যাগম hashmap_first_চাবি_and_মান(HASHMAP_BASE(h), true, রিক্ত);
}
স্থির অন্তঃস্থ শূন্য *hashmap_first(Hashmap *h) {
        প্রত্যাগম hashmap_first_চাবি_and_মান(HASHMAP_BASE(h), false, রিক্ত);
}
স্থির অন্তঃস্থ শূন্য *ordered_hashmap_first(OrderedHashmap *h) {
        প্রত্যাগম hashmap_first_চাবি_and_মান(HASHMAP_BASE(h), false, রিক্ত);
}

স্থির অন্তঃস্থ শূন্য *hashmap_first_চাবি(HashmapBase *h, bool remove) {
        শূন্য *চাবি = রিক্ত;

        (শূন্য) hashmap_first_চাবি_and_মান(HASHMAP_BASE(h), remove, &চাবি);
        প্রত্যাগম চাবি;
}
স্থির অন্তঃস্থ শূন্য *hashmap_steal_first_চাবি(Hashmap *h) {
        প্রত্যাগম hashmap_first_চাবি(HASHMAP_BASE(h), true);
}
স্থির অন্তঃস্থ শূন্য *ordered_hashmap_steal_first_চাবি(OrderedHashmap *h) {
        প্রত্যাগম hashmap_first_চাবি(HASHMAP_BASE(h), true);
}
স্থির অন্তঃস্থ শূন্য *hashmap_first_চাবি_2(Hashmap *h) {
        প্রত্যাগম hashmap_first_চাবি(HASHMAP_BASE(h), false);
}
স্থির অন্তঃস্থ শূন্য *ordered_hashmap_first_চাবি(OrderedHashmap *h) {
        প্রত্যাগম hashmap_first_চাবি(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
শূন্য* ordered_hashmap_next(OrderedHashmap *h, ধ্রুব শূন্য *চাবি);

বর্ণ** _hashmap_get_strv(HashmapBase *h);
স্থির অন্তঃস্থ বর্ণ** hashmap_get_strv(Hashmap *h) {
        প্রত্যাগম _hashmap_get_strv(HASHMAP_BASE(h));
}
স্থির অন্তঃস্থ বর্ণ** ordered_hashmap_get_strv(OrderedHashmap *h) {
        প্রত্যাগম _hashmap_get_strv(HASHMAP_BASE(h));
}

পূর্ণাঙ্ক _hashmap_dump_sorted(HashmapBase *h, শূন্য ***ret, size_t *ret_n);
স্থির অন্তঃস্থ পূর্ণাঙ্ক hashmap_dump_sorted(Hashmap *h, শূন্য ***ret, size_t *ret_n) {
        প্রত্যাগম _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_hashmap_dump_sorted(OrderedHashmap *h, শূন্য ***ret, size_t *ret_n) {
        প্রত্যাগম _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক set_dump_sorted(Set *h, শূন্য ***ret, size_t *ret_n) {
        প্রত্যাগম _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

পূর্ণাঙ্ক hashmap_dump_চাবিগুলি_sorted(HashmapBase *h, শূন্য ***ret, size_t *ret_n);
স্থির অন্তঃস্থ পূর্ণাঙ্ক hashmap_dump_চাবিগুলি_sorted_2(Hashmap *h, শূন্য ***ret, size_t *ret_n) {
        প্রত্যাগম hashmap_dump_চাবিগুলি_sorted(HASHMAP_BASE(h), ret, ret_n);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_hashmap_dump_চাবিগুলি_sorted(OrderedHashmap *h, শূন্য ***ret, size_t *ret_n) {
        প্রত্যাগম hashmap_dump_চাবিগুলি_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        প্রতি (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (শূন্য**)&(e), রিক্ত); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        প্রতি (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (শূন্য**)&(e), রিক্ত); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        প্রতি (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (শূন্য**)&(e), রিক্ত); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define HASHMAP_BASE_FOREACH_চাবি(e, k, h, i) \
        প্রতি (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (শূন্য**)&(e), (ধ্রুব শূন্য**) &(k)); )
#define HASHMAP_BASE_FOREACH_চাবি_2(e, k, h) \
        HASHMAP_BASE_FOREACH_চাবি(e, k, h, UNIQ_T(i, UNIQ))

#define HASHMAP_FOREACH_চাবি(e, k, h, i) \
        প্রতি (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (শূন্য**)&(e), (ধ্রুব শূন্য**) &(k)); )
#define HASHMAP_FOREACH_চাবি_2(e, k, h) \
        HASHMAP_FOREACH_চাবি(e, k, h, UNIQ_T(i, UNIQ))

#define ORDERED_HASHMAP_FOREACH_চাবি(e, k, h, i) \
        প্রতি (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (শূন্য**)&(e), (ধ্রুব শূন্য**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_চাবি_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_চাবি(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

শূন্য hashmap_trim_pools(শূন্য);
