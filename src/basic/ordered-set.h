/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"
#ضمن "hashmap.h"

تعريفنوع بنية Ordered_مجموعه Ordered_مجموعه;

ساكن مضمن Ordered_مجموعه* ordered_مجموعه_new(ثابت بنية hash_ops *ops) {
        أرجع (Ordered_مجموعه*) ordered_hashmap_new(ops);
}

صحيح ordered_مجموعه_ensure_allocated(Ordered_مجموعه **s, ثابت بنية hash_ops *ops);

صحيح ordered_مجموعه_ensure_put(Ordered_مجموعه **s, ثابت بنية hash_ops *ops, فراغ *p);

ساكن مضمن فراغ ordered_مجموعه_clear(Ordered_مجموعه *s) {
        أرجع ordered_hashmap_clear((OrderedHashmap*) s);
}

ساكن مضمن Ordered_مجموعه* ordered_مجموعه_free(Ordered_مجموعه *s) {
        أرجع (Ordered_مجموعه*) ordered_hashmap_free((OrderedHashmap*) s);
}

ساكن مضمن صحيح ordered_مجموعه_شامل_است(Ordered_مجموعه *s, ثابت فراغ *p) {
        أرجع ordered_hashmap_شامل_است((OrderedHashmap*) s, p);
}

ساكن مضمن صحيح ordered_مجموعه_put(Ordered_مجموعه *s, فراغ *p) {
        أرجع ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ساكن مضمن فراغ *ordered_مجموعه_get(Ordered_مجموعه *s, ثابت فراغ *p) {
        أرجع ordered_hashmap_get((OrderedHashmap*) s, p);
}

ساكن مضمن بیعلامت ordered_مجموعه_اندازه(Ordered_مجموعه *s) {
        أرجع ordered_hashmap_اندازه((OrderedHashmap*) s);
}

ساكن مضمن bool ordered_مجموعه_isempty(Ordered_مجموعه *s) {
        أرجع ordered_hashmap_isempty((OrderedHashmap*) s);
}

ساكن مضمن bool ordered_مجموعه_iterate(Ordered_مجموعه *s, Iterator *i, فراغ **مقدار) {
        أرجع ordered_hashmap_iterate((OrderedHashmap*) s, i, مقدار, عدم);
}

ساكن مضمن فراغ* ordered_مجموعه_remove(Ordered_مجموعه *s, فراغ *p) {
        أرجع ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ساكن مضمن فراغ* ordered_مجموعه_اولی(Ordered_مجموعه *s) {
        أرجع ordered_hashmap_اولی((OrderedHashmap*) s);
}

ساكن مضمن فراغ* ordered_مجموعه_steal_اولی(Ordered_مجموعه *s) {
        أرجع ordered_hashmap_steal_اولی((OrderedHashmap*) s);
}

ساكن مضمن حرف** ordered_مجموعه_get_strv(Ordered_مجموعه *s) {
        أرجع _hashmap_get_strv(HASHMAP_پایه((OrderedHashmap*) s));
}

ساكن مضمن صحيح ordered_مجموعه_reserve(Ordered_مجموعه *s, بیعلامت entries_جمع_کن) {
        أرجع ordered_hashmap_reserve((OrderedHashmap*) s, entries_جمع_کن);
}

صحيح ordered_مجموعه_consume(Ordered_مجموعه *s, فراغ *p);

صحيح ordered_مجموعه_put_strdup_پر(Ordered_مجموعه **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p);
#تعریف_کن ordered_مجموعه_put_strdup(s, p) ordered_مجموعه_put_strdup_پر(s, &string_hash_ops_free, p)
صحيح ordered_مجموعه_put_strdupv_پر(Ordered_مجموعه **s, ثابت بنية hash_ops *hash_ops, حرف **l);
#تعریف_کن ordered_مجموعه_put_strdupv(s, l) ordered_مجموعه_put_strdupv_پر(s, &string_hash_ops_free, l)
صحيح ordered_مجموعه_put_string_مجموعه_پر(Ordered_مجموعه **s, ثابت بنية hash_ops *hash_ops, Ordered_مجموعه *l);
#تعریف_کن ordered_مجموعه_put_string_مجموعه(s, l) ordered_مجموعه_put_string_مجموعه_پر(s, &string_hash_ops_free, l)

فراغ ordered_مجموعه_print(FILE *f, ثابت حرف *فیلد, Ordered_مجموعه *s);

#تعریف_کن ORDERED_مجموعه_FOREACH_2(e, s, i) \
        لكل (Iterator i = ITERATOR_اولی; ordered_مجموعه_iterate((s), &i, (فراغ**)&(e)); )
#تعریف_کن ORDERED_مجموعه_FOREACH(e, s) \
        ORDERED_مجموعه_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Ordered_مجموعه*, ordered_مجموعه_free);

#تعریف_کن cleanup_ordered_مجموعه_free _cleanup_(ordered_مجموعه_freep)
