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
typedef sa_nracanaa HashmapBase HashmapBase;

/* Specific hashmap/set types */
typedef sa_nracanaa Hashmap Hashmap;               /* Maps keys to values */
typedef sa_nracanaa OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
typedef sa_nracanaa Set Set;                       /* Stores just keys */

typedef sa_nracanaa IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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

Hashmap* hashmap_new(const sa_nracanaa hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(const sa_nracanaa hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
static anta_hstha Hashmap* hashmap_free(Hashmap *h) {
        ပြန်ပေး (void*) _hashmap_free(HASHMAP_BASE(h));
}
static anta_hstha OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        ပြန်ပေး (void*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
ကိန်းပြည့် iterated_cache_get(IteratedCache *cache, const void ***res_keys, const void ***res_values, unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
static anta_hstha Hashmap* hashmap_copy(Hashmap *h) {
        ပြန်ပေး (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
static anta_hstha OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        ပြန်ပေး (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

ကိန်းပြည့် hashmap_ensure_allocated(Hashmap **h, const sa_nracanaa hash_ops *hash_ops);
ကိန်းပြည့် hashmap_ensure_put(Hashmap **h, const sa_nracanaa hash_ops *hash_ops, const void *key, void *value);
ကိန်းပြည့် ordered_hashmap_ensure_allocated(OrderedHashmap **h, const sa_nracanaa hash_ops *hash_ops);
ကိန်းပြည့် hashmap_ensure_replace(Hashmap **h, const sa_nracanaa hash_ops *hash_ops, const void *key, void *value);

ကိန်းပြည့် ordered_hashmap_ensure_put(OrderedHashmap **h, const sa_nracanaa hash_ops *hash_ops, const void *key, void *value);
ကိန်းပြည့် ordered_hashmap_ensure_replace(OrderedHashmap **h, const sa_nracanaa hash_ops *hash_ops, const void *key, void *value);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
static anta_hstha IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        ပြန်ပေး _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
static anta_hstha IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        ပြန်ပေး _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

ကိန်းပြည့် hashmap_put(Hashmap *h, const void *key, void *value);
static anta_hstha ကိန်းပြည့် ordered_hashmap_put(OrderedHashmap *h, const void *key, void *value) {
        ပြန်ပေး hashmap_put(PLAIN_HASHMAP(h), key, value);
}

ကိန်းပြည့် hashmap_put_strdup_full(Hashmap **h, const sa_nracanaa hash_ops *hash_ops, const var_na *k, const var_na *v);
static anta_hstha ကိန်းပြည့် hashmap_put_strdup(Hashmap **h, const var_na *k, const var_na *v) {
        ပြန်ပေး hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

ကိန်းပြည့် hashmap_update(Hashmap *h, const void *key, void *value);
static anta_hstha ကိန်းပြည့် ordered_hashmap_update(OrderedHashmap *h, const void *key, void *value) {
        ပြန်ပေး hashmap_update(PLAIN_HASHMAP(h), key, value);
}

ကိန်းပြည့် hashmap_replace(Hashmap *h, const void *key, void *value);
static anta_hstha ကိန်းပြည့် ordered_hashmap_replace(OrderedHashmap *h, const void *key, void *value) {
        ပြန်ပေး hashmap_replace(PLAIN_HASHMAP(h), key, value);
}

void* _hashmap_get(HashmapBase *h, const void *key);
static anta_hstha void *hashmap_get(Hashmap *h, const void *key) {
        ပြန်ပေး _hashmap_get(HASHMAP_BASE(h), key);
}
static anta_hstha void *ordered_hashmap_get(OrderedHashmap *h, const void *key) {
        ပြန်ပေး _hashmap_get(HASHMAP_BASE(h), key);
}

void* hashmap_get2(Hashmap *h, const void *key, void **ret);
static anta_hstha void *ordered_hashmap_get2(OrderedHashmap *h, const void *key, void **ret) {
        ပြန်ပေး hashmap_get2(PLAIN_HASHMAP(h), key, ret);
}

bool _hashmap_contains(HashmapBase *h, const void *key);
static anta_hstha bool hashmap_contains(Hashmap *h, const void *key) {
        ပြန်ပေး _hashmap_contains(HASHMAP_BASE(h), key);
}
static anta_hstha bool ordered_hashmap_contains(OrderedHashmap *h, const void *key) {
        ပြန်ပေး _hashmap_contains(HASHMAP_BASE(h), key);
}

void* _hashmap_remove(HashmapBase *h, const void *key);
static anta_hstha void *hashmap_remove(Hashmap *h, const void *key) {
        ပြန်ပေး _hashmap_remove(HASHMAP_BASE(h), key);
}
static anta_hstha void *ordered_hashmap_remove(OrderedHashmap *h, const void *key) {
        ပြန်ပေး _hashmap_remove(HASHMAP_BASE(h), key);
}

void* hashmap_remove2(Hashmap *h, const void *key, void **ret);
static anta_hstha void *ordered_hashmap_remove2(OrderedHashmap *h, const void *key, void **ret) {
        ပြန်ပေး hashmap_remove2(PLAIN_HASHMAP(h), key, ret);
}

void* _hashmap_remove_value(HashmapBase *h, const void *key, void *value);
static anta_hstha void *hashmap_remove_value(Hashmap *h, const void *key, void *value) {
        ပြန်ပေး _hashmap_remove_value(HASHMAP_BASE(h), key, value);
}

static anta_hstha void* ordered_hashmap_remove_value(OrderedHashmap *h, const void *key, void *value) {
        ပြန်ပေး hashmap_remove_value(PLAIN_HASHMAP(h), key, value);
}

ကိန်းပြည့် hashmap_remove_and_put(Hashmap *h, const void *old_key, const void *new_key, void *value);
static anta_hstha ကိန်းပြည့် ordered_hashmap_remove_and_put(OrderedHashmap *h, const void *old_key, const void *new_key, void *value) {
        ပြန်ပေး hashmap_remove_and_put(PLAIN_HASHMAP(h), old_key, new_key, value);
}

ကိန်းပြည့် hashmap_remove_and_replace(Hashmap *h, const void *old_key, const void *new_key, void *value);
static anta_hstha ကိန်းပြည့် ordered_hashmap_remove_and_replace(OrderedHashmap *h, const void *old_key, const void *new_key, void *value) {
        ပြန်ပေး hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_key, new_key, value);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
ကိန်းပြည့် _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

ကိန်းပြည့် _hashmap_reserve(HashmapBase *h, unsigned entries_add);
static anta_hstha ကိန်းပြည့် hashmap_reserve(Hashmap *h, unsigned entries_add) {
        ပြန်ပေး _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
static anta_hstha ကိန်းပြည့် ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        ပြန်ပေး _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

ကိန်းပြည့် _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
static anta_hstha ကိန်းပြည့် hashmap_move(Hashmap *h, Hashmap *other) {
        ပြန်ပေး _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
static anta_hstha ကိန်းပြည့် ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        ပြန်ပေး _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

ကိန်းပြည့် _hashmap_move_one(HashmapBase *h, HashmapBase *other, const void *key);
static anta_hstha ကိန်းပြည့် hashmap_move_one(Hashmap *h, Hashmap *other, const void *key) {
        ပြန်ပေး _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), key);
}
static anta_hstha ကိန်းပြည့် ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, const void *key) {
        ပြန်ပေး _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), key);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
static anta_hstha unsigned hashmap_size(Hashmap *h) {
        ပြန်ပေး _hashmap_size(HASHMAP_BASE(h));
}
static anta_hstha unsigned ordered_hashmap_size(OrderedHashmap *h) {
        ပြန်ပေး _hashmap_size(HASHMAP_BASE(h));
}

static anta_hstha bool hashmap_isempty(Hashmap *h) {
        ပြန်ပေး hashmap_size(h) == 0;
}
static anta_hstha bool ordered_hashmap_isempty(OrderedHashmap *h) {
        ပြန်ပေး ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
static anta_hstha unsigned hashmap_buckets(Hashmap *h) {
        ပြန်ပေး _hashmap_buckets(HASHMAP_BASE(h));
}
static anta_hstha unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        ပြန်ပေး _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, void **value, const void **key);
static anta_hstha bool hashmap_iterate(Hashmap *h, Iterator *i, void **value, const void **key) {
        ပြန်ပေး _hashmap_iterate(HASHMAP_BASE(h), i, value, key);
}
static anta_hstha bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, void **value, const void **key) {
        ပြန်ပေး _hashmap_iterate(HASHMAP_BASE(h), i, value, key);
}

void _hashmap_clear(HashmapBase *h);
static anta_hstha void hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
static anta_hstha void ordered_hashmap_clear(OrderedHashmap *h) {
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
static anta_hstha void *hashmap_steal_first_key_and_value(Hashmap *h, void **ret) {
        ပြန်ပေး _hashmap_first_key_and_value(HASHMAP_BASE(h), true, ret);
}
static anta_hstha void *ordered_hashmap_steal_first_key_and_value(OrderedHashmap *h, void **ret) {
        ပြန်ပေး _hashmap_first_key_and_value(HASHMAP_BASE(h), true, ret);
}
static anta_hstha void *hashmap_first_key_and_value(Hashmap *h, void **ret) {
        ပြန်ပေး _hashmap_first_key_and_value(HASHMAP_BASE(h), false, ret);
}
static anta_hstha void *ordered_hashmap_first_key_and_value(OrderedHashmap *h, void **ret) {
        ပြန်ပေး _hashmap_first_key_and_value(HASHMAP_BASE(h), false, ret);
}

static anta_hstha void *hashmap_steal_first(Hashmap *h) {
        ပြန်ပေး _hashmap_first_key_and_value(HASHMAP_BASE(h), true, NULL);
}
static anta_hstha void *ordered_hashmap_steal_first(OrderedHashmap *h) {
        ပြန်ပေး _hashmap_first_key_and_value(HASHMAP_BASE(h), true, NULL);
}
static anta_hstha void *hashmap_first(Hashmap *h) {
        ပြန်ပေး _hashmap_first_key_and_value(HASHMAP_BASE(h), false, NULL);
}
static anta_hstha void *ordered_hashmap_first(OrderedHashmap *h) {
        ပြန်ပေး _hashmap_first_key_and_value(HASHMAP_BASE(h), false, NULL);
}

static anta_hstha void *_hashmap_first_key(HashmapBase *h, bool remove) {
        void *key = NULL;

        (void) _hashmap_first_key_and_value(HASHMAP_BASE(h), remove, &key);
        ပြန်ပေး key;
}
static anta_hstha void *hashmap_steal_first_key(Hashmap *h) {
        ပြန်ပေး _hashmap_first_key(HASHMAP_BASE(h), true);
}
static anta_hstha void *ordered_hashmap_steal_first_key(OrderedHashmap *h) {
        ပြန်ပေး _hashmap_first_key(HASHMAP_BASE(h), true);
}
static anta_hstha void *hashmap_first_key(Hashmap *h) {
        ပြန်ပေး _hashmap_first_key(HASHMAP_BASE(h), false);
}
static anta_hstha void *ordered_hashmap_first_key(OrderedHashmap *h) {
        ပြန်ပေး _hashmap_first_key(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
void* ordered_hashmap_next(OrderedHashmap *h, const void *key);

var_na** _hashmap_get_strv(HashmapBase *h);
static anta_hstha var_na** hashmap_get_strv(Hashmap *h) {
        ပြန်ပေး _hashmap_get_strv(HASHMAP_BASE(h));
}
static anta_hstha var_na** ordered_hashmap_get_strv(OrderedHashmap *h) {
        ပြန်ပေး _hashmap_get_strv(HASHMAP_BASE(h));
}

ကိန်းပြည့် _hashmap_dump_sorted(HashmapBase *h, void ***ret, size_t *ret_n);
static anta_hstha ကိန်းပြည့် hashmap_dump_sorted(Hashmap *h, void ***ret, size_t *ret_n) {
        ပြန်ပေး _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
static anta_hstha ကိန်းပြည့် ordered_hashmap_dump_sorted(OrderedHashmap *h, void ***ret, size_t *ret_n) {
        ပြန်ပေး _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
static anta_hstha ကိန်းပြည့် set_dump_sorted(Set *h, void ***ret, size_t *ret_n) {
        ပြန်ပေး _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

ကိန်းပြည့် _hashmap_dump_keys_sorted(HashmapBase *h, void ***ret, size_t *ret_n);
static anta_hstha ကိန်းပြည့် hashmap_dump_keys_sorted(Hashmap *h, void ***ret, size_t *ret_n) {
        ပြန်ပေး _hashmap_dump_keys_sorted(HASHMAP_BASE(h), ret, ret_n);
}
static anta_hstha ကိန်းပြည့် ordered_hashmap_dump_keys_sorted(OrderedHashmap *h, void ***ret, size_t *ret_n) {
        ပြန်ပေး _hashmap_dump_keys_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        အတွက် (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (void**)&(e), NULL); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        အတွက် (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (void**)&(e), NULL); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        အတွက် (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (void**)&(e), NULL); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_BASE_FOREACH_KEY(e, k, h, i) \
        အတွက် (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (void**)&(e), (const void**) &(k)); )
#define HASHMAP_BASE_FOREACH_KEY(e, k, h) \
        _HASHMAP_BASE_FOREACH_KEY(e, k, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH_KEY(e, k, h, i) \
        အတွက် (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (void**)&(e), (const void**) &(k)); )
#define HASHMAP_FOREACH_KEY(e, k, h) \
        _HASHMAP_FOREACH_KEY(e, k, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH_KEY(e, k, h, i) \
        အတွက် (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (void**)&(e), (const void**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_KEY(e, k, h) \
        _ORDERED_HASHMAP_FOREACH_KEY(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

void hashmap_trim_pools(void);
