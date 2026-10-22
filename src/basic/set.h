/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"
#ضمن "hashmap.h"

#وصف_ڪريو سيٽ_free_and_replace(a, b)              \
        free_and_replace_ڀريل(a, b, سيٽ_free)

سيٽ_2* سيٽ_new(ثابت بنية hash_ops *hash_ops);

ساكن مضمن سيٽ_2* سيٽ_free(سيٽ_2 *s) {
        أرجع (سيٽ_2*) _hashmap_free(HASHMAP_بنياد(s));
}

#وصف_ڪريو سيٽ_copy(s) ((سيٽ_2*) _hashmap_copy(HASHMAP_بنياد(s)))

صحيح سيٽ_ensure_allocated(سيٽ_2 **s, ثابت بنية hash_ops *hash_ops);

صحيح سيٽ_put(سيٽ_2 *s, ثابت فراغ *ڪنجي);
/* no set_update */
/* no set_replace */
ساكن مضمن فراغ *سيٽ_get(ثابت سيٽ_2 *s, ثابت فراغ *ڪنجي) {
        أرجع _hashmap_get(HASHMAP_بنياد((سيٽ_2 *) s), ڪنجي);
}
/* no set_get2 */

ساكن مضمن bool سيٽ_شامل_آهي(ثابت سيٽ_2 *s, ثابت فراغ *ڪنجي) {
        أرجع hashmap_شامل_آهي_2(HASHMAP_بنياد((سيٽ_2 *) s), ڪنجي);
}

ساكن مضمن فراغ *سيٽ_remove(سيٽ_2 *s, ثابت فراغ *ڪنجي) {
        أرجع _hashmap_remove(HASHMAP_بنياد(s), ڪنجي);
}

/* no set_remove2 */
/* no set_remove_value */
صحيح سيٽ_remove_and_put(سيٽ_2 *s, ثابت فراغ *old_ڪنجي, ثابت فراغ *new_ڪنجي);
/* no set_remove_and_replace */
صحيح سيٽ_ملايو(سيٽ_2 *s, سيٽ_2 *other);

ساكن مضمن صحيح سيٽ_reserve(سيٽ_2 *h, بنانشان entries_جوڙيو) {
        أرجع _hashmap_reserve(HASHMAP_بنياد(h), entries_جوڙيو);
}

ساكن مضمن صحيح سيٽ_move(سيٽ_2 *s, سيٽ_2 *other) {
        أرجع _hashmap_move(HASHMAP_بنياد(s), HASHMAP_بنياد(other));
}

ساكن مضمن صحيح سيٽ_move_one(سيٽ_2 *s, سيٽ_2 *other, ثابت فراغ *ڪنجي) {
        أرجع _hashmap_move_one(HASHMAP_بنياد(s), HASHMAP_بنياد(other), ڪنجي);
}

ساكن مضمن بنانشان سيٽ_سائيز(ثابت سيٽ_2 *s) {
        أرجع hashmap_سائيز(HASHMAP_بنياد((سيٽ_2 *) s));
}

ساكن مضمن bool سيٽ_isempty(ثابت سيٽ_2 *s) {
        أرجع سيٽ_سائيز(s) == 0;
}

ساكن مضمن بنانشان سيٽ_buckets(ثابت سيٽ_2 *s) {
        أرجع _hashmap_buckets(HASHMAP_بنياد((سيٽ_2 *) s));
}

ساكن مضمن bool سيٽ_iterate(ثابت سيٽ_2 *s, Iterator *i, فراغ **قدر) {
        أرجع _hashmap_iterate(HASHMAP_بنياد((سيٽ_2*) s), i, قدر, عدم);
}

ساكن مضمن فراغ سيٽ_clear(سيٽ_2 *s) {
        _hashmap_clear(HASHMAP_بنياد(s));
}

ساكن مضمن فراغ *سيٽ_steal_پهريون(سيٽ_2 *s) {
        أرجع hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد(s), true, عدم);
}

/* no set_steal_first_key */
/* no set_first_key */

ساكن مضمن فراغ *سيٽ_پهريون(ثابت سيٽ_2 *s) {
        أرجع hashmap_پهريون_ڪنجي_and_قدر(HASHMAP_بنياد((سيٽ_2 *) s), false, عدم);
}

/* no set_next */

ساكن مضمن حرف **سيٽ_get_strv(سيٽ_2 *s) {
        أرجع _hashmap_get_strv(HASHMAP_بنياد(s));
}

حرف** سيٽ_منزل_strv(سيٽ_2 **s);

صحيح سيٽ_ensure_put(سيٽ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ڪنجي);

صحيح سيٽ_ensure_consume(سيٽ_2 **s, ثابت بنية hash_ops *hash_ops, فراغ *ڪنجي);

صحيح سيٽ_consume(سيٽ_2 *s, فراغ *قدر);

صحيح سيٽ_put_strndup_ڀريل(سيٽ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n);
#وصف_ڪريو سيٽ_put_strdup_ڀريل(s, hash_ops, p) سيٽ_put_strndup_ڀريل(s, hash_ops, p, SIZE_MAX)
#وصف_ڪريو سيٽ_put_strndup(s, p, n) سيٽ_put_strndup_ڀريل(s, &string_hash_ops_free, p, n)
#وصف_ڪريو سيٽ_put_strdup(s, p) سيٽ_put_strndup(s, p, SIZE_MAX)

صحيح سيٽ_put_strdupv_ڀريل(سيٽ_2 **s, ثابت بنية hash_ops *hash_ops, حرف **l);
#وصف_ڪريو سيٽ_put_strdupv(s, l) سيٽ_put_strdupv_ڀريل(s, &string_hash_ops_free, l)

صحيح سيٽ_put_strsplit(سيٽ_2 *s, ثابت حرف *v, ثابت حرف *separators, ڪڍو_Flags flags);

#وصف_ڪريو سيٽ_FOREACH_2(e, s, i) \
        لكل (Iterator i = ITERATOR_پهريون; سيٽ_iterate((s), &i, (فراغ**)&(e)); )
#وصف_ڪريو سيٽ_FOREACH(e, s) \
        سيٽ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

#وصف_ڪريو سيٽ_FOREACH_MOVE(e, d, s)                                       \
        لكل (; ({ e = سيٽ_پهريون(s); assert_se(!e || سيٽ_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(سيٽ_2*, سيٽ_free);

#وصف_ڪريو cleanup_سيٽ_free _cleanup_(سيٽ_freep)

صحيح سيٽ_strjoin(سيٽ_2 *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret);

bool سيٽ_equal(سيٽ_2 *a, سيٽ_2 *b);

bool سيٽ_fnmatch(سيٽ_2 *include_patterns, سيٽ_2 *exclude_patterns, ثابت حرف *needle);
