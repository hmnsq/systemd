/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(स्थिरांक संरचना hash_ops *hash_ops);

स्थिर ओळीत Set* set_free(Set *s) {
        परत (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

पूर्णांक set_ensure_allocated(Set **s, स्थिरांक संरचना hash_ops *hash_ops);

पूर्णांक set_put(Set *s, स्थिरांक शून्य *किल्ली);
/* no set_update */
/* no set_replace */
स्थिर ओळीत शून्य *set_get(स्थिरांक Set *s, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_get(HASHMAP_BASE((Set *) s), किल्ली);
}
/* no set_get2 */

स्थिर ओळीत bool set_contains(स्थिरांक Set *s, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_contains(HASHMAP_BASE((Set *) s), किल्ली);
}

स्थिर ओळीत शून्य *set_remove(Set *s, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_remove(HASHMAP_BASE(s), किल्ली);
}

/* no set_remove2 */
/* no set_remove_value */
पूर्णांक set_remove_and_put(Set *s, स्थिरांक शून्य *old_किल्ली, स्थिरांक शून्य *new_किल्ली);
/* no set_remove_and_replace */
पूर्णांक set_merge(Set *s, Set *other);

स्थिर ओळीत पूर्णांक set_reserve(Set *h, अचिन्हित entries_add) {
        परत _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

स्थिर ओळीत पूर्णांक set_move(Set *s, Set *other) {
        परत _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

स्थिर ओळीत पूर्णांक set_move_one(Set *s, Set *other, स्थिरांक शून्य *किल्ली) {
        परत _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), किल्ली);
}

स्थिर ओळीत अचिन्हित set_size(स्थिरांक Set *s) {
        परत _hashmap_size(HASHMAP_BASE((Set *) s));
}

स्थिर ओळीत bool set_isempty(स्थिरांक Set *s) {
        परत set_size(s) == 0;
}

स्थिर ओळीत अचिन्हित set_buckets(स्थिरांक Set *s) {
        परत _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

स्थिर ओळीत bool set_iterate(स्थिरांक Set *s, Iterator *i, शून्य **मूल्य) {
        परत _hashmap_iterate(HASHMAP_BASE((Set*) s), i, मूल्य, रिक्त);
}

स्थिर ओळीत शून्य set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

स्थिर ओळीत शून्य *set_steal_first(Set *s) {
        परत hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE(s), true, रिक्त);
}

/* no set_steal_first_key */
/* no set_first_key */

स्थिर ओळीत शून्य *set_first(स्थिरांक Set *s) {
        परत hashmap_first_किल्ली_and_मूल्य(HASHMAP_BASE((Set *) s), false, रिक्त);
}

/* no set_next */

स्थिर ओळीत अक्षर **set_get_strv(Set *s) {
        परत _hashmap_get_strv(HASHMAP_BASE(s));
}

अक्षर** set_to_strv(Set **s);

पूर्णांक set_ensure_put(Set **s, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक शून्य *किल्ली);

पूर्णांक set_ensure_consume(Set **s, स्थिरांक संरचना hash_ops *hash_ops, शून्य *किल्ली);

पूर्णांक set_consume(Set *s, शून्य *मूल्य);

पूर्णांक set_put_strndup_full(Set **s, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक अक्षर *p, माप_प्रकार n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

पूर्णांक set_put_strdupv_full(Set **s, स्थिरांक संरचना hash_ops *hash_ops, अक्षर **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

पूर्णांक set_put_strsplit(Set *s, स्थिरांक अक्षर *v, स्थिरांक अक्षर *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        साठी (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (शून्य**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        साठी (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

पूर्णांक set_strjoin(Set *s, स्थिरांक अक्षर *separator, bool wrap_with_separator, अक्षर **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, स्थिरांक अक्षर *needle);
