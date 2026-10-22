/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"
#شامل "hashmap.h"

تعریفنوع ساختار OrderedSet OrderedSet;

ایستا درونخطی OrderedSet* ordered_set_new(ثابت ساختار hash_ops *ops) {
        بازگشت (OrderedSet*) ordered_hashmap_new(ops);
}

صحیح ordered_set_ensure_allocated(OrderedSet **s, ثابت ساختار hash_ops *ops);

صحیح ordered_set_ensure_put(OrderedSet **s, ثابت ساختار hash_ops *ops, تهی *p);

ایستا درونخطی تهی ordered_set_clear(OrderedSet *s) {
        بازگشت ordered_hashmap_clear((OrderedHashmap*) s);
}

ایستا درونخطی OrderedSet* ordered_set_free(OrderedSet *s) {
        بازگشت (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

ایستا درونخطی صحیح ordered_set_contains(OrderedSet *s, ثابت تهی *p) {
        بازگشت ordered_hashmap_contains((OrderedHashmap*) s, p);
}

ایستا درونخطی صحیح ordered_set_put(OrderedSet *s, تهی *p) {
        بازگشت ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ایستا درونخطی تهی *ordered_set_get(OrderedSet *s, ثابت تهی *p) {
        بازگشت ordered_hashmap_get((OrderedHashmap*) s, p);
}

ایستا درونخطی unsigned ordered_set_size(OrderedSet *s) {
        بازگشت ordered_hashmap_size((OrderedHashmap*) s);
}

ایستا درونخطی bool ordered_set_isempty(OrderedSet *s) {
        بازگشت ordered_hashmap_isempty((OrderedHashmap*) s);
}

ایستا درونخطی bool ordered_set_iterate(OrderedSet *s, Iterator *i, تهی **مقدار) {
        بازگشت ordered_hashmap_iterate((OrderedHashmap*) s, i, مقدار, پوچ);
}

ایستا درونخطی تهی* ordered_set_remove(OrderedSet *s, تهی *p) {
        بازگشت ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ایستا درونخطی تهی* ordered_set_first(OrderedSet *s) {
        بازگشت ordered_hashmap_first((OrderedHashmap*) s);
}

ایستا درونخطی تهی* ordered_set_steal_first(OrderedSet *s) {
        بازگشت ordered_hashmap_steal_first((OrderedHashmap*) s);
}

ایستا درونخطی نویسه** ordered_set_get_strv(OrderedSet *s) {
        بازگشت _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

ایستا درونخطی صحیح ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        بازگشت ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

صحیح ordered_set_consume(OrderedSet *s, تهی *p);

صحیح ordered_set_put_strdup_full(OrderedSet **s, ثابت ساختار hash_ops *hash_ops, ثابت نویسه *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
صحیح ordered_set_put_strdupv_full(OrderedSet **s, ثابت ساختار hash_ops *hash_ops, نویسه **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
صحیح ordered_set_put_string_set_full(OrderedSet **s, ثابت ساختار hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

تهی ordered_set_print(FILE *f, ثابت نویسه *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        برای (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (تهی**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
