/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "hashmap.h"

प्रकारनिर्देश संरचना OrderedSet OrderedSet;

स्थिर अन्तःस्थ OrderedSet* ordered_set_new(अचर संरचना hash_ops *ops) {
        फर्काऊ (OrderedSet*) ordered_hashmap_new(ops);
}

पूर्णाङ्क ordered_set_ensure_allocated(OrderedSet **s, अचर संरचना hash_ops *ops);

पूर्णाङ्क ordered_set_ensure_put(OrderedSet **s, अचर संरचना hash_ops *ops, शून्य *p);

स्थिर अन्तःस्थ शून्य ordered_set_clear(OrderedSet *s) {
        फर्काऊ ordered_hashmap_clear((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ OrderedSet* ordered_set_free(OrderedSet *s) {
        फर्काऊ (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ पूर्णाङ्क ordered_set_contains(OrderedSet *s, अचर शून्य *p) {
        फर्काऊ ordered_hashmap_contains((OrderedHashmap*) s, p);
}

स्थिर अन्तःस्थ पूर्णाङ्क ordered_set_put(OrderedSet *s, शून्य *p) {
        फर्काऊ ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

स्थिर अन्तःस्थ शून्य *ordered_set_get(OrderedSet *s, अचर शून्य *p) {
        फर्काऊ ordered_hashmap_get((OrderedHashmap*) s, p);
}

स्थिर अन्तःस्थ अचिन्हित ordered_set_size(OrderedSet *s) {
        फर्काऊ ordered_hashmap_size((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ bool ordered_set_isempty(OrderedSet *s) {
        फर्काऊ ordered_hashmap_isempty((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ bool ordered_set_iterate(OrderedSet *s, Iterator *i, शून्य **मान) {
        फर्काऊ ordered_hashmap_iterate((OrderedHashmap*) s, i, मान, रिक्त);
}

स्थिर अन्तःस्थ शून्य* ordered_set_remove(OrderedSet *s, शून्य *p) {
        फर्काऊ ordered_hashmap_remove((OrderedHashmap*) s, p);
}

स्थिर अन्तःस्थ शून्य* ordered_set_first(OrderedSet *s) {
        फर्काऊ ordered_hashmap_first((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ शून्य* ordered_set_steal_first(OrderedSet *s) {
        फर्काऊ ordered_hashmap_steal_first((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ वर्ण** ordered_set_get_strv(OrderedSet *s) {
        फर्काऊ _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

स्थिर अन्तःस्थ पूर्णाङ्क ordered_set_reserve(OrderedSet *s, अचिन्हित entries_add) {
        फर्काऊ ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

पूर्णाङ्क ordered_set_consume(OrderedSet *s, शून्य *p);

पूर्णाङ्क ordered_set_put_strdup_full(OrderedSet **s, अचर संरचना hash_ops *hash_ops, अचर वर्ण *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
पूर्णाङ्क ordered_set_put_strdupv_full(OrderedSet **s, अचर संरचना hash_ops *hash_ops, वर्ण **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
पूर्णाङ्क ordered_set_put_string_set_full(OrderedSet **s, अचर संरचना hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

शून्य ordered_set_print(खाता *f, अचर वर्ण *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        लागि (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (शून्य**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
