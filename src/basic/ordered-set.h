/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "hashmap.h"

प्रकारव्याख्या संरचना OrderedSet OrderedSet;

स्थिर ओळीत OrderedSet* ordered_set_new(स्थिरांक संरचना hash_ops *ops) {
        परत (OrderedSet*) ordered_hashmap_new(ops);
}

पूर्णांक ordered_set_ensure_allocated(OrderedSet **s, स्थिरांक संरचना hash_ops *ops);

पूर्णांक ordered_set_ensure_put(OrderedSet **s, स्थिरांक संरचना hash_ops *ops, शून्य *p);

स्थिर ओळीत शून्य ordered_set_clear(OrderedSet *s) {
        परत ordered_hashmap_clear((OrderedHashmap*) s);
}

स्थिर ओळीत OrderedSet* ordered_set_free(OrderedSet *s) {
        परत (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

स्थिर ओळीत पूर्णांक ordered_set_contains(OrderedSet *s, स्थिरांक शून्य *p) {
        परत ordered_hashmap_contains((OrderedHashmap*) s, p);
}

स्थिर ओळीत पूर्णांक ordered_set_put(OrderedSet *s, शून्य *p) {
        परत ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

स्थिर ओळीत शून्य *ordered_set_get(OrderedSet *s, स्थिरांक शून्य *p) {
        परत ordered_hashmap_get((OrderedHashmap*) s, p);
}

स्थिर ओळीत अचिन्हित ordered_set_size(OrderedSet *s) {
        परत ordered_hashmap_size((OrderedHashmap*) s);
}

स्थिर ओळीत bool ordered_set_isempty(OrderedSet *s) {
        परत ordered_hashmap_isempty((OrderedHashmap*) s);
}

स्थिर ओळीत bool ordered_set_iterate(OrderedSet *s, Iterator *i, शून्य **मूल्य) {
        परत ordered_hashmap_iterate((OrderedHashmap*) s, i, मूल्य, रिक्त);
}

स्थिर ओळीत शून्य* ordered_set_remove(OrderedSet *s, शून्य *p) {
        परत ordered_hashmap_remove((OrderedHashmap*) s, p);
}

स्थिर ओळीत शून्य* ordered_set_first(OrderedSet *s) {
        परत ordered_hashmap_first((OrderedHashmap*) s);
}

स्थिर ओळीत शून्य* ordered_set_steal_first(OrderedSet *s) {
        परत ordered_hashmap_steal_first((OrderedHashmap*) s);
}

स्थिर ओळीत अक्षर** ordered_set_get_strv(OrderedSet *s) {
        परत _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

स्थिर ओळीत पूर्णांक ordered_set_reserve(OrderedSet *s, अचिन्हित entries_add) {
        परत ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

पूर्णांक ordered_set_consume(OrderedSet *s, शून्य *p);

पूर्णांक ordered_set_put_strdup_full(OrderedSet **s, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक अक्षर *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
पूर्णांक ordered_set_put_strdupv_full(OrderedSet **s, स्थिरांक संरचना hash_ops *hash_ops, अक्षर **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
पूर्णांक ordered_set_put_string_set_full(OrderedSet **s, स्थिरांक संरचना hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

शून्य ordered_set_print(खाता *f, स्थिरांक अक्षर *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        साठी (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (शून्य**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
