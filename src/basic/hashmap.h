/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"
#समावेश "hash-funcs.h"
#समावेश "iterator.h"

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

#घोषणा HASH_कुंजी_आकार 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
प्रकार काष्ठा HashmapBase HashmapBase;

/* Specific hashmap/set types */
प्रकार काष्ठा Hashmap Hashmap;               /* Maps keys to values */
प्रकार काष्ठा OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
प्रकार काष्ठा Set Set;                       /* Stores just keys */

प्रकार काष्ठा IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

/* Macros for type checking */
#घोषणा PTR_COMPATIBLE_WITH_HASHMAP_BASE(h) \
        (__builtin_types_compatible_p(typeof(h), HashmapBase*) || \
         __builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*) || \
         __builtin_types_compatible_p(typeof(h), Set*))

#घोषणा PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*)) \

#घोषणा HASHMAP_BASE(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_HASHMAP_BASE(h), \
                (HashmapBase*)(h), \
                (व्योम)0)

#घोषणा PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (व्योम)0)

Hashmap* hashmap_new(स्थिर काष्ठा hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(स्थिर काष्ठा hash_ops *hash_ops);

#घोषणा hashmap_free_and_replace(a, b)                          \
        free_and_replace_full(a, b, hashmap_free)
#घोषणा ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ordered_hashmap_free)

HashmapBase* _hashmap_free(HashmapBase *h);
अटल अंतरभूत Hashmap* hashmap_free(Hashmap *h) {
        वापस (व्योम*) _hashmap_free(HASHMAP_BASE(h));
}
अटल अंतरभूत OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        वापस (व्योम*) _hashmap_free(HASHMAP_BASE(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
पूर्णांक iterated_cache_get(IteratedCache *cache, स्थिर व्योम ***res_कुंजियाँ, स्थिर व्योम ***res_मूल्यावली, अचिन्हित *res_n_entries);

HashmapBase* _hashmap_copy(HashmapBase *h);
अटल अंतरभूत Hashmap* hashmap_copy(Hashmap *h) {
        वापस (Hashmap*) _hashmap_copy(HASHMAP_BASE(h));
}
अटल अंतरभूत OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        वापस (OrderedHashmap*) _hashmap_copy(HASHMAP_BASE(h));
}

पूर्णांक hashmap_ensure_allocated(Hashmap **h, स्थिर काष्ठा hash_ops *hash_ops);
पूर्णांक hashmap_ensure_put(Hashmap **h, स्थिर काष्ठा hash_ops *hash_ops, स्थिर व्योम *कुंजी, व्योम *मूल्य);
पूर्णांक ordered_hashmap_ensure_allocated(OrderedHashmap **h, स्थिर काष्ठा hash_ops *hash_ops);
पूर्णांक hashmap_ensure_replace(Hashmap **h, स्थिर काष्ठा hash_ops *hash_ops, स्थिर व्योम *कुंजी, व्योम *मूल्य);

पूर्णांक ordered_hashmap_ensure_put(OrderedHashmap **h, स्थिर काष्ठा hash_ops *hash_ops, स्थिर व्योम *कुंजी, व्योम *मूल्य);
पूर्णांक ordered_hashmap_ensure_replace(OrderedHashmap **h, स्थिर काष्ठा hash_ops *hash_ops, स्थिर व्योम *कुंजी, व्योम *मूल्य);

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h);
अटल अंतरभूत IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        वापस _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}
अटल अंतरभूत IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        वापस _hashmap_iterated_cache_new(HASHMAP_BASE(h));
}

पूर्णांक hashmap_put(Hashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य);
अटल अंतरभूत पूर्णांक ordered_hashmap_put(OrderedHashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        वापस hashmap_put(PLAIN_HASHMAP(h), कुंजी, मूल्य);
}

पूर्णांक hashmap_put_strdup_full(Hashmap **h, स्थिर काष्ठा hash_ops *hash_ops, स्थिर अक्षर *k, स्थिर अक्षर *v);
अटल अंतरभूत पूर्णांक hashmap_put_strdup(Hashmap **h, स्थिर अक्षर *k, स्थिर अक्षर *v) {
        वापस hashmap_put_strdup_full(h, &string_hash_ops_free_free, k, v);
}

पूर्णांक hashmap_update(Hashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य);
अटल अंतरभूत पूर्णांक ordered_hashmap_update(OrderedHashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        वापस hashmap_update(PLAIN_HASHMAP(h), कुंजी, मूल्य);
}

पूर्णांक hashmap_replace(Hashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य);
अटल अंतरभूत पूर्णांक ordered_hashmap_replace(OrderedHashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        वापस hashmap_replace(PLAIN_HASHMAP(h), कुंजी, मूल्य);
}

व्योम* _hashmap_get(HashmapBase *h, स्थिर व्योम *कुंजी);
अटल अंतरभूत व्योम *hashmap_get(Hashmap *h, स्थिर व्योम *कुंजी) {
        वापस _hashmap_get(HASHMAP_BASE(h), कुंजी);
}
अटल अंतरभूत व्योम *ordered_hashmap_get(OrderedHashmap *h, स्थिर व्योम *कुंजी) {
        वापस _hashmap_get(HASHMAP_BASE(h), कुंजी);
}

व्योम* hashmap_get2(Hashmap *h, स्थिर व्योम *कुंजी, व्योम **ret);
अटल अंतरभूत व्योम *ordered_hashmap_get2(OrderedHashmap *h, स्थिर व्योम *कुंजी, व्योम **ret) {
        वापस hashmap_get2(PLAIN_HASHMAP(h), कुंजी, ret);
}

bool _hashmap_contains(HashmapBase *h, स्थिर व्योम *कुंजी);
अटल अंतरभूत bool hashmap_contains(Hashmap *h, स्थिर व्योम *कुंजी) {
        वापस _hashmap_contains(HASHMAP_BASE(h), कुंजी);
}
अटल अंतरभूत bool ordered_hashmap_contains(OrderedHashmap *h, स्थिर व्योम *कुंजी) {
        वापस _hashmap_contains(HASHMAP_BASE(h), कुंजी);
}

व्योम* _hashmap_remove(HashmapBase *h, स्थिर व्योम *कुंजी);
अटल अंतरभूत व्योम *hashmap_remove(Hashmap *h, स्थिर व्योम *कुंजी) {
        वापस _hashmap_remove(HASHMAP_BASE(h), कुंजी);
}
अटल अंतरभूत व्योम *ordered_hashmap_remove(OrderedHashmap *h, स्थिर व्योम *कुंजी) {
        वापस _hashmap_remove(HASHMAP_BASE(h), कुंजी);
}

व्योम* hashmap_remove2(Hashmap *h, स्थिर व्योम *कुंजी, व्योम **ret);
अटल अंतरभूत व्योम *ordered_hashmap_remove2(OrderedHashmap *h, स्थिर व्योम *कुंजी, व्योम **ret) {
        वापस hashmap_remove2(PLAIN_HASHMAP(h), कुंजी, ret);
}

व्योम* hashmap_remove_मूल्य(HashmapBase *h, स्थिर व्योम *कुंजी, व्योम *मूल्य);
अटल अंतरभूत व्योम *hashmap_remove_मूल्य_2(Hashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        वापस hashmap_remove_मूल्य(HASHMAP_BASE(h), कुंजी, मूल्य);
}

अटल अंतरभूत व्योम* ordered_hashmap_remove_मूल्य(OrderedHashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        वापस hashmap_remove_मूल्य_2(PLAIN_HASHMAP(h), कुंजी, मूल्य);
}

पूर्णांक hashmap_remove_and_put(Hashmap *h, स्थिर व्योम *old_कुंजी, स्थिर व्योम *new_कुंजी, व्योम *मूल्य);
अटल अंतरभूत पूर्णांक ordered_hashmap_remove_and_put(OrderedHashmap *h, स्थिर व्योम *old_कुंजी, स्थिर व्योम *new_कुंजी, व्योम *मूल्य) {
        वापस hashmap_remove_and_put(PLAIN_HASHMAP(h), old_कुंजी, new_कुंजी, मूल्य);
}

पूर्णांक hashmap_remove_and_replace(Hashmap *h, स्थिर व्योम *old_कुंजी, स्थिर व्योम *new_कुंजी, व्योम *मूल्य);
अटल अंतरभूत पूर्णांक ordered_hashmap_remove_and_replace(OrderedHashmap *h, स्थिर व्योम *old_कुंजी, स्थिर व्योम *new_कुंजी, व्योम *मूल्य) {
        वापस hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_कुंजी, new_कुंजी, मूल्य);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
पूर्णांक _hashmap_merge(Hashmap *h, Hashmap *other);
#घोषणा hashmap_merge(h, other) _hashmap_merge(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#घोषणा ordered_hashmap_merge(h, other) hashmap_merge(h, other)

पूर्णांक _hashmap_reserve(HashmapBase *h, अचिन्हित entries_add);
अटल अंतरभूत पूर्णांक hashmap_reserve(Hashmap *h, अचिन्हित entries_add) {
        वापस _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}
अटल अंतरभूत पूर्णांक ordered_hashmap_reserve(OrderedHashmap *h, अचिन्हित entries_add) {
        वापस _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

पूर्णांक _hashmap_move(HashmapBase *h, HashmapBase *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
अटल अंतरभूत पूर्णांक hashmap_move(Hashmap *h, Hashmap *other) {
        वापस _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}
अटल अंतरभूत पूर्णांक ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        वापस _hashmap_move(HASHMAP_BASE(h), HASHMAP_BASE(other));
}

पूर्णांक _hashmap_move_one(HashmapBase *h, HashmapBase *other, स्थिर व्योम *कुंजी);
अटल अंतरभूत पूर्णांक hashmap_move_one(Hashmap *h, Hashmap *other, स्थिर व्योम *कुंजी) {
        वापस _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), कुंजी);
}
अटल अंतरभूत पूर्णांक ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, स्थिर व्योम *कुंजी) {
        वापस _hashmap_move_one(HASHMAP_BASE(h), HASHMAP_BASE(other), कुंजी);
}

अचिन्हित hashmap_आकार(HashmapBase *h) _pure_;
अटल अंतरभूत अचिन्हित hashmap_आकार_2(Hashmap *h) {
        वापस hashmap_आकार(HASHMAP_BASE(h));
}
अटल अंतरभूत अचिन्हित ordered_hashmap_आकार(OrderedHashmap *h) {
        वापस hashmap_आकार(HASHMAP_BASE(h));
}

अटल अंतरभूत bool hashmap_isempty(Hashmap *h) {
        वापस hashmap_आकार_2(h) == 0;
}
अटल अंतरभूत bool ordered_hashmap_isempty(OrderedHashmap *h) {
        वापस ordered_hashmap_आकार(h) == 0;
}

अचिन्हित _hashmap_buckets(HashmapBase *h) _pure_;
अटल अंतरभूत अचिन्हित hashmap_buckets(Hashmap *h) {
        वापस _hashmap_buckets(HASHMAP_BASE(h));
}
अटल अंतरभूत अचिन्हित ordered_hashmap_buckets(OrderedHashmap *h) {
        वापस _hashmap_buckets(HASHMAP_BASE(h));
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, व्योम **मूल्य, स्थिर व्योम **कुंजी);
अटल अंतरभूत bool hashmap_iterate(Hashmap *h, Iterator *i, व्योम **मूल्य, स्थिर व्योम **कुंजी) {
        वापस _hashmap_iterate(HASHMAP_BASE(h), i, मूल्य, कुंजी);
}
अटल अंतरभूत bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, व्योम **मूल्य, स्थिर व्योम **कुंजी) {
        वापस _hashmap_iterate(HASHMAP_BASE(h), i, मूल्य, कुंजी);
}

व्योम _hashmap_clear(HashmapBase *h);
अटल अंतरभूत व्योम hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_BASE(h));
}
अटल अंतरभूत व्योम ordered_hashmap_clear(OrderedHashmap *h) {
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

व्योम *hashmap_first_कुंजी_and_मूल्य(HashmapBase *h, bool हटाओ, व्योम **ret_कुंजी);
अटल अंतरभूत व्योम *hashmap_steal_first_कुंजी_and_मूल्य(Hashmap *h, व्योम **ret) {
        वापस hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE(h), true, ret);
}
अटल अंतरभूत व्योम *ordered_hashmap_steal_first_कुंजी_and_मूल्य(OrderedHashmap *h, व्योम **ret) {
        वापस hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE(h), true, ret);
}
अटल अंतरभूत व्योम *hashmap_first_कुंजी_and_मूल्य_2(Hashmap *h, व्योम **ret) {
        वापस hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE(h), false, ret);
}
अटल अंतरभूत व्योम *ordered_hashmap_first_कुंजी_and_मूल्य(OrderedHashmap *h, व्योम **ret) {
        वापस hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE(h), false, ret);
}

अटल अंतरभूत व्योम *hashmap_steal_first(Hashmap *h) {
        वापस hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE(h), true, शून्य);
}
अटल अंतरभूत व्योम *ordered_hashmap_steal_first(OrderedHashmap *h) {
        वापस hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE(h), true, शून्य);
}
अटल अंतरभूत व्योम *hashmap_first(Hashmap *h) {
        वापस hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE(h), false, शून्य);
}
अटल अंतरभूत व्योम *ordered_hashmap_first(OrderedHashmap *h) {
        वापस hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE(h), false, शून्य);
}

अटल अंतरभूत व्योम *hashmap_first_कुंजी(HashmapBase *h, bool हटाओ) {
        व्योम *कुंजी = शून्य;

        (व्योम) hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE(h), हटाओ, &कुंजी);
        वापस कुंजी;
}
अटल अंतरभूत व्योम *hashmap_steal_first_कुंजी(Hashmap *h) {
        वापस hashmap_first_कुंजी(HASHMAP_BASE(h), true);
}
अटल अंतरभूत व्योम *ordered_hashmap_steal_first_कुंजी(OrderedHashmap *h) {
        वापस hashmap_first_कुंजी(HASHMAP_BASE(h), true);
}
अटल अंतरभूत व्योम *hashmap_first_कुंजी_2(Hashmap *h) {
        वापस hashmap_first_कुंजी(HASHMAP_BASE(h), false);
}
अटल अंतरभूत व्योम *ordered_hashmap_first_कुंजी(OrderedHashmap *h) {
        वापस hashmap_first_कुंजी(HASHMAP_BASE(h), false);
}

/* no hashmap_next */
व्योम* ordered_hashmap_अग्र(OrderedHashmap *h, स्थिर व्योम *कुंजी);

अक्षर** _hashmap_get_strv(HashmapBase *h);
अटल अंतरभूत अक्षर** hashmap_get_strv(Hashmap *h) {
        वापस _hashmap_get_strv(HASHMAP_BASE(h));
}
अटल अंतरभूत अक्षर** ordered_hashmap_get_strv(OrderedHashmap *h) {
        वापस _hashmap_get_strv(HASHMAP_BASE(h));
}

पूर्णांक _hashmap_dump_sorted(HashmapBase *h, व्योम ***ret, माप_प्रकार *ret_n);
अटल अंतरभूत पूर्णांक hashmap_dump_sorted(Hashmap *h, व्योम ***ret, माप_प्रकार *ret_n) {
        वापस _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
अटल अंतरभूत पूर्णांक ordered_hashmap_dump_sorted(OrderedHashmap *h, व्योम ***ret, माप_प्रकार *ret_n) {
        वापस _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}
अटल अंतरभूत पूर्णांक set_dump_sorted(Set *h, व्योम ***ret, माप_प्रकार *ret_n) {
        वापस _hashmap_dump_sorted(HASHMAP_BASE(h), ret, ret_n);
}

पूर्णांक hashmap_dump_कुंजियाँ_sorted(HashmapBase *h, व्योम ***ret, माप_प्रकार *ret_n);
अटल अंतरभूत पूर्णांक hashmap_dump_कुंजियाँ_sorted_2(Hashmap *h, व्योम ***ret, माप_प्रकार *ret_n) {
        वापस hashmap_dump_कुंजियाँ_sorted(HASHMAP_BASE(h), ret, ret_n);
}
अटल अंतरभूत पूर्णांक ordered_hashmap_dump_कुंजियाँ_sorted(OrderedHashmap *h, व्योम ***ret, माप_प्रकार *ret_n) {
        वापस hashmap_dump_कुंजियाँ_sorted(HASHMAP_BASE(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#घोषणा _HASHMAP_BASE_FOREACH(e, h, i) \
        क्रम (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (व्योम**)&(e), शून्य); )
#घोषणा HASHMAP_BASE_FOREACH(e, h) \
        _HASHMAP_BASE_FOREACH(e, h, UNIQ_T(i, UNIQ))

#घोषणा _HASHMAP_FOREACH(e, h, i) \
        क्रम (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (व्योम**)&(e), शून्य); )
#घोषणा HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#घोषणा _ORDERED_HASHMAP_FOREACH(e, h, i) \
        क्रम (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (व्योम**)&(e), शून्य); )
#घोषणा ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#घोषणा HASHMAP_BASE_FOREACH_कुंजी(e, k, h, i) \
        क्रम (Iterator i = ITERATOR_FIRST; _hashmap_iterate((h), &i, (व्योम**)&(e), (स्थिर व्योम**) &(k)); )
#घोषणा HASHMAP_BASE_FOREACH_कुंजी_2(e, k, h) \
        HASHMAP_BASE_FOREACH_कुंजी(e, k, h, UNIQ_T(i, UNIQ))

#घोषणा HASHMAP_FOREACH_कुंजी(e, k, h, i) \
        क्रम (Iterator i = ITERATOR_FIRST; hashmap_iterate((h), &i, (व्योम**)&(e), (स्थिर व्योम**) &(k)); )
#घोषणा HASHMAP_FOREACH_कुंजी_2(e, k, h) \
        HASHMAP_FOREACH_कुंजी(e, k, h, UNIQ_T(i, UNIQ))

#घोषणा ORDERED_HASHMAP_FOREACH_कुंजी(e, k, h, i) \
        क्रम (Iterator i = ITERATOR_FIRST; ordered_hashmap_iterate((h), &i, (व्योम**)&(e), (स्थिर व्योम**) &(k)); )
#घोषणा ORDERED_HASHMAP_FOREACH_कुंजी_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_कुंजी(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#घोषणा _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#घोषणा _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#घोषणा _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

व्योम hashmap_trim_pools(व्योम);
