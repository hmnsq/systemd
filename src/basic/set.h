/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_free_and_replace(a, b)              \
        free_and_replace_ತುಂಬಿದ(a, b, ಗಣ_free)

ಗಣ_2* ಗಣ_new(ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಗಣ_2* ಗಣ_free(ಗಣ_2 *s) {
        ಹಿಂತಿರುಗಿ (ಗಣ_2*) _hashmap_free(HASHMAP_ತಳ(s));
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_copy(s) ((ಗಣ_2*) _hashmap_copy(HASHMAP_ತಳ(s)))

ಪೂರ್ಣಾಂಕ ಗಣ_ensure_allocated(ಗಣ_2 **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops);

ಪೂರ್ಣಾಂಕ ಗಣ_put(ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ);
/* no set_update */
/* no set_replace */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ಗಣ_get(ಸ್ಥಿರಾಂಕ ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_get(HASHMAP_ತಳ((ಗಣ_2 *) s), ಕೀಲಿ);
}
/* no set_get2 */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಗಣ_ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ hashmap_ಒಳಗೊಂಡಿದೆ_2(HASHMAP_ತಳ((ಗಣ_2 *) s), ಕೀಲಿ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ಗಣ_remove(ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_remove(HASHMAP_ತಳ(s), ಕೀಲಿ);
}

/* no set_remove2 */
/* no set_remove_value */
ಪೂರ್ಣಾಂಕ ಗಣ_remove_and_put(ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *old_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *new_ಕೀಲಿ);
/* no set_remove_and_replace */
ಪೂರ್ಣಾಂಕ ಗಣ_ವಿಲೀನ(ಗಣ_2 *s, ಗಣ_2 *other);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಗಣ_reserve(ಗಣ_2 *h, ಚಿಹ್ನೆರಹಿತ entries_ಕೂಡಿಸು) {
        ಹಿಂತಿರುಗಿ _hashmap_reserve(HASHMAP_ತಳ(h), entries_ಕೂಡಿಸು);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಗಣ_move(ಗಣ_2 *s, ಗಣ_2 *other) {
        ಹಿಂತಿರುಗಿ _hashmap_move(HASHMAP_ತಳ(s), HASHMAP_ತಳ(other));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಗಣ_move_one(ಗಣ_2 *s, ಗಣ_2 *other, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಹಿಂತಿರುಗಿ _hashmap_move_one(HASHMAP_ತಳ(s), HASHMAP_ತಳ(other), ಕೀಲಿ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಗಣ_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಗಣ_2 *s) {
        ಹಿಂತಿರುಗಿ hashmap_ಗಾತ್ರ(HASHMAP_ತಳ((ಗಣ_2 *) s));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಗಣ_isempty(ಸ್ಥಿರಾಂಕ ಗಣ_2 *s) {
        ಹಿಂತಿರುಗಿ ಗಣ_ಗಾತ್ರ(s) == 0;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಗಣ_buckets(ಸ್ಥಿರಾಂಕ ಗಣ_2 *s) {
        ಹಿಂತಿರುಗಿ _hashmap_buckets(HASHMAP_ತಳ((ಗಣ_2 *) s));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಗಣ_iterate(ಸ್ಥಿರಾಂಕ ಗಣ_2 *s, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ _hashmap_iterate(HASHMAP_ತಳ((ಗಣ_2*) s), i, ಮೌಲ್ಯ, ಖಾಲಿ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ_clear(ಗಣ_2 *s) {
        _hashmap_clear(HASHMAP_ತಳ(s));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ಗಣ_steal_ಮೊದಲ(ಗಣ_2 *s) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ(s), true, ಖಾಲಿ);
}

/* no set_steal_first_key */
/* no set_first_key */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ಗಣ_ಮೊದಲ(ಸ್ಥಿರಾಂಕ ಗಣ_2 *s) {
        ಹಿಂತಿರುಗಿ hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(HASHMAP_ತಳ((ಗಣ_2 *) s), false, ಖಾಲಿ);
}

/* no set_next */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ **ಗಣ_get_strv(ಗಣ_2 *s) {
        ಹಿಂತಿರುಗಿ _hashmap_get_strv(HASHMAP_ತಳ(s));
}

ಅಕ್ಷರ** ಗಣ_ಗಮ್ಯ_strv(ಗಣ_2 **s);

ಪೂರ್ಣಾಂಕ ಗಣ_ensure_put(ಗಣ_2 **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ);

ಪೂರ್ಣಾಂಕ ಗಣ_ensure_consume(ಗಣ_2 **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಶೂನ್ಯ *ಕೀಲಿ);

ಪೂರ್ಣಾಂಕ ಗಣ_consume(ಗಣ_2 *s, ಶೂನ್ಯ *ಮೌಲ್ಯ);

ಪೂರ್ಣಾಂಕ ಗಣ_put_strndup_ತುಂಬಿದ(ಗಣ_2 **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t n);
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_put_strdup_ತುಂಬಿದ(s, hash_ops, p) ಗಣ_put_strndup_ತುಂಬಿದ(s, hash_ops, p, SIZE_MAX)
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_put_strndup(s, p, n) ಗಣ_put_strndup_ತುಂಬಿದ(s, &string_hash_ops_free, p, n)
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_put_strdup(s, p) ಗಣ_put_strndup(s, p, SIZE_MAX)

ಪೂರ್ಣಾಂಕ ಗಣ_put_strdupv_ತುಂಬಿದ(ಗಣ_2 **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಅಕ್ಷರ **l);
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_put_strdupv(s, l) ಗಣ_put_strdupv_ತುಂಬಿದ(s, &string_hash_ops_free, l)

ಪೂರ್ಣಾಂಕ ಗಣ_put_strsplit(ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಹೊರತೆಗೆ_Flags flags);

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_FOREACH_2(e, s, i) \
        ಗಾಗಿ (Iterator i = ITERATOR_ಮೊದಲ; ಗಣ_iterate((s), &i, (ಶೂನ್ಯ**)&(e)); )
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_FOREACH(e, s) \
        ಗಣ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_FOREACH_MOVE(e, d, s)                                       \
        ಗಾಗಿ (; ({ e = ಗಣ_ಮೊದಲ(s); assert_se(!e || ಗಣ_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(ಗಣ_2*, ಗಣ_free);

#ವ್ಯಾಖ್ಯಾನಿಸು cleanup_ಗಣ_free _cleanup_(ಗಣ_freep)

ಪೂರ್ಣಾಂಕ ಗಣ_strjoin(ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, bool wrap_with_separator, ಅಕ್ಷರ **ret);

bool ಗಣ_equal(ಗಣ_2 *a, ಗಣ_2 *b);

bool ಗಣ_fnmatch(ಗಣ_2 *include_patterns, ಗಣ_2 *exclude_patterns, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle);
