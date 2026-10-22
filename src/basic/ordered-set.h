/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"
#ضمن "hashmap.h"

تعريفنوع بنية Ordered_سیٹ Ordered_سیٹ;

ساكن مضمن Ordered_سیٹ* ordered_سیٹ_new(ثابت بنية hash_ops *ops) {
        أرجع (Ordered_سیٹ*) ordered_hashmap_new(ops);
}

صحيح ordered_سیٹ_ensure_allocated(Ordered_سیٹ **s, ثابت بنية hash_ops *ops);

صحيح ordered_سیٹ_ensure_put(Ordered_سیٹ **s, ثابت بنية hash_ops *ops, فراغ *p);

ساكن مضمن فراغ ordered_سیٹ_clear(Ordered_سیٹ *s) {
        أرجع ordered_hashmap_clear((OrderedHashmap*) s);
}

ساكن مضمن Ordered_سیٹ* ordered_سیٹ_free(Ordered_سیٹ *s) {
        أرجع (Ordered_سیٹ*) ordered_hashmap_free((OrderedHashmap*) s);
}

ساكن مضمن صحيح ordered_سیٹ_شامل_اے(Ordered_سیٹ *s, ثابت فراغ *p) {
        أرجع ordered_hashmap_شامل_اے((OrderedHashmap*) s, p);
}

ساكن مضمن صحيح ordered_سیٹ_put(Ordered_سیٹ *s, فراغ *p) {
        أرجع ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ساكن مضمن فراغ *ordered_سیٹ_get(Ordered_سیٹ *s, ثابت فراغ *p) {
        أرجع ordered_hashmap_get((OrderedHashmap*) s, p);
}

ساكن مضمن بنانشانی ordered_سیٹ_ناپ(Ordered_سیٹ *s) {
        أرجع ordered_hashmap_ناپ((OrderedHashmap*) s);
}

ساكن مضمن bool ordered_سیٹ_isempty(Ordered_سیٹ *s) {
        أرجع ordered_hashmap_isempty((OrderedHashmap*) s);
}

ساكن مضمن bool ordered_سیٹ_iterate(Ordered_سیٹ *s, Iterator *i, فراغ **قدر) {
        أرجع ordered_hashmap_iterate((OrderedHashmap*) s, i, قدر, عدم);
}

ساكن مضمن فراغ* ordered_سیٹ_remove(Ordered_سیٹ *s, فراغ *p) {
        أرجع ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ساكن مضمن فراغ* ordered_سیٹ_پہلا(Ordered_سیٹ *s) {
        أرجع ordered_hashmap_پہلا((OrderedHashmap*) s);
}

ساكن مضمن فراغ* ordered_سیٹ_steal_پہلا(Ordered_سیٹ *s) {
        أرجع ordered_hashmap_steal_پہلا((OrderedHashmap*) s);
}

ساكن مضمن حرف** ordered_سیٹ_get_strv(Ordered_سیٹ *s) {
        أرجع _hashmap_get_strv(HASHMAP_ادھار((OrderedHashmap*) s));
}

ساكن مضمن صحيح ordered_سیٹ_reserve(Ordered_سیٹ *s, بنانشانی entries_جوڑو) {
        أرجع ordered_hashmap_reserve((OrderedHashmap*) s, entries_جوڑو);
}

صحيح ordered_سیٹ_consume(Ordered_سیٹ *s, فراغ *p);

صحيح ordered_سیٹ_put_strdup_بھریا(Ordered_سیٹ **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p);
#تعریف_کرو ordered_سیٹ_put_strdup(s, p) ordered_سیٹ_put_strdup_بھریا(s, &string_hash_ops_free, p)
صحيح ordered_سیٹ_put_strdupv_بھریا(Ordered_سیٹ **s, ثابت بنية hash_ops *hash_ops, حرف **l);
#تعریف_کرو ordered_سیٹ_put_strdupv(s, l) ordered_سیٹ_put_strdupv_بھریا(s, &string_hash_ops_free, l)
صحيح ordered_سیٹ_put_string_سیٹ_بھریا(Ordered_سیٹ **s, ثابت بنية hash_ops *hash_ops, Ordered_سیٹ *l);
#تعریف_کرو ordered_سیٹ_put_string_سیٹ(s, l) ordered_سیٹ_put_string_سیٹ_بھریا(s, &string_hash_ops_free, l)

فراغ ordered_سیٹ_print(FILE *f, ثابت حرف *میدان, Ordered_سیٹ *s);

#تعریف_کرو ORDERED_سیٹ_FOREACH_2(e, s, i) \
        لكل (Iterator i = ITERATOR_پہلا; ordered_سیٹ_iterate((s), &i, (فراغ**)&(e)); )
#تعریف_کرو ORDERED_سیٹ_FOREACH(e, s) \
        ORDERED_سیٹ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Ordered_سیٹ*, ordered_سیٹ_free);

#تعریف_کرو cleanup_ordered_سیٹ_free _cleanup_(ordered_سیٹ_freep)
