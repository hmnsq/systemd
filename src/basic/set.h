/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"
#অন্তর্ভাৱ "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(ধ্রুৱ সংরচনা hash_ops *hash_ops);

স্থির অন্তঃস্থ Set* set_free(Set *s) {
        ঘূৰাই (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

পূর্ণাঙ্ক set_ensure_allocated(Set **s, ধ্রুৱ সংরচনা hash_ops *hash_ops);

পূর্ণাঙ্ক set_put(Set *s, ধ্রুৱ শূন্য *চাবি);
/* no set_update */
/* no set_replace */
স্থির অন্তঃস্থ শূন্য *set_get(ধ্রুৱ Set *s, ধ্রুৱ শূন্য *চাবি) {
        ঘূৰাই _hashmap_get(HASHMAP_BASE((Set *) s), চাবি);
}
/* no set_get2 */

স্থির অন্তঃস্থ bool set_contains(ধ্রুৱ Set *s, ধ্রুৱ শূন্য *চাবি) {
        ঘূৰাই _hashmap_contains(HASHMAP_BASE((Set *) s), চাবি);
}

স্থির অন্তঃস্থ শূন্য *set_remove(Set *s, ধ্রুৱ শূন্য *চাবি) {
        ঘূৰাই _hashmap_remove(HASHMAP_BASE(s), চাবি);
}

/* no set_remove2 */
/* no set_remove_value */
পূর্ণাঙ্ক set_remove_and_put(Set *s, ধ্রুৱ শূন্য *old_চাবি, ধ্রুৱ শূন্য *new_চাবি);
/* no set_remove_and_replace */
পূর্ণাঙ্ক set_merge(Set *s, Set *other);

স্থির অন্তঃস্থ পূর্ণাঙ্ক set_reserve(Set *h, unsigned entries_add) {
        ঘূৰাই _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক set_move(Set *s, Set *other) {
        ঘূৰাই _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক set_move_one(Set *s, Set *other, ধ্রুৱ শূন্য *চাবি) {
        ঘূৰাই _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), চাবি);
}

স্থির অন্তঃস্থ unsigned set_size(ধ্রুৱ Set *s) {
        ঘূৰাই _hashmap_size(HASHMAP_BASE((Set *) s));
}

স্থির অন্তঃস্থ bool set_isempty(ধ্রুৱ Set *s) {
        ঘূৰাই set_size(s) == 0;
}

স্থির অন্তঃস্থ unsigned set_buckets(ধ্রুৱ Set *s) {
        ঘূৰাই _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

স্থির অন্তঃস্থ bool set_iterate(ধ্রুৱ Set *s, Iterator *i, শূন্য **মান) {
        ঘূৰাই _hashmap_iterate(HASHMAP_BASE((Set*) s), i, মান, রিক্ত);
}

স্থির অন্তঃস্থ শূন্য set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

স্থির অন্তঃস্থ শূন্য *set_steal_first(Set *s) {
        ঘূৰাই hashmap_first_চাবি_and_মান(HASHMAP_BASE(s), true, রিক্ত);
}

/* no set_steal_first_key */
/* no set_first_key */

স্থির অন্তঃস্থ শূন্য *set_first(ধ্রুৱ Set *s) {
        ঘূৰাই hashmap_first_চাবি_and_মান(HASHMAP_BASE((Set *) s), false, রিক্ত);
}

/* no set_next */

স্থির অন্তঃস্থ ৱর্ণ **set_get_strv(Set *s) {
        ঘূৰাই _hashmap_get_strv(HASHMAP_BASE(s));
}

ৱর্ণ** set_to_strv(Set **s);

পূর্ণাঙ্ক set_ensure_put(Set **s, ধ্রুৱ সংরচনা hash_ops *hash_ops, ধ্রুৱ শূন্য *চাবি);

পূর্ণাঙ্ক set_ensure_consume(Set **s, ধ্রুৱ সংরচনা hash_ops *hash_ops, শূন্য *চাবি);

পূর্ণাঙ্ক set_consume(Set *s, শূন্য *মান);

পূর্ণাঙ্ক set_put_strndup_full(Set **s, ধ্রুৱ সংরচনা hash_ops *hash_ops, ধ্রুৱ ৱর্ণ *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

পূর্ণাঙ্ক set_put_strdupv_full(Set **s, ধ্রুৱ সংরচনা hash_ops *hash_ops, ৱর্ণ **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

পূর্ণাঙ্ক set_put_strsplit(Set *s, ধ্রুৱ ৱর্ণ *v, ধ্রুৱ ৱর্ণ *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        বাবে (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (শূন্য**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        বাবে (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

পূর্ণাঙ্ক set_strjoin(Set *s, ধ্রুৱ ৱর্ণ *separator, bool wrap_with_separator, ৱর্ণ **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ধ্রুৱ ৱর্ণ *needle);
