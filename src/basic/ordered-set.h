/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"
#ضمن "hashmap.h"

تعريفنوع بنية Ordered_سيٽ Ordered_سيٽ;

ساكن مضمن Ordered_سيٽ* ordered_سيٽ_new(ثابت بنية hash_ops *ops) {
        أرجع (Ordered_سيٽ*) ordered_hashmap_new(ops);
}

صحيح ordered_سيٽ_ensure_allocated(Ordered_سيٽ **s, ثابت بنية hash_ops *ops);

صحيح ordered_سيٽ_ensure_put(Ordered_سيٽ **s, ثابت بنية hash_ops *ops, فراغ *p);

ساكن مضمن فراغ ordered_سيٽ_clear(Ordered_سيٽ *s) {
        أرجع ordered_hashmap_clear((OrderedHashmap*) s);
}

ساكن مضمن Ordered_سيٽ* ordered_سيٽ_free(Ordered_سيٽ *s) {
        أرجع (Ordered_سيٽ*) ordered_hashmap_free((OrderedHashmap*) s);
}

ساكن مضمن صحيح ordered_سيٽ_شامل_آهي(Ordered_سيٽ *s, ثابت فراغ *p) {
        أرجع ordered_hashmap_شامل_آهي((OrderedHashmap*) s, p);
}

ساكن مضمن صحيح ordered_سيٽ_put(Ordered_سيٽ *s, فراغ *p) {
        أرجع ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ساكن مضمن فراغ *ordered_سيٽ_get(Ordered_سيٽ *s, ثابت فراغ *p) {
        أرجع ordered_hashmap_get((OrderedHashmap*) s, p);
}

ساكن مضمن بنانشان ordered_سيٽ_سائيز(Ordered_سيٽ *s) {
        أرجع ordered_hashmap_سائيز((OrderedHashmap*) s);
}

ساكن مضمن bool ordered_سيٽ_isempty(Ordered_سيٽ *s) {
        أرجع ordered_hashmap_isempty((OrderedHashmap*) s);
}

ساكن مضمن bool ordered_سيٽ_iterate(Ordered_سيٽ *s, Iterator *i, فراغ **قدر) {
        أرجع ordered_hashmap_iterate((OrderedHashmap*) s, i, قدر, عدم);
}

ساكن مضمن فراغ* ordered_سيٽ_remove(Ordered_سيٽ *s, فراغ *p) {
        أرجع ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ساكن مضمن فراغ* ordered_سيٽ_پهريون(Ordered_سيٽ *s) {
        أرجع ordered_hashmap_پهريون((OrderedHashmap*) s);
}

ساكن مضمن فراغ* ordered_سيٽ_steal_پهريون(Ordered_سيٽ *s) {
        أرجع ordered_hashmap_steal_پهريون((OrderedHashmap*) s);
}

ساكن مضمن حرف** ordered_سيٽ_get_strv(Ordered_سيٽ *s) {
        أرجع _hashmap_get_strv(HASHMAP_بنياد((OrderedHashmap*) s));
}

ساكن مضمن صحيح ordered_سيٽ_reserve(Ordered_سيٽ *s, بنانشان entries_جوڙيو) {
        أرجع ordered_hashmap_reserve((OrderedHashmap*) s, entries_جوڙيو);
}

صحيح ordered_سيٽ_consume(Ordered_سيٽ *s, فراغ *p);

صحيح ordered_سيٽ_put_strdup_ڀريل(Ordered_سيٽ **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p);
#وصف_ڪريو ordered_سيٽ_put_strdup(s, p) ordered_سيٽ_put_strdup_ڀريل(s, &string_hash_ops_free, p)
صحيح ordered_سيٽ_put_strdupv_ڀريل(Ordered_سيٽ **s, ثابت بنية hash_ops *hash_ops, حرف **l);
#وصف_ڪريو ordered_سيٽ_put_strdupv(s, l) ordered_سيٽ_put_strdupv_ڀريل(s, &string_hash_ops_free, l)
صحيح ordered_سيٽ_put_string_سيٽ_ڀريل(Ordered_سيٽ **s, ثابت بنية hash_ops *hash_ops, Ordered_سيٽ *l);
#وصف_ڪريو ordered_سيٽ_put_string_سيٽ(s, l) ordered_سيٽ_put_string_سيٽ_ڀريل(s, &string_hash_ops_free, l)

فراغ ordered_سيٽ_print(FILE *f, ثابت حرف *ميدان, Ordered_سيٽ *s);

#وصف_ڪريو ORDERED_سيٽ_FOREACH_2(e, s, i) \
        لكل (Iterator i = ITERATOR_پهريون; ordered_سيٽ_iterate((s), &i, (فراغ**)&(e)); )
#وصف_ڪريو ORDERED_سيٽ_FOREACH(e, s) \
        ORDERED_سيٽ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Ordered_سيٽ*, ordered_سيٽ_free);

#وصف_ڪريو cleanup_ordered_سيٽ_free _cleanup_(ordered_سيٽ_freep)
