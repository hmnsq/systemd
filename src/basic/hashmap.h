/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "hash-funcs.h"
#ಅನ್ತರ್ಭಾವ "iterator.h"

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

#define HASH_ಕೀಲಿ_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ HashmapBase HashmapBase;

/* Specific hashmap/set types */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Hashmap Hashmap;               /* Maps keys to values */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Set Set;                       /* Stores just keys */

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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
                (ಶೂನ್ಯ)0)

#define PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (ಶೂನ್ಯ)0)

Hashmap* hashmap_new(ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hashmap* hashmap_free(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ*) _hashmap_free(HASHMAP_BASE(h));
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
ಪೂರ್ಣಾಙ್ಕ iterated_cache_get(IteratedCache *cache, ಧ್ರುವ ಶೂನ್ಯ ***res_ಕೀಲಿಗಳು, ಧ್ರುವ ಶೂನ್ಯ ***res_ಮೌಲ್ಯಗಳು, unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hashmap* hashmap_copy(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

ಪೂರ್ಣಾಙ್ಕ hashmap_ensure_allocated(Hashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops);
ಪೂರ್ಣಾಙ್ಕ hashmap_ensure_put(Hashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_ensure_allocated(OrderedHashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops);
ಪೂರ್ಣಾಙ್ಕ hashmap_ensure_replace(Hashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);

ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_ensure_put(OrderedHashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_ensure_replace(OrderedHashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

ಪೂರ್ಣಾಙ್ಕ hashmap_put(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_put(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ hashmap_put(PLAIN_HASHMAP(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ hashmap_put_strdup_full(Hashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ವರ್ಣ *k, ಧ್ರುವ ವರ್ಣ *v);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ hashmap_put_strdup(Hashmap **h, ಧ್ರುವ ವರ್ಣ *k, ಧ್ರುವ ವರ್ಣ *v) {
        ಪ್ರತ್ಯಾಗಮ hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

ಪೂರ್ಣಾಙ್ಕ hashmap_update(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_update(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ hashmap_update(PLAIN_HASHMAP(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ hashmap_replace(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_replace(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ hashmap_replace(PLAIN_HASHMAP(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಶೂನ್ಯ* _hashmap_get(HashmapBase *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hashmap_get(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_get(HASHMAP_BASE(h), ಕೀಲಿ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_hashmap_get(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_get(HASHMAP_BASE(h), ಕೀಲಿ);
}

ಶೂನ್ಯ* hashmap_get2(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_hashmap_get2(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret) {
        ಪ್ರತ್ಯಾಗಮ hashmap_get2(PLAIN_HASHMAP(h), ಕೀಲಿ, ret);
}

bool _hashmap_contains(HashmapBase *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool hashmap_contains(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_contains(HASHMAP_BASE(h), ಕೀಲಿ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ordered_hashmap_contains(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_contains(HASHMAP_BASE(h), ಕೀಲಿ);
}

ಶೂನ್ಯ* _hashmap_remove(HashmapBase *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hashmap_remove(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_remove(HASHMAP_BASE(h), ಕೀಲಿ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_hashmap_remove(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_remove(HASHMAP_BASE(h), ಕೀಲಿ);
}

ಶೂನ್ಯ* hashmap_remove2(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_hashmap_remove2(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret) {
        ಪ್ರತ್ಯಾಗಮ hashmap_remove2(PLAIN_HASHMAP(h), ಕೀಲಿ, ret);
}

ಶೂನ್ಯ* hashmap_remove_ಮೌಲ್ಯ(HashmapBase *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hashmap_remove_ಮೌಲ್ಯ_2(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ hashmap_remove_ಮೌಲ್ಯ(HASHMAP_BASE(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* ordered_hashmap_remove_ಮೌಲ್ಯ(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ hashmap_remove_ಮೌಲ್ಯ_2(PLAIN_HASHMAP(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ hashmap_remove_and_put(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *old_ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_remove_and_put(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *old_ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ hashmap_remove_and_put(PLAIN_HASHMAP(h), old_ಕೀಲಿ, new_ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ hashmap_remove_and_replace(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *old_ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_remove_and_replace(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *old_ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_ಕೀಲಿ, new_ಕೀಲಿ, ಮೌಲ್ಯ);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
ಪೂರ್ಣಾಙ್ಕ _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

ಪೂರ್ಣಾಙ್ಕ _hashmap_reserve(HashmapBase *h, unsigned entries_add);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ hashmap_reserve(Hashmap *h, unsigned entries_add) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

ಪೂರ್ಣಾಙ್ಕ _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ hashmap_move(Hashmap *h, Hashmap *other) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

ಪೂರ್ಣಾಙ್ಕ _hashmap_move_one(HashmapBase *h, HashmapBase *other, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ hashmap_move_one(Hashmap *h, Hashmap *other, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), ಕೀಲಿ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), ಕೀಲಿ);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned hashmap_size(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_size(HASHMAP_BASE(h));
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned ordered_hashmap_size(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_size(HASHMAP_BASE(h));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool hashmap_isempty(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ hashmap_size(h) == 0;
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ordered_hashmap_isempty(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned hashmap_buckets(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_buckets(HASHMAP_BASE(h));
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ, ಧ್ರುವ ಶೂನ್ಯ **ಕೀಲಿ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool hashmap_iterate(Hashmap *h, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ, ಧ್ರುವ ಶೂನ್ಯ **ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_iterate(HASHMAP_BASE(h), i, ಮೌಲ್ಯ, ಕೀಲಿ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ, ಧ್ರುವ ಶೂನ್ಯ **ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_iterate(HASHMAP_BASE(h), i, ಮೌಲ್ಯ, ಕೀಲಿ);
}

ಶೂನ್ಯ _hashmap_clear(HashmapBase *h);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ ordered_hashmap_clear(OrderedHashmap *h) {
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

ಶೂನ್ಯ *hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HashmapBase *h, bool remove, ಶೂನ್ಯ **ret_ಕೀಲಿ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hashmap_steal_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(Hashmap *h, ಶೂನ್ಯ **ret) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE(h), true, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_hashmap_steal_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(OrderedHashmap *h, ಶೂನ್ಯ **ret) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE(h), true, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ_2(Hashmap *h, ಶೂನ್ಯ **ret) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE(h), false, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(OrderedHashmap *h, ಶೂನ್ಯ **ret) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE(h), false, ret);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hashmap_steal_first(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE(h), true, ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_hashmap_steal_first(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE(h), true, ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hashmap_first(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE(h), false, ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_hashmap_first(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE(h), false, ರಿಕ್ತ);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hashmap_first_ಕೀಲಿ(HashmapBase *h, bool remove) {
        ಶೂನ್ಯ *ಕೀಲಿ = ರಿಕ್ತ;

        (ಶೂನ್ಯ) hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE(h), remove, &ಕೀಲಿ);
        ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hashmap_steal_first_ಕೀಲಿ(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ(HASHMAP_BASE(h), true);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_hashmap_steal_first_ಕೀಲಿ(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ(HASHMAP_BASE(h), true);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hashmap_first_ಕೀಲಿ_2(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ(HASHMAP_BASE(h), false);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_hashmap_first_ಕೀಲಿ(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
ಶೂನ್ಯ* ordered_hashmap_next(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ);

ವರ್ಣ** _hashmap_get_strv(HashmapBase *h);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ** hashmap_get_strv(Hashmap *h) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_get_strv(HASHMAP_BASE(h));
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ** ordered_hashmap_get_strv(OrderedHashmap *h) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_get_strv(HASHMAP_BASE(h));
}

ಪೂರ್ಣಾಙ್ಕ _hashmap_dump_sorted(HashmapBase *h, ಶೂನ್ಯ ***ret, size_t *ret_n);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ hashmap_dump_sorted(Hashmap *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_dump_sorted(OrderedHashmap *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ set_dump_sorted(Set *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

ಪೂರ್ಣಾಙ್ಕ hashmap_dump_ಕೀಲಿಗಳು_sorted(HashmapBase *h, ಶೂನ್ಯ ***ret, size_t *ret_n);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ hashmap_dump_ಕೀಲಿಗಳು_sorted_2(Hashmap *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        ಪ್ರತ್ಯಾಗಮ hashmap_dump_ಕೀಲಿಗಳು_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_dump_ಕೀಲಿಗಳು_sorted(OrderedHashmap *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        ಪ್ರತ್ಯಾಗಮ hashmap_dump_ಕೀಲಿಗಳು_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        ಪ್ರತಿ (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), ರಿಕ್ತ); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        ಪ್ರತಿ (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), ರಿಕ್ತ); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        ಪ್ರತಿ (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), ರಿಕ್ತ); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define HASHMAP_BASE_FOREACH_ಕೀಲಿ(e, k, h, i) \
        ಪ್ರತಿ (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), (ಧ್ರುವ ಶೂನ್ಯ**) &(k)); )
#define HASHMAP_BASE_FOREACH_ಕೀಲಿ_2(e, k, h) \
        HASHMAP_BASE_FOREACH_ಕೀಲಿ(e, k, h, UNIQ_T(i, UNIQ))

#define HASHMAP_FOREACH_ಕೀಲಿ(e, k, h, i) \
        ಪ್ರತಿ (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), (ಧ್ರುವ ಶೂನ್ಯ**) &(k)); )
#define HASHMAP_FOREACH_ಕೀಲಿ_2(e, k, h) \
        HASHMAP_FOREACH_ಕೀಲಿ(e, k, h, UNIQ_T(i, UNIQ))

#define ORDERED_HASHMAP_FOREACH_ಕೀಲಿ(e, k, h, i) \
        ಪ್ರತಿ (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), (ಧ್ರುವ ಶೂನ್ಯ**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_ಕೀಲಿ_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_ಕೀಲಿ(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

ಶೂನ್ಯ hashmap_trim_pools(ಶೂನ್ಯ);
