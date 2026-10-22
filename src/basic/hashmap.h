/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "hash-funcs.h"
#அந்தர்பாவ "iterator.h"

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

#define HASH_சாவி_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
வகைவரை கட்டமைப்பு HashmapBase HashmapBase;

/* Specific hashmap/set types */
வகைவரை கட்டமைப்பு Hashmap Hashmap;               /* Maps keys to values */
வகைவரை கட்டமைப்பு OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
வகைவரை கட்டமைப்பு Set Set;                       /* Stores just keys */

வகைவரை கட்டமைப்பு IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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
                (வெறுமை)0)

#define PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (வெறுமை)0)

Hashmap* hashmap_new(மாறிலி கட்டமைப்பு hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(மாறிலி கட்டமைப்பு hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
நிலையான உள்வரி Hashmap* hashmap_free(Hashmap *h) {
        திரும்பு (வெறுமை*) _hashmap_free(HASHMAP_BASE(h));
}
நிலையான உள்வரி OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        திரும்பு (வெறுமை*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
முழு iterated_cache_get(IteratedCache *cache, மாறிலி வெறுமை ***res_சாவிகள், மாறிலி வெறுமை ***res_மதிப்புகள், unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
நிலையான உள்வரி Hashmap* hashmap_copy(Hashmap *h) {
        திரும்பு (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
நிலையான உள்வரி OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        திரும்பு (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

முழு hashmap_ensure_allocated(Hashmap **h, மாறிலி கட்டமைப்பு hash_ops *hash_ops);
முழு hashmap_ensure_put(Hashmap **h, மாறிலி கட்டமைப்பு hash_ops *hash_ops, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு);
முழு ordered_hashmap_ensure_allocated(OrderedHashmap **h, மாறிலி கட்டமைப்பு hash_ops *hash_ops);
முழு hashmap_ensure_replace(Hashmap **h, மாறிலி கட்டமைப்பு hash_ops *hash_ops, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு);

முழு ordered_hashmap_ensure_put(OrderedHashmap **h, மாறிலி கட்டமைப்பு hash_ops *hash_ops, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு);
முழு ordered_hashmap_ensure_replace(OrderedHashmap **h, மாறிலி கட்டமைப்பு hash_ops *hash_ops, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
நிலையான உள்வரி IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        திரும்பு _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
நிலையான உள்வரி IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        திரும்பு _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

முழு hashmap_put(Hashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு);
நிலையான உள்வரி முழு ordered_hashmap_put(OrderedHashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு) {
        திரும்பு hashmap_put(PLAIN_HASHMAP(h), சாவி, மதிப்பு);
}

முழு hashmap_put_strdup_full(Hashmap **h, மாறிலி கட்டமைப்பு hash_ops *hash_ops, மாறிலி எழுத்து *k, மாறிலி எழுத்து *v);
நிலையான உள்வரி முழு hashmap_put_strdup(Hashmap **h, மாறிலி எழுத்து *k, மாறிலி எழுத்து *v) {
        திரும்பு hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

முழு hashmap_update(Hashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு);
நிலையான உள்வரி முழு ordered_hashmap_update(OrderedHashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு) {
        திரும்பு hashmap_update(PLAIN_HASHMAP(h), சாவி, மதிப்பு);
}

முழு hashmap_replace(Hashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு);
நிலையான உள்வரி முழு ordered_hashmap_replace(OrderedHashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு) {
        திரும்பு hashmap_replace(PLAIN_HASHMAP(h), சாவி, மதிப்பு);
}

வெறுமை* _hashmap_get(HashmapBase *h, மாறிலி வெறுமை *சாவி);
நிலையான உள்வரி வெறுமை *hashmap_get(Hashmap *h, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_get(HASHMAP_BASE(h), சாவி);
}
நிலையான உள்வரி வெறுமை *ordered_hashmap_get(OrderedHashmap *h, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_get(HASHMAP_BASE(h), சாவி);
}

வெறுமை* hashmap_get2(Hashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை **ret);
நிலையான உள்வரி வெறுமை *ordered_hashmap_get2(OrderedHashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை **ret) {
        திரும்பு hashmap_get2(PLAIN_HASHMAP(h), சாவி, ret);
}

bool _hashmap_contains(HashmapBase *h, மாறிலி வெறுமை *சாவி);
நிலையான உள்வரி bool hashmap_contains(Hashmap *h, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_contains(HASHMAP_BASE(h), சாவி);
}
நிலையான உள்வரி bool ordered_hashmap_contains(OrderedHashmap *h, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_contains(HASHMAP_BASE(h), சாவி);
}

வெறுமை* _hashmap_remove(HashmapBase *h, மாறிலி வெறுமை *சாவி);
நிலையான உள்வரி வெறுமை *hashmap_remove(Hashmap *h, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_remove(HASHMAP_BASE(h), சாவி);
}
நிலையான உள்வரி வெறுமை *ordered_hashmap_remove(OrderedHashmap *h, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_remove(HASHMAP_BASE(h), சாவி);
}

வெறுமை* hashmap_remove2(Hashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை **ret);
நிலையான உள்வரி வெறுமை *ordered_hashmap_remove2(OrderedHashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை **ret) {
        திரும்பு hashmap_remove2(PLAIN_HASHMAP(h), சாவி, ret);
}

வெறுமை* hashmap_remove_மதிப்பு(HashmapBase *h, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு);
நிலையான உள்வரி வெறுமை *hashmap_remove_மதிப்பு_2(Hashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு) {
        திரும்பு hashmap_remove_மதிப்பு(HASHMAP_BASE(h), சாவி, மதிப்பு);
}

நிலையான உள்வரி வெறுமை* ordered_hashmap_remove_மதிப்பு(OrderedHashmap *h, மாறிலி வெறுமை *சாவி, வெறுமை *மதிப்பு) {
        திரும்பு hashmap_remove_மதிப்பு_2(PLAIN_HASHMAP(h), சாவி, மதிப்பு);
}

முழு hashmap_remove_and_put(Hashmap *h, மாறிலி வெறுமை *old_சாவி, மாறிலி வெறுமை *new_சாவி, வெறுமை *மதிப்பு);
நிலையான உள்வரி முழு ordered_hashmap_remove_and_put(OrderedHashmap *h, மாறிலி வெறுமை *old_சாவி, மாறிலி வெறுமை *new_சாவி, வெறுமை *மதிப்பு) {
        திரும்பு hashmap_remove_and_put(PLAIN_HASHMAP(h), old_சாவி, new_சாவி, மதிப்பு);
}

முழு hashmap_remove_and_replace(Hashmap *h, மாறிலி வெறுமை *old_சாவி, மாறிலி வெறுமை *new_சாவி, வெறுமை *மதிப்பு);
நிலையான உள்வரி முழு ordered_hashmap_remove_and_replace(OrderedHashmap *h, மாறிலி வெறுமை *old_சாவி, மாறிலி வெறுமை *new_சாவி, வெறுமை *மதிப்பு) {
        திரும்பு hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_சாவி, new_சாவி, மதிப்பு);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
முழு _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

முழு _hashmap_reserve(HashmapBase *h, unsigned entries_add);
நிலையான உள்வரி முழு hashmap_reserve(Hashmap *h, unsigned entries_add) {
        திரும்பு _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
நிலையான உள்வரி முழு ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        திரும்பு _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

முழு _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
நிலையான உள்வரி முழு hashmap_move(Hashmap *h, Hashmap *other) {
        திரும்பு _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
நிலையான உள்வரி முழு ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        திரும்பு _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

முழு _hashmap_move_one(HashmapBase *h, HashmapBase *other, மாறிலி வெறுமை *சாவி);
நிலையான உள்வரி முழு hashmap_move_one(Hashmap *h, Hashmap *other, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), சாவி);
}
நிலையான உள்வரி முழு ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), சாவி);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
நிலையான உள்வரி unsigned hashmap_size(Hashmap *h) {
        திரும்பு _hashmap_size(HASHMAP_BASE(h));
}
நிலையான உள்வரி unsigned ordered_hashmap_size(OrderedHashmap *h) {
        திரும்பு _hashmap_size(HASHMAP_BASE(h));
}

நிலையான உள்வரி bool hashmap_isempty(Hashmap *h) {
        திரும்பு hashmap_size(h) == 0;
}
நிலையான உள்வரி bool ordered_hashmap_isempty(OrderedHashmap *h) {
        திரும்பு ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
நிலையான உள்வரி unsigned hashmap_buckets(Hashmap *h) {
        திரும்பு _hashmap_buckets(HASHMAP_BASE(h));
}
நிலையான உள்வரி unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        திரும்பு _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, வெறுமை **மதிப்பு, மாறிலி வெறுமை **சாவி);
நிலையான உள்வரி bool hashmap_iterate(Hashmap *h, Iterator *i, வெறுமை **மதிப்பு, மாறிலி வெறுமை **சாவி) {
        திரும்பு _hashmap_iterate(HASHMAP_BASE(h), i, மதிப்பு, சாவி);
}
நிலையான உள்வரி bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, வெறுமை **மதிப்பு, மாறிலி வெறுமை **சாவி) {
        திரும்பு _hashmap_iterate(HASHMAP_BASE(h), i, மதிப்பு, சாவி);
}

வெறுமை _hashmap_clear(HashmapBase *h);
நிலையான உள்வரி வெறுமை hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
நிலையான உள்வரி வெறுமை ordered_hashmap_clear(OrderedHashmap *h) {
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

வெறுமை *hashmap_first_சாவி_and_மதிப்பு(HashmapBase *h, bool remove, வெறுமை **ret_சாவி);
நிலையான உள்வரி வெறுமை *hashmap_steal_first_சாவி_and_மதிப்பு(Hashmap *h, வெறுமை **ret) {
        திரும்பு hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE(h), true, ret);
}
நிலையான உள்வரி வெறுமை *ordered_hashmap_steal_first_சாவி_and_மதிப்பு(OrderedHashmap *h, வெறுமை **ret) {
        திரும்பு hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE(h), true, ret);
}
நிலையான உள்வரி வெறுமை *hashmap_first_சாவி_and_மதிப்பு_2(Hashmap *h, வெறுமை **ret) {
        திரும்பு hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE(h), false, ret);
}
நிலையான உள்வரி வெறுமை *ordered_hashmap_first_சாவி_and_மதிப்பு(OrderedHashmap *h, வெறுமை **ret) {
        திரும்பு hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE(h), false, ret);
}

நிலையான உள்வரி வெறுமை *hashmap_steal_first(Hashmap *h) {
        திரும்பு hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE(h), true, வெற்று);
}
நிலையான உள்வரி வெறுமை *ordered_hashmap_steal_first(OrderedHashmap *h) {
        திரும்பு hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE(h), true, வெற்று);
}
நிலையான உள்வரி வெறுமை *hashmap_first(Hashmap *h) {
        திரும்பு hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE(h), false, வெற்று);
}
நிலையான உள்வரி வெறுமை *ordered_hashmap_first(OrderedHashmap *h) {
        திரும்பு hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE(h), false, வெற்று);
}

நிலையான உள்வரி வெறுமை *hashmap_first_சாவி(HashmapBase *h, bool remove) {
        வெறுமை *சாவி = வெற்று;

        (வெறுமை) hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE(h), remove, &சாவி);
        திரும்பு சாவி;
}
நிலையான உள்வரி வெறுமை *hashmap_steal_first_சாவி(Hashmap *h) {
        திரும்பு hashmap_first_சாவி(HASHMAP_BASE(h), true);
}
நிலையான உள்வரி வெறுமை *ordered_hashmap_steal_first_சாவி(OrderedHashmap *h) {
        திரும்பு hashmap_first_சாவி(HASHMAP_BASE(h), true);
}
நிலையான உள்வரி வெறுமை *hashmap_first_சாவி_2(Hashmap *h) {
        திரும்பு hashmap_first_சாவி(HASHMAP_BASE(h), false);
}
நிலையான உள்வரி வெறுமை *ordered_hashmap_first_சாவி(OrderedHashmap *h) {
        திரும்பு hashmap_first_சாவி(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
வெறுமை* ordered_hashmap_next(OrderedHashmap *h, மாறிலி வெறுமை *சாவி);

எழுத்து** _hashmap_get_strv(HashmapBase *h);
நிலையான உள்வரி எழுத்து** hashmap_get_strv(Hashmap *h) {
        திரும்பு _hashmap_get_strv(HASHMAP_BASE(h));
}
நிலையான உள்வரி எழுத்து** ordered_hashmap_get_strv(OrderedHashmap *h) {
        திரும்பு _hashmap_get_strv(HASHMAP_BASE(h));
}

முழு _hashmap_dump_sorted(HashmapBase *h, வெறுமை ***ret, size_t *ret_n);
நிலையான உள்வரி முழு hashmap_dump_sorted(Hashmap *h, வெறுமை ***ret, size_t *ret_n) {
        திரும்பு _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
நிலையான உள்வரி முழு ordered_hashmap_dump_sorted(OrderedHashmap *h, வெறுமை ***ret, size_t *ret_n) {
        திரும்பு _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
நிலையான உள்வரி முழு set_dump_sorted(Set *h, வெறுமை ***ret, size_t *ret_n) {
        திரும்பு _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

முழு hashmap_dump_சாவிகள்_sorted(HashmapBase *h, வெறுமை ***ret, size_t *ret_n);
நிலையான உள்வரி முழு hashmap_dump_சாவிகள்_sorted_2(Hashmap *h, வெறுமை ***ret, size_t *ret_n) {
        திரும்பு hashmap_dump_சாவிகள்_sorted(HASHMAP_BASE(h), ret, ret_n);
}
நிலையான உள்வரி முழு ordered_hashmap_dump_சாவிகள்_sorted(OrderedHashmap *h, வெறுமை ***ret, size_t *ret_n) {
        திரும்பு hashmap_dump_சாவிகள்_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        க்கு (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (வெறுமை**)&(e), வெற்று); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        க்கு (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (வெறுமை**)&(e), வெற்று); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        க்கு (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (வெறுமை**)&(e), வெற்று); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define HASHMAP_BASE_FOREACH_சாவி(e, k, h, i) \
        க்கு (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (வெறுமை**)&(e), (மாறிலி வெறுமை**) &(k)); )
#define HASHMAP_BASE_FOREACH_சாவி_2(e, k, h) \
        HASHMAP_BASE_FOREACH_சாவி(e, k, h, UNIQ_T(i, UNIQ))

#define HASHMAP_FOREACH_சாவி(e, k, h, i) \
        க்கு (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (வெறுமை**)&(e), (மாறிலி வெறுமை**) &(k)); )
#define HASHMAP_FOREACH_சாவி_2(e, k, h) \
        HASHMAP_FOREACH_சாவி(e, k, h, UNIQ_T(i, UNIQ))

#define ORDERED_HASHMAP_FOREACH_சாவி(e, k, h, i) \
        க்கு (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (வெறுமை**)&(e), (மாறிலி வெறுமை**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_சாவி_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_சாவி(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

வெறுமை hashmap_trim_pools(வெறுமை);
