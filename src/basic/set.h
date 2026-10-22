/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(மாறிலி கட்டமைப்பு hash_ops *hash_ops);

நிலையான உள்வரி Set* set_free(Set *s) {
        திரும்பு (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

முழு set_ensure_allocated(Set **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops);

முழு set_put(Set *s, மாறிலி வெறுமை *சாவி);
/* no set_update */
/* no set_replace */
நிலையான உள்வரி வெறுமை *set_get(மாறிலி Set *s, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_get(HASHMAP_BASE((Set *) s), சாவி);
}
/* no set_get2 */

நிலையான உள்வரி bool set_contains(மாறிலி Set *s, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_contains(HASHMAP_BASE((Set *) s), சாவி);
}

நிலையான உள்வரி வெறுமை *set_remove(Set *s, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_remove(HASHMAP_BASE(s), சாவி);
}

/* no set_remove2 */
/* no set_remove_value */
முழு set_remove_and_put(Set *s, மாறிலி வெறுமை *old_சாவி, மாறிலி வெறுமை *new_சாவி);
/* no set_remove_and_replace */
முழு set_merge(Set *s, Set *other);

நிலையான உள்வரி முழு set_reserve(Set *h, unsigned entries_add) {
        திரும்பு _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

நிலையான உள்வரி முழு set_move(Set *s, Set *other) {
        திரும்பு _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

நிலையான உள்வரி முழு set_move_one(Set *s, Set *other, மாறிலி வெறுமை *சாவி) {
        திரும்பு _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), சாவி);
}

நிலையான உள்வரி unsigned set_size(மாறிலி Set *s) {
        திரும்பு _hashmap_size(HASHMAP_BASE((Set *) s));
}

நிலையான உள்வரி bool set_isempty(மாறிலி Set *s) {
        திரும்பு set_size(s) == 0;
}

நிலையான உள்வரி unsigned set_buckets(மாறிலி Set *s) {
        திரும்பு _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

நிலையான உள்வரி bool set_iterate(மாறிலி Set *s, Iterator *i, வெறுமை **மதிப்பு) {
        திரும்பு _hashmap_iterate(HASHMAP_BASE((Set*) s), i, மதிப்பு, வெற்று);
}

நிலையான உள்வரி வெறுமை set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

நிலையான உள்வரி வெறுமை *set_steal_first(Set *s) {
        திரும்பு hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE(s), true, வெற்று);
}

/* no set_steal_first_key */
/* no set_first_key */

நிலையான உள்வரி வெறுமை *set_first(மாறிலி Set *s) {
        திரும்பு hashmap_first_சாவி_and_மதிப்பு(HASHMAP_BASE((Set *) s), false, வெற்று);
}

/* no set_next */

நிலையான உள்வரி எழுத்து **set_get_strv(Set *s) {
        திரும்பு _hashmap_get_strv(HASHMAP_BASE(s));
}

எழுத்து** set_to_strv(Set **s);

முழு set_ensure_put(Set **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops, மாறிலி வெறுமை *சாவி);

முழு set_ensure_consume(Set **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops, வெறுமை *சாவி);

முழு set_consume(Set *s, வெறுமை *மதிப்பு);

முழு set_put_strndup_full(Set **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops, மாறிலி எழுத்து *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

முழு set_put_strdupv_full(Set **s, மாறிலி கட்டமைப்பு hash_ops *hash_ops, எழுத்து **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

முழு set_put_strsplit(Set *s, மாறிலி எழுத்து *v, மாறிலி எழுத்து *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        க்கு (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (வெறுமை**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        க்கு (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

முழு set_strjoin(Set *s, மாறிலி எழுத்து *separator, bool wrap_with_separator, எழுத்து **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, மாறிலி எழுத்து *needle);
