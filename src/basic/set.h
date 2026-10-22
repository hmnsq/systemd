/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hashmap.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_free_and_replace(a, b)              \
        free_and_replace_ਭਰਿਆ(a, b, ਸੈੱਟ_free)

ਸੈੱਟ_2* ਸੈੱਟ_new(ਅਚਲ ਬਣਤਰ hash_ops *hash_ops);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਸੈੱਟ_2* ਸੈੱਟ_free(ਸੈੱਟ_2 *s) {
        ਵਾਪਸ (ਸੈੱਟ_2*) _hashmap_free(HASHMAP_ਅਧਾਰ(s));
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_copy(s) ((ਸੈੱਟ_2*) _hashmap_copy(HASHMAP_ਅਧਾਰ(s)))

ਪੂਰਨ ਸੈੱਟ_ensure_allocated(ਸੈੱਟ_2 **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops);

ਪੂਰਨ ਸੈੱਟ_put(ਸੈੱਟ_2 *s, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ);
/* no set_update */
/* no set_replace */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ਸੈੱਟ_get(ਅਚਲ ਸੈੱਟ_2 *s, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_get(HASHMAP_ਅਧਾਰ((ਸੈੱਟ_2 *) s), ਕੁੰਜੀ);
}
/* no set_get2 */

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਸੈੱਟ_ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ ਸੈੱਟ_2 *s, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ hashmap_ਸ਼ਾਮਲ_ਹੈ_2(HASHMAP_ਅਧਾਰ((ਸੈੱਟ_2 *) s), ਕੁੰਜੀ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ਸੈੱਟ_remove(ਸੈੱਟ_2 *s, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_remove(HASHMAP_ਅਧਾਰ(s), ਕੁੰਜੀ);
}

/* no set_remove2 */
/* no set_remove_value */
ਪੂਰਨ ਸੈੱਟ_remove_and_put(ਸੈੱਟ_2 *s, ਅਚਲ ਖਾਲੀ *old_ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *new_ਕੁੰਜੀ);
/* no set_remove_and_replace */
ਪੂਰਨ ਸੈੱਟ_ਮਿਲਾਓ(ਸੈੱਟ_2 *s, ਸੈੱਟ_2 *other);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੈੱਟ_reserve(ਸੈੱਟ_2 *h, ਬਿਨਾਨਿਸ਼ਾਨੀ entries_ਜੋੜੋ) {
        ਵਾਪਸ _hashmap_reserve(HASHMAP_ਅਧਾਰ(h), entries_ਜੋੜੋ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੈੱਟ_move(ਸੈੱਟ_2 *s, ਸੈੱਟ_2 *other) {
        ਵਾਪਸ _hashmap_move(HASHMAP_ਅਧਾਰ(s), HASHMAP_ਅਧਾਰ(other));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੈੱਟ_move_one(ਸੈੱਟ_2 *s, ਸੈੱਟ_2 *other, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਵਾਪਸ _hashmap_move_one(HASHMAP_ਅਧਾਰ(s), HASHMAP_ਅਧਾਰ(other), ਕੁੰਜੀ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੈੱਟ_ਮਾਪ(ਅਚਲ ਸੈੱਟ_2 *s) {
        ਵਾਪਸ hashmap_ਮਾਪ(HASHMAP_ਅਧਾਰ((ਸੈੱਟ_2 *) s));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਸੈੱਟ_isempty(ਅਚਲ ਸੈੱਟ_2 *s) {
        ਵਾਪਸ ਸੈੱਟ_ਮਾਪ(s) == 0;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੈੱਟ_buckets(ਅਚਲ ਸੈੱਟ_2 *s) {
        ਵਾਪਸ _hashmap_buckets(HASHMAP_ਅਧਾਰ((ਸੈੱਟ_2 *) s));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਸੈੱਟ_iterate(ਅਚਲ ਸੈੱਟ_2 *s, Iterator *i, ਖਾਲੀ **ਮੁੱਲ) {
        ਵਾਪਸ _hashmap_iterate(HASHMAP_ਅਧਾਰ((ਸੈੱਟ_2*) s), i, ਮੁੱਲ, ਨਿਰਾ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ_clear(ਸੈੱਟ_2 *s) {
        _hashmap_clear(HASHMAP_ਅਧਾਰ(s));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ਸੈੱਟ_steal_ਪਹਿਲਾ(ਸੈੱਟ_2 *s) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ(s), true, ਨਿਰਾ);
}

/* no set_steal_first_key */
/* no set_first_key */

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ਸੈੱਟ_ਪਹਿਲਾ(ਅਚਲ ਸੈੱਟ_2 *s) {
        ਵਾਪਸ hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(HASHMAP_ਅਧਾਰ((ਸੈੱਟ_2 *) s), false, ਨਿਰਾ);
}

/* no set_next */

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ **ਸੈੱਟ_get_strv(ਸੈੱਟ_2 *s) {
        ਵਾਪਸ _hashmap_get_strv(HASHMAP_ਅਧਾਰ(s));
}

ਅੱਖਰ** ਸੈੱਟ_ਮੰਜ਼ਿਲ_strv(ਸੈੱਟ_2 **s);

ਪੂਰਨ ਸੈੱਟ_ensure_put(ਸੈੱਟ_2 **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ);

ਪੂਰਨ ਸੈੱਟ_ensure_consume(ਸੈੱਟ_2 **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਖਾਲੀ *ਕੁੰਜੀ);

ਪੂਰਨ ਸੈੱਟ_consume(ਸੈੱਟ_2 *s, ਖਾਲੀ *ਮੁੱਲ);

ਪੂਰਨ ਸੈੱਟ_put_strndup_ਭਰਿਆ(ਸੈੱਟ_2 **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਅੱਖਰ *p, size_t n);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_put_strdup_ਭਰਿਆ(s, hash_ops, p) ਸੈੱਟ_put_strndup_ਭਰਿਆ(s, hash_ops, p, SIZE_MAX)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_put_strndup(s, p, n) ਸੈੱਟ_put_strndup_ਭਰਿਆ(s, &string_hash_ops_free, p, n)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_put_strdup(s, p) ਸੈੱਟ_put_strndup(s, p, SIZE_MAX)

ਪੂਰਨ ਸੈੱਟ_put_strdupv_ਭਰਿਆ(ਸੈੱਟ_2 **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅੱਖਰ **l);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_put_strdupv(s, l) ਸੈੱਟ_put_strdupv_ਭਰਿਆ(s, &string_hash_ops_free, l)

ਪੂਰਨ ਸੈੱਟ_put_strsplit(ਸੈੱਟ_2 *s, ਅਚਲ ਅੱਖਰ *v, ਅਚਲ ਅੱਖਰ *separators, ਕੱਢੋ_Flags flags);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_FOREACH_2(e, s, i) \
        ਲਈ (Iterator i = ITERATOR_ਪਹਿਲਾ; ਸੈੱਟ_iterate((s), &i, (ਖਾਲੀ**)&(e)); )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_FOREACH(e, s) \
        ਸੈੱਟ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_FOREACH_MOVE(e, d, s)                                       \
        ਲਈ (; ({ e = ਸੈੱਟ_ਪਹਿਲਾ(s); assert_se(!e || ਸੈੱਟ_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(ਸੈੱਟ_2*, ਸੈੱਟ_free);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cleanup_ਸੈੱਟ_free _cleanup_(ਸੈੱਟ_freep)

ਪੂਰਨ ਸੈੱਟ_strjoin(ਸੈੱਟ_2 *s, ਅਚਲ ਅੱਖਰ *separator, bool wrap_with_separator, ਅੱਖਰ **ret);

bool ਸੈੱਟ_equal(ਸੈੱਟ_2 *a, ਸੈੱਟ_2 *b);

bool ਸੈੱਟ_fnmatch(ਸੈੱਟ_2 *include_patterns, ਸੈੱਟ_2 *exclude_patterns, ਅਚਲ ਅੱਖਰ *needle);
