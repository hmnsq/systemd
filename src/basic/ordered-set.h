/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ OrderedSet OrderedSet;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ OrderedSet* ordered_set_new(ಧ್ರುವ ಸಂರಚನಾ hash_ops *ops) {
        ಪ್ರತ್ಯಾಗಮ (OrderedSet*) ordered_hashmap_new(ops);
}

ಪೂರ್ಣಾಙ್ಕ ordered_set_ensure_allocated(OrderedSet **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *ops);

ಪೂರ್ಣಾಙ್ಕ ordered_set_ensure_put(OrderedSet **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *ops, ಶೂನ್ಯ *p);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ ordered_set_clear(OrderedSet *s) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_clear((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ OrderedSet* ordered_set_free(OrderedSet *s) {
        ಪ್ರತ್ಯಾಗಮ (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_set_contains(OrderedSet *s, ಧ್ರುವ ಶೂನ್ಯ *p) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_contains((OrderedHashmap*) s, p);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_set_put(OrderedSet *s, ಶೂನ್ಯ *p) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *ordered_set_get(OrderedSet *s, ಧ್ರುವ ಶೂನ್ಯ *p) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_get((OrderedHashmap*) s, p);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned ordered_set_size(OrderedSet *s) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_size((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ordered_set_isempty(OrderedSet *s) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_isempty((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ordered_set_iterate(OrderedSet *s, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_iterate((OrderedHashmap*) s, i, ಮೌಲ್ಯ, ರಿಕ್ತ);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* ordered_set_remove(OrderedSet *s, ಶೂನ್ಯ *p) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* ordered_set_first(OrderedSet *s) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_first((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* ordered_set_steal_first(OrderedSet *s) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_steal_first((OrderedHashmap*) s);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ** ordered_set_get_strv(OrderedSet *s) {
        ಪ್ರತ್ಯಾಗಮ _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

ಪೂರ್ಣಾಙ್ಕ ordered_set_consume(OrderedSet *s, ಶೂನ್ಯ *p);

ಪೂರ್ಣಾಙ್ಕ ordered_set_put_strdup_full(OrderedSet **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ವರ್ಣ *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
ಪೂರ್ಣಾಙ್ಕ ordered_set_put_strdupv_full(OrderedSet **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ವರ್ಣ **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
ಪೂರ್ಣಾಙ್ಕ ordered_set_put_string_set_full(OrderedSet **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

ಶೂನ್ಯ ordered_set_print(FILE *f, ಧ್ರುವ ವರ್ಣ *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        ಪ್ರತಿ (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (ಶೂನ್ಯ**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
