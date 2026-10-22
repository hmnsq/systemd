/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Set* set_free(Set *s) {
        ಪ್ರತ್ಯಾಗಮ (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

ಪೂರ್ಣಾಙ್ಕ set_ensure_allocated(Set **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops);

ಪೂರ್ಣಾಙ್ಕ set_put(Set *s, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ);
/* no set_update */
/* no set_replace */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *set_get(ಧ್ರುವ Set *s, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_get(HASHMAP_BASE((Set *) s), ಕೀಲಿ);
}
/* no set_get2 */

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool set_contains(ಧ್ರುವ Set *s, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_contains(HASHMAP_BASE((Set *) s), ಕೀಲಿ);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *set_remove(Set *s, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_remove(HASHMAP_BASE(s), ಕೀಲಿ);
}

/* no set_remove2 */
/* no set_remove_value */
ಪೂರ್ಣಾಙ್ಕ set_remove_and_put(Set *s, ಧ್ರುವ ಶೂನ್ಯ *old_ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *new_ಕೀಲಿ);
/* no set_remove_and_replace */
ಪೂರ್ಣಾಙ್ಕ set_merge(Set *s, Set *other);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ set_reserve(Set *h, unsigned entries_add) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ set_move(Set *s, Set *other) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ set_move_one(Set *s, Set *other, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), ಕೀಲಿ);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned set_size(ಧ್ರುವ Set *s) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_size(HASHMAP_BASE((Set *) s));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool set_isempty(ಧ್ರುವ Set *s) {
        ಪ್ರತ್ಯಾಗಮ set_size(s) == 0;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned set_buckets(ಧ್ರುವ Set *s) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool set_iterate(ಧ್ರುವ Set *s, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_iterate(HASHMAP_BASE((Set*) s), i, ಮೌಲ್ಯ, ರಿಕ್ತ);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *set_steal_first(Set *s) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE(s), true, ರಿಕ್ತ);
}

/* no set_steal_first_key */
/* no set_first_key */

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *set_first(ಧ್ರುವ Set *s) {
        ಪ್ರತ್ಯಾಗಮ hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_BASE((Set *) s), false, ರಿಕ್ತ);
}

/* no set_next */

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ **set_get_strv(Set *s) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_get_strv(HASHMAP_BASE(s));
}

ವರ್ಣ** set_to_strv(Set **s);

ಪೂರ್ಣಾಙ್ಕ set_ensure_put(Set **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ);

ಪೂರ್ಣಾಙ್ಕ set_ensure_consume(Set **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಶೂನ್ಯ *ಕೀಲಿ);

ಪೂರ್ಣಾಙ್ಕ set_consume(Set *s, ಶೂನ್ಯ *ಮೌಲ್ಯ);

ಪೂರ್ಣಾಙ್ಕ set_put_strndup_full(Set **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ವರ್ಣ *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

ಪೂರ್ಣಾಙ್ಕ set_put_strdupv_full(Set **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ವರ್ಣ **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

ಪೂರ್ಣಾಙ್ಕ set_put_strsplit(Set *s, ಧ್ರುವ ವರ್ಣ *v, ಧ್ರುವ ವರ್ಣ *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        ಪ್ರತಿ (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (ಶೂನ್ಯ**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        ಪ್ರತಿ (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

ಪೂರ್ಣಾಙ್ಕ set_strjoin(Set *s, ಧ್ರುವ ವರ್ಣ *separator, bool wrap_with_separator, ವರ್ಣ **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ಧ್ರುವ ವರ್ಣ *needle);
