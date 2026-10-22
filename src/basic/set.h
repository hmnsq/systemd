/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(ध्रुव संरचना hash_ops *hash_ops);

स्थिर अन्तःस्थ Set* set_free(Set *s) {
        प्रत्यागम (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

पूर्णाङ्क set_ensure_allocated(Set **s, ध्रुव संरचना hash_ops *hash_ops);

पूर्णाङ्क set_put(Set *s, ध्रुव शून्य *कुञ्चिका);
/* no set_update */
/* no set_replace */
स्थिर अन्तःस्थ शून्य *set_get(ध्रुव Set *s, ध्रुव शून्य *कुञ्चिका) {
        प्रत्यागम _hashmap_get(HASHMAP_BASE((Set *) s), कुञ्चिका);
}
/* no set_get2 */

स्थिर अन्तःस्थ bool set_contains(ध्रुव Set *s, ध्रुव शून्य *कुञ्चिका) {
        प्रत्यागम _hashmap_contains(HASHMAP_BASE((Set *) s), कुञ्चिका);
}

स्थिर अन्तःस्थ शून्य *set_remove(Set *s, ध्रुव शून्य *कुञ्चिका) {
        प्रत्यागम _hashmap_remove(HASHMAP_BASE(s), कुञ्चिका);
}

/* no set_remove2 */
/* no set_remove_value */
पूर्णाङ्क set_remove_and_put(Set *s, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका);
/* no set_remove_and_replace */
पूर्णाङ्क set_merge(Set *s, Set *other);

स्थिर अन्तःस्थ पूर्णाङ्क set_reserve(Set *h, unsigned entries_add) {
        प्रत्यागम _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

स्थिर अन्तःस्थ पूर्णाङ्क set_move(Set *s, Set *other) {
        प्रत्यागम _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

स्थिर अन्तःस्थ पूर्णाङ्क set_move_one(Set *s, Set *other, ध्रुव शून्य *कुञ्चिका) {
        प्रत्यागम _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), कुञ्चिका);
}

स्थिर अन्तःस्थ unsigned set_size(ध्रुव Set *s) {
        प्रत्यागम _hashmap_size(HASHMAP_BASE((Set *) s));
}

स्थिर अन्तःस्थ bool set_isempty(ध्रुव Set *s) {
        प्रत्यागम set_size(s) == 0;
}

स्थिर अन्तःस्थ unsigned set_buckets(ध्रुव Set *s) {
        प्रत्यागम _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

स्थिर अन्तःस्थ bool set_iterate(ध्रुव Set *s, Iterator *i, शून्य **मानम्) {
        प्रत्यागम _hashmap_iterate(HASHMAP_BASE((Set*) s), i, मानम्, रिक्त);
}

स्थिर अन्तःस्थ शून्य set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

स्थिर अन्तःस्थ शून्य *set_steal_first(Set *s) {
        प्रत्यागम hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE(s), true, रिक्त);
}

/* no set_steal_first_key */
/* no set_first_key */

स्थिर अन्तःस्थ शून्य *set_first(ध्रुव Set *s) {
        प्रत्यागम hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_BASE((Set *) s), false, रिक्त);
}

/* no set_next */

स्थिर अन्तःस्थ वर्ण **set_get_strv(Set *s) {
        प्रत्यागम _hashmap_get_strv(HASHMAP_BASE(s));
}

वर्ण** set_to_strv(Set **s);

पूर्णाङ्क set_ensure_put(Set **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका);

पूर्णाङ्क set_ensure_consume(Set **s, ध्रुव संरचना hash_ops *hash_ops, शून्य *कुञ्चिका);

पूर्णाङ्क set_consume(Set *s, शून्य *मानम्);

पूर्णाङ्क set_put_strndup_full(Set **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

पूर्णाङ्क set_put_strdupv_full(Set **s, ध्रुव संरचना hash_ops *hash_ops, वर्ण **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

पूर्णाङ्क set_put_strsplit(Set *s, ध्रुव वर्ण *v, ध्रुव वर्ण *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        प्रति (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (शून्य**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        प्रति (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

पूर्णाङ्क set_strjoin(Set *s, ध्रुव वर्ण *separator, bool wrap_with_separator, वर्ण **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ध्रुव वर्ण *needle);
