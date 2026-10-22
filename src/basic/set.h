/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"
#ضمن "hashmap.h"

#تعریف_کرو سیٹ_free_and_replace(a, b)              \
        free_and_replace_بھریا(a, b, سیٹ_free)

سیٹ_2* سیٹ_new(ثابت بنية hash_ops *hash_ops);

ساكن مضمن سیٹ_2* سیٹ_free(سیٹ_2 *s) {
        أرجع (سیٹ_2*) _hashmap_free(HASHMAP_ادھار(s));
}

#تعریف_کرو سیٹ_copy(s) ((سیٹ_2*) _hashmap_copy(HASHMAP_ادھار(s)))

صحيح سیٹ_ensure_allocated(سیٹ_2 **s, ثابت بنية hash_ops *hash_ops);

صحيح سیٹ_put(سیٹ_2 *s, ثابت فراغ *کلید);
/* no set_update */
/* no set_replace */
ساكن مضمن فراغ *سیٹ_get(ثابت سیٹ_2 *s, ثابت فراغ *کلید) {
        أرجع _hashmap_get(HASHMAP_ادھار((سیٹ_2 *) s), کلید);
}
/* no set_get2 */

ساكن مضمن bool سیٹ_شامل_اے(ثابت سیٹ_2 *s, ثابت فراغ *کلید) {
        أرجع hashmap_شامل_اے_2(HASHMAP_ادھار((سیٹ_2 *) s), کلید);
}

ساكن مضمن فراغ *سیٹ_remove(سیٹ_2 *s, ثابت فراغ *کلید) {
        أرجع _hashmap_remove(HASHMAP_ادھار(s), کلید);
}

/* no set_remove2 */
/* no set_remove_value */
صحيح سیٹ_remove_and_put(سیٹ_2 *s, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید);
/* no set_remove_and_replace */
صحيح سیٹ_ملاؤ(سیٹ_2 *s, سیٹ_2 *other);

ساكن مضمن صحيح سیٹ_reserve(سیٹ_2 *h, بنانشانی entries_جوڑو) {
        أرجع _hashmap_reserve(HASHMAP_ادھار(h), entries_جوڑو);
}

ساكن مضمن صحيح سیٹ_move(سیٹ_2 *s, سیٹ_2 *other) {
        أرجع _hashmap_move(HASHMAP_ادھار(s), HASHMAP_ادھار(other));
}

ساكن مضمن صحيح سیٹ_move_one(سیٹ_2 *s, سیٹ_2 *other, ثابت فراغ *کلید) {
        أرجع _hashmap_move_one(HASHMAP_ادھار(s), HASHMAP_ادھار(other), کلید);
}

ساكن مضمن بنانشانی سیٹ_ناپ(ثابت سیٹ_2 *s) {
        أرجع hashmap_ناپ(HASHMAP_ادھار((سیٹ_2 *) s));
}

ساكن مضمن bool سیٹ_isempty(ثابت سیٹ_2 *s) {
        أرجع سیٹ_ناپ(s) == 0;
}

ساكن مضمن بنانشانی سیٹ_buckets(ثابت سیٹ_2 *s) {
        أرجع _hashmap_buckets(HASHMAP_ادھار((سیٹ_2 *) s));
}

ساكن مضمن bool سیٹ_iterate(ثابت سیٹ_2 *s, Iterator *i, فراغ **قدر) {
        أرجع _hashmap_iterate(HASHMAP_ادھار((سیٹ_2*) s), i, قدر, عدم);
}

ساكن مضمن فراغ سیٹ_clear(سیٹ_2 *s) {
        _hashmap_clear(HASHMAP_ادھار(s));
}

ساكن مضمن فراغ *سیٹ_steal_پہلا(سیٹ_2 *s) {
        أرجع hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار(s), true, عدم);
}

/* no set_steal_first_key */
/* no set_first_key */

ساكن مضمن فراغ *سیٹ_پہلا(ثابت سیٹ_2 *s) {
        أرجع hashmap_پہلا_کلید_and_قدر(HASHMAP_ادھار((سیٹ_2 *) s), false, عدم);
}

/* no set_next */

ساكن مضمن حرف **سیٹ_get_strv(سیٹ_2 *s) {
        أرجع _hashmap_get_strv(HASHMAP_ادھار(s));
}

حرف** سیٹ_منزل_strv(سیٹ_2 **s);

صحيح سیٹ_ensure_put(سیٹ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید);

صحيح سیٹ_ensure_consume(سیٹ_2 **s, ثابت بنية hash_ops *hash_ops, فراغ *کلید);

صحيح سیٹ_consume(سیٹ_2 *s, فراغ *قدر);

صحيح سیٹ_put_strndup_بھریا(سیٹ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n);
#تعریف_کرو سیٹ_put_strdup_بھریا(s, hash_ops, p) سیٹ_put_strndup_بھریا(s, hash_ops, p, SIZE_MAX)
#تعریف_کرو سیٹ_put_strndup(s, p, n) سیٹ_put_strndup_بھریا(s, &string_hash_ops_free, p, n)
#تعریف_کرو سیٹ_put_strdup(s, p) سیٹ_put_strndup(s, p, SIZE_MAX)

صحيح سیٹ_put_strdupv_بھریا(سیٹ_2 **s, ثابت بنية hash_ops *hash_ops, حرف **l);
#تعریف_کرو سیٹ_put_strdupv(s, l) سیٹ_put_strdupv_بھریا(s, &string_hash_ops_free, l)

صحيح سیٹ_put_strsplit(سیٹ_2 *s, ثابت حرف *v, ثابت حرف *separators, کڈھو_Flags flags);

#تعریف_کرو سیٹ_FOREACH_2(e, s, i) \
        لكل (Iterator i = ITERATOR_پہلا; سیٹ_iterate((s), &i, (فراغ**)&(e)); )
#تعریف_کرو سیٹ_FOREACH(e, s) \
        سیٹ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

#تعریف_کرو سیٹ_FOREACH_MOVE(e, d, s)                                       \
        لكل (; ({ e = سیٹ_پہلا(s); assert_se(!e || سیٹ_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(سیٹ_2*, سیٹ_free);

#تعریف_کرو cleanup_سیٹ_free _cleanup_(سیٹ_freep)

صحيح سیٹ_strjoin(سیٹ_2 *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret);

bool سیٹ_equal(سیٹ_2 *a, سیٹ_2 *b);

bool سیٹ_fnmatch(سیٹ_2 *include_patterns, سیٹ_2 *exclude_patterns, ثابت حرف *needle);
