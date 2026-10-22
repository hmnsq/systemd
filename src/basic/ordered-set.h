/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hashmap.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Ordered_ਸੈੱਟ Ordered_ਸੈੱਟ;

ਸਥਿਰ ਸਤਰਅੰਦਰ Ordered_ਸੈੱਟ* ordered_ਸੈੱਟ_new(ਅਚਲ ਬਣਤਰ hash_ops *ops) {
        ਵਾਪਸ (Ordered_ਸੈੱਟ*) ordered_hashmap_new(ops);
}

ਪੂਰਨ ordered_ਸੈੱਟ_ensure_allocated(Ordered_ਸੈੱਟ **s, ਅਚਲ ਬਣਤਰ hash_ops *ops);

ਪੂਰਨ ordered_ਸੈੱਟ_ensure_put(Ordered_ਸੈੱਟ **s, ਅਚਲ ਬਣਤਰ hash_ops *ops, ਖਾਲੀ *p);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ordered_ਸੈੱਟ_clear(Ordered_ਸੈੱਟ *s) {
        ਵਾਪਸ ordered_hashmap_clear((OrderedHashmap*) s);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ Ordered_ਸੈੱਟ* ordered_ਸੈੱਟ_free(Ordered_ਸੈੱਟ *s) {
        ਵਾਪਸ (Ordered_ਸੈੱਟ*) ordered_hashmap_free((OrderedHashmap*) s);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_ਸੈੱਟ_ਸ਼ਾਮਲ_ਹੈ(Ordered_ਸੈੱਟ *s, ਅਚਲ ਖਾਲੀ *p) {
        ਵਾਪਸ ordered_hashmap_ਸ਼ਾਮਲ_ਹੈ((OrderedHashmap*) s, p);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_ਸੈੱਟ_put(Ordered_ਸੈੱਟ *s, ਖਾਲੀ *p) {
        ਵਾਪਸ ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *ordered_ਸੈੱਟ_get(Ordered_ਸੈੱਟ *s, ਅਚਲ ਖਾਲੀ *p) {
        ਵਾਪਸ ordered_hashmap_get((OrderedHashmap*) s, p);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ordered_ਸੈੱਟ_ਮਾਪ(Ordered_ਸੈੱਟ *s) {
        ਵਾਪਸ ordered_hashmap_ਮਾਪ((OrderedHashmap*) s);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ordered_ਸੈੱਟ_isempty(Ordered_ਸੈੱਟ *s) {
        ਵਾਪਸ ordered_hashmap_isempty((OrderedHashmap*) s);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ordered_ਸੈੱਟ_iterate(Ordered_ਸੈੱਟ *s, Iterator *i, ਖਾਲੀ **ਮੁੱਲ) {
        ਵਾਪਸ ordered_hashmap_iterate((OrderedHashmap*) s, i, ਮੁੱਲ, ਨਿਰਾ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* ordered_ਸੈੱਟ_remove(Ordered_ਸੈੱਟ *s, ਖਾਲੀ *p) {
        ਵਾਪਸ ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* ordered_ਸੈੱਟ_ਪਹਿਲਾ(Ordered_ਸੈੱਟ *s) {
        ਵਾਪਸ ordered_hashmap_ਪਹਿਲਾ((OrderedHashmap*) s);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* ordered_ਸੈੱਟ_steal_ਪਹਿਲਾ(Ordered_ਸੈੱਟ *s) {
        ਵਾਪਸ ordered_hashmap_steal_ਪਹਿਲਾ((OrderedHashmap*) s);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ** ordered_ਸੈੱਟ_get_strv(Ordered_ਸੈੱਟ *s) {
        ਵਾਪਸ _hashmap_get_strv(HASHMAP_ਅਧਾਰ((OrderedHashmap*) s));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ordered_ਸੈੱਟ_reserve(Ordered_ਸੈੱਟ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ entries_ਜੋੜੋ) {
        ਵਾਪਸ ordered_hashmap_reserve((OrderedHashmap*) s, entries_ਜੋੜੋ);
}

ਪੂਰਨ ordered_ਸੈੱਟ_consume(Ordered_ਸੈੱਟ *s, ਖਾਲੀ *p);

ਪੂਰਨ ordered_ਸੈੱਟ_put_strdup_ਭਰਿਆ(Ordered_ਸੈੱਟ **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਅੱਖਰ *p);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ordered_ਸੈੱਟ_put_strdup(s, p) ordered_ਸੈੱਟ_put_strdup_ਭਰਿਆ(s, &string_hash_ops_free, p)
ਪੂਰਨ ordered_ਸੈੱਟ_put_strdupv_ਭਰਿਆ(Ordered_ਸੈੱਟ **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅੱਖਰ **l);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ordered_ਸੈੱਟ_put_strdupv(s, l) ordered_ਸੈੱਟ_put_strdupv_ਭਰਿਆ(s, &string_hash_ops_free, l)
ਪੂਰਨ ordered_ਸੈੱਟ_put_string_ਸੈੱਟ_ਭਰਿਆ(Ordered_ਸੈੱਟ **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, Ordered_ਸੈੱਟ *l);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ordered_ਸੈੱਟ_put_string_ਸੈੱਟ(s, l) ordered_ਸੈੱਟ_put_string_ਸੈੱਟ_ਭਰਿਆ(s, &string_hash_ops_free, l)

ਖਾਲੀ ordered_ਸੈੱਟ_print(FILE *f, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, Ordered_ਸੈੱਟ *s);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ORDERED_ਸੈੱਟ_FOREACH_2(e, s, i) \
        ਲਈ (Iterator i = ITERATOR_ਪਹਿਲਾ; ordered_ਸੈੱਟ_iterate((s), &i, (ਖਾਲੀ**)&(e)); )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ORDERED_ਸੈੱਟ_FOREACH(e, s) \
        ORDERED_ਸੈੱਟ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Ordered_ਸੈੱਟ*, ordered_ਸੈੱਟ_free);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cleanup_ordered_ਸੈੱਟ_free _cleanup_(ordered_ਸੈੱਟ_freep)
