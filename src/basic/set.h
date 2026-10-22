/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"
#समावेश "hashmap.h"

#घोषणा set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(स्थिर काष्ठा hash_ops *hash_ops);

अटल अंतरभूत Set* set_free(Set *s) {
        वापस (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#घोषणा set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

पूर्णांक set_ensure_allocated(Set **s, स्थिर काष्ठा hash_ops *hash_ops);

पूर्णांक set_put(Set *s, स्थिर व्योम *कुंजी);
/* no set_update */
/* no set_replace */
अटल अंतरभूत व्योम *set_get(स्थिर Set *s, स्थिर व्योम *कुंजी) {
        वापस _hashmap_get(HASHMAP_BASE((Set *) s), कुंजी);
}
/* no set_get2 */

अटल अंतरभूत bool set_contains(स्थिर Set *s, स्थिर व्योम *कुंजी) {
        वापस _hashmap_contains(HASHMAP_BASE((Set *) s), कुंजी);
}

अटल अंतरभूत व्योम *set_remove(Set *s, स्थिर व्योम *कुंजी) {
        वापस _hashmap_remove(HASHMAP_BASE(s), कुंजी);
}

/* no set_remove2 */
/* no set_remove_value */
पूर्णांक set_remove_and_put(Set *s, स्थिर व्योम *old_कुंजी, स्थिर व्योम *new_कुंजी);
/* no set_remove_and_replace */
पूर्णांक set_merge(Set *s, Set *other);

अटल अंतरभूत पूर्णांक set_reserve(Set *h, अचिन्हित entries_add) {
        वापस _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

अटल अंतरभूत पूर्णांक set_move(Set *s, Set *other) {
        वापस _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

अटल अंतरभूत पूर्णांक set_move_one(Set *s, Set *other, स्थिर व्योम *कुंजी) {
        वापस _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), कुंजी);
}

अटल अंतरभूत अचिन्हित set_आकार(स्थिर Set *s) {
        वापस hashmap_आकार(HASHMAP_BASE((Set *) s));
}

अटल अंतरभूत bool set_isempty(स्थिर Set *s) {
        वापस set_आकार(s) == 0;
}

अटल अंतरभूत अचिन्हित set_buckets(स्थिर Set *s) {
        वापस _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

अटल अंतरभूत bool set_iterate(स्थिर Set *s, Iterator *i, व्योम **मूल्य) {
        वापस _hashmap_iterate(HASHMAP_BASE((Set*) s), i, मूल्य, शून्य);
}

अटल अंतरभूत व्योम set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

अटल अंतरभूत व्योम *set_steal_first(Set *s) {
        वापस hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE(s), true, शून्य);
}

/* no set_steal_first_key */
/* no set_first_key */

अटल अंतरभूत व्योम *set_first(स्थिर Set *s) {
        वापस hashmap_first_कुंजी_and_मूल्य(HASHMAP_BASE((Set *) s), false, शून्य);
}

/* no set_next */

अटल अंतरभूत अक्षर **set_get_strv(Set *s) {
        वापस _hashmap_get_strv(HASHMAP_BASE(s));
}

अक्षर** set_to_strv(Set **s);

पूर्णांक set_ensure_put(Set **s, स्थिर काष्ठा hash_ops *hash_ops, स्थिर व्योम *कुंजी);

पूर्णांक set_ensure_consume(Set **s, स्थिर काष्ठा hash_ops *hash_ops, व्योम *कुंजी);

पूर्णांक set_consume(Set *s, व्योम *मूल्य);

पूर्णांक set_put_strndup_full(Set **s, स्थिर काष्ठा hash_ops *hash_ops, स्थिर अक्षर *p, माप_प्रकार n);
#घोषणा set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#घोषणा set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#घोषणा set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

पूर्णांक set_put_strdupv_full(Set **s, स्थिर काष्ठा hash_ops *hash_ops, अक्षर **l);
#घोषणा set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

पूर्णांक set_put_strsplit(Set *s, स्थिर अक्षर *v, स्थिर अक्षर *separators, ExtractFlags flags);

#घोषणा _SET_FOREACH(e, s, i) \
        क्रम (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (व्योम**)&(e)); )
#घोषणा SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#घोषणा SET_FOREACH_MOVE(e, d, s)                                       \
        क्रम (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#घोषणा _cleanup_set_free_ _cleanup_(set_freep)

पूर्णांक set_strjoin(Set *s, स्थिर अक्षर *separator, bool wrap_with_separator, अक्षर **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, स्थिर अक्षर *needle);
