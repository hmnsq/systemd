/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"
#ضمن "hashmap.h"

تعريفنوع بنية Ordered_سېټ Ordered_سېټ;

ساكن مضمن Ordered_سېټ* ordered_سېټ_new(ثابت بنية hash_ops *ops) {
        أرجع (Ordered_سېټ*) ordered_hashmap_new(ops);
}

صحيح ordered_سېټ_ensure_allocated(Ordered_سېټ **s, ثابت بنية hash_ops *ops);

صحيح ordered_سېټ_ensure_put(Ordered_سېټ **s, ثابت بنية hash_ops *ops, فراغ *p);

ساكن مضمن فراغ ordered_سېټ_clear(Ordered_سېټ *s) {
        أرجع ordered_hashmap_clear((OrderedHashmap*) s);
}

ساكن مضمن Ordered_سېټ* ordered_سېټ_free(Ordered_سېټ *s) {
        أرجع (Ordered_سېټ*) ordered_hashmap_free((OrderedHashmap*) s);
}

ساكن مضمن صحيح ordered_سېټ_لري(Ordered_سېټ *s, ثابت فراغ *p) {
        أرجع ordered_hashmap_لري((OrderedHashmap*) s, p);
}

ساكن مضمن صحيح ordered_سېټ_put(Ordered_سېټ *s, فراغ *p) {
        أرجع ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ساكن مضمن فراغ *ordered_سېټ_get(Ordered_سېټ *s, ثابت فراغ *p) {
        أرجع ordered_hashmap_get((OrderedHashmap*) s, p);
}

ساكن مضمن بینښه ordered_سېټ_اندازه(Ordered_سېټ *s) {
        أرجع ordered_hashmap_اندازه((OrderedHashmap*) s);
}

ساكن مضمن bool ordered_سېټ_isempty(Ordered_سېټ *s) {
        أرجع ordered_hashmap_isempty((OrderedHashmap*) s);
}

ساكن مضمن bool ordered_سېټ_iterate(Ordered_سېټ *s, Iterator *i, فراغ **ارزښت) {
        أرجع ordered_hashmap_iterate((OrderedHashmap*) s, i, ارزښت, عدم);
}

ساكن مضمن فراغ* ordered_سېټ_remove(Ordered_سېټ *s, فراغ *p) {
        أرجع ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ساكن مضمن فراغ* ordered_سېټ_لومړی(Ordered_سېټ *s) {
        أرجع ordered_hashmap_لومړی((OrderedHashmap*) s);
}

ساكن مضمن فراغ* ordered_سېټ_steal_لومړی(Ordered_سېټ *s) {
        أرجع ordered_hashmap_steal_لومړی((OrderedHashmap*) s);
}

ساكن مضمن حرف** ordered_سېټ_get_strv(Ordered_سېټ *s) {
        أرجع _hashmap_get_strv(HASHMAP_بنسټ((OrderedHashmap*) s));
}

ساكن مضمن صحيح ordered_سېټ_reserve(Ordered_سېټ *s, بینښه entries_جمع_کړه) {
        أرجع ordered_hashmap_reserve((OrderedHashmap*) s, entries_جمع_کړه);
}

صحيح ordered_سېټ_consume(Ordered_سېټ *s, فراغ *p);

صحيح ordered_سېټ_put_strdup_ډک(Ordered_سېټ **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p);
#تعریف_کړه ordered_سېټ_put_strdup(s, p) ordered_سېټ_put_strdup_ډک(s, &string_hash_ops_free, p)
صحيح ordered_سېټ_put_strdupv_ډک(Ordered_سېټ **s, ثابت بنية hash_ops *hash_ops, حرف **l);
#تعریف_کړه ordered_سېټ_put_strdupv(s, l) ordered_سېټ_put_strdupv_ډک(s, &string_hash_ops_free, l)
صحيح ordered_سېټ_put_string_سېټ_ډک(Ordered_سېټ **s, ثابت بنية hash_ops *hash_ops, Ordered_سېټ *l);
#تعریف_کړه ordered_سېټ_put_string_سېټ(s, l) ordered_سېټ_put_string_سېټ_ډک(s, &string_hash_ops_free, l)

فراغ ordered_سېټ_print(FILE *f, ثابت حرف *ډګر, Ordered_سېټ *s);

#تعریف_کړه ORDERED_سېټ_FOREACH_2(e, s, i) \
        لكل (Iterator i = ITERATOR_لومړی; ordered_سېټ_iterate((s), &i, (فراغ**)&(e)); )
#تعریف_کړه ORDERED_سېټ_FOREACH(e, s) \
        ORDERED_سېټ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Ordered_سېټ*, ordered_سېټ_free);

#تعریف_کړه cleanup_ordered_سېټ_free _cleanup_(ordered_سېټ_freep)
