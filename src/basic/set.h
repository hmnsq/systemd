/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"
#ضمن "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(ثابت بنية hash_ops *hash_ops);

ساكن مضمن Set* set_free(Set *s) {
        أرجع (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

صحيح set_ensure_allocated(Set **s, ثابت بنية hash_ops *hash_ops);

صحيح set_put(Set *s, ثابت فراغ *کلید);
/* no set_update */
/* no set_replace */
ساكن مضمن فراغ *set_get(ثابت Set *s, ثابت فراغ *کلید) {
        أرجع _hashmap_get(HASHMAP_BASE((Set *) s), کلید);
}
/* no set_get2 */

ساكن مضمن bool set_contains(ثابت Set *s, ثابت فراغ *کلید) {
        أرجع _hashmap_contains(HASHMAP_BASE((Set *) s), کلید);
}

ساكن مضمن فراغ *set_remove(Set *s, ثابت فراغ *کلید) {
        أرجع _hashmap_remove(HASHMAP_BASE(s), کلید);
}

/* no set_remove2 */
/* no set_remove_value */
صحيح set_remove_and_put(Set *s, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید);
/* no set_remove_and_replace */
صحيح set_merge(Set *s, Set *other);

ساكن مضمن صحيح set_reserve(Set *h, unsigned entries_add) {
        أرجع _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

ساكن مضمن صحيح set_move(Set *s, Set *other) {
        أرجع _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

ساكن مضمن صحيح set_move_one(Set *s, Set *other, ثابت فراغ *کلید) {
        أرجع _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), کلید);
}

ساكن مضمن unsigned set_size(ثابت Set *s) {
        أرجع _hashmap_size(HASHMAP_BASE((Set *) s));
}

ساكن مضمن bool set_isempty(ثابت Set *s) {
        أرجع set_size(s) == 0;
}

ساكن مضمن unsigned set_buckets(ثابت Set *s) {
        أرجع _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

ساكن مضمن bool set_iterate(ثابت Set *s, Iterator *i, فراغ **قدر) {
        أرجع _hashmap_iterate(HASHMAP_BASE((Set*) s), i, قدر, عدم);
}

ساكن مضمن فراغ set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

ساكن مضمن فراغ *set_steal_first(Set *s) {
        أرجع hashmap_first_کلید_and_قدر(HASHMAP_BASE(s), true, عدم);
}

/* no set_steal_first_key */
/* no set_first_key */

ساكن مضمن فراغ *set_first(ثابت Set *s) {
        أرجع hashmap_first_کلید_and_قدر(HASHMAP_BASE((Set *) s), false, عدم);
}

/* no set_next */

ساكن مضمن حرف **set_get_strv(Set *s) {
        أرجع _hashmap_get_strv(HASHMAP_BASE(s));
}

حرف** set_to_strv(Set **s);

صحيح set_ensure_put(Set **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید);

صحيح set_ensure_consume(Set **s, ثابت بنية hash_ops *hash_ops, فراغ *کلید);

صحيح set_consume(Set *s, فراغ *قدر);

صحيح set_put_strndup_full(Set **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

صحيح set_put_strdupv_full(Set **s, ثابت بنية hash_ops *hash_ops, حرف **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

صحيح set_put_strsplit(Set *s, ثابت حرف *v, ثابت حرف *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        لكل (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (فراغ**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        لكل (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

صحيح set_strjoin(Set *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ثابت حرف *needle);
