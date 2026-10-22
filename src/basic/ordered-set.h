/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "hashmap.h"

প্রকারলেখ কাঠামো OrderedSet OrderedSet;

স্থির অন্তঃরেখ OrderedSet* ordered_set_new(ধ্রুবক কাঠামো hash_ops *ops) {
        ফেরত (OrderedSet*) ordered_hashmap_new(ops);
}

পূর্ণসংখ্যা ordered_set_ensure_allocated(OrderedSet **s, ধ্রুবক কাঠামো hash_ops *ops);

পূর্ণসংখ্যা ordered_set_ensure_put(OrderedSet **s, ধ্রুবক কাঠামো hash_ops *ops, শূন্য *p);

স্থির অন্তঃরেখ শূন্য ordered_set_clear(OrderedSet *s) {
        ফেরত ordered_hashmap_clear((OrderedHashmap*) s);
}

স্থির অন্তঃরেখ OrderedSet* ordered_set_free(OrderedSet *s) {
        ফেরত (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা ordered_set_contains(OrderedSet *s, ধ্রুবক শূন্য *p) {
        ফেরত ordered_hashmap_contains((OrderedHashmap*) s, p);
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা ordered_set_put(OrderedSet *s, শূন্য *p) {
        ফেরত ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

স্থির অন্তঃরেখ শূন্য *ordered_set_get(OrderedSet *s, ধ্রুবক শূন্য *p) {
        ফেরত ordered_hashmap_get((OrderedHashmap*) s, p);
}

স্থির অন্তঃরেখ unsigned ordered_set_আকার(OrderedSet *s) {
        ফেরত ordered_hashmap_আকার((OrderedHashmap*) s);
}

স্থির অন্তঃরেখ bool ordered_set_isempty(OrderedSet *s) {
        ফেরত ordered_hashmap_isempty((OrderedHashmap*) s);
}

স্থির অন্তঃরেখ bool ordered_set_iterate(OrderedSet *s, Iterator *i, শূন্য **মান) {
        ফেরত ordered_hashmap_iterate((OrderedHashmap*) s, i, মান, ফাঁকা);
}

স্থির অন্তঃরেখ শূন্য* ordered_set_remove(OrderedSet *s, শূন্য *p) {
        ফেরত ordered_hashmap_remove((OrderedHashmap*) s, p);
}

স্থির অন্তঃরেখ শূন্য* ordered_set_first(OrderedSet *s) {
        ফেরত ordered_hashmap_first((OrderedHashmap*) s);
}

স্থির অন্তঃরেখ শূন্য* ordered_set_steal_first(OrderedSet *s) {
        ফেরত ordered_hashmap_steal_first((OrderedHashmap*) s);
}

স্থির অন্তঃরেখ অক্ষর** ordered_set_get_strv(OrderedSet *s) {
        ফেরত _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        ফেরত ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

পূর্ণসংখ্যা ordered_set_consume(OrderedSet *s, শূন্য *p);

পূর্ণসংখ্যা ordered_set_put_strdup_full(OrderedSet **s, ধ্রুবক কাঠামো hash_ops *hash_ops, ধ্রুবক অক্ষর *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
পূর্ণসংখ্যা ordered_set_put_strdupv_full(OrderedSet **s, ধ্রুবক কাঠামো hash_ops *hash_ops, অক্ষর **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
পূর্ণসংখ্যা ordered_set_put_string_set_full(OrderedSet **s, ধ্রুবক কাঠামো hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

শূন্য ordered_set_print(FILE *f, ধ্রুবক অক্ষর *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        জন্য (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (শূন্য**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
