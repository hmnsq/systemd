/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(സ്ഥിരം ഘടന hash_ops *hash_ops);

സ്ഥിതം വരിയിൽ Set* set_free(Set *s) {
        തിരികെ (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

പൂർണ്ണസംഖ്യ set_ensure_allocated(Set **s, സ്ഥിരം ഘടന hash_ops *hash_ops);

പൂർണ്ണസംഖ്യ set_put(Set *s, സ്ഥിരം ശൂന്യം *താക്കോൽ);
/* no set_update */
/* no set_replace */
സ്ഥിതം വരിയിൽ ശൂന്യം *set_get(സ്ഥിരം Set *s, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_get(HASHMAP_BASE((Set *) s), താക്കോൽ);
}
/* no set_get2 */

സ്ഥിതം വരിയിൽ bool set_contains(സ്ഥിരം Set *s, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_contains(HASHMAP_BASE((Set *) s), താക്കോൽ);
}

സ്ഥിതം വരിയിൽ ശൂന്യം *set_remove(Set *s, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_remove(HASHMAP_BASE(s), താക്കോൽ);
}

/* no set_remove2 */
/* no set_remove_value */
പൂർണ്ണസംഖ്യ set_remove_and_put(Set *s, സ്ഥിരം ശൂന്യം *old_താക്കോൽ, സ്ഥിരം ശൂന്യം *new_താക്കോൽ);
/* no set_remove_and_replace */
പൂർണ്ണസംഖ്യ set_merge(Set *s, Set *other);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ set_reserve(Set *h, unsigned entries_add) {
        തിരികെ _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ set_move(Set *s, Set *other) {
        തിരികെ _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ set_move_one(Set *s, Set *other, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        തിരികെ _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), താക്കോൽ);
}

സ്ഥിതം വരിയിൽ unsigned set_size(സ്ഥിരം Set *s) {
        തിരികെ _hashmap_size(HASHMAP_BASE((Set *) s));
}

സ്ഥിതം വരിയിൽ bool set_isempty(സ്ഥിരം Set *s) {
        തിരികെ set_size(s) == 0;
}

സ്ഥിതം വരിയിൽ unsigned set_buckets(സ്ഥിരം Set *s) {
        തിരികെ _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

സ്ഥിതം വരിയിൽ bool set_iterate(സ്ഥിരം Set *s, Iterator *i, ശൂന്യം **മൂല്യം) {
        തിരികെ _hashmap_iterate(HASHMAP_BASE((Set*) s), i, മൂല്യം, ശൂന്യ);
}

സ്ഥിതം വരിയിൽ ശൂന്യം set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

സ്ഥിതം വരിയിൽ ശൂന്യം *set_steal_first(Set *s) {
        തിരികെ hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE(s), true, ശൂന്യ);
}

/* no set_steal_first_key */
/* no set_first_key */

സ്ഥിതം വരിയിൽ ശൂന്യം *set_first(സ്ഥിരം Set *s) {
        തിരികെ hashmap_first_താക്കോൽ_and_മൂല്യം(HASHMAP_BASE((Set *) s), false, ശൂന്യ);
}

/* no set_next */

സ്ഥിതം വരിയിൽ അക്ഷരം **set_get_strv(Set *s) {
        തിരികെ _hashmap_get_strv(HASHMAP_BASE(s));
}

അക്ഷരം** set_to_strv(Set **s);

പൂർണ്ണസംഖ്യ set_ensure_put(Set **s, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം ശൂന്യം *താക്കോൽ);

പൂർണ്ണസംഖ്യ set_ensure_consume(Set **s, സ്ഥിരം ഘടന hash_ops *hash_ops, ശൂന്യം *താക്കോൽ);

പൂർണ്ണസംഖ്യ set_consume(Set *s, ശൂന്യം *മൂല്യം);

പൂർണ്ണസംഖ്യ set_put_strndup_full(Set **s, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം അക്ഷരം *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

പൂർണ്ണസംഖ്യ set_put_strdupv_full(Set **s, സ്ഥിരം ഘടന hash_ops *hash_ops, അക്ഷരം **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

പൂർണ്ണസംഖ്യ set_put_strsplit(Set *s, സ്ഥിരം അക്ഷരം *v, സ്ഥിരം അക്ഷരം *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        വേണ്ടി (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (ശൂന്യം**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        വേണ്ടി (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

പൂർണ്ണസംഖ്യ set_strjoin(Set *s, സ്ഥിരം അക്ഷരം *separator, bool wrap_with_separator, അക്ഷരം **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, സ്ഥിരം അക്ഷരം *needle);
