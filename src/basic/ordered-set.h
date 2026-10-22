/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "hashmap.h"

typedef sa_nracanaa OrderedSet OrderedSet;

static anta_hstha OrderedSet* ordered_set_new(const sa_nracanaa hash_ops *ops) {
        ආපසු (OrderedSet*) ordered_hashmap_new(ops);
}

පූර්ණ_සංඛ්‍යා ordered_set_ensure_allocated(OrderedSet **s, const sa_nracanaa hash_ops *ops);

පූර්ණ_සංඛ්‍යා ordered_set_ensure_put(OrderedSet **s, const sa_nracanaa hash_ops *ops, void *p);

static anta_hstha void ordered_set_clear(OrderedSet *s) {
        ආපසු ordered_hashmap_clear((OrderedHashmap*) s);
}

static anta_hstha OrderedSet* ordered_set_free(OrderedSet *s) {
        ආපසු (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

static anta_hstha පූර්ණ_සංඛ්‍යා ordered_set_contains(OrderedSet *s, const void *p) {
        ආපසු ordered_hashmap_contains((OrderedHashmap*) s, p);
}

static anta_hstha පූර්ණ_සංඛ්‍යා ordered_set_put(OrderedSet *s, void *p) {
        ආපසු ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

static anta_hstha void *ordered_set_get(OrderedSet *s, const void *p) {
        ආපසු ordered_hashmap_get((OrderedHashmap*) s, p);
}

static anta_hstha unsigned ordered_set_size(OrderedSet *s) {
        ආපසු ordered_hashmap_size((OrderedHashmap*) s);
}

static anta_hstha bool ordered_set_isempty(OrderedSet *s) {
        ආපසු ordered_hashmap_isempty((OrderedHashmap*) s);
}

static anta_hstha bool ordered_set_iterate(OrderedSet *s, Iterator *i, void **value) {
        ආපසු ordered_hashmap_iterate((OrderedHashmap*) s, i, value, NULL);
}

static anta_hstha void* ordered_set_remove(OrderedSet *s, void *p) {
        ආපසු ordered_hashmap_remove((OrderedHashmap*) s, p);
}

static anta_hstha void* ordered_set_first(OrderedSet *s) {
        ආපසු ordered_hashmap_first((OrderedHashmap*) s);
}

static anta_hstha void* ordered_set_steal_first(OrderedSet *s) {
        ආපසු ordered_hashmap_steal_first((OrderedHashmap*) s);
}

static anta_hstha var_na** ordered_set_get_strv(OrderedSet *s) {
        ආපසු _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

static anta_hstha පූර්ණ_සංඛ්‍යා ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        ආපසු ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

පූර්ණ_සංඛ්‍යා ordered_set_consume(OrderedSet *s, void *p);

පූර්ණ_සංඛ්‍යා ordered_set_put_strdup_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, const var_na *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
පූර්ණ_සංඛ්‍යා ordered_set_put_strdupv_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, var_na **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
පූර්ණ_සංඛ්‍යා ordered_set_put_string_set_full(OrderedSet **s, const sa_nracanaa hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

void ordered_set_print(FILE *f, const var_na *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        සඳහා (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (void**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
