/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"
#ضمن "hashmap.h"

#تعریف_کړه سېټ_free_and_replace(a, b)              \
        free_and_replace_ډک(a, b, سېټ_free)

سېټ_2* سېټ_new(ثابت بنية hash_ops *hash_ops);

ساكن مضمن سېټ_2* سېټ_free(سېټ_2 *s) {
        أرجع (سېټ_2*) _hashmap_free(HASHMAP_بنسټ(s));
}

#تعریف_کړه سېټ_copy(s) ((سېټ_2*) _hashmap_copy(HASHMAP_بنسټ(s)))

صحيح سېټ_ensure_allocated(سېټ_2 **s, ثابت بنية hash_ops *hash_ops);

صحيح سېټ_put(سېټ_2 *s, ثابت فراغ *کلی);
/* no set_update */
/* no set_replace */
ساكن مضمن فراغ *سېټ_get(ثابت سېټ_2 *s, ثابت فراغ *کلی) {
        أرجع _hashmap_get(HASHMAP_بنسټ((سېټ_2 *) s), کلی);
}
/* no set_get2 */

ساكن مضمن bool سېټ_لري(ثابت سېټ_2 *s, ثابت فراغ *کلی) {
        أرجع hashmap_لري_2(HASHMAP_بنسټ((سېټ_2 *) s), کلی);
}

ساكن مضمن فراغ *سېټ_remove(سېټ_2 *s, ثابت فراغ *کلی) {
        أرجع _hashmap_remove(HASHMAP_بنسټ(s), کلی);
}

/* no set_remove2 */
/* no set_remove_value */
صحيح سېټ_remove_and_put(سېټ_2 *s, ثابت فراغ *old_کلی, ثابت فراغ *new_کلی);
/* no set_remove_and_replace */
صحيح سېټ_یوځای(سېټ_2 *s, سېټ_2 *other);

ساكن مضمن صحيح سېټ_reserve(سېټ_2 *h, بینښه entries_جمع_کړه) {
        أرجع _hashmap_reserve(HASHMAP_بنسټ(h), entries_جمع_کړه);
}

ساكن مضمن صحيح سېټ_move(سېټ_2 *s, سېټ_2 *other) {
        أرجع _hashmap_move(HASHMAP_بنسټ(s), HASHMAP_بنسټ(other));
}

ساكن مضمن صحيح سېټ_move_one(سېټ_2 *s, سېټ_2 *other, ثابت فراغ *کلی) {
        أرجع _hashmap_move_one(HASHMAP_بنسټ(s), HASHMAP_بنسټ(other), کلی);
}

ساكن مضمن بینښه سېټ_اندازه(ثابت سېټ_2 *s) {
        أرجع hashmap_اندازه(HASHMAP_بنسټ((سېټ_2 *) s));
}

ساكن مضمن bool سېټ_isempty(ثابت سېټ_2 *s) {
        أرجع سېټ_اندازه(s) == 0;
}

ساكن مضمن بینښه سېټ_buckets(ثابت سېټ_2 *s) {
        أرجع _hashmap_buckets(HASHMAP_بنسټ((سېټ_2 *) s));
}

ساكن مضمن bool سېټ_iterate(ثابت سېټ_2 *s, Iterator *i, فراغ **ارزښت) {
        أرجع _hashmap_iterate(HASHMAP_بنسټ((سېټ_2*) s), i, ارزښت, عدم);
}

ساكن مضمن فراغ سېټ_clear(سېټ_2 *s) {
        _hashmap_clear(HASHMAP_بنسټ(s));
}

ساكن مضمن فراغ *سېټ_steal_لومړی(سېټ_2 *s) {
        أرجع hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ(s), true, عدم);
}

/* no set_steal_first_key */
/* no set_first_key */

ساكن مضمن فراغ *سېټ_لومړی(ثابت سېټ_2 *s) {
        أرجع hashmap_لومړی_کلی_and_ارزښت(HASHMAP_بنسټ((سېټ_2 *) s), false, عدم);
}

/* no set_next */

ساكن مضمن حرف **سېټ_get_strv(سېټ_2 *s) {
        أرجع _hashmap_get_strv(HASHMAP_بنسټ(s));
}

حرف** سېټ_منزل_strv(سېټ_2 **s);

صحيح سېټ_ensure_put(سېټ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلی);

صحيح سېټ_ensure_consume(سېټ_2 **s, ثابت بنية hash_ops *hash_ops, فراغ *کلی);

صحيح سېټ_consume(سېټ_2 *s, فراغ *ارزښت);

صحيح سېټ_put_strndup_ډک(سېټ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n);
#تعریف_کړه سېټ_put_strdup_ډک(s, hash_ops, p) سېټ_put_strndup_ډک(s, hash_ops, p, SIZE_MAX)
#تعریف_کړه سېټ_put_strndup(s, p, n) سېټ_put_strndup_ډک(s, &string_hash_ops_free, p, n)
#تعریف_کړه سېټ_put_strdup(s, p) سېټ_put_strndup(s, p, SIZE_MAX)

صحيح سېټ_put_strdupv_ډک(سېټ_2 **s, ثابت بنية hash_ops *hash_ops, حرف **l);
#تعریف_کړه سېټ_put_strdupv(s, l) سېټ_put_strdupv_ډک(s, &string_hash_ops_free, l)

صحيح سېټ_put_strsplit(سېټ_2 *s, ثابت حرف *v, ثابت حرف *separators, ایستل_Flags flags);

#تعریف_کړه سېټ_FOREACH_2(e, s, i) \
        لكل (Iterator i = ITERATOR_لومړی; سېټ_iterate((s), &i, (فراغ**)&(e)); )
#تعریف_کړه سېټ_FOREACH(e, s) \
        سېټ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

#تعریف_کړه سېټ_FOREACH_MOVE(e, d, s)                                       \
        لكل (; ({ e = سېټ_لومړی(s); assert_se(!e || سېټ_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(سېټ_2*, سېټ_free);

#تعریف_کړه cleanup_سېټ_free _cleanup_(سېټ_freep)

صحيح سېټ_strjoin(سېټ_2 *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret);

bool سېټ_equal(سېټ_2 *a, سېټ_2 *b);

bool سېټ_fnmatch(سېټ_2 *include_patterns, سېټ_2 *exclude_patterns, ثابت حرف *needle);
