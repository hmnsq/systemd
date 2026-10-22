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

#define HASH_कुञ्चिका_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
प्रकारनिर्देश संरचना HashmapBase HashmapBase;

/* Specific hashmap/set types */
प्रकारनिर्देश संरचना Hashmap Hashmap;               /* Maps keys to values */
प्रकारनिर्देश संरचना OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
प्रकारनिर्देश संरचना Set Set;                       /* Stores just keys */

प्रकारनिर्देश संरचना IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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

Hashmap* hashmap_new(ध्रुव संरचना hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ध्रुव संरचना hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
स्थिर अन्तःस्थ Hashmap* hashmap_free(Hashmap *h) {
        पटिनिवत्त (शून्य*) _hashmap_free(HASHMAP_BASE(h));
}
स्थिर अन्तःस्थ OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        पटिनिवत्त (शून्य*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
पूर्णाङ्क iterated_cache_get(IteratedCache *cache, ध्रुव शून्य ***res_कुञ्चिकाः, ध्रुव शून्य ***res_मानानि, unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
स्थिर अन्तःस्थ Hashmap* hashmap_copy(Hashmap *h) {
        पटिनिवत्त (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
स्थिर अन्तःस्थ OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        पटिनिवत्त (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

पूर्णाङ्क hashmap_ensure_allocated(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops);
पूर्णाङ्क hashmap_ensure_put(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्);
पूर्णाङ्क ordered_hashmap_ensure_allocated(OrderedHashmap **h, ध्रुव संरचना hash_ops *hash_ops);
पूर्णाङ्क hashmap_ensure_replace(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्);

पूर्णाङ्क ordered_hashmap_ensure_put(OrderedHashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्);
पूर्णाङ्क ordered_hashmap_ensure_replace(OrderedHashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
स्थिर अन्तःस्थ IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        पटिनिवत्त _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
स्थिर अन्तःस्थ IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        पटिनिवत्त _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

पूर्णाङ्क hashmap_put(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क ordered_hashmap_put(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पटिनिवत्त hashmap_put(PLAIN_HASHMAP(h), कुञ्चिका, मानम्);
}

पूर्णाङ्क hashmap_put_strdup_full(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *k, ध्रुव वर्ण *v);
स्थिर अन्तःस्थ पूर्णाङ्क hashmap_put_strdup(Hashmap **h, ध्रुव वर्ण *k, ध्रुव वर्ण *v) {
        पटिनिवत्त hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

पूर्णाङ्क hashmap_update(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क ordered_hashmap_update(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पटिनिवत्त hashmap_update(PLAIN_HASHMAP(h), कुञ्चिका, मानम्);
}

पूर्णाङ्क hashmap_replace(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क ordered_hashmap_replace(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पटिनिवत्त hashmap_replace(PLAIN_HASHMAP(h), कुञ्चिका, मानम्);
}

शून्य* _hashmap_get(HashmapBase *h, ध्रुव शून्य *कुञ्चिका);
स्थिर अन्तःस्थ शून्य *hashmap_get(Hashmap *h, ध्रुव शून्य *कुञ्चिका) {
        पटिनिवत्त _hashmap_get(HASHMAP_BASE(h), कुञ्चिका);
}
स्थिर अन्तःस्थ शून्य *ordered_hashmap_get(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका) {
        पटिनिवत्त _hashmap_get(HASHMAP_BASE(h), कुञ्चिका);
}

शून्य* hashmap_get2(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य **ret);
स्थिर अन्तःस्थ शून्य *ordered_hashmap_get2(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य **ret) {
        पटिनिवत्त hashmap_get2(PLAIN_HASHMAP(h), कुञ्चिका, ret);
}

bool _hashmap_contains(HashmapBase *h, ध्रुव शून्य *कुञ्चिका);
स्थिर अन्तःस्थ bool hashmap_contains(Hashmap *h, ध्रुव शून्य *कुञ्चिका) {
        पटिनिवत्त _hashmap_contains(HASHMAP_BASE(h), कुञ्चिका);
}
स्थिर अन्तःस्थ bool ordered_hashmap_contains(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका) {
        पटिनिवत्त _hashmap_contains(HASHMAP_BASE(h), कुञ्चिका);
}

शून्य* _hashmap_remove(HashmapBase *h, ध्रुव शून्य *कुञ्चिका);
स्थिर अन्तःस्थ शून्य *hashmap_remove(Hashmap *h, ध्रुव शून्य *कुञ्चिका) {
        पटिनिवत्त _hashmap_remove(HASHMAP_BASE(h), कुञ्चिका);
}
स्थिर अन्तःस्थ शून्य *ordered_hashmap_remove(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका) {
        पटिनिवत्त _hashmap_remove(HASHMAP_BASE(h), कुञ्चिका);
}

शून्य* hashmap_remove2(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य **ret);
स्थिर अन्तःस्थ शून्य *ordered_hashmap_remove2(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य **ret) {
        पटिनिवत्त hashmap_remove2(PLAIN_HASHMAP(h), कुञ्चिका, ret);
}

शून्य* hashmap_remove_मानम्(HashmapBase *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्);
स्थिर अन्तःस्थ शून्य *hashmap_remove_मानम्_2(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पटिनिवत्त hashmap_remove_मानम्(HASHMAP_BASE(h), कुञ्चिका, मानम्);
}

स्थिर अन्तःस्थ शून्य* ordered_hashmap_remove_मानम्(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पटिनिवत्त hashmap_remove_मानम्_2(PLAIN_HASHMAP(h), कुञ्चिका, मानम्);
}

पूर्णाङ्क hashmap_remove_and_put(Hashmap *h, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका, शून्य *मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क ordered_hashmap_remove_and_put(OrderedHashmap *h, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका, शून्य *मानम्) {
        पटिनिवत्त hashmap_remove_and_put(PLAIN_HASHMAP(h), old_कुञ्चिका, new_कुञ्चिका, मानम्);
}

पूर्णाङ्क hashmap_remove_and_replace(Hashmap *h, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका, शून्य *मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क ordered_hashmap_remove_and_replace(OrderedHashmap *h, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका, शून्य *मानम्) {
        पटिनिवत्त hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_कुञ्चिका, new_कुञ्चिका, मानम्);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
पूर्णाङ्क _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

पूर्णाङ्क _hashmap_reserve(HashmapBase *h, unsigned entries_add);
स्थिर अन्तःस्थ पूर्णाङ्क hashmap_reserve(Hashmap *h, unsigned entries_add) {
        पटिनिवत्त _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
स्थिर अन्तःस्थ पूर्णाङ्क ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        पटिनिवत्त _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

पूर्णाङ्क _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
स्थिर अन्तःस्थ पूर्णाङ्क hashmap_move(Hashmap *h, Hashmap *other) {
        पटिनिवत्त _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
स्थिर अन्तःस्थ पूर्णाङ्क ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        पटिनिवत्त _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

पूर्णाङ्क _hashmap_move_one(HashmapBase *h, HashmapBase *other, ध्रुव शून्य *कुञ्चिका);
स्थिर अन्तःस्थ पूर्णाङ्क hashmap_move_one(Hashmap *h, Hashmap *other, ध्रुव शून्य *कुञ्चिका) {
        पटिनिवत्त _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), कुञ्चिका);
}
स्थिर अन्तःस्थ पूर्णाङ्क ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ध्रुव शून्य *कुञ्चिका) {
        पटिनिवत्त _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), कुञ्चिका);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
स्थिर अन्तःस्थ unsigned hashmap_size(Hashmap *h) {
        पटिनिवत्त _hashmap_size(HASHMAP_BASE(h));
}
स्थिर अन्तःस्थ unsigned ordered_hashmap_size(OrderedHashmap *h) {
        पटिनिवत्त _hashmap_size(HASHMAP_BASE(h));
}

स्थिर अन्तःस्थ bool hashmap_isempty(Hashmap *h) {
        पटिनिवत्त hashmap_size(h) == 0;
}
स्थिर अन्तःस्थ bool ordered_hashmap_isempty(OrderedHashmap *h) {
        पटिनिवत्त ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
स्थिर अन्तःस्थ unsigned hashmap_buckets(Hashmap *h) {
        पटिनिवत्त _hashmap_buckets(HASHMAP_BASE(h));
}
स्थिर अन्तःस्थ unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        पटिनिवत्त _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, शून्य **मानम्, ध्रुव शून्य **कुञ्चिका);
स्थिर अन्तःस्थ bool hashmap_iterate(Hashmap *h, Iterator *i, शून्य **मानम्, ध्रुव शून्य **कुञ्चिका) {
        पटिनिवत्त _hashmap_iterate(HASHMAP_BASE(h), i, मानम्, कुञ्चिका);
}
स्थिर अन्तःस्थ bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, शून्य **मानम्, ध्रुव शून्य **कुञ्चिका) {
        पटिनिवत्त _hashmap_iterate(HASHMAP_BASE(h), i, मानम्, कुञ्चिका);
}

शून्य _hashmap_clear(HashmapBase *h);
स्थिर अन्तःस्थ शून्य hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
स्थिर अन्तःस्थ शून्य ordered_hashmap_clear(OrderedHashmap *h) {
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

शून्य *hashmap_first_कुञ्चिका_and_मानम्(HashmapBase *h, bool remove, शून्य **ret_कुञ्चिका);
स्थिर अन्तःस्थ शून्य *hashmap_steal_first_कुञ्चिका_and_मानम्(Hashmap *h, शून्य **ret) {
        पटिनिवत्त hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE(h), true, ret);
}
स्थिर अन्तःस्थ शून्य *ordered_hashmap_steal_first_कुञ्चिका_and_मानम्(OrderedHashmap *h, शून्य **ret) {
        पटिनिवत्त hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE(h), true, ret);
}
स्थिर अन्तःस्थ शून्य *hashmap_first_कुञ्चिका_and_मानम्_2(Hashmap *h, शून्य **ret) {
        पटिनिवत्त hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE(h), false, ret);
}
स्थिर अन्तःस्थ शून्य *ordered_hashmap_first_कुञ्चिका_and_मानम्(OrderedHashmap *h, शून्य **ret) {
        पटिनिवत्त hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE(h), false, ret);
}

स्थिर अन्तःस्थ शून्य *hashmap_steal_first(Hashmap *h) {
        पटिनिवत्त hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE(h), true, रिक्त);
}
स्थिर अन्तःस्थ शून्य *ordered_hashmap_steal_first(OrderedHashmap *h) {
        पटिनिवत्त hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE(h), true, रिक्त);
}
स्थिर अन्तःस्थ शून्य *hashmap_first(Hashmap *h) {
        पटिनिवत्त hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE(h), false, रिक्त);
}
स्थिर अन्तःस्थ शून्य *ordered_hashmap_first(OrderedHashmap *h) {
        पटिनिवत्त hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE(h), false, रिक्त);
}

स्थिर अन्तःस्थ शून्य *hashmap_first_कुञ्चिका(HashmapBase *h, bool remove) {
        शून्य *कुञ्चिका = रिक्त;

        (शून्य) hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE(h), remove, &कुञ्चिका);
        पटिनिवत्त कुञ्चिका;
}
स्थिर अन्तःस्थ शून्य *hashmap_steal_first_कुञ्चिका(Hashmap *h) {
        पटिनिवत्त hashmap_first_कुञ्चिका(HASHMAP_BASE(h), true);
}
स्थिर अन्तःस्थ शून्य *ordered_hashmap_steal_first_कुञ्चिका(OrderedHashmap *h) {
        पटिनिवत्त hashmap_first_कुञ्चिका(HASHMAP_BASE(h), true);
}
स्थिर अन्तःस्थ शून्य *hashmap_first_कुञ्चिका_2(Hashmap *h) {
        पटिनिवत्त hashmap_first_कुञ्चिका(HASHMAP_BASE(h), false);
}
स्थिर अन्तःस्थ शून्य *ordered_hashmap_first_कुञ्चिका(OrderedHashmap *h) {
        पटिनिवत्त hashmap_first_कुञ्चिका(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
शून्य* ordered_hashmap_next(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका);

वर्ण** _hashmap_get_strv(HashmapBase *h);
स्थिर अन्तःस्थ वर्ण** hashmap_get_strv(Hashmap *h) {
        पटिनिवत्त _hashmap_get_strv(HASHMAP_BASE(h));
}
स्थिर अन्तःस्थ वर्ण** ordered_hashmap_get_strv(OrderedHashmap *h) {
        पटिनिवत्त _hashmap_get_strv(HASHMAP_BASE(h));
}

पूर्णाङ्क _hashmap_dump_sorted(HashmapBase *h, शून्य ***ret, size_t *ret_n);
स्थिर अन्तःस्थ पूर्णाङ्क hashmap_dump_sorted(Hashmap *h, शून्य ***ret, size_t *ret_n) {
        पटिनिवत्त _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
स्थिर अन्तःस्थ पूर्णाङ्क ordered_hashmap_dump_sorted(OrderedHashmap *h, शून्य ***ret, size_t *ret_n) {
        पटिनिवत्त _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
स्थिर अन्तःस्थ पूर्णाङ्क set_dump_sorted(Set *h, शून्य ***ret, size_t *ret_n) {
        पटिनिवत्त _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

पूर्णाङ्क hashmap_dump_कुञ्चिकाः_sorted(HashmapBase *h, शून्य ***ret, size_t *ret_n);
स्थिर अन्तःस्थ पूर्णाङ्क hashmap_dump_कुञ्चिकाः_sorted_2(Hashmap *h, शून्य ***ret, size_t *ret_n) {
        पटिनिवत्त hashmap_dump_कुञ्चिकाः_sorted(HASHMAP_BASE(h), ret, ret_n);
}
स्थिर अन्तःस्थ पूर्णाङ्क ordered_hashmap_dump_कुञ्चिकाः_sorted(OrderedHashmap *h, शून्य ***ret, size_t *ret_n) {
        पटिनिवत्त hashmap_dump_कुञ्चिकाः_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        अत्थाय (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (शून्य**)&(e), रिक्त); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        अत्थाय (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (शून्य**)&(e), रिक्त); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        अत्थाय (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (शून्य**)&(e), रिक्त); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define HASHMAP_BASE_FOREACH_कुञ्चिका(e, k, h, i) \
        अत्थाय (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (शून्य**)&(e), (ध्रुव शून्य**) &(k)); )
#define HASHMAP_BASE_FOREACH_कुञ्चिका_2(e, k, h) \
        HASHMAP_BASE_FOREACH_कुञ्चिका(e, k, h, UNIQ_T(i, UNIQ))

#define HASHMAP_FOREACH_कुञ्चिका(e, k, h, i) \
        अत्थाय (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (शून्य**)&(e), (ध्रुव शून्य**) &(k)); )
#define HASHMAP_FOREACH_कुञ्चिका_2(e, k, h) \
        HASHMAP_FOREACH_कुञ्चिका(e, k, h, UNIQ_T(i, UNIQ))

#define ORDERED_HASHMAP_FOREACH_कुञ्चिका(e, k, h, i) \
        अत्थाय (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (शून्य**)&(e), (ध्रुव शून्य**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_कुञ्चिका_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_कुञ्चिका(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

शून्य hashmap_trim_pools(शून्य);
