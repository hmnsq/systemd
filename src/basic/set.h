/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"
#شامل "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(ثابت ساختار hash_ops *hash_ops);

ایستا درونخطی Set* set_free(Set *s) {
        بازگشت (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

صحیح set_ensure_allocated(Set **s, ثابت ساختار hash_ops *hash_ops);

صحیح set_put(Set *s, ثابت تهی *کلید);
/* no set_update */
/* no set_replace */
ایستا درونخطی تهی *set_get(ثابت Set *s, ثابت تهی *کلید) {
        بازگشت _hashmap_get(HASHMAP_BASE((Set *) s), کلید);
}
/* no set_get2 */

ایستا درونخطی bool set_contains(ثابت Set *s, ثابت تهی *کلید) {
        بازگشت _hashmap_contains(HASHMAP_BASE((Set *) s), کلید);
}

ایستا درونخطی تهی *set_remove(Set *s, ثابت تهی *کلید) {
        بازگشت _hashmap_remove(HASHMAP_BASE(s), کلید);
}

/* no set_remove2 */
/* no set_remove_value */
صحیح set_remove_and_put(Set *s, ثابت تهی *old_کلید, ثابت تهی *new_کلید);
/* no set_remove_and_replace */
صحیح set_merge(Set *s, Set *other);

ایستا درونخطی صحیح set_reserve(Set *h, unsigned entries_add) {
        بازگشت _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

ایستا درونخطی صحیح set_move(Set *s, Set *other) {
        بازگشت _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

ایستا درونخطی صحیح set_move_one(Set *s, Set *other, ثابت تهی *کلید) {
        بازگشت _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), کلید);
}

ایستا درونخطی unsigned set_size(ثابت Set *s) {
        بازگشت _hashmap_size(HASHMAP_BASE((Set *) s));
}

ایستا درونخطی bool set_isempty(ثابت Set *s) {
        بازگشت set_size(s) == 0;
}

ایستا درونخطی unsigned set_buckets(ثابت Set *s) {
        بازگشت _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

ایستا درونخطی bool set_iterate(ثابت Set *s, Iterator *i, تهی **مقدار) {
        بازگشت _hashmap_iterate(HASHMAP_BASE((Set*) s), i, مقدار, پوچ);
}

ایستا درونخطی تهی set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

ایستا درونخطی تهی *set_steal_first(Set *s) {
        بازگشت hashmap_first_کلید_and_مقدار(HASHMAP_BASE(s), true, پوچ);
}

/* no set_steal_first_key */
/* no set_first_key */

ایستا درونخطی تهی *set_first(ثابت Set *s) {
        بازگشت hashmap_first_کلید_and_مقدار(HASHMAP_BASE((Set *) s), false, پوچ);
}

/* no set_next */

ایستا درونخطی نویسه **set_get_strv(Set *s) {
        بازگشت _hashmap_get_strv(HASHMAP_BASE(s));
}

نویسه** set_to_strv(Set **s);

صحیح set_ensure_put(Set **s, ثابت ساختار hash_ops *hash_ops, ثابت تهی *کلید);

صحیح set_ensure_consume(Set **s, ثابت ساختار hash_ops *hash_ops, تهی *کلید);

صحیح set_consume(Set *s, تهی *مقدار);

صحیح set_put_strndup_full(Set **s, ثابت ساختار hash_ops *hash_ops, ثابت نویسه *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

صحیح set_put_strdupv_full(Set **s, ثابت ساختار hash_ops *hash_ops, نویسه **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

صحیح set_put_strsplit(Set *s, ثابت نویسه *v, ثابت نویسه *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        برای (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (تهی**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        برای (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

صحیح set_strjoin(Set *s, ثابت نویسه *separator, bool wrap_with_separator, نویسه **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ثابت نویسه *needle);
