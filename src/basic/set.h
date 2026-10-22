/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "hashmap.h"

#define set_free_and_replace(a, b)              \
        free_and_replace_full(a, b, set_free)

Set* set_new(ଅଚଳ ସଂରଚନା hash_ops *hash_ops);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ Set* set_free(Set *s) {
        ଫେରନ୍ତୁ (Set*) _hashmap_free(HASHMAP_BASE(s));
}

#define set_copy(s) ((Set*) _hashmap_copy(HASHMAP_BASE(s)))

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_ensure_allocated(Set **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_put(Set *s, ଅଚଳ ଶୂନ୍ୟ *ଚାବି);
/* no set_update */
/* no set_replace */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *set_get(ଅଚଳ Set *s, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_get(HASHMAP_BASE((Set *) s), ଚାବି);
}
/* no set_get2 */

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool set_contains(ଅଚଳ Set *s, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_contains(HASHMAP_BASE((Set *) s), ଚାବି);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *set_remove(Set *s, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_remove(HASHMAP_BASE(s), ଚାବି);
}

/* no set_remove2 */
/* no set_remove_value */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_remove_and_put(Set *s, ଅଚଳ ଶୂନ୍ୟ *old_ଚାବି, ଅଚଳ ଶୂନ୍ୟ *new_ଚାବି);
/* no set_remove_and_replace */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_merge(Set *s, Set *other);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_reserve(Set *h, unsigned entries_add) {
        ଫେରନ୍ତୁ _hashmap_reserve(HASHMAP_BASE(h), entries_add);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_move(Set *s, Set *other) {
        ଫେରନ୍ତୁ _hashmap_move(HASHMAP_BASE(s), HASHMAP_BASE(other));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_move_one(Set *s, Set *other, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ଫେରନ୍ତୁ _hashmap_move_one(HASHMAP_BASE(s), HASHMAP_BASE(other), ଚାବି);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned set_size(ଅଚଳ Set *s) {
        ଫେରନ୍ତୁ _hashmap_size(HASHMAP_BASE((Set *) s));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool set_isempty(ଅଚଳ Set *s) {
        ଫେରନ୍ତୁ set_size(s) == 0;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned set_buckets(ଅଚଳ Set *s) {
        ଫେରନ୍ତୁ _hashmap_buckets(HASHMAP_BASE((Set *) s));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool set_iterate(ଅଚଳ Set *s, Iterator *i, ଶୂନ୍ୟ **ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ _hashmap_iterate(HASHMAP_BASE((Set*) s), i, ମୂଲ୍ୟ, ଖାଲି);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ set_clear(Set *s) {
        _hashmap_clear(HASHMAP_BASE(s));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *set_steal_first(Set *s) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE(s), true, ଖାଲି);
}

/* no set_steal_first_key */
/* no set_first_key */

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *set_first(ଅଚଳ Set *s) {
        ଫେରନ୍ତୁ hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HASHMAP_BASE((Set *) s), false, ଖାଲି);
}

/* no set_next */

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର **set_get_strv(Set *s) {
        ଫେରନ୍ତୁ _hashmap_get_strv(HASHMAP_BASE(s));
}

ଅକ୍ଷର** set_to_strv(Set **s);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_ensure_put(Set **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଶୂନ୍ୟ *ଚାବି);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_ensure_consume(Set **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଶୂନ୍ୟ *ଚାବି);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_consume(Set *s, ଶୂନ୍ୟ *ମୂଲ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_put_strndup_full(Set **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଅକ୍ଷର *p, size_t n);
#define set_put_strdup_full(s, hash_ops, p) set_put_strndup_full(s, hash_ops, p, SIZE_MAX)
#define set_put_strndup(s, p, n) set_put_strndup_full(s, &string_hash_ops_free, p, n)
#define set_put_strdup(s, p) set_put_strndup(s, p, SIZE_MAX)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_put_strdupv_full(Set **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅକ୍ଷର **l);
#define set_put_strdupv(s, l) set_put_strdupv_full(s, &string_hash_ops_free, l)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_put_strsplit(Set *s, ଅଚଳ ଅକ୍ଷର *v, ଅଚଳ ଅକ୍ଷର *separators, ExtractFlags flags);

#define _SET_FOREACH(e, s, i) \
        ପାଇଁ (Iterator i = ITERATOR_FIRST; set_iterate((s), &i, (ଶୂନ୍ୟ**)&(e)); )
#define SET_FOREACH(e, s) \
        _SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

#define SET_FOREACH_MOVE(e, d, s)                                       \
        ପାଇଁ (; ({ e = set_first(s); assert_se(!e || set_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(Set*, set_free);

#define _cleanup_set_free_ _cleanup_(set_freep)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_strjoin(Set *s, ଅଚଳ ଅକ୍ଷର *separator, bool wrap_with_separator, ଅକ୍ଷର **ret);

bool set_equal(Set *a, Set *b);

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ଅଚଳ ଅକ୍ଷର *needle);
