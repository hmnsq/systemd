/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "hash-funcs.h"
#अन्तर्भाव "iterator.h"

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

#define HASH_किल्ली_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
प्रकारव्याख्या संरचना HashmapBase HashmapBase;

/* Specific hashmap/set types */
प्रकारव्याख्या संरचना Hashmap Hashmap;               /* Maps keys to values */
प्रकारव्याख्या संरचना OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
प्रकारव्याख्या संरचना Set Set;                       /* Stores just keys */

प्रकारव्याख्या संरचना IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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
                (शून्य)0)

#define PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (शून्य)0)

Hashmap* hashmap_new(स्थिरांक संरचना hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(स्थिरांक संरचना hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
स्थिर ओळीत Hashmap* hashmap_free(Hashmap *h) {
        परत (शून्य*) _hashmap_free(HASHMAP_BASE(h));
}
स्थिर ओळीत OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        परत (शून्य*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
पूर्णांक iterated_cache_get(IteratedCache *cache, स्थिरांक शून्य ***res_किल्ल्या, स्थिरांक शून्य ***res_मूल्ये, अचिन्हित *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
स्थिर ओळीत Hashmap* hashmap_copy(Hashmap *h) {
        परत (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
स्थिर ओळीत OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        परत (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

पूर्णांक hashmap_ensure_allocated(Hashmap **h, स्थिरांक संरचना hash_ops *hash_ops);
पूर्णांक hashmap_ensure_put(Hashmap **h, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक शून्य *किल्ली, शून्य *मूल्य);
पूर्णांक ordered_hashmap_ensure_allocated(OrderedHashmap **h, स्थिरांक संरचना hash_ops *hash_ops);
पूर्णांक hashmap_ensure_replace(Hashmap **h, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक शून्य *किल्ली, शून्य *मूल्य);

पूर्णांक ordered_hashmap_ensure_put(OrderedHashmap **h, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक शून्य *किल्ली, शून्य *मूल्य);
पूर्णांक ordered_hashmap_ensure_replace(OrderedHashmap **h, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक शून्य *किल्ली, शून्य *मूल्य);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
स्थिर ओळीत IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        परत _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
स्थिर ओळीत IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        परत _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

पूर्णांक hashmap_put(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य);
स्थिर ओळीत पूर्णांक ordered_hashmap_put(OrderedHashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        परत hashmap_put(PLAIN_HASHMAP(h), किल्ली, मूल्य);
}

पूर्णांक hashmap_put_strdup_full(Hashmap **h, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक अक्षर *k, स्थिरांक अक्षर *v);
स्थिर ओळीत पूर्णांक hashmap_put_strdup(Hashmap **h, स्थिरांक अक्षर *k, स्थिरांक अक्षर *v) {
        परत hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

पूर्णांक hashmap_update(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य);
स्थिर ओळीत पूर्णांक ordered_hashmap_update(OrderedHashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        परत hashmap_update(PLAIN_HASHMAP(h), किल्ली, मूल्य);
}

पूर्णांक hashmap_replace(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य);
स्थिर ओळीत पूर्णांक ordered_hashmap_replace(OrderedHashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        परत hashmap_replace(PLAIN_HASHMAP(h), किल्ली, मूल्य);
}

शून्य* _hashmap_get(HashmapBase *h, स्थिरांक शून्य *किल्ली);
स्थिर ओळीत शून्य *hashmap_get(Hashmap *h, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_get(HASHMAP_BASE(h), किल्ली);
}
स्थिर ओळीत शून्य *ordered_hashmap_get(OrderedHashmap *h, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_get(HASHMAP_BASE(h), किल्ली);
}

शून्य* hashmap_get2(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य **ret);
स्थिर ओळीत शून्य *ordered_hashmap_get2(OrderedHashmap *h, स्थिरांक शून्य *किल्ली, शून्य **ret) {
        परत hashmap_get2(PLAIN_HASHMAP(h), किल्ली, ret);
}

bool _hashmap_contains(HashmapBase *h, स्थिरांक शून्य *किल्ली);
स्थिर ओळीत bool hashmap_contains(Hashmap *h, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_contains(HASHMAP_BASE(h), किल्ली);
}
स्थिर ओळीत bool ordered_hashmap_contains(OrderedHashmap *h, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_contains(HASHMAP_BASE(h), किल्ली);
}

शून्य* _hashmap_remove(HashmapBase *h, स्थिरांक शून्य *किल्ली);
स्थिर ओळीत शून्य *hashmap_remove(Hashmap *h, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_remove(HASHMAP_BASE(h), किल्ली);
}
स्थिर ओळीत शून्य *ordered_hashmap_remove(OrderedHashmap *h, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_remove(HASHMAP_BASE(h), किल्ली);
}

शून्य* hashmap_remove2(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य **ret);
स्थिर ओळीत शून्य *ordered_hashmap_remove2(OrderedHashmap *h, स्थिरांक शून्य *किल्ली, शून्य **ret) {
        परत hashmap_remove2(PLAIN_HASHMAP(h), किल्ली, ret);
}

शून्य* hashmap_remove_मूल्य(HashmapBase *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य);
स्थिर ओळीत शून्य *hashmap_remove_मूल्य_2(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        परत hashmap_remove_मूल्य(HASHMAP_BASE(h), किल्ली, मूल्य);
}

स्थिर ओळीत शून्य* ordered_hashmap_remove_मूल्य(OrderedHashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        परत hashmap_remove_मूल्य_2(PLAIN_HASHMAP(h), किल्ली, मूल्य);
}

पूर्णांक hashmap_remove_and_put(Hashmap *h, स्थिरांक शून्य *old_किल्ली, स्थिरांक शून्य *new_किल्ली, शून्य *मूल्य);
स्थिर ओळीत पूर्णांक ordered_hashmap_remove_and_put(OrderedHashmap *h, स्थिरांक शून्य *old_किल्ली, स्थिरांक शून्य *new_किल्ली, शून्य *मूल्य) {
        परत hashmap_remove_and_put(PLAIN_HASHMAP(h), old_किल्ली, new_किल्ली, मूल्य);
}

पूर्णांक hashmap_remove_and_replace(Hashmap *h, स्थिरांक शून्य *old_किल्ली, स्थिरांक शून्य *new_किल्ली, शून्य *मूल्य);
स्थिर ओळीत पूर्णांक ordered_hashmap_remove_and_replace(OrderedHashmap *h, स्थिरांक शून्य *old_किल्ली, स्थिरांक शून्य *new_किल्ली, शून्य *मूल्य) {
        परत hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_किल्ली, new_किल्ली, मूल्य);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
पूर्णांक _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

पूर्णांक _hashmap_reserve(HashmapBase *h, अचिन्हित entries_add);
स्थिर ओळीत पूर्णांक hashmap_reserve(Hashmap *h, अचिन्हित entries_add) {
        परत _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
स्थिर ओळीत पूर्णांक ordered_hashmap_reserve(OrderedHashmap *h, अचिन्हित entries_add) {
        परत _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

पूर्णांक _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
स्थिर ओळीत पूर्णांक hashmap_move(Hashmap *h, Hashmap *other) {
        परत _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
स्थिर ओळीत पूर्णांक ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        परत _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

पूर्णांक _hashmap_move_one(HashmapBase *h, HashmapBase *other, स्थिरांक शून्य *किल्ली);
स्थिर ओळीत पूर्णांक hashmap_move_one(Hashmap *h, Hashmap *other, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), किल्ली);
}
स्थिर ओळीत पूर्णांक ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), किल्ली);
}

अचिन्हित _hashmap_size(HashmapBase *h) _pure_;
स्थिर ओळीत अचिन्हित hashmap_size(Hashmap *h) {
        परत _hashmap_size(HASHMAP_BASE(h));
}
स्थिर ओळीत अचिन्हित ordered_hashmap_size(OrderedHashmap *h) {
        परत _hashmap_size(HASHMAP_BASE(h));
}

स्थिर ओळीत bool hashmap_isempty(Hashmap *h) {
        परत hashmap_size(h) == 0;
}
स्थिर ओळीत bool ordered_hashmap_isempty(OrderedHashmap *h) {
        परत ordered_hashmap_size(h) == 0;
}

अचिन्हित _hashmap_buckets(HashmapBase *h) _pure_;
स्थिर ओळीत अचिन्हित hashmap_buckets(Hashmap *h) {
        परत _hashmap_buckets(HASHMAP_BASE(h));
}
स्थिर ओळीत अचिन्हित ordered_hashmap_buckets(OrderedHashmap *h) {
        परत _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, शून्य **मूल्य, स्थिरांक शून्य **किल्ली);
स्थिर ओळीत bool hashmap_iterate(Hashmap *h, Iterator *i, शून्य **मूल्य, स्थिरांक शून्य **किल्ली) {
        परत _hashmap_iterate(HASHMAP_BASE(h), i, मूल्य, किल्ली);
}
स्थिर ओळीत bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, शून्य **मूल्य, स्थिरांक शून्य **किल्ली) {
        परत _hashmap_iterate(HASHMAP_BASE(h), i, मूल्य, किल्ली);
}

शून्य _hashmap_clear(HashmapBase *h);
स्थिर ओळीत शून्य hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
स्थिर ओळीत शून्य ordered_hashmap_clear(OrderedHashmap *h) {
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

शून्य *hashmap_first_किल्ली_and_मूल्य(HashmapBase *h, bool हटाओ, शून्य **ret_किल्ली);
स्थिर ओळीत शून्य *hashmap_steal_first_किल्ली_and_मूल्य(Hashmap *h, शून्य **ret) {
        परत hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE(h), true, ret);
}
स्थिर ओळीत शून्य *ordered_hashmap_steal_first_किल्ली_and_मूल्य(OrderedHashmap *h, शून्य **ret) {
        परत hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE(h), true, ret);
}
स्थिर ओळीत शून्य *hashmap_first_किल्ली_and_मूल्य_2(Hashmap *h, शून्य **ret) {
        परत hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE(h), false, ret);
}
स्थिर ओळीत शून्य *ordered_hashmap_first_किल्ली_and_मूल्य(OrderedHashmap *h, शून्य **ret) {
        परत hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE(h), false, ret);
}

स्थिर ओळीत शून्य *hashmap_steal_first(Hashmap *h) {
        परत hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE(h), true, रिक्त);
}
स्थिर ओळीत शून्य *ordered_hashmap_steal_first(OrderedHashmap *h) {
        परत hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE(h), true, रिक्त);
}
स्थिर ओळीत शून्य *hashmap_first(Hashmap *h) {
        परत hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE(h), false, रिक्त);
}
स्थिर ओळीत शून्य *ordered_hashmap_first(OrderedHashmap *h) {
        परत hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE(h), false, रिक्त);
}

स्थिर ओळीत शून्य *hashmap_first_किल्ली(HashmapBase *h, bool हटाओ) {
        शून्य *किल्ली = रिक्त;

        (शून्य) hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE(h), हटाओ, &किल्ली);
        परत किल्ली;
}
स्थिर ओळीत शून्य *hashmap_steal_first_किल्ली(Hashmap *h) {
        परत hashmap_first_किल्ली(HASHMAP_BASE(h), true);
}
स्थिर ओळीत शून्य *ordered_hashmap_steal_first_किल्ली(OrderedHashmap *h) {
        परत hashmap_first_किल्ली(HASHMAP_BASE(h), true);
}
स्थिर ओळीत शून्य *hashmap_first_किल्ली_2(Hashmap *h) {
        परत hashmap_first_किल्ली(HASHMAP_BASE(h), false);
}
स्थिर ओळीत शून्य *ordered_hashmap_first_किल्ली(OrderedHashmap *h) {
        परत hashmap_first_किल्ली(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
शून्य* ordered_hashmap_next(OrderedHashmap *h, स्थिरांक शून्य *किल्ली);

अक्षर** _hashmap_get_strv(HashmapBase *h);
स्थिर ओळीत अक्षर** hashmap_get_strv(Hashmap *h) {
        परत _hashmap_get_strv(HASHMAP_BASE(h));
}
स्थिर ओळीत अक्षर** ordered_hashmap_get_strv(OrderedHashmap *h) {
        परत _hashmap_get_strv(HASHMAP_BASE(h));
}

पूर्णांक _hashmap_dump_sorted(HashmapBase *h, शून्य ***ret, माप_प्रकार *ret_n);
स्थिर ओळीत पूर्णांक hashmap_dump_sorted(Hashmap *h, शून्य ***ret, माप_प्रकार *ret_n) {
        परत _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
स्थिर ओळीत पूर्णांक ordered_hashmap_dump_sorted(OrderedHashmap *h, शून्य ***ret, माप_प्रकार *ret_n) {
        परत _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
स्थिर ओळीत पूर्णांक set_dump_sorted(Set *h, शून्य ***ret, माप_प्रकार *ret_n) {
        परत _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

पूर्णांक hashmap_dump_किल्ल्या_sorted(HashmapBase *h, शून्य ***ret, माप_प्रकार *ret_n);
स्थिर ओळीत पूर्णांक hashmap_dump_किल्ल्या_sorted_2(Hashmap *h, शून्य ***ret, माप_प्रकार *ret_n) {
        परत hashmap_dump_किल्ल्या_sorted(HASHMAP_BASE(h), ret, ret_n);
}
स्थिर ओळीत पूर्णांक ordered_hashmap_dump_किल्ल्या_sorted(OrderedHashmap *h, शून्य ***ret, माप_प्रकार *ret_n) {
        परत hashmap_dump_किल्ल्या_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        साठी (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (शून्य**)&(e), रिक्त); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        साठी (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (शून्य**)&(e), रिक्त); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        साठी (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (शून्य**)&(e), रिक्त); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define HASHMAP_BASE_FOREACH_किल्ली(e, k, h, i) \
        साठी (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (शून्य**)&(e), (स्थिरांक शून्य**) &(k)); )
#define HASHMAP_BASE_FOREACH_किल्ली_2(e, k, h) \
        HASHMAP_BASE_FOREACH_किल्ली(e, k, h, UNIQ_T(i, UNIQ))

#define HASHMAP_FOREACH_किल्ली(e, k, h, i) \
        साठी (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (शून्य**)&(e), (स्थिरांक शून्य**) &(k)); )
#define HASHMAP_FOREACH_किल्ली_2(e, k, h) \
        HASHMAP_FOREACH_किल्ली(e, k, h, UNIQ_T(i, UNIQ))

#define ORDERED_HASHMAP_FOREACH_किल्ली(e, k, h, i) \
        साठी (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (शून्य**)&(e), (स्थिरांक शून्य**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_किल्ली_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_किल्ली(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

शून्य hashmap_trim_pools(शून्य);
