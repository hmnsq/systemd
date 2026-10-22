/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(const sa_nracanaa hash_ops *hash_ops);

static anta_hstha Set* set_free(Set *s) {
        ᤀᤡᤴ (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

puur_naa_nka set_ensure_allocated(Set **s, const sa_nracanaa hash_ops *hash_ops);

puur_naa_nka set_put(Set *s, const void *key);
/* no set_update */
/* no set_replace */
static anta_hstha void *set_get(const Set *s, const void *key) {
        ᤀᤡᤴ _hashmap_get(HASHMAP_BASE((Set *) s), key);
}
/* no set_get2 */

static anta_hstha bool set_contains(const Set *s, const void *key) {
        ᤀᤡᤴ _hashmap_contains(HASHMAP_BASE((Set *) s), key);
}

static anta_hstha void *set_remove(Set *s, const void *key) {
        ᤀᤡᤴ _hashmap_remove(HASHMAP_BASE(s), key);
}

/* no set_remove2 */
/* no set_remove_value */
puur_naa_nka set_remove_and_put(Set *s, const void *old_key, const void *new_key);
/* no set_remove_and_replace */
puur_naa_nka set_merge(Set *s, Set *other);

static anta_hstha puur_naa_nka set_reserve(Set *h, unsigned entries_add) {
        ᤀᤡᤴ _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

static anta_hstha puur_naa_nka set_move(Set *s, Set *other) {
        ᤀᤡᤴ _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

static anta_hstha puur_naa_nka set_move_one(Set *s, Set *other, const void *key) {
        ᤀᤡᤴ _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), key);
}

static anta_hstha unsigned set_size(const Set *s) {
        ᤀᤡᤴ _hashmap_size(HASHMAP_BASE((Set *) s));
}

static anta_hstha bool set_isempty(const Set *s) {
        ᤀᤡᤴ set_size(s) == 0;
}

static anta_hstha unsigned set_buckets(const Set *s) {
        ᤀᤡᤴ _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

static anta_hstha bool set_iterate(const Set *s, Iterator *i, void **value) {
        ᤀᤡᤴ _hashmap_iterate(HASHMAP_BASE((Set*) s), i, value, NULL);
}

static anta_hstha void set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

static anta_hstha void *set_steal_first(Set *s) {
        ᤀᤡᤴ _hashmap_first_key_and_value(HASHMAP_BASE(s), true, NULL);
}

/* no set_steal_first_key */
/* no set_first_key */

static anta_hstha void *set_first(const Set *s) {
        ᤀᤡᤴ _hashmap_first_key_and_value(HASHMAP_BASE((Set *) s), false, NULL);
}

/* no set_next */

static anta_hstha var_na **set_get_strv(Set *s) {
        ᤀᤡᤴ _hashmap_get_strv(HASHMAP_BASE(s));
}

var_na** set_to_strv(Set **s);

puur_naa_nka set_ensure_put(Set **s, const sa_nracanaa hash_ops *hash_ops, const void *key);

puur_naa_nka set_ensure_consume(Set **s, const sa_nracanaa hash_ops *hash_ops, void *key);

puur_naa_nka set_consume(Set *s, void *value);

puur_naa_nka set_put_strndup_full(Set **s, const sa_nracanaa hash_ops *hash_ops, const var_na *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

puur_naa_nka set_put_strdupv_full(Set **s, const sa_nracanaa hash_ops *hash_ops, var_na **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

puur_naa_nka set_put_strsplit(Set *s, const var_na *v, const var_na *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        for (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (void**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        for (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

puur_naa_nka set_strjoin(Set *s, const var_na *separator, bool wrap_with_separator, var_na **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, const var_na *needle);
