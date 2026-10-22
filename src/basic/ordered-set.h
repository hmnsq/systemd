/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"
#ضمن "hashmap.h"

تعريفنوع بنية OrderedSet OrderedSet;

ساكن مضمن OrderedSet* ordered_set_new(ثابت بنية hash_ops *ops) {
        إرجاع (OrderedSet*) ordered_hashmap_new(ops);
}

صحيح ordered_set_ensure_allocated(OrderedSet **s, ثابت بنية hash_ops *ops);

صحيح ordered_set_ensure_put(OrderedSet **s, ثابت بنية hash_ops *ops, فراغ *p);

ساكن مضمن فراغ ordered_set_clear(OrderedSet *s) {
        إرجاع ordered_hashmap_clear((OrderedHashmap*) s);
}

ساكن مضمن OrderedSet* ordered_set_free(OrderedSet *s) {
        إرجاع (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

ساكن مضمن صحيح ordered_set_contains(OrderedSet *s, ثابت فراغ *p) {
        إرجاع ordered_hashmap_contains((OrderedHashmap*) s, p);
}

ساكن مضمن صحيح ordered_set_put(OrderedSet *s, فراغ *p) {
        إرجاع ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ساكن مضمن فراغ *ordered_set_get(OrderedSet *s, ثابت فراغ *p) {
        إرجاع ordered_hashmap_get((OrderedHashmap*) s, p);
}

ساكن مضمن unsigned ordered_set_size(OrderedSet *s) {
        إرجاع ordered_hashmap_size((OrderedHashmap*) s);
}

ساكن مضمن bool ordered_set_isempty(OrderedSet *s) {
        إرجاع ordered_hashmap_isempty((OrderedHashmap*) s);
}

ساكن مضمن bool ordered_set_iterate(OrderedSet *s, Iterator *i, فراغ **قيمة) {
        إرجاع ordered_hashmap_iterate((OrderedHashmap*) s, i, قيمة, عدم);
}

ساكن مضمن فراغ* ordered_set_remove(OrderedSet *s, فراغ *p) {
        إرجاع ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ساكن مضمن فراغ* ordered_set_first(OrderedSet *s) {
        إرجاع ordered_hashmap_first((OrderedHashmap*) s);
}

ساكن مضمن فراغ* ordered_set_steal_first(OrderedSet *s) {
        إرجاع ordered_hashmap_steal_first((OrderedHashmap*) s);
}

ساكن مضمن حرف** ordered_set_get_strv(OrderedSet *s) {
        إرجاع _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

ساكن مضمن صحيح ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        إرجاع ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

صحيح ordered_set_consume(OrderedSet *s, فراغ *p);

صحيح ordered_set_put_strdup_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
صحيح ordered_set_put_strdupv_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, حرف **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
صحيح ordered_set_put_string_set_full(OrderedSet **s, ثابت بنية hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

فراغ ordered_set_print(FILE *f, ثابت حرف *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        لكل (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (فراغ**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
