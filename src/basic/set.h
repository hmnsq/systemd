/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(const struct hash_ops *hash_ops);

static inline Set* set_free(Set *s) {
        반환 (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

정수 set_ensure_allocated(Set **s, const struct hash_ops *hash_ops);

정수 set_put(Set *s, const void *key);
/* no set_update */
/* no set_replace */
static inline void *set_get(const Set *s, const void *key) {
        반환 _hashmap_get(HASHMAP_BASE((Set *) s), key);
}
/* no set_get2 */

static inline bool set_contains(const Set *s, const void *key) {
        반환 _hashmap_contains(HASHMAP_BASE((Set *) s), key);
}

static inline void *set_remove(Set *s, const void *key) {
        반환 _hashmap_remove(HASHMAP_BASE(s), key);
}

/* no set_remove2 */
/* no set_remove_value */
정수 set_remove_and_put(Set *s, const void *old_key, const void *new_key);
/* no set_remove_and_replace */
정수 set_merge(Set *s, Set *other);

static inline 정수 set_reserve(Set *h, unsigned entries_add) {
        반환 _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

static inline 정수 set_move(Set *s, Set *other) {
        반환 _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

static inline 정수 set_move_one(Set *s, Set *other, const void *key) {
        반환 _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), key);
}

static inline unsigned set_size(const Set *s) {
        반환 _hashmap_size(HASHMAP_BASE((Set *) s));
}

static inline bool set_isempty(const Set *s) {
        반환 set_size(s) == 0;
}

static inline unsigned set_buckets(const Set *s) {
        반환 _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

static inline bool set_iterate(const Set *s, Iterator *i, void **value) {
        반환 _hashmap_iterate(HASHMAP_BASE((Set*) s), i, value, NULL);
}

static inline void set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

static inline void *set_steal_first(Set *s) {
        반환 _hashmap_first_key_and_value(HASHMAP_BASE(s), true, NULL);
}

/* no set_steal_first_key */
/* no set_first_key */

static inline void *set_first(const Set *s) {
        반환 _hashmap_first_key_and_value(HASHMAP_BASE((Set *) s), false, NULL);
}

/* no set_next */

static inline char **set_get_strv(Set *s) {
        반환 _hashmap_get_strv(HASHMAP_BASE(s));
}

char** set_to_strv(Set **s);

정수 set_ensure_put(Set **s, const struct hash_ops *hash_ops, const void *key);

정수 set_ensure_consume(Set **s, const struct hash_ops *hash_ops, void *key);

정수 set_consume(Set *s, void *value);

정수 set_put_strndup_full(Set **s, const struct hash_ops *hash_ops, const char *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

정수 set_put_strdupv_full(Set **s, const struct hash_ops *hash_ops, char **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

정수 set_put_strsplit(Set *s, const char *v, const char *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        위해 (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (void**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        위해 (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

정수 set_strjoin(Set *s, const char *separator, bool wrap_with_separator, char **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, const char *needle);
