/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "hash-funcs.h"
#ଅନ୍ତର୍ଭାଵ "iterator.h"

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

#define HASH_ଚାବି_SIZE 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା HashmapBase HashmapBase;

/* Specific hashmap/set types */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Hashmap Hashmap;               /* Maps keys to values */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Set Set;                       /* Stores just keys */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

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
                (ଶୂନ୍ୟ)0)

#define PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (ଶୂନ୍ୟ)0)

Hashmap* hashmap_new(ଅଚଳ ସଂରଚନା hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ଅଚଳ ସଂରଚନା hash_ops *hash_ops);

#define hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#define ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hashmap* hashmap_free(Hashmap *h) {
        ଫେରନ୍ତୁ (ଶୂନ୍ୟ*) _hashmap_free(HASHMAP_BASE(h));
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        ଫେରନ୍ତୁ (ଶୂନ୍ୟ*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iterated_cache_get(IteratedCache *cache, ଅଚଳ ଶୂନ୍ୟ ***res_ଚାବିଗୁଡ଼ିକ, ଅଚଳ ଶୂନ୍ୟ ***res_ମୂଲ୍ୟଗୁଡ଼ିକ, unsigned *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hashmap* hashmap_copy(Hashmap *h) {
        ଫେରନ୍ତୁ (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        ଫେରନ୍ତୁ (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_ensure_allocated(Hashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_ensure_put(Hashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_ensure_allocated(OrderedHashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_ensure_replace(Hashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_ensure_put(OrderedHashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_ensure_replace(OrderedHashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        ଫେରନ୍ତୁ _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        ଫେରନ୍ତୁ _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_put(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_put(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ hashmap_put(PLAIN_HASHMAP(h), ଚାବି, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_put_strdup_full(Hashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଅକ୍ଷର *k, ଅଚଳ ଅକ୍ଷର *v);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_put_strdup(Hashmap **h, ଅଚଳ ଅକ୍ଷର *k, ଅଚଳ ଅକ୍ଷର *v) {
        ଫେରନ୍ତୁ hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_update(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_update(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ hashmap_update(PLAIN_HASHMAP(h), ଚାବି, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_replace(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_replace(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ hashmap_replace(PLAIN_HASHMAP(h), ଚାବି, ମୂଲ୍ୟ);
}

ଶୂନ୍ୟ* _hashmap_get(HashmapBase *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hashmap_get(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_get(HASHMAP_BASE(h), ଚାବି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_hashmap_get(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_get(HASHMAP_BASE(h), ଚାବି);
}

ଶୂନ୍ୟ* hashmap_get2(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_hashmap_get2(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ **ret) {
        ଫେରନ୍ତୁ hashmap_get2(PLAIN_HASHMAP(h), ଚାବି, ret);
}

bool _hashmap_contains(HashmapBase *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool hashmap_contains(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_contains(HASHMAP_BASE(h), ଚାବି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ordered_hashmap_contains(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_contains(HASHMAP_BASE(h), ଚାବି);
}

ଶୂନ୍ୟ* _hashmap_remove(HashmapBase *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hashmap_remove(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_remove(HASHMAP_BASE(h), ଚାବି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_hashmap_remove(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_remove(HASHMAP_BASE(h), ଚାବି);
}

ଶୂନ୍ୟ* hashmap_remove2(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_hashmap_remove2(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ **ret) {
        ଫେରନ୍ତୁ hashmap_remove2(PLAIN_HASHMAP(h), ଚାବି, ret);
}

ଶୂନ୍ୟ* hashmap_remove_ମୂଲ୍ୟ(HashmapBase *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hashmap_remove_ମୂଲ୍ୟ_2(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ hashmap_remove_ମୂଲ୍ୟ(HASHMAP_BASE(h), ଚାବି, ମୂଲ୍ୟ);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* ordered_hashmap_remove_ମୂଲ୍ୟ(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ hashmap_remove_ମୂଲ୍ୟ_2(PLAIN_HASHMAP(h), ଚାବି, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_remove_and_put(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *old_ଚାବି, ଅଚଳ ଶୂନ୍ୟ *new_ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_remove_and_put(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *old_ଚାବି, ଅଚଳ ଶୂନ୍ୟ *new_ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ hashmap_remove_and_put(PLAIN_HASHMAP(h), old_ଚାବି, new_ଚାବି, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_remove_and_replace(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *old_ଚାବି, ଅଚଳ ଶୂନ୍ୟ *new_ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_remove_and_replace(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *old_ଚାବି, ଅଚଳ ଶୂନ୍ୟ *new_ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_ଚାବି, new_ଚାବି, ମୂଲ୍ୟ);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_merge(Hashmap *h, Hashmap *other);
#define hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#define ordered_hashmap_merge(h, other) hashmap_merge(h, other)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_reserve(HashmapBase *h, unsigned entries_add);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_reserve(Hashmap *h, unsigned entries_add) {
        ଫେରନ୍ତୁ _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_reserve(OrderedHashmap *h, unsigned entries_add) {
        ଫେରନ୍ତୁ _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_move(Hashmap *h, Hashmap *other) {
        ଫେରନ୍ତୁ _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        ଫେରନ୍ତୁ _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_move_one(HashmapBase *h, HashmapBase *other, ଅଚଳ ଶୂନ୍ୟ *ଚାବି);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_move_one(Hashmap *h, Hashmap *other, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), ଚାବି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), ଚାବି);
}

unsigned _hashmap_size(HashmapBase *h) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned hashmap_size(Hashmap *h) {
        ଫେରନ୍ତୁ _hashmap_size(HASHMAP_BASE(h));
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned ordered_hashmap_size(OrderedHashmap *h) {
        ଫେରନ୍ତୁ _hashmap_size(HASHMAP_BASE(h));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool hashmap_isempty(Hashmap *h) {
        ଫେରନ୍ତୁ hashmap_size(h) == 0;
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ordered_hashmap_isempty(OrderedHashmap *h) {
        ଫେରନ୍ତୁ ordered_hashmap_size(h) == 0;
}

unsigned _hashmap_buckets(HashmapBase *h) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned hashmap_buckets(Hashmap *h) {
        ଫେରନ୍ତୁ _hashmap_buckets(HASHMAP_BASE(h));
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned ordered_hashmap_buckets(OrderedHashmap *h) {
        ଫେରନ୍ତୁ _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, ଶୂନ୍ୟ **ମୂଲ୍ୟ, ଅଚଳ ଶୂନ୍ୟ **ଚାବି);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool hashmap_iterate(Hashmap *h, Iterator *i, ଶୂନ୍ୟ **ମୂଲ୍ୟ, ଅଚଳ ଶୂନ୍ୟ **ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_iterate(HASHMAP_BASE(h), i, ମୂଲ୍ୟ, ଚାବି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, ଶୂନ୍ୟ **ମୂଲ୍ୟ, ଅଚଳ ଶୂନ୍ୟ **ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_iterate(HASHMAP_BASE(h), i, ମୂଲ୍ୟ, ଚାବି);
}

ଶୂନ୍ୟ _hashmap_clear(HashmapBase *h);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ordered_hashmap_clear(OrderedHashmap *h) {
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

ଶୂନ୍ୟ *hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HashmapBase *h, bool remove, ଶୂନ୍ୟ **ret_ଚାବି);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hashmap_steal_first_ଚାବି_and_ମୂଲ୍ୟ(Hashmap *h, ଶୂନ୍ୟ **ret) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE(h), true, ret);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_hashmap_steal_first_ଚାବି_and_ମୂଲ୍ୟ(OrderedHashmap *h, ଶୂନ୍ୟ **ret) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE(h), true, ret);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hashmap_first_ଚାବି_and_ମୂଲ୍ୟ_2(Hashmap *h, ଶୂନ୍ୟ **ret) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE(h), false, ret);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(OrderedHashmap *h, ଶୂନ୍ୟ **ret) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE(h), false, ret);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hashmap_steal_first(Hashmap *h) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE(h), true, ଖାଲି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_hashmap_steal_first(OrderedHashmap *h) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE(h), true, ଖାଲି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hashmap_first(Hashmap *h) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE(h), false, ଖାଲି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_hashmap_first(OrderedHashmap *h) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE(h), false, ଖାଲି);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hashmap_first_ଚାବି(HashmapBase *h, bool remove) {
        ଶୂନ୍ୟ *ଚାବି = ଖାଲି;

        (ଶୂନ୍ୟ) hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE(h), remove, &ଚାବି);
        ଫେରନ୍ତୁ ଚାବି;
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hashmap_steal_first_ଚାବି(Hashmap *h) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି(HASHMAP_BASE(h), true);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_hashmap_steal_first_ଚାବି(OrderedHashmap *h) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି(HASHMAP_BASE(h), true);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hashmap_first_ଚାବି_2(Hashmap *h) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି(HASHMAP_BASE(h), false);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_hashmap_first_ଚାବି(OrderedHashmap *h) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
ଶୂନ୍ୟ* ordered_hashmap_next(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି);

ଅକ୍ଷର** _hashmap_get_strv(HashmapBase *h);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର** hashmap_get_strv(Hashmap *h) {
        ଫେରନ୍ତୁ _hashmap_get_strv(HASHMAP_BASE(h));
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର** ordered_hashmap_get_strv(OrderedHashmap *h) {
        ଫେରନ୍ତୁ _hashmap_get_strv(HASHMAP_BASE(h));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_dump_sorted(HashmapBase *h, ଶୂନ୍ୟ ***ret, size_t *ret_n);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_dump_sorted(Hashmap *h, ଶୂନ୍ୟ ***ret, size_t *ret_n) {
        ଫେରନ୍ତୁ _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_dump_sorted(OrderedHashmap *h, ଶୂନ୍ୟ ***ret, size_t *ret_n) {
        ଫେରନ୍ତୁ _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_dump_sorted(Set *h, ଶୂନ୍ୟ ***ret, size_t *ret_n) {
        ଫେରନ୍ତୁ _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_dump_ଚାବିଗୁଡ଼ିକ_sorted(HashmapBase *h, ଶୂନ୍ୟ ***ret, size_t *ret_n);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_dump_ଚାବିଗୁଡ଼ିକ_sorted_2(Hashmap *h, ଶୂନ୍ୟ ***ret, size_t *ret_n) {
        ଫେରନ୍ତୁ hashmap_dump_ଚାବିଗୁଡ଼ିକ_sorted(HASHMAP_BASE(h), ret, ret_n);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_dump_ଚାବିଗୁଡ଼ିକ_sorted(OrderedHashmap *h, ଶୂନ୍ୟ ***ret, size_t *ret_n) {
        ଫେରନ୍ତୁ hashmap_dump_ଚାବିଗୁଡ଼ିକ_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#define _HASHMAP_BASE_FOREACH(e, h, i) \
        ପାଇଁ (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (ଶୂନ୍ୟ**)&(e), ଖାଲି); )
#define HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _HASHMAP_FOREACH(e, h, i) \
        ପାଇଁ (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (ଶୂନ୍ୟ**)&(e), ଖାଲି); )
#define HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define _ORDERED_HASHMAP_FOREACH(e, h, i) \
        ପାଇଁ (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (ଶୂନ୍ୟ**)&(e), ଖାଲି); )
#define ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#define HASHMAP_BASE_FOREACH_ଚାବି(e, k, h, i) \
        ପାଇଁ (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (ଶୂନ୍ୟ**)&(e), (ଅଚଳ ଶୂନ୍ୟ**) &(k)); )
#define HASHMAP_BASE_FOREACH_ଚାବି_2(e, k, h) \
        HASHMAP_BASE_FOREACH_ଚାବି(e, k, h, UNIQ_T(i, UNIQ))

#define HASHMAP_FOREACH_ଚାବି(e, k, h, i) \
        ପାଇଁ (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (ଶୂନ୍ୟ**)&(e), (ଅଚଳ ଶୂନ୍ୟ**) &(k)); )
#define HASHMAP_FOREACH_ଚାବି_2(e, k, h) \
        HASHMAP_FOREACH_ଚାବି(e, k, h, UNIQ_T(i, UNIQ))

#define ORDERED_HASHMAP_FOREACH_ଚାବି(e, k, h, i) \
        ପାଇଁ (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (ଶୂନ୍ୟ**)&(e), (ଅଚଳ ଶୂନ୍ୟ**) &(k)); )
#define ORDERED_HASHMAP_FOREACH_ଚାବି_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_ଚାବି(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#define _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#define _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#define _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

ଶୂନ୍ୟ hashmap_trim_pools(ଶୂନ୍ୟ);
