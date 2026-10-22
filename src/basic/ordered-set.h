/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Ordered_ಗಣ Ordered_ಗಣ;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ Ordered_ಗಣ* ordered_ಗಣ_new(ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *ops) {
        ಹಿಂತಿರುಗಿ (Ordered_ಗಣ*) ordered_hashmap_new(ops);
}

ಪೂರ್ಣಾಂಕ ordered_ಗಣ_ensure_allocated(Ordered_ಗಣ **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *ops);

ಪೂರ್ಣಾಂಕ ordered_ಗಣ_ensure_put(Ordered_ಗಣ **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *ops, ಶೂನ್ಯ *p);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ordered_ಗಣ_clear(Ordered_ಗಣ *s) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_clear((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ Ordered_ಗಣ* ordered_ಗಣ_free(Ordered_ಗಣ *s) {
        ಹಿಂತಿರುಗಿ (Ordered_ಗಣ*) ordered_hashmap_free((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_ಗಣ_ಒಳಗೊಂಡಿದೆ(Ordered_ಗಣ *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_ಒಳಗೊಂಡಿದೆ((OrderedHashmap*) s, p);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_ಗಣ_put(Ordered_ಗಣ *s, ಶೂನ್ಯ *p) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *ordered_ಗಣ_get(Ordered_ಗಣ *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_get((OrderedHashmap*) s, p);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ordered_ಗಣ_ಗಾತ್ರ(Ordered_ಗಣ *s) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_ಗಾತ್ರ((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ordered_ಗಣ_isempty(Ordered_ಗಣ *s) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_isempty((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ordered_ಗಣ_iterate(Ordered_ಗಣ *s, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_iterate((OrderedHashmap*) s, i, ಮೌಲ್ಯ, ಖಾಲಿ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ordered_ಗಣ_remove(Ordered_ಗಣ *s, ಶೂನ್ಯ *p) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ordered_ಗಣ_ಮೊದಲ(Ordered_ಗಣ *s) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_ಮೊದಲ((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ordered_ಗಣ_steal_ಮೊದಲ(Ordered_ಗಣ *s) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_steal_ಮೊದಲ((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ** ordered_ಗಣ_get_strv(Ordered_ಗಣ *s) {
        ಹಿಂತಿರುಗಿ _hashmap_get_strv(HASHMAP_ತಳ((OrderedHashmap*) s));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ordered_ಗಣ_reserve(Ordered_ಗಣ *s, ಚಿಹ್ನೆರಹಿತ entries_ಕೂಡಿಸು) {
        ಹಿಂತಿರುಗಿ ordered_hashmap_reserve((OrderedHashmap*) s, entries_ಕೂಡಿಸು);
}

ಪೂರ್ಣಾಂಕ ordered_ಗಣ_consume(Ordered_ಗಣ *s, ಶೂನ್ಯ *p);

ಪೂರ್ಣಾಂಕ ordered_ಗಣ_put_strdup_ತುಂಬಿದ(Ordered_ಗಣ **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);
#ವ್ಯಾಖ್ಯಾನಿಸು ordered_ಗಣ_put_strdup(s, p) ordered_ಗಣ_put_strdup_ತುಂಬಿದ(s, &string_hash_ops_free, p)
ಪೂರ್ಣಾಂಕ ordered_ಗಣ_put_strdupv_ತುಂಬಿದ(Ordered_ಗಣ **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಅಕ್ಷರ **l);
#ವ್ಯಾಖ್ಯಾನಿಸು ordered_ಗಣ_put_strdupv(s, l) ordered_ಗಣ_put_strdupv_ತುಂಬಿದ(s, &string_hash_ops_free, l)
ಪೂರ್ಣಾಂಕ ordered_ಗಣ_put_string_ಗಣ_ತುಂಬಿದ(Ordered_ಗಣ **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, Ordered_ಗಣ *l);
#ವ್ಯಾಖ್ಯಾನಿಸು ordered_ಗಣ_put_string_ಗಣ(s, l) ordered_ಗಣ_put_string_ಗಣ_ತುಂಬಿದ(s, &string_hash_ops_free, l)

ಶೂನ್ಯ ordered_ಗಣ_print(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, Ordered_ಗಣ *s);

#ವ್ಯಾಖ್ಯಾನಿಸು ORDERED_ಗಣ_FOREACH_2(e, s, i) \
        ಗಾಗಿ (Iterator i = ITERATOR_ಮೊದಲ; ordered_ಗಣ_iterate((s), &i, (ಶೂನ್ಯ**)&(e)); )
#ವ್ಯಾಖ್ಯಾನಿಸು ORDERED_ಗಣ_FOREACH(e, s) \
        ORDERED_ಗಣ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Ordered_ಗಣ*, ordered_ಗಣ_free);

#ವ್ಯಾಖ್ಯಾನಿಸು cleanup_ordered_ಗಣ_free _cleanup_(ordered_ಗಣ_freep)
