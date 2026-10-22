/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "hash-funcs.h"
#include "iterator.h"

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

#define HASH_KEY_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
typedef struct HashmapBase HashmapBase;

/* Specific hashmap/set types */
typedef struct Hashmap Hashmap;               /* Maps keys to values */
typedef struct OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
typedef struct Set Set;                       /* Stores just keys */

typedef struct IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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
                (void)0)

#define PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (void)0)

Hashmap* hashmap_new(const struct hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(const struct hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
static inline Hashmap* hashmap_free(Hashmap *h) {
        врати (void*) _hashmap_free(HASHMAP_BASE(h));
}
static inline OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        врати (void*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
цео iterated_cache_get(IteratedCache *cache, const void ***res_keys, const void ***res_values, unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
static inline Hashmap* hashmap_copy(Hashmap *h) {
        врати (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
static inline OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        врати (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

цео hashmap_ensure_allocated(Hashmap **h, const struct hash_ops *hash_ops);
цео hashmap_ensure_put(Hashmap **h, const struct hash_ops *hash_ops, const void *key, void *value);
цео ordered_hashmap_ensure_allocated(OrderedHashmap **h, const struct hash_ops *hash_ops);
цео hashmap_ensure_replace(Hashmap **h, const struct hash_ops *hash_ops, const void *key, void *value);

цео ordered_hashmap_ensure_put(OrderedHashmap **h, const struct hash_ops *hash_ops, const void *key, void *value);
цео ordered_hashmap_ensure_replace(OrderedHashmap **h, const struct hash_ops *hash_ops, const void *key, void *value);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
static inline IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        врати _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
static inline IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        врати _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

цео hashmap_put(Hashmap *h, const void *key, void *value);
static inline цео ordered_hashmap_put(OrderedHashmap *h, const void *key, void *value) {
        врати hashmap_put(PLAIN_HASHMAP(h), key, value);
}

цео hashmap_put_strdup_full(Hashmap **h, const struct hash_ops *hash_ops, const char *k, const char *v);
static inline цео hashmap_put_strdup(Hashmap **h, const char *k, const char *v) {
        врати hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

цео hashmap_update(Hashmap *h, const void *key, void *value);
static inline цео ordered_hashmap_update(OrderedHashmap *h, const void *key, void *value) {
        врати hashmap_update(PLAIN_HASHMAP(h), key, value);
}

цео hashmap_replace(Hashmap *h, const void *key, void *value);
static inline цео ordered_hashmap_replace(OrderedHashmap *h, const void *key, void *value) {
        врати hashmap_replace(PLAIN_HASHMAP(h), key, value);
}

void* _hashmap_get(HashmapBase *h, const void *key);
static inline void *hashmap_get(Hashmap *h, const void *key) {
        врати _hashmap_get(HASHMAP_BASE(h), key);
}
static inline void *ordered_hashmap_get(OrderedHashmap *h, const void *key) {
        врати _hashmap_get(HASHMAP_BASE(h), key);
}

void* hashmap_get2(Hashmap *h, const void *key, void **ret);
static inline void *ordered_hashmap_get2(OrderedHashmap *h, const void *key, void **ret) {
        врати hashmap_get2(PLAIN_HASHMAP(h), key, ret);
}

bool _hashmap_contains(HashmapBase *h, const void *key);
static inline bool hashmap_contains(Hashmap *h, const void *key) {
        врати _hashmap_contains(HASHMAP_BASE(h), key);
}
static inline bool ordered_hashmap_contains(OrderedHashmap *h, const void *key) {
        врати _hashmap_contains(HASHMAP_BASE(h), key);
}

void* _hashmap_remove(HashmapBase *h, const void *key);
static inline void *hashmap_remove(Hashmap *h, const void *key) {
        врати _hashmap_remove(HASHMAP_BASE(h), key);
}
static inline void *ordered_hashmap_remove(OrderedHashmap *h, const void *key) {
        врати _hashmap_remove(HASHMAP_BASE(h), key);
}

void* hashmap_remove2(Hashmap *h, const void *key, void **ret);
static inline void *ordered_hashmap_remove2(OrderedHashmap *h, const void *key, void **ret) {
        врати hashmap_remove2(PLAIN_HASHMAP(h), key, ret);
}

void* _hashmap_remove_value(HashmapBase *h, const void *key, void *value);
static inline void *hashmap_remove_value(Hashmap *h, const void *key, void *value) {
        врати _hashmap_remove_value(HASHMAP_BASE(h), key, value);
}

static inline void* ordered_hashmap_remove_value(OrderedHashmap *h, const void *key, void *value) {
        врати hashmap_remove_value(PLAIN_HASHMAP(h), key, value);
}

цео hashmap_remove_and_put(Hashmap *h, const void *old_key, const void *new_key, void *value);
static inline цео ordered_hashmap_remove_and_put(OrderedHashmap *h, const void *old_key, const void *new_key, void *value) {
        врати hashmap_remove_and_put(PLAIN_HASHMAP(h), old_key, new_key, value);
}

цео hashmap_remove_and_replace(Hashmap *h, const void *old_key, const void *new_key, void *value);
static inline цео ordered_hashmap_remove_and_replace(OrderedHashmap *h, const void *old_key, const void *new_key, void *value) {
        врати hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_key, new_key, value);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
цео _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

цео _hashmap_reserve(HashmapBase *h, unsigned entries_add);
static inline цео hashmap_reserve(Hashmap *h, unsigned entries_add) {
        врати _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
static inline цео ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        врати _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

цео _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
static inline цео hashmap_move(Hashmap *h, Hashmap *other) {
        врати _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
static inline цео ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        врати _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

цео _hashmap_move_one(HashmapBase *h, HashmapBase *other, const void *key);
static inline цео hashmap_move_one(Hashmap *h, Hashmap *other, const void *key) {
        врати _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), key);
}
static inline цео ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, const void *key) {
        врати _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), key);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
static inline unsigned hashmap_size(Hashmap *h) {
        врати _hashmap_size(HASHMAP_BASE(h));
}
static inline unsigned ordered_hashmap_size(OrderedHashmap *h) {
        врати _hashmap_size(HASHMAP_BASE(h));
}

static inline bool hashmap_isempty(Hashmap *h) {
        врати hashmap_size(h) == 0;
}
static inline bool ordered_hashmap_isempty(OrderedHashmap *h) {
        врати ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
static inline unsigned hashmap_buckets(Hashmap *h) {
        врати _hashmap_buckets(HASHMAP_BASE(h));
}
static inline unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        врати _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, void **value, const void **key);
static inline bool hashmap_iterate(Hashmap *h, Iterator *i, void **value, const void **key) {
        врати _hashmap_iterate(HASHMAP_BASE(h), i, value, key);
}
static inline bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, void **value, const void **key) {
        врати _hashmap_iterate(HASHMAP_BASE(h), i, value, key);
}

void _hashmap_clear(HashmapBase *h);
static inline void hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
static inline void ordered_hashmap_clear(OrderedHashmap *h) {
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

void *_hashmap_first_key_and_value(HashmapBase *h, bool remove, void **ret_key);
static inline void *hashmap_steal_first_key_and_value(Hashmap *h, void **ret) {
        врати _hashmap_first_key_and_value(HASHMAP_BASE(h), true, ret);
}
static inline void *ordered_hashmap_steal_first_key_and_value(OrderedHashmap *h, void **ret) {
        врати _hashmap_first_key_and_value(HASHMAP_BASE(h), true, ret);
}
static inline void *hashmap_first_key_and_value(Hashmap *h, void **ret) {
        врати _hashmap_first_key_and_value(HASHMAP_BASE(h), false, ret);
}
static inline void *ordered_hashmap_first_key_and_value(OrderedHashmap *h, void **ret) {
        врати _hashmap_first_key_and_value(HASHMAP_BASE(h), false, ret);
}

static inline void *hashmap_steal_first(Hashmap *h) {
        врати _hashmap_first_key_and_value(HASHMAP_BASE(h), true, NULL);
}
static inline void *ordered_hashmap_steal_first(OrderedHashmap *h) {
        врати _hashmap_first_key_and_value(HASHMAP_BASE(h), true, NULL);
}
static inline void *hashmap_first(Hashmap *h) {
        врати _hashmap_first_key_and_value(HASHMAP_BASE(h), false, NULL);
}
static inline void *ordered_hashmap_first(OrderedHashmap *h) {
        врати _hashmap_first_key_and_value(HASHMAP_BASE(h), false, NULL);
}

static inline void *_hashmap_first_key(HashmapBase *h, bool remove) {
        void *key = NULL;

        (void) _hashmap_first_key_and_value(HASHMAP_BASE(h), remove, &key);
        врати key;
}
static inline void *hashmap_steal_first_key(Hashmap *h) {
        врати _hashmap_first_key(HASHMAP_BASE(h), true);
}
static inline void *ordered_hashmap_steal_first_key(OrderedHashmap *h) {
        врати _hashmap_first_key(HASHMAP_BASE(h), true);
}
static inline void *hashmap_first_key(Hashmap *h) {
        врати _hashmap_first_key(HASHMAP_BASE(h), false);
}
static inline void *ordered_hashmap_first_key(OrderedHashmap *h) {
        врати _hashmap_first_key(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
void* ordered_hashmap_next(OrderedHashmap *h, const void *key);

char** _hashmap_get_strv(HashmapBase *h);
static inline char** hashmap_get_strv(Hashmap *h) {
        врати _hashmap_get_strv(HASHMAP_BASE(h));
}
static inline char** ordered_hashmap_get_strv(OrderedHashmap *h) {
        врати _hashmap_get_strv(HASHMAP_BASE(h));
}

цео _hashmap_dump_sorted(HashmapBase *h, void ***ret, size_t *ret_n);
static inline цео hashmap_dump_sorted(Hashmap *h, void ***ret, size_t *ret_n) {
        врати _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
static inline цео ordered_hashmap_dump_sorted(OrderedHashmap *h, void ***ret, size_t *ret_n) {
        врати _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
static inline цео set_dump_sorted(Set *h, void ***ret, size_t *ret_n) {
        врати _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

цео _hashmap_dump_keys_sorted(HashmapBase *h, void ***ret, size_t *ret_n);
static inline цео hashmap_dump_keys_sorted(Hashmap *h, void ***ret, size_t *ret_n) {
        врати _hashmap_dump_keys_sorted(HASHMAP_BASE(h), ret, ret_n);
}
static inline цео ordered_hashmap_dump_keys_sorted(OrderedHashmap *h, void ***ret, size_t *ret_n) {
        врати _hashmap_dump_keys_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        за (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (void**)&(e), NULL); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        за (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (void**)&(e), NULL); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        за (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (void**)&(e), NULL); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_BASE_FOREACH_KEY(e, k, h, i) \
        за (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (void**)&(e), (const void**) &(k)); )
#define HASHMAP_BASE_FOREACH_KEY(e, k, h) \
        _HASHMAP_BASE_FOREACH_KEY(e, k, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH_KEY(e, k, h, i) \
        за (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (void**)&(e), (const void**) &(k)); )
#define HASHMAP_FOREACH_KEY(e, k, h) \
        _HASHMAP_FOREACH_KEY(e, k, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH_KEY(e, k, h, i) \
        за (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (void**)&(e), (const void**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_KEY(e, k, h) \
        _ORDERED_HASHMAP_FOREACH_KEY(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

void hashmap_trim_pools(void);
