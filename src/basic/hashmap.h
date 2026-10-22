/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

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

#ವ್ಯಾಖ್ಯಾನಿಸು HASH_ಕೀಲಿ_ಗಾತ್ರ 16


/* The base type for all hashmap and set types. Many functions in the implementation take (HashmapBase*)
 * parameters and are run-time polymorphic, though the API is not meant to be polymorphic (do not call
 * underscore-prefixed functions directly). */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Hashmap_ತಳ Hashmap_ತಳ;

/* Specific hashmap/set types */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Hashmap Hashmap;               /* Maps keys to values */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ OrderedHashmap OrderedHashmap; /* Like Hashmap, but also remembers entry insertion order */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಗಣ_2 ಗಣ_2;                       /* Stores just keys */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ IteratedCache IteratedCache;   /* Caches the iterated order of one of the above */

/* Macros for type checking */
#ವ್ಯಾಖ್ಯಾನಿಸು PTR_COMPATIBLE_WITH_HASHMAP_ತಳ(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap_ತಳ*) || \
         __builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*) || \
         __builtin_types_compatible_p(typeof(h), ಗಣ_2*))

#ವ್ಯಾಖ್ಯಾನಿಸು PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h) \
        (__builtin_types_compatible_p(typeof(h), Hashmap*) || \
         __builtin_types_compatible_p(typeof(h), OrderedHashmap*)) \

#ವ್ಯಾಖ್ಯಾನಿಸು HASHMAP_ತಳ(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_HASHMAP_ತಳ(h), \
                (Hashmap_ತಳ*)(h), \
                (ಶೂನ್ಯ)0)

#ವ್ಯಾಖ್ಯಾನಿಸು PLAIN_HASHMAP(h) \
        __builtin_choose_expr(PTR_COMPATIBLE_WITH_PLAIN_HASHMAP(h), \
                (Hashmap*)(h), \
                (ಶೂನ್ಯ)0)

Hashmap* hashmap_new(ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops);
OrderedHashmap* ordered_hashmap_new(ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops);

#ವ್ಯಾಖ್ಯಾನಿಸು hashmap_free_and_replace(a, b)                          \
        free_and_replace_ತುಂಬಿದ(a, b, hashmap_free)
#ವ್ಯಾಖ್ಯಾನಿಸು ordered_hashmap_free_and_replace(a, b)                  \
        free_and_replace_ತುಂಬಿದ(a, b, ordered_hashmap_free)

Hashmap_ತಳ* _hashmap_free(Hashmap_ತಳ *h);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hashmap* hashmap_free(Hashmap *h) {
        ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ*) _hashmap_free(HASHMAP_ತಳ(h));
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ OrderedHashmap* ordered_hashmap_free(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ*) _hashmap_free(HASHMAP_ತಳ(h));
}

IteratedCache* iterated_cache_free(IteratedCache *cache);
ಪೂರ್ಣಾಂಕ iterated_cache_get(IteratedCache *cache, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ ***res_ಕೀಲಿಗಳು, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ ***res_ಮೌಲ್ಯಗಳು, ಚಿಹ್ನೆರಹಿತ *res_n_entries);

Hashmap_ತಳ* _hashmap_copy(Hashmap_ತಳ *h);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hashmap* hashmap_copy(Hashmap *h) {
        ಹಿಂತಿರುಗಿ (Hashmap*) _hashmap_copy(HASHMAP_ತಳ(h));
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ OrderedHashmap* ordered_hashmap_copy(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ (OrderedHashmap*) _hashmap_copy(HASHMAP_ತಳ(h));
}

ಪೂರ್ಣಾಂಕ hashmap_ensure_allocated(Hashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops);
ಪೂರ್ಣಾಂಕ hashmap_ensure_put(Hashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಂಕ ordered_hashmap_ensure_allocated(OrderedHashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops);
ಪೂರ್ಣಾಂಕ hashmap_ensure_replace(Hashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);

ಪೂರ್ಣಾಂಕ ordered_hashmap_ensure_put(OrderedHashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಂಕ ordered_hashmap_ensure_replace(OrderedHashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);

IteratedCache* _hashmap_iterated_cache_new(Hashmap_ತಳ *h);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ IteratedCache* hashmap_iterated_cache_new(Hashmap *h) {
        ಹಿಂತಿರುಗಿ _hashmap_iterated_cache_new(HASHMAP_ತಳ(h));
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ IteratedCache* ordered_hashmap_iterated_cache_new(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ _hashmap_iterated_cache_new(HASHMAP_ತಳ(h));
}

ಪೂರ್ಣಾಂಕ hashmap_put(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_hashmap_put(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ hashmap_put(PLAIN_HASHMAP(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ hashmap_put_strdup_ತುಂಬಿದ(Hashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ hashmap_put_strdup(Hashmap **h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v) {
        ಹಿಂತಿರುಗಿ hashmap_put_strdup_ತುಂಬಿದ(h, &string_hash_ops_free_free, k, v);
}

ಪೂರ್ಣಾಂಕ hashmap_update(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_hashmap_update(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ hashmap_update(PLAIN_HASHMAP(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ hashmap_replace(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_hashmap_replace(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ hashmap_replace(PLAIN_HASHMAP(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಶೂನ್ಯ* _hashmap_get(Hashmap_ತಳ *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hashmap_get(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_get(HASHMAP_ತಳ(h), ಕೀಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_hashmap_get(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_get(HASHMAP_ತಳ(h), ಕೀಲಿ);
}

ಶೂನ್ಯ* hashmap_get2(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_hashmap_get2(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret) {
        ಹಿಂತಿರುಗಿ hashmap_get2(PLAIN_HASHMAP(h), ಕೀಲಿ, ret);
}

bool hashmap_ಒಳಗೊಂಡಿದೆ_2(Hashmap_ತಳ *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool hashmap_ಒಳಗೊಂಡಿದೆ(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ hashmap_ಒಳಗೊಂಡಿದೆ_2(HASHMAP_ತಳ(h), ಕೀಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ordered_hashmap_ಒಳಗೊಂಡಿದೆ(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ hashmap_ಒಳಗೊಂಡಿದೆ_2(HASHMAP_ತಳ(h), ಕೀಲಿ);
}

ಶೂನ್ಯ* _hashmap_remove(Hashmap_ತಳ *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hashmap_remove(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_remove(HASHMAP_ತಳ(h), ಕೀಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_hashmap_remove(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_remove(HASHMAP_ತಳ(h), ಕೀಲಿ);
}

ಶೂನ್ಯ* hashmap_remove2(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_hashmap_remove2(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret) {
        ಹಿಂತಿರುಗಿ hashmap_remove2(PLAIN_HASHMAP(h), ಕೀಲಿ, ret);
}

ಶೂನ್ಯ* hashmap_remove_ಮೌಲ್ಯ(Hashmap_ತಳ *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hashmap_remove_ಮೌಲ್ಯ_2(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ hashmap_remove_ಮೌಲ್ಯ(HASHMAP_ತಳ(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ordered_hashmap_remove_ಮೌಲ್ಯ(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ hashmap_remove_ಮೌಲ್ಯ_2(PLAIN_HASHMAP(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ hashmap_remove_and_put(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *old_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_hashmap_remove_and_put(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *old_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ hashmap_remove_and_put(PLAIN_HASHMAP(h), old_ಕೀಲಿ, new_ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ hashmap_remove_and_replace(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *old_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_hashmap_remove_and_replace(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *old_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ hashmap_remove_and_replace(PLAIN_HASHMAP(h), old_ಕೀಲಿ, new_ಕೀಲಿ, ಮೌಲ್ಯ);
}

/* Since merging data from an OrderedHashmap into a Hashmap or vice-versa
 * should just work, allow this by having looser type-checking here. */
ಪೂರ್ಣಾಂಕ hashmap_ವಿಲೀನ(Hashmap *h, Hashmap *other);
#ವ್ಯಾಖ್ಯಾನಿಸು hashmap_ವಿಲೀನ_2(h, other) hashmap_ವಿಲೀನ(PLAIN_HASHMAP(h), PLAIN_HASHMAP(other))
#ವ್ಯಾಖ್ಯಾನಿಸು ordered_hashmap_ವಿಲೀನ(h, other) hashmap_ವಿಲೀನ_2(h, other)

ಪೂರ್ಣಾಂಕ _hashmap_reserve(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ entries_ಕೂಡಿಸು);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ hashmap_reserve(Hashmap *h, ಚಿಹ್ನೆರಹಿತ entries_ಕೂಡಿಸು) {
        ಹಿಂತಿರುಗಿ _hashmap_reserve(HASHMAP_ತಳ(h), entries_ಕೂಡಿಸು);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_hashmap_reserve(OrderedHashmap *h, ಚಿಹ್ನೆರಹಿತ entries_ಕೂಡಿಸು) {
        ಹಿಂತಿರುಗಿ _hashmap_reserve(HASHMAP_ತಳ(h), entries_ಕೂಡಿಸು);
}

ಪೂರ್ಣಾಂಕ _hashmap_move(Hashmap_ತಳ *h, Hashmap_ತಳ *other);
/* Unlike hashmap_merge, hashmap_move does not allow mixing the types. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ hashmap_move(Hashmap *h, Hashmap *other) {
        ಹಿಂತಿರುಗಿ _hashmap_move(HASHMAP_ತಳ(h), HASHMAP_ತಳ(other));
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_hashmap_move(OrderedHashmap *h, OrderedHashmap *other) {
        ಹಿಂತಿರುಗಿ _hashmap_move(HASHMAP_ತಳ(h), HASHMAP_ತಳ(other));
}

ಪೂರ್ಣಾಂಕ _hashmap_move_one(Hashmap_ತಳ *h, Hashmap_ತಳ *other, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ hashmap_move_one(Hashmap *h, Hashmap *other, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_move_one(HASHMAP_ತಳ(h), HASHMAP_ತಳ(other), ಕೀಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_hashmap_move_one(OrderedHashmap *h, OrderedHashmap *other, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_move_one(HASHMAP_ತಳ(h), HASHMAP_ತಳ(other), ಕೀಲಿ);
}

ಚಿಹ್ನೆರಹಿತ hashmap_ಗಾತ್ರ(Hashmap_ತಳ *h) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ hashmap_ಗಾತ್ರ_2(Hashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಗಾತ್ರ(HASHMAP_ತಳ(h));
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ordered_hashmap_ಗಾತ್ರ(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಗಾತ್ರ(HASHMAP_ತಳ(h));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool hashmap_isempty(Hashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಗಾತ್ರ_2(h) == 0;
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ordered_hashmap_isempty(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_ಗಾತ್ರ(h) == 0;
}

ಚಿಹ್ನೆರಹಿತ _hashmap_buckets(Hashmap_ತಳ *h) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ hashmap_buckets(Hashmap *h) {
        ಹಿಂತಿರುಗಿ _hashmap_buckets(HASHMAP_ತಳ(h));
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ordered_hashmap_buckets(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ _hashmap_buckets(HASHMAP_ತಳ(h));
}

bool _hashmap_iterate(Hashmap_ತಳ *h, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **ಕೀಲಿ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool hashmap_iterate(Hashmap *h, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_iterate(HASHMAP_ತಳ(h), i, ಮೌಲ್ಯ, ಕೀಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ordered_hashmap_iterate(OrderedHashmap *h, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_iterate(HASHMAP_ತಳ(h), i, ಮೌಲ್ಯ, ಕೀಲಿ);
}

ಶೂನ್ಯ _hashmap_clear(Hashmap_ತಳ *h);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ hashmap_clear(Hashmap *h) {
        _hashmap_clear(HASHMAP_ತಳ(h));
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ordered_hashmap_clear(OrderedHashmap *h) {
        _hashmap_clear(HASHMAP_ತಳ(h));
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

ಶೂನ್ಯ *hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(Hashmap_ತಳ *h, bool remove, ಶೂನ್ಯ **ret_ಕೀಲಿ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hashmap_steal_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(Hashmap *h, ಶೂನ್ಯ **ret) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ(h), true, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_hashmap_steal_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(OrderedHashmap *h, ಶೂನ್ಯ **ret) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ(h), true, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ_2(Hashmap *h, ಶೂನ್ಯ **ret) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ(h), false, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(OrderedHashmap *h, ಶೂನ್ಯ **ret) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ(h), false, ret);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hashmap_steal_ಮೊದಲ(Hashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ(h), true, ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_hashmap_steal_ಮೊದಲ(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ(h), true, ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hashmap_ಮೊದಲ(Hashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ(h), false, ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_hashmap_ಮೊದಲ(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ(h), false, ಖಾಲಿ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hashmap_ಮೊದಲ_ಕೀಲಿ(Hashmap_ತಳ *h, bool remove) {
        ಶೂನ್ಯ *ಕೀಲಿ = ಖಾಲಿ;

        (ಶೂನ್ಯ) hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ(h), remove, &ಕೀಲಿ);
        ಹಿಂತಿರುಗಿ ಕೀಲಿ;
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hashmap_steal_ಮೊದಲ_ಕೀಲಿ(Hashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ(HASHMAP_ತಳ(h), true);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_hashmap_steal_ಮೊದಲ_ಕೀಲಿ(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ(HASHMAP_ತಳ(h), true);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hashmap_ಮೊದಲ_ಕೀಲಿ_2(Hashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ(HASHMAP_ತಳ(h), false);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_hashmap_ಮೊದಲ_ಕೀಲಿ(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ(HASHMAP_ತಳ(h), false);
}

/* no hashmap_next */
ಶೂನ್ಯ* ordered_hashmap_ಮುಂದಿನ(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ);

ಅಕ್ಷರ** _hashmap_get_strv(Hashmap_ತಳ *h);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ** hashmap_get_strv(Hashmap *h) {
        ಹಿಂತಿರುಗಿ _hashmap_get_strv(HASHMAP_ತಳ(h));
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ** ordered_hashmap_get_strv(OrderedHashmap *h) {
        ಹಿಂತಿರುಗಿ _hashmap_get_strv(HASHMAP_ತಳ(h));
}

ಪೂರ್ಣಾಂಕ _hashmap_dump_sorted(Hashmap_ತಳ *h, ಶೂನ್ಯ ***ret, size_t *ret_n);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ hashmap_dump_sorted(Hashmap *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        ಹಿಂತಿರುಗಿ _hashmap_dump_sorted(HASHMAP_ತಳ(h), ret, ret_n);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_hashmap_dump_sorted(OrderedHashmap *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        ಹಿಂತಿರುಗಿ _hashmap_dump_sorted(HASHMAP_ತಳ(h), ret, ret_n);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಗಣ_dump_sorted(ಗಣ_2 *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        ಹಿಂತಿರುಗಿ _hashmap_dump_sorted(HASHMAP_ತಳ(h), ret, ret_n);
}

ಪೂರ್ಣಾಂಕ hashmap_dump_ಕೀಲಿಗಳು_sorted(Hashmap_ತಳ *h, ಶೂನ್ಯ ***ret, size_t *ret_n);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ hashmap_dump_ಕೀಲಿಗಳು_sorted_2(Hashmap *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        ಹಿಂತಿರುಗಿ hashmap_dump_ಕೀಲಿಗಳು_sorted(HASHMAP_ತಳ(h), ret, ret_n);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_hashmap_dump_ಕೀಲಿಗಳು_sorted(OrderedHashmap *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        ಹಿಂತಿರುಗಿ hashmap_dump_ಕೀಲಿಗಳು_sorted(HASHMAP_ತಳ(h), ret, ret_n);
}

/*
 * Hashmaps are iterated in unpredictable order.
 * OrderedHashmaps are an exception to this. They are iterated in the order
 * the entries were inserted.
 * It is safe to remove the current entry.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು HASHMAP_ತಳ_FOREACH(e, h, i) \
        ಗಾಗಿ (Iterator i = ITERATOR_ಮೊದಲ; _hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), ಖಾಲಿ); )
#ವ್ಯಾಖ್ಯಾನಿಸು HASHMAP_ತಳ_FOREACH_2(e, h) \
        HASHMAP_ತಳ_FOREACH(e, h, UNIQ_T(i, UNIQ))

#ವ್ಯಾಖ್ಯಾನಿಸು _HASHMAP_FOREACH(e, h, i) \
        ಗಾಗಿ (Iterator i = ITERATOR_ಮೊದಲ; hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), ಖಾಲಿ); )
#ವ್ಯಾಖ್ಯಾನಿಸು HASHMAP_FOREACH(e, h) \
        _HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#ವ್ಯಾಖ್ಯಾನಿಸು _ORDERED_HASHMAP_FOREACH(e, h, i) \
        ಗಾಗಿ (Iterator i = ITERATOR_ಮೊದಲ; ordered_hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), ಖಾಲಿ); )
#ವ್ಯಾಖ್ಯಾನಿಸು ORDERED_HASHMAP_FOREACH(e, h) \
        _ORDERED_HASHMAP_FOREACH(e, h, UNIQ_T(i, UNIQ))

#ವ್ಯಾಖ್ಯಾನಿಸು HASHMAP_ತಳ_FOREACH_ಕೀಲಿ(e, k, h, i) \
        ಗಾಗಿ (Iterator i = ITERATOR_ಮೊದಲ; _hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ**) &(k)); )
#ವ್ಯಾಖ್ಯಾನಿಸು HASHMAP_ತಳ_FOREACH_ಕೀಲಿ_2(e, k, h) \
        HASHMAP_ತಳ_FOREACH_ಕೀಲಿ(e, k, h, UNIQ_T(i, UNIQ))

#ವ್ಯಾಖ್ಯಾನಿಸು HASHMAP_FOREACH_ಕೀಲಿ(e, k, h, i) \
        ಗಾಗಿ (Iterator i = ITERATOR_ಮೊದಲ; hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ**) &(k)); )
#ವ್ಯಾಖ್ಯಾನಿಸು HASHMAP_FOREACH_ಕೀಲಿ_2(e, k, h) \
        HASHMAP_FOREACH_ಕೀಲಿ(e, k, h, UNIQ_T(i, UNIQ))

#ವ್ಯಾಖ್ಯಾನಿಸು ORDERED_HASHMAP_FOREACH_ಕೀಲಿ(e, k, h, i) \
        ಗಾಗಿ (Iterator i = ITERATOR_ಮೊದಲ; ordered_hashmap_iterate((h), &i, (ಶೂನ್ಯ**)&(e), (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ**) &(k)); )
#ವ್ಯಾಖ್ಯಾನಿಸು ORDERED_HASHMAP_FOREACH_ಕೀಲಿ_2(e, k, h) \
        ORDERED_HASHMAP_FOREACH_ಕೀಲಿ(e, k, h, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Hashmap*, hashmap_free);
DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedHashmap*, ordered_hashmap_free);

#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_hashmap_free_ _cleanup_(hashmap_freep)
#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_ordered_hashmap_free_ _cleanup_(ordered_hashmap_freep)

DEFINE_TRIVIAL_CLEANUP_FUNC(IteratedCache*, iterated_cache_free);

#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_iterated_cache_free_ _cleanup_(iterated_cache_freep)

ಶೂನ್ಯ hashmap_trim_pools(ಶೂನ್ಯ);
