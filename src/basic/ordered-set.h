/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"
#समावेश "hashmap.h"

प्रकार काष्ठा OrderedSet OrderedSet;

अटल अंतरभूत OrderedSet* ordered_set_new(स्थिर काष्ठा hash_ops *ops) {
        वापस (OrderedSet*) ordered_hashmap_new(ops);
}

पूर्णांक ordered_set_ensure_allocated(OrderedSet **s, स्थिर काष्ठा hash_ops *ops);

पूर्णांक ordered_set_ensure_put(OrderedSet **s, स्थिर काष्ठा hash_ops *ops, व्योम *p);

अटल अंतरभूत व्योम ordered_set_clear(OrderedSet *s) {
        वापस ordered_hashmap_clear((OrderedHashmap*) s);
}

अटल अंतरभूत OrderedSet* ordered_set_free(OrderedSet *s) {
        वापस (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

अटल अंतरभूत पूर्णांक ordered_set_contains(OrderedSet *s, स्थिर व्योम *p) {
        वापस ordered_hashmap_contains((OrderedHashmap*) s, p);
}

अटल अंतरभूत पूर्णांक ordered_set_put(OrderedSet *s, व्योम *p) {
        वापस ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

अटल अंतरभूत व्योम *ordered_set_get(OrderedSet *s, स्थिर व्योम *p) {
        वापस ordered_hashmap_get((OrderedHashmap*) s, p);
}

अटल अंतरभूत अचिन्हित ordered_set_आकार(OrderedSet *s) {
        वापस ordered_hashmap_आकार((OrderedHashmap*) s);
}

अटल अंतरभूत bool ordered_set_isempty(OrderedSet *s) {
        वापस ordered_hashmap_isempty((OrderedHashmap*) s);
}

अटल अंतरभूत bool ordered_set_iterate(OrderedSet *s, Iterator *i, व्योम **मूल्य) {
        वापस ordered_hashmap_iterate((OrderedHashmap*) s, i, मूल्य, शून्य);
}

अटल अंतरभूत व्योम* ordered_set_remove(OrderedSet *s, व्योम *p) {
        वापस ordered_hashmap_remove((OrderedHashmap*) s, p);
}

अटल अंतरभूत व्योम* ordered_set_first(OrderedSet *s) {
        वापस ordered_hashmap_first((OrderedHashmap*) s);
}

अटल अंतरभूत व्योम* ordered_set_steal_first(OrderedSet *s) {
        वापस ordered_hashmap_steal_first((OrderedHashmap*) s);
}

अटल अंतरभूत अक्षर** ordered_set_get_strv(OrderedSet *s) {
        वापस _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

अटल अंतरभूत पूर्णांक ordered_set_reserve(OrderedSet *s, अचिन्हित entries_add) {
        वापस ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

पूर्णांक ordered_set_consume(OrderedSet *s, व्योम *p);

पूर्णांक ordered_set_put_strdup_full(OrderedSet **s, स्थिर काष्ठा hash_ops *hash_ops, स्थिर अक्षर *p);
#घोषणा ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
पूर्णांक ordered_set_put_strdupv_full(OrderedSet **s, स्थिर काष्ठा hash_ops *hash_ops, अक्षर **l);
#घोषणा ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
पूर्णांक ordered_set_put_string_set_full(OrderedSet **s, स्थिर काष्ठा hash_ops *hash_ops, OrderedSet *l);
#घोषणा ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

व्योम ordered_set_print(खाता *f, स्थिर अक्षर *field, OrderedSet *s);

#घोषणा _ORDERED_SET_FOREACH(e, s, i) \
        क्रम (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (व्योम**)&(e)); )
#घोषणा ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#घोषणा _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
