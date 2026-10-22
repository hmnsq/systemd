/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"
#شامل "hashmap.h"

#تعریف_کرو سیٹ_free_and_replace(a, b)              \
        free_and_replace_بھرا(a, b, سیٹ_free)

سیٹ_2* سیٹ_new(ثابت ساخت hash_ops *hash_ops);

ساکن اندرونی سیٹ_2* سیٹ_free(سیٹ_2 *s) {
        واپس (سیٹ_2*) _hashmap_free(HASHMAP_بنیاد(s));
}

#تعریف_کرو سیٹ_copy(s) ((سیٹ_2*) _hashmap_copy(HASHMAP_بنیاد(s)))

صحیح سیٹ_ensure_allocated(سیٹ_2 **s, ثابت ساخت hash_ops *hash_ops);

صحیح سیٹ_put(سیٹ_2 *s, ثابت خالی *کلید);
/* no set_update */
/* no set_replace */
ساکن اندرونی خالی *سیٹ_get(ثابت سیٹ_2 *s, ثابت خالی *کلید) {
        واپس _hashmap_get(HASHMAP_بنیاد((سیٹ_2 *) s), کلید);
}
/* no set_get2 */

ساکن اندرونی bool سیٹ_شامل_ہے(ثابت سیٹ_2 *s, ثابت خالی *کلید) {
        واپس hashmap_شامل_ہے_2(HASHMAP_بنیاد((سیٹ_2 *) s), کلید);
}

ساکن اندرونی خالی *سیٹ_remove(سیٹ_2 *s, ثابت خالی *کلید) {
        واپس _hashmap_remove(HASHMAP_بنیاد(s), کلید);
}

/* no set_remove2 */
/* no set_remove_value */
صحیح سیٹ_remove_and_put(سیٹ_2 *s, ثابت خالی *old_کلید, ثابت خالی *new_کلید);
/* no set_remove_and_replace */
صحیح سیٹ_ملاؤ(سیٹ_2 *s, سیٹ_2 *other);

ساکن اندرونی صحیح سیٹ_reserve(سیٹ_2 *h, غیرعلامتی entries_جوڑو) {
        واپس _hashmap_reserve(HASHMAP_بنیاد(h), entries_جوڑو);
}

ساکن اندرونی صحیح سیٹ_move(سیٹ_2 *s, سیٹ_2 *other) {
        واپس _hashmap_move(HASHMAP_بنیاد(s), HASHMAP_بنیاد(other));
}

ساکن اندرونی صحیح سیٹ_move_one(سیٹ_2 *s, سیٹ_2 *other, ثابت خالی *کلید) {
        واپس _hashmap_move_one(HASHMAP_بنیاد(s), HASHMAP_بنیاد(other), کلید);
}

ساکن اندرونی غیرعلامتی سیٹ_سائز(ثابت سیٹ_2 *s) {
        واپس hashmap_سائز(HASHMAP_بنیاد((سیٹ_2 *) s));
}

ساکن اندرونی bool سیٹ_isempty(ثابت سیٹ_2 *s) {
        واپس سیٹ_سائز(s) == 0;
}

ساکن اندرونی غیرعلامتی سیٹ_buckets(ثابت سیٹ_2 *s) {
        واپس _hashmap_buckets(HASHMAP_بنیاد((سیٹ_2 *) s));
}

ساکن اندرونی bool سیٹ_iterate(ثابت سیٹ_2 *s, Iterator *i, خالی **قدر) {
        واپس _hashmap_iterate(HASHMAP_بنیاد((سیٹ_2*) s), i, قدر, تہی);
}

ساکن اندرونی خالی سیٹ_clear(سیٹ_2 *s) {
        _hashmap_clear(HASHMAP_بنیاد(s));
}

ساکن اندرونی خالی *سیٹ_steal_پہلا(سیٹ_2 *s) {
        واپس hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد(s), true, تہی);
}

/* no set_steal_first_key */
/* no set_first_key */

ساکن اندرونی خالی *سیٹ_پہلا(ثابت سیٹ_2 *s) {
        واپس hashmap_پہلا_کلید_and_قدر(HASHMAP_بنیاد((سیٹ_2 *) s), false, تہی);
}

/* no set_next */

ساکن اندرونی حرف **سیٹ_get_strv(سیٹ_2 *s) {
        واپس _hashmap_get_strv(HASHMAP_بنیاد(s));
}

حرف** سیٹ_مقصد_strv(سیٹ_2 **s);

صحیح سیٹ_ensure_put(سیٹ_2 **s, ثابت ساخت hash_ops *hash_ops, ثابت خالی *کلید);

صحیح سیٹ_ensure_consume(سیٹ_2 **s, ثابت ساخت hash_ops *hash_ops, خالی *کلید);

صحیح سیٹ_consume(سیٹ_2 *s, خالی *قدر);

صحیح سیٹ_put_strndup_بھرا(سیٹ_2 **s, ثابت ساخت hash_ops *hash_ops, ثابت حرف *p, size_t n);
#تعریف_کرو سیٹ_put_strdup_بھرا(s, hash_ops, p) سیٹ_put_strndup_بھرا(s, hash_ops, p, SIZE_MAX)
#تعریف_کرو سیٹ_put_strndup(s, p, n) سیٹ_put_strndup_بھرا(s, &string_hash_ops_free, p, n)
#تعریف_کرو سیٹ_put_strdup(s, p) سیٹ_put_strndup(s, p, SIZE_MAX)

صحیح سیٹ_put_strdupv_بھرا(سیٹ_2 **s, ثابت ساخت hash_ops *hash_ops, حرف **l);
#تعریف_کرو سیٹ_put_strdupv(s, l) سیٹ_put_strdupv_بھرا(s, &string_hash_ops_free, l)

صحیح سیٹ_put_strsplit(سیٹ_2 *s, ثابت حرف *v, ثابت حرف *separators, نکالو_Flags flags);

#تعریف_کرو سیٹ_FOREACH_2(e, s, i) \
        برائے (Iterator i = ITERATOR_پہلا; سیٹ_iterate((s), &i, (خالی**)&(e)); )
#تعریف_کرو سیٹ_FOREACH(e, s) \
        سیٹ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

#تعریف_کرو سیٹ_FOREACH_MOVE(e, d, s)                                       \
        برائے (; ({ e = سیٹ_پہلا(s); assert_se(!e || سیٹ_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(سیٹ_2*, سیٹ_free);

#تعریف_کرو cleanup_سیٹ_free _cleanup_(سیٹ_freep)

صحیح سیٹ_strjoin(سیٹ_2 *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret);

bool سیٹ_equal(سیٹ_2 *a, سیٹ_2 *b);

bool سیٹ_fnmatch(سیٹ_2 *include_patterns, سیٹ_2 *exclude_patterns, ثابت حرف *needle);
