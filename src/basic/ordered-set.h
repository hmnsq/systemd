/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "hashmap.h"

typedef struct OrderedSet OrderedSet;

static inline OrderedSet* ordered_set_new(const struct hash_ops *ops) {
        반환 (OrderedSet*) ordered_hashmap_new(ops);
}

정수 ordered_set_ensure_allocated(OrderedSet **s, const struct hash_ops *ops);

정수 ordered_set_ensure_put(OrderedSet **s, const struct hash_ops *ops, void *p);

static inline void ordered_set_clear(OrderedSet *s) {
        반환 ordered_hashmap_clear((OrderedHashmap*) s);
}

static inline OrderedSet* ordered_set_free(OrderedSet *s) {
        반환 (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

static inline 정수 ordered_set_contains(OrderedSet *s, const void *p) {
        반환 ordered_hashmap_contains((OrderedHashmap*) s, p);
}

static inline 정수 ordered_set_put(OrderedSet *s, void *p) {
        반환 ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

static inline void *ordered_set_get(OrderedSet *s, const void *p) {
        반환 ordered_hashmap_get((OrderedHashmap*) s, p);
}

static inline unsigned ordered_set_size(OrderedSet *s) {
        반환 ordered_hashmap_size((OrderedHashmap*) s);
}

static inline bool ordered_set_isempty(OrderedSet *s) {
        반환 ordered_hashmap_isempty((OrderedHashmap*) s);
}

static inline bool ordered_set_iterate(OrderedSet *s, Iterator *i, void **value) {
        반환 ordered_hashmap_iterate((OrderedHashmap*) s, i, value, NULL);
}

static inline void* ordered_set_remove(OrderedSet *s, void *p) {
        반환 ordered_hashmap_remove((OrderedHashmap*) s, p);
}

static inline void* ordered_set_first(OrderedSet *s) {
        반환 ordered_hashmap_first((OrderedHashmap*) s);
}

static inline void* ordered_set_steal_first(OrderedSet *s) {
        반환 ordered_hashmap_steal_first((OrderedHashmap*) s);
}

static inline char** ordered_set_get_strv(OrderedSet *s) {
        반환 _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

static inline 정수 ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        반환 ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

정수 ordered_set_consume(OrderedSet *s, void *p);

정수 ordered_set_put_strdup_full(OrderedSet **s, const struct hash_ops *hash_ops, const char *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
정수 ordered_set_put_strdupv_full(OrderedSet **s, const struct hash_ops *hash_ops, char **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
정수 ordered_set_put_string_set_full(OrderedSet **s, const struct hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

void ordered_set_print(FILE *f, const char *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        위해 (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (void**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
