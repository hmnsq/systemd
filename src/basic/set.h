/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(અચલ સંરચના hash_ops *hash_ops);

સ્થિર અંતઃરેખ Set* set_free(Set *s) {
        પાછા (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

પૂર્ણાંક set_ensure_allocated(Set **s, અચલ સંરચના hash_ops *hash_ops);

પૂર્ણાંક set_put(Set *s, અચલ શૂન્ય *ચાવી);
/* no set_update */
/* no set_replace */
સ્થિર અંતઃરેખ શૂન્ય *set_get(અચલ Set *s, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_get(HASHMAP_BASE((Set *) s), ચાવી);
}
/* no set_get2 */

સ્થિર અંતઃરેખ bool set_contains(અચલ Set *s, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_contains(HASHMAP_BASE((Set *) s), ચાવી);
}

સ્થિર અંતઃરેખ શૂન્ય *set_remove(Set *s, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_remove(HASHMAP_BASE(s), ચાવી);
}

/* no set_remove2 */
/* no set_remove_value */
પૂર્ણાંક set_remove_and_put(Set *s, અચલ શૂન્ય *old_ચાવી, અચલ શૂન્ય *new_ચાવી);
/* no set_remove_and_replace */
પૂર્ણાંક set_merge(Set *s, Set *other);

સ્થિર અંતઃરેખ પૂર્ણાંક set_reserve(Set *h, unsigned entries_add) {
        પાછા _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

સ્થિર અંતઃરેખ પૂર્ણાંક set_move(Set *s, Set *other) {
        પાછા _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

સ્થિર અંતઃરેખ પૂર્ણાંક set_move_one(Set *s, Set *other, અચલ શૂન્ય *ચાવી) {
        પાછા _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), ચાવી);
}

સ્થિર અંતઃરેખ unsigned set_size(અચલ Set *s) {
        પાછા _hashmap_size(HASHMAP_BASE((Set *) s));
}

સ્થિર અંતઃરેખ bool set_isempty(અચલ Set *s) {
        પાછા set_size(s) == 0;
}

સ્થિર અંતઃરેખ unsigned set_buckets(અચલ Set *s) {
        પાછા _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

સ્થિર અંતઃરેખ bool set_iterate(અચલ Set *s, Iterator *i, શૂન્ય **કિંમત) {
        પાછા _hashmap_iterate(HASHMAP_BASE((Set*) s), i, કિંમત, ખાલી);
}

સ્થિર અંતઃરેખ શૂન્ય set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

સ્થિર અંતઃરેખ શૂન્ય *set_steal_first(Set *s) {
        પાછા hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE(s), true, ખાલી);
}

/* no set_steal_first_key */
/* no set_first_key */

સ્થિર અંતઃરેખ શૂન્ય *set_first(અચલ Set *s) {
        પાછા hashmap_first_ચાવી_and_કિંમત(HASHMAP_BASE((Set *) s), false, ખાલી);
}

/* no set_next */

સ્થિર અંતઃરેખ અક્ષર **set_get_strv(Set *s) {
        પાછા _hashmap_get_strv(HASHMAP_BASE(s));
}

અક્ષર** set_to_strv(Set **s);

પૂર્ણાંક set_ensure_put(Set **s, અચલ સંરચના hash_ops *hash_ops, અચલ શૂન્ય *ચાવી);

પૂર્ણાંક set_ensure_consume(Set **s, અચલ સંરચના hash_ops *hash_ops, શૂન્ય *ચાવી);

પૂર્ણાંક set_consume(Set *s, શૂન્ય *કિંમત);

પૂર્ણાંક set_put_strndup_full(Set **s, અચલ સંરચના hash_ops *hash_ops, અચલ અક્ષર *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

પૂર્ણાંક set_put_strdupv_full(Set **s, અચલ સંરચના hash_ops *hash_ops, અક્ષર **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

પૂર્ણાંક set_put_strsplit(Set *s, અચલ અક્ષર *v, અચલ અક્ષર *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        માટે (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (શૂન્ય**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        માટે (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

પૂર્ણાંક set_strjoin(Set *s, અચલ અક્ષર *separator, bool wrap_with_separator, અક્ષર **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, અચલ અક્ષર *needle);
