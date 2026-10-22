/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "hash-funcs.h"
#അന്തര്ഭാവ "iterator.h"

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

#define HASH_താക്കോൽ_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
തരനിർവചനം ഘടന HashmapBase HashmapBase;

/* Specific hashmap/set types */
തരനിർവചനം ഘടന Hashmap Hashmap;               /* Maps keys to values */
തരനിർവചനം ഘടന OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
തരനിർവചനം ഘടന Set Set;                       /* Stores just keys */

തരനിർവചനം ഘടന IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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
                (ശൂന്യം)0)

#define PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (ശൂന്യം)0)

Hashmap* hashmap_new(സ്ഥിരം ഘടന hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(സ്ഥിരം ഘടന hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
സ്ഥിതം വരിയിൽ Hashmap* hashmap_free(Hashmap *h) {
        തിരികെ (ശൂന്യം*) _hashmap_free(HASHMAP_BASE(h));
}
സ്ഥിതം വരിയിൽ OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        തിരികെ (ശൂന്യം*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
പൂർണ്ണസംഖ്യ iterated_cache_get(IteratedCache *cache, സ്ഥിരം ശൂന്യം ***res_താക്കോലുകൾ, സ്ഥിരം ശൂന്യം ***res_മൂല്യങ്ങൾ, unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
സ്ഥിതം വരിയിൽ Hashmap* hashmap_copy(Hashmap *h) {
        തിരികെ (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
സ്ഥിതം വരിയിൽ OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        തിരികെ (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

പൂർണ്ണസംഖ്യ hashmap_ensure_allocated(Hashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops);
പൂർണ്ണസംഖ്യ hashmap_ensure_put(Hashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം);
പൂർണ്ണസംഖ്യ ordered_hashmap_ensure_allocated(OrderedHashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops);
പൂർണ്ണസംഖ്യ hashmap_ensure_replace(Hashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം);

പൂർണ്ണസംഖ്യ ordered_hashmap_ensure_put(OrderedHashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം);
പൂർണ്ണസംഖ്യ ordered_hashmap_ensure_replace(OrderedHashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
സ്ഥിതം വരിയിൽ IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        തിരികെ _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
സ്ഥിതം വരിയിൽ IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        തിരികെ _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

പൂർണ്ണസംഖ്യ hashmap_put(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_hashmap_put(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        തിരികെ hashmap_put(PLAIN_HASHMAP(h), താക്കോൽ, മൂല്യം);
}

പൂർണ്ണസംഖ്യ hashmap_put_strdup_full(Hashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം അക്ഷരം *k, സ്ഥിരം അക്ഷരം *v);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ hashmap_put_strdup(Hashmap **h, സ്ഥിരം അക്ഷരം *k, സ്ഥിരം അക്ഷരം *v) {
        തിരികെ hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

പൂർണ്ണസംഖ്യ hashmap_update(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_hashmap_update(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        തിരികെ hashmap_update(PLAIN_HASHMAP(h), താക്കോൽ, മൂല്യം);
}

പൂർണ്ണസംഖ്യ hashmap_replace(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_hashmap_replace(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        തിരികെ hashmap_replace(PLAIN_HASHMAP(h), താക്കോൽ, മൂല്യം);
}

ശൂന്യം* _hashmap_get(HashmapBase *h, സ്ഥിരം ശൂന്യം *താക്കോൽ);
സ്ഥിതം വരിയിൽ ശൂന്യം *hashmap_get(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_get(HASHMAP_BASE(h), താക്കോൽ);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_hashmap_get(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_get(HASHMAP_BASE(h), താക്കോൽ);
}

ശൂന്യം* hashmap_get2(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം **ret);
സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_hashmap_get2(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം **ret) {
        തിരികെ hashmap_get2(PLAIN_HASHMAP(h), താക്കോൽ, ret);
}

bool _hashmap_contains(HashmapBase *h, സ്ഥിരം ശൂന്യം *താക്കോൽ);
സ്ഥിതം വരിയിൽ bool hashmap_contains(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_contains(HASHMAP_BASE(h), താക്കോൽ);
}
സ്ഥിതം വരിയിൽ bool ordered_hashmap_contains(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_contains(HASHMAP_BASE(h), താക്കോൽ);
}

ശൂന്യം* _hashmap_remove(HashmapBase *h, സ്ഥിരം ശൂന്യം *താക്കോൽ);
സ്ഥിതം വരിയിൽ ശൂന്യം *hashmap_remove(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_remove(HASHMAP_BASE(h), താക്കോൽ);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_hashmap_remove(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_remove(HASHMAP_BASE(h), താക്കോൽ);
}

ശൂന്യം* hashmap_remove2(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം **ret);
സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_hashmap_remove2(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം **ret) {
        തിരികെ hashmap_remove2(PLAIN_HASHMAP(h), താക്കോൽ, ret);
}

ശൂന്യം* hashmap_remove_മൂല്യം(HashmapBase *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം);
സ്ഥിതം വരിയിൽ ശൂന്യം *hashmap_remove_മൂല്യം_2(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        തിരികെ hashmap_remove_മൂല്യം(HASHMAP_BASE(h), താക്കോൽ, മൂല്യം);
}

സ്ഥിതം വരിയിൽ ശൂന്യം* ordered_hashmap_remove_മൂല്യം(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        തിരികെ hashmap_remove_മൂല്യം_2(PLAIN_HASHMAP(h), താക്കോൽ, മൂല്യം);
}

പൂർണ്ണസംഖ്യ hashmap_remove_and_put(Hashmap *h, സ്ഥിരം ശൂന്യം *old_താക്കോൽ, സ്ഥിരം ശൂന്യം *new_താക്കോൽ, ശൂന്യം *മൂല്യം);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_hashmap_remove_and_put(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *old_താക്കോൽ, സ്ഥിരം ശൂന്യം *new_താക്കോൽ, ശൂന്യം *മൂല്യം) {
        തിരികെ hashmap_remove_and_put(PLAIN_HASHMAP(h), old_താക്കോൽ, new_താക്കോൽ, മൂല്യം);
}

പൂർണ്ണസംഖ്യ hashmap_remove_and_replace(Hashmap *h, സ്ഥിരം ശൂന്യം *old_താക്കോൽ, സ്ഥിരം ശൂന്യം *new_താക്കോൽ, ശൂന്യം *മൂല്യം);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_hashmap_remove_and_replace(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *old_താക്കോൽ, സ്ഥിരം ശൂന്യം *new_താക്കോൽ, ശൂന്യം *മൂല്യം) {
        തിരികെ hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_താക്കോൽ, new_താക്കോൽ, മൂല്യം);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
പൂർണ്ണസംഖ്യ _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

പൂർണ്ണസംഖ്യ _hashmap_reserve(HashmapBase *h, unsigned entries_add);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ hashmap_reserve(Hashmap *h, unsigned entries_add) {
        തിരികെ _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        തിരികെ _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

പൂർണ്ണസംഖ്യ _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ hashmap_move(Hashmap *h, Hashmap *other) {
        തിരികെ _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        തിരികെ _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

പൂർണ്ണസംഖ്യ _hashmap_move_one(HashmapBase *h, HashmapBase *other, സ്ഥിരം ശൂന്യം *താക്കോൽ);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ hashmap_move_one(Hashmap *h, Hashmap *other, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), താക്കോൽ);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), താക്കോൽ);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
സ്ഥിതം വരിയിൽ unsigned hashmap_size(Hashmap *h) {
        തിരികെ _hashmap_size(HASHMAP_BASE(h));
}
സ്ഥിതം വരിയിൽ unsigned ordered_hashmap_size(OrderedHashmap *h) {
        തിരികെ _hashmap_size(HASHMAP_BASE(h));
}

സ്ഥിതം വരിയിൽ bool hashmap_isempty(Hashmap *h) {
        തിരികെ hashmap_size(h) == 0;
}
സ്ഥിതം വരിയിൽ bool ordered_hashmap_isempty(OrderedHashmap *h) {
        തിരികെ ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
സ്ഥിതം വരിയിൽ unsigned hashmap_buckets(Hashmap *h) {
        തിരികെ _hashmap_buckets(HASHMAP_BASE(h));
}
സ്ഥിതം വരിയിൽ unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        തിരികെ _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, ശൂന്യം **മൂല്യം, സ്ഥിരം ശൂന്യം **താക്കോൽ);
സ്ഥിതം വരിയിൽ bool hashmap_iterate(Hashmap *h, Iterator *i, ശൂന്യം **മൂല്യം, സ്ഥിരം ശൂന്യം **താക്കോൽ) {
        തിരികെ _hashmap_iterate(HASHMAP_BASE(h), i, മൂല്യം, താക്കോൽ);
}
സ്ഥിതം വരിയിൽ bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, ശൂന്യം **മൂല്യം, സ്ഥിരം ശൂന്യം **താക്കോൽ) {
        തിരികെ _hashmap_iterate(HASHMAP_BASE(h), i, മൂല്യം, താക്കോൽ);
}

ശൂന്യം _hashmap_clear(HashmapBase *h);
സ്ഥിതം വരിയിൽ ശൂന്യം hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
സ്ഥിതം വരിയിൽ ശൂന്യം ordered_hashmap_clear(OrderedHashmap *h) {
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

ശൂന്യം *hashmap_first_താക്കോൽ_and_മൂല്യം(HashmapBase *h, bool remove, ശൂന്യം **ret_താക്കോൽ);
സ്ഥിതം വരിയിൽ ശൂന്യം *hashmap_steal_first_താക്കോൽ_and_മൂല്യം(Hashmap *h, ശൂന്യം **ret) {
        തിരികെ hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE(h), true, ret);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_hashmap_steal_first_താക്കോൽ_and_മൂല്യം(OrderedHashmap *h, ശൂന്യം **ret) {
        തിരികെ hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE(h), true, ret);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *hashmap_first_താക്കോൽ_and_മൂല്യം_2(Hashmap *h, ശൂന്യം **ret) {
        തിരികെ hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE(h), false, ret);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_hashmap_first_താക്കോൽ_and_മൂല്യം(OrderedHashmap *h, ശൂന്യം **ret) {
        തിരികെ hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE(h), false, ret);
}

സ്ഥിതം വരിയിൽ ശൂന്യം *hashmap_steal_first(Hashmap *h) {
        തിരികെ hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE(h), true, ശൂന്യ);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_hashmap_steal_first(OrderedHashmap *h) {
        തിരികെ hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE(h), true, ശൂന്യ);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *hashmap_first(Hashmap *h) {
        തിരികെ hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE(h), false, ശൂന്യ);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_hashmap_first(OrderedHashmap *h) {
        തിരികെ hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE(h), false, ശൂന്യ);
}

സ്ഥിതം വരിയിൽ ശൂന്യം *hashmap_first_താക്കോൽ(HashmapBase *h, bool remove) {
        ശൂന്യം *താക്കോൽ = ശൂന്യ;

        (ശൂന്യം) hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE(h), remove, &താക്കോൽ);
        തിരികെ താക്കോൽ;
}
സ്ഥിതം വരിയിൽ ശൂന്യം *hashmap_steal_first_താക്കോൽ(Hashmap *h) {
        തിരികെ hashmap_first_താക്കോൽ(HASHMAP_BASE(h), true);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_hashmap_steal_first_താക്കോൽ(OrderedHashmap *h) {
        തിരികെ hashmap_first_താക്കോൽ(HASHMAP_BASE(h), true);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *hashmap_first_താക്കോൽ_2(Hashmap *h) {
        തിരികെ hashmap_first_താക്കോൽ(HASHMAP_BASE(h), false);
}
സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_hashmap_first_താക്കോൽ(OrderedHashmap *h) {
        തിരികെ hashmap_first_താക്കോൽ(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
ശൂന്യം* ordered_hashmap_next(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ);

അക്ഷരം** _hashmap_get_strv(HashmapBase *h);
സ്ഥിതം വരിയിൽ അക്ഷരം** hashmap_get_strv(Hashmap *h) {
        തിരികെ _hashmap_get_strv(HASHMAP_BASE(h));
}
സ്ഥിതം വരിയിൽ അക്ഷരം** ordered_hashmap_get_strv(OrderedHashmap *h) {
        തിരികെ _hashmap_get_strv(HASHMAP_BASE(h));
}

പൂർണ്ണസംഖ്യ _hashmap_dump_sorted(HashmapBase *h, ശൂന്യം ***ret, size_t *ret_n);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ hashmap_dump_sorted(Hashmap *h, ശൂന്യം ***ret, size_t *ret_n) {
        തിരികെ _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_hashmap_dump_sorted(OrderedHashmap *h, ശൂന്യം ***ret, size_t *ret_n) {
        തിരികെ _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ set_dump_sorted(Set *h, ശൂന്യം ***ret, size_t *ret_n) {
        തിരികെ _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

പൂർണ്ണസംഖ്യ hashmap_dump_താക്കോലുകൾ_sorted(HashmapBase *h, ശൂന്യം ***ret, size_t *ret_n);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ hashmap_dump_താക്കോലുകൾ_sorted_2(Hashmap *h, ശൂന്യം ***ret, size_t *ret_n) {
        തിരികെ hashmap_dump_താക്കോലുകൾ_sorted(HASHMAP_BASE(h), ret, ret_n);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_hashmap_dump_താക്കോലുകൾ_sorted(OrderedHashmap *h, ശൂന്യം ***ret, size_t *ret_n) {
        തിരികെ hashmap_dump_താക്കോലുകൾ_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        വേണ്ടി (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (ശൂന്യം**)&(e), ശൂന്യ); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        വേണ്ടി (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (ശൂന്യം**)&(e), ശൂന്യ); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        വേണ്ടി (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (ശൂന്യം**)&(e), ശൂന്യ); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define HASHMAP_BASE_FOREACH_താക്കോൽ(e, k, h, i) \
        വേണ്ടി (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (ശൂന്യം**)&(e), (സ്ഥിരം ശൂന്യം**) &(k)); )
#define HASHMAP_BASE_FOREACH_താക്കോൽ_2(e, k, h) \
        HASHMAP_BASE_FOREACH_താക്കോൽ(e, k, h, UNIQ_T(i, UNIQ))

#define HASHMAP_FOREACH_താക്കോൽ(e, k, h, i) \
        വേണ്ടി (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (ശൂന്യം**)&(e), (സ്ഥിരം ശൂന്യം**) &(k)); )
#define HASHMAP_FOREACH_താക്കോൽ_2(e, k, h) \
        HASHMAP_FOREACH_താക്കോൽ(e, k, h, UNIQ_T(i, UNIQ))

#define ORDERED_HASHMAP_FOREACH_താക്കോൽ(e, k, h, i) \
        വേണ്ടി (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (ശൂന്യം**)&(e), (സ്ഥിരം ശൂന്യം**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_താക്കോൽ_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_താക്കോൽ(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

ശൂന്യം hashmap_trim_pools(ശൂന്യം);
