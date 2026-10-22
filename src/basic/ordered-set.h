/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "hashmap.h"

வகைவரை கட்டமைப்பு OrderedSet OrderedSet;

நிலையான உள்வரி OrderedSet* ordered_set_new(மாறிலி கட்டமைப்பு hash_ops *ops) {
        திரும்பு (OrderedSet*) ordered_hashmap_new(ops);
}

முழு ordered_set_ensure_allocated(OrderedSet **s, மாறிலி கட்டமைப்பு hash_ops *ops);

முழு ordered_set_ensure_put(OrderedSet **s, மாறிலி கட்டமைப்பு hash_ops *ops, வெறுமை *p);

நிலையான உள்வரி வெறுமை ordered_set_clear(OrderedSet *s) {
        திரும்பு ordered_hashmap_clear((OrderedHashmap*) s);
}

நிலையான உள்வரி OrderedSet* ordered_set_free(OrderedSet *s) {
        திரும்பு (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

நிலையான உள்வரி முழு ordered_set_contains(OrderedSet *s, மாறிலி வெறுமை *p) {
        திரும்பு ordered_hashmap_contains((OrderedHashmap*) s, p);
}

நிலையான உள்வரி முழு ordered_set_put(OrderedSet *s, வெறுமை *p) {
        திரும்பு ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

நிலையான உள்வரி வெறுமை *ordered_set_get(OrderedSet *s, மாறிலி வெறுமை *p) {
        திரும்பு ordered_hashmap_get((OrderedHashmap*) s, p);
}

நிலையான உள்வரி unsigned ordered_set_size(OrderedSet *s) {
        திரும்பு ordered_hashmap_size((OrderedHashmap*) s);
}

நிலையான உள்வரி bool ordered_set_isempty(OrderedSet *s) {
        திரும்பு ordered_hashmap_isempty((OrderedHashmap*) s);
}

நிலையான உள்வரி bool ordered_set_iterate(OrderedSet *s, Iterator *i, வெறுமை **மதிப்பு) {
        திரும்பு ordered_hashmap_iterate((OrderedHashmap*) s, i, மதிப்பு, வெற்று);
}

நிலையான உள்வரி வெறுமை* ordered_set_remove(OrderedSet *s, வெறுமை *p) {
        திரும்பு ordered_hashmap_remove((OrderedHashmap*) s, p);
}

நிலையான உள்வரி வெறுமை* ordered_set_first(OrderedSet *s) {
        திரும்பு ordered_hashmap_first((OrderedHashmap*) s);
}

நிலையான உள்வரி வெறுமை* ordered_set_steal_first(OrderedSet *s) {
        திரும்பு ordered_hashmap_steal_first((OrderedHashmap*) s);
}

நிலையான உள்வரி எழுத்து** ordered_set_get_strv(OrderedSet *s) {
        திரும்பு _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

நிலையான உள்வரி முழு ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        திரும்பு ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

முழு ordered_set_consume(OrderedSet *s, வெறுமை *p);

முழு ordered_set_put_strdup_full(OrderedSet **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops, மாறிலி எழுத்து *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
முழு ordered_set_put_strdupv_full(OrderedSet **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops, எழுத்து **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
முழு ordered_set_put_string_set_full(OrderedSet **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

வெறுமை ordered_set_print(FILE *f, மாறிலி எழுத்து *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        க்கு (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (வெறுமை**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
