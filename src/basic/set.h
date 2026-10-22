/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"
#ضمن "hashmap.h"

#تعریف_کن مجموعه_free_and_replace(a, b)              \
        free_and_replace_پر(a, b, مجموعه_free)

مجموعه_2* مجموعه_new(ثابت بنية hash_ops *hash_ops);

ساكن مضمن مجموعه_2* مجموعه_free(مجموعه_2 *s) {
        أرجع (مجموعه_2*) _hashmap_free(HASHMAP_پایه(s));
}

#تعریف_کن مجموعه_copy(s) ((مجموعه_2*) _hashmap_copy(HASHMAP_پایه(s)))

صحيح مجموعه_ensure_allocated(مجموعه_2 **s, ثابت بنية hash_ops *hash_ops);

صحيح مجموعه_put(مجموعه_2 *s, ثابت فراغ *کلید);
/* no set_update */
/* no set_replace */
ساكن مضمن فراغ *مجموعه_get(ثابت مجموعه_2 *s, ثابت فراغ *کلید) {
        أرجع _hashmap_get(HASHMAP_پایه((مجموعه_2 *) s), کلید);
}
/* no set_get2 */

ساكن مضمن bool مجموعه_شامل_است(ثابت مجموعه_2 *s, ثابت فراغ *کلید) {
        أرجع hashmap_شامل_است_2(HASHMAP_پایه((مجموعه_2 *) s), کلید);
}

ساكن مضمن فراغ *مجموعه_remove(مجموعه_2 *s, ثابت فراغ *کلید) {
        أرجع _hashmap_remove(HASHMAP_پایه(s), کلید);
}

/* no set_remove2 */
/* no set_remove_value */
صحيح مجموعه_remove_and_put(مجموعه_2 *s, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید);
/* no set_remove_and_replace */
صحيح مجموعه_ادغام(مجموعه_2 *s, مجموعه_2 *other);

ساكن مضمن صحيح مجموعه_reserve(مجموعه_2 *h, بیعلامت entries_جمع_کن) {
        أرجع _hashmap_reserve(HASHMAP_پایه(h), entries_جمع_کن);
}

ساكن مضمن صحيح مجموعه_move(مجموعه_2 *s, مجموعه_2 *other) {
        أرجع _hashmap_move(HASHMAP_پایه(s), HASHMAP_پایه(other));
}

ساكن مضمن صحيح مجموعه_move_one(مجموعه_2 *s, مجموعه_2 *other, ثابت فراغ *کلید) {
        أرجع _hashmap_move_one(HASHMAP_پایه(s), HASHMAP_پایه(other), کلید);
}

ساكن مضمن بیعلامت مجموعه_اندازه(ثابت مجموعه_2 *s) {
        أرجع hashmap_اندازه(HASHMAP_پایه((مجموعه_2 *) s));
}

ساكن مضمن bool مجموعه_isempty(ثابت مجموعه_2 *s) {
        أرجع مجموعه_اندازه(s) == 0;
}

ساكن مضمن بیعلامت مجموعه_buckets(ثابت مجموعه_2 *s) {
        أرجع _hashmap_buckets(HASHMAP_پایه((مجموعه_2 *) s));
}

ساكن مضمن bool مجموعه_iterate(ثابت مجموعه_2 *s, Iterator *i, فراغ **مقدار) {
        أرجع _hashmap_iterate(HASHMAP_پایه((مجموعه_2*) s), i, مقدار, عدم);
}

ساكن مضمن فراغ مجموعه_clear(مجموعه_2 *s) {
        _hashmap_clear(HASHMAP_پایه(s));
}

ساكن مضمن فراغ *مجموعه_steal_اولی(مجموعه_2 *s) {
        أرجع hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه(s), true, عدم);
}

/* no set_steal_first_key */
/* no set_first_key */

ساكن مضمن فراغ *مجموعه_اولی(ثابت مجموعه_2 *s) {
        أرجع hashmap_اولی_کلید_and_مقدار(HASHMAP_پایه((مجموعه_2 *) s), false, عدم);
}

/* no set_next */

ساكن مضمن حرف **مجموعه_get_strv(مجموعه_2 *s) {
        أرجع _hashmap_get_strv(HASHMAP_پایه(s));
}

حرف** مجموعه_مقصد_strv(مجموعه_2 **s);

صحيح مجموعه_ensure_put(مجموعه_2 **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید);

صحيح مجموعه_ensure_consume(مجموعه_2 **s, ثابت بنية hash_ops *hash_ops, فراغ *کلید);

صحيح مجموعه_consume(مجموعه_2 *s, فراغ *مقدار);

صحيح مجموعه_put_strndup_پر(مجموعه_2 **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n);
#تعریف_کن مجموعه_put_strdup_پر(s, hash_ops, p) مجموعه_put_strndup_پر(s, hash_ops, p, SIZE_MAX)
#تعریف_کن مجموعه_put_strndup(s, p, n) مجموعه_put_strndup_پر(s, &string_hash_ops_free, p, n)
#تعریف_کن مجموعه_put_strdup(s, p) مجموعه_put_strndup(s, p, SIZE_MAX)

صحيح مجموعه_put_strdupv_پر(مجموعه_2 **s, ثابت بنية hash_ops *hash_ops, حرف **l);
#تعریف_کن مجموعه_put_strdupv(s, l) مجموعه_put_strdupv_پر(s, &string_hash_ops_free, l)

صحيح مجموعه_put_strsplit(مجموعه_2 *s, ثابت حرف *v, ثابت حرف *separators, استخراج_Flags flags);

#تعریف_کن مجموعه_FOREACH_2(e, s, i) \
        لكل (Iterator i = ITERATOR_اولی; مجموعه_iterate((s), &i, (فراغ**)&(e)); )
#تعریف_کن مجموعه_FOREACH(e, s) \
        مجموعه_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

#تعریف_کن مجموعه_FOREACH_MOVE(e, d, s)                                       \
        لكل (; ({ e = مجموعه_اولی(s); assert_se(!e || مجموعه_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(مجموعه_2*, مجموعه_free);

#تعریف_کن cleanup_مجموعه_free _cleanup_(مجموعه_freep)

صحيح مجموعه_strjoin(مجموعه_2 *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret);

bool مجموعه_equal(مجموعه_2 *a, مجموعه_2 *b);

bool مجموعه_fnmatch(مجموعه_2 *include_patterns, مجموعه_2 *exclude_patterns, ثابت حرف *needle);
