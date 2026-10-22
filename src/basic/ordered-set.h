/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "hashmap.h"

প্রকারনির্দেশ সংরচনা OrderedSet OrderedSet;

স্থির অন্তঃস্থ OrderedSet* ordered_set_new(ধ্রুব সংরচনা hash_ops *ops) {
        প্রত্যাগম (OrderedSet*) ordered_hashmap_new(ops);
}

পূর্ণাঙ্ক ordered_set_ensure_allocated(OrderedSet **s, ধ্রুব সংরচনা hash_ops *ops);

পূর্ণাঙ্ক ordered_set_ensure_put(OrderedSet **s, ধ্রুব সংরচনা hash_ops *ops, শূন্য *p);

স্থির অন্তঃস্থ শূন্য ordered_set_clear(OrderedSet *s) {
        প্রত্যাগম ordered_hashmap_clear((OrderedHashmap*) s);
}

স্থির অন্তঃস্থ OrderedSet* ordered_set_free(OrderedSet *s) {
        প্রত্যাগম (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_set_contains(OrderedSet *s, ধ্রুব শূন্য *p) {
        প্রত্যাগম ordered_hashmap_contains((OrderedHashmap*) s, p);
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_set_put(OrderedSet *s, শূন্য *p) {
        প্রত্যাগম ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

স্থির অন্তঃস্থ শূন্য *ordered_set_get(OrderedSet *s, ধ্রুব শূন্য *p) {
        প্রত্যাগম ordered_hashmap_get((OrderedHashmap*) s, p);
}

স্থির অন্তঃস্থ unsigned ordered_set_size(OrderedSet *s) {
        প্রত্যাগম ordered_hashmap_size((OrderedHashmap*) s);
}

স্থির অন্তঃস্থ bool ordered_set_isempty(OrderedSet *s) {
        প্রত্যাগম ordered_hashmap_isempty((OrderedHashmap*) s);
}

স্থির অন্তঃস্থ bool ordered_set_iterate(OrderedSet *s, Iterator *i, শূন্য **মান) {
        প্রত্যাগম ordered_hashmap_iterate((OrderedHashmap*) s, i, মান, রিক্ত);
}

স্থির অন্তঃস্থ শূন্য* ordered_set_remove(OrderedSet *s, শূন্য *p) {
        প্রত্যাগম ordered_hashmap_remove((OrderedHashmap*) s, p);
}

স্থির অন্তঃস্থ শূন্য* ordered_set_first(OrderedSet *s) {
        প্রত্যাগম ordered_hashmap_first((OrderedHashmap*) s);
}

স্থির অন্তঃস্থ শূন্য* ordered_set_steal_first(OrderedSet *s) {
        প্রত্যাগম ordered_hashmap_steal_first((OrderedHashmap*) s);
}

স্থির অন্তঃস্থ বর্ণ** ordered_set_get_strv(OrderedSet *s) {
        প্রত্যাগম _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        প্রত্যাগম ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

পূর্ণাঙ্ক ordered_set_consume(OrderedSet *s, শূন্য *p);

পূর্ণাঙ্ক ordered_set_put_strdup_full(OrderedSet **s, ধ্রুব সংরচনা hash_ops *hash_ops, ধ্রুব বর্ণ *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
পূর্ণাঙ্ক ordered_set_put_strdupv_full(OrderedSet **s, ধ্রুব সংরচনা hash_ops *hash_ops, বর্ণ **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
পূর্ণাঙ্ক ordered_set_put_string_set_full(OrderedSet **s, ধ্রুব সংরচনা hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

শূন্য ordered_set_print(FILE *f, ধ্রুব বর্ণ *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        প্রতি (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (শূন্য**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
