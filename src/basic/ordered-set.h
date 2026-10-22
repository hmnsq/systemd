/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "hashmap.h"

પ્રકારવ્યાખ્યા સંરચના OrderedSet OrderedSet;

સ્થિર અંતઃરેખ OrderedSet* ordered_set_new(અચલ સંરચના hash_ops *ops) {
        પાછા (OrderedSet*) ordered_hashmap_new(ops);
}

પૂર્ણાંક ordered_set_ensure_allocated(OrderedSet **s, અચલ સંરચના hash_ops *ops);

પૂર્ણાંક ordered_set_ensure_put(OrderedSet **s, અચલ સંરચના hash_ops *ops, શૂન્ય *p);

સ્થિર અંતઃરેખ શૂન્ય ordered_set_clear(OrderedSet *s) {
        પાછા ordered_hashmap_clear((OrderedHashmap*) s);
}

સ્થિર અંતઃરેખ OrderedSet* ordered_set_free(OrderedSet *s) {
        પાછા (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

સ્થિર અંતઃરેખ પૂર્ણાંક ordered_set_contains(OrderedSet *s, અચલ શૂન્ય *p) {
        પાછા ordered_hashmap_contains((OrderedHashmap*) s, p);
}

સ્થિર અંતઃરેખ પૂર્ણાંક ordered_set_put(OrderedSet *s, શૂન્ય *p) {
        પાછા ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

સ્થિર અંતઃરેખ શૂન્ય *ordered_set_get(OrderedSet *s, અચલ શૂન્ય *p) {
        પાછા ordered_hashmap_get((OrderedHashmap*) s, p);
}

સ્થિર અંતઃરેખ unsigned ordered_set_size(OrderedSet *s) {
        પાછા ordered_hashmap_size((OrderedHashmap*) s);
}

સ્થિર અંતઃરેખ bool ordered_set_isempty(OrderedSet *s) {
        પાછા ordered_hashmap_isempty((OrderedHashmap*) s);
}

સ્થિર અંતઃરેખ bool ordered_set_iterate(OrderedSet *s, Iterator *i, શૂન્ય **કિંમત) {
        પાછા ordered_hashmap_iterate((OrderedHashmap*) s, i, કિંમત, ખાલી);
}

સ્થિર અંતઃરેખ શૂન્ય* ordered_set_remove(OrderedSet *s, શૂન્ય *p) {
        પાછા ordered_hashmap_remove((OrderedHashmap*) s, p);
}

સ્થિર અંતઃરેખ શૂન્ય* ordered_set_first(OrderedSet *s) {
        પાછા ordered_hashmap_first((OrderedHashmap*) s);
}

સ્થિર અંતઃરેખ શૂન્ય* ordered_set_steal_first(OrderedSet *s) {
        પાછા ordered_hashmap_steal_first((OrderedHashmap*) s);
}

સ્થિર અંતઃરેખ અક્ષર** ordered_set_get_strv(OrderedSet *s) {
        પાછા _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

સ્થિર અંતઃરેખ પૂર્ણાંક ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        પાછા ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

પૂર્ણાંક ordered_set_consume(OrderedSet *s, શૂન્ય *p);

પૂર્ણાંક ordered_set_put_strdup_full(OrderedSet **s, અચલ સંરચના hash_ops *hash_ops, અચલ અક્ષર *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
પૂર્ણાંક ordered_set_put_strdupv_full(OrderedSet **s, અચલ સંરચના hash_ops *hash_ops, અક્ષર **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
પૂર્ણાંક ordered_set_put_string_set_full(OrderedSet **s, અચલ સંરચના hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

શૂન્ય ordered_set_print(FILE *f, અચલ અક્ષર *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        માટે (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (શૂન્ય**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
