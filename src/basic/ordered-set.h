/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "hashmap.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା OrderedSet OrderedSet;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ OrderedSet* ordered_set_new(ଅଚଳ ସଂରଚନା hash_ops *ops) {
        ଫେରନ୍ତୁ (OrderedSet*) ordered_hashmap_new(ops);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_ensure_allocated(OrderedSet **s, ଅଚଳ ସଂରଚନା hash_ops *ops);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_ensure_put(OrderedSet **s, ଅଚଳ ସଂରଚନା hash_ops *ops, ଶୂନ୍ୟ *p);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ordered_set_clear(OrderedSet *s) {
        ଫେରନ୍ତୁ ordered_hashmap_clear((OrderedHashmap*) s);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ OrderedSet* ordered_set_free(OrderedSet *s) {
        ଫେରନ୍ତୁ (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_contains(OrderedSet *s, ଅଚଳ ଶୂନ୍ୟ *p) {
        ଫେରନ୍ତୁ ordered_hashmap_contains((OrderedHashmap*) s, p);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_put(OrderedSet *s, ଶୂନ୍ୟ *p) {
        ଫେରନ୍ତୁ ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *ordered_set_get(OrderedSet *s, ଅଚଳ ଶୂନ୍ୟ *p) {
        ଫେରନ୍ତୁ ordered_hashmap_get((OrderedHashmap*) s, p);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned ordered_set_size(OrderedSet *s) {
        ଫେରନ୍ତୁ ordered_hashmap_size((OrderedHashmap*) s);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ordered_set_isempty(OrderedSet *s) {
        ଫେରନ୍ତୁ ordered_hashmap_isempty((OrderedHashmap*) s);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ordered_set_iterate(OrderedSet *s, Iterator *i, ଶୂନ୍ୟ **ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ ordered_hashmap_iterate((OrderedHashmap*) s, i, ମୂଲ୍ୟ, ଖାଲି);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* ordered_set_remove(OrderedSet *s, ଶୂନ୍ୟ *p) {
        ଫେରନ୍ତୁ ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* ordered_set_first(OrderedSet *s) {
        ଫେରନ୍ତୁ ordered_hashmap_first((OrderedHashmap*) s);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* ordered_set_steal_first(OrderedSet *s) {
        ଫେରନ୍ତୁ ordered_hashmap_steal_first((OrderedHashmap*) s);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର** ordered_set_get_strv(OrderedSet *s) {
        ଫେରନ୍ତୁ _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        ଫେରନ୍ତୁ ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_consume(OrderedSet *s, ଶୂନ୍ୟ *p);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_put_strdup_full(OrderedSet **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଅକ୍ଷର *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_put_strdupv_full(OrderedSet **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅକ୍ଷର **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_set_put_string_set_full(OrderedSet **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

ଶୂନ୍ୟ ordered_set_print(FILE *f, ଅଚଳ ଅକ୍ଷର *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        ପାଇଁ (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (ଶୂନ୍ୟ**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
