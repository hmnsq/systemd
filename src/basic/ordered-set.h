/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"
#شامل "hashmap.h"

قسمتعریف ساخت Ordered_سیٹ Ordered_سیٹ;

ساکن اندرونی Ordered_سیٹ* ordered_سیٹ_new(ثابت ساخت hash_ops *ops) {
        واپس (Ordered_سیٹ*) ordered_hashmap_new(ops);
}

صحیح ordered_سیٹ_ensure_allocated(Ordered_سیٹ **s, ثابت ساخت hash_ops *ops);

صحیح ordered_سیٹ_ensure_put(Ordered_سیٹ **s, ثابت ساخت hash_ops *ops, خالی *p);

ساکن اندرونی خالی ordered_سیٹ_clear(Ordered_سیٹ *s) {
        واپس ordered_hashmap_clear((OrderedHashmap*) s);
}

ساکن اندرونی Ordered_سیٹ* ordered_سیٹ_free(Ordered_سیٹ *s) {
        واپس (Ordered_سیٹ*) ordered_hashmap_free((OrderedHashmap*) s);
}

ساکن اندرونی صحیح ordered_سیٹ_شامل_ہے(Ordered_سیٹ *s, ثابت خالی *p) {
        واپس ordered_hashmap_شامل_ہے((OrderedHashmap*) s, p);
}

ساکن اندرونی صحیح ordered_سیٹ_put(Ordered_سیٹ *s, خالی *p) {
        واپس ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

ساکن اندرونی خالی *ordered_سیٹ_get(Ordered_سیٹ *s, ثابت خالی *p) {
        واپس ordered_hashmap_get((OrderedHashmap*) s, p);
}

ساکن اندرونی غیرعلامتی ordered_سیٹ_سائز(Ordered_سیٹ *s) {
        واپس ordered_hashmap_سائز((OrderedHashmap*) s);
}

ساکن اندرونی bool ordered_سیٹ_isempty(Ordered_سیٹ *s) {
        واپس ordered_hashmap_isempty((OrderedHashmap*) s);
}

ساکن اندرونی bool ordered_سیٹ_iterate(Ordered_سیٹ *s, Iterator *i, خالی **قدر) {
        واپس ordered_hashmap_iterate((OrderedHashmap*) s, i, قدر, تہی);
}

ساکن اندرونی خالی* ordered_سیٹ_remove(Ordered_سیٹ *s, خالی *p) {
        واپس ordered_hashmap_remove((OrderedHashmap*) s, p);
}

ساکن اندرونی خالی* ordered_سیٹ_پہلا(Ordered_سیٹ *s) {
        واپس ordered_hashmap_پہلا((OrderedHashmap*) s);
}

ساکن اندرونی خالی* ordered_سیٹ_steal_پہلا(Ordered_سیٹ *s) {
        واپس ordered_hashmap_steal_پہلا((OrderedHashmap*) s);
}

ساکن اندرونی حرف** ordered_سیٹ_get_strv(Ordered_سیٹ *s) {
        واپس _hashmap_get_strv(HASHMAP_بنیاد((OrderedHashmap*) s));
}

ساکن اندرونی صحیح ordered_سیٹ_reserve(Ordered_سیٹ *s, غیرعلامتی entries_جوڑو) {
        واپس ordered_hashmap_reserve((OrderedHashmap*) s, entries_جوڑو);
}

صحیح ordered_سیٹ_consume(Ordered_سیٹ *s, خالی *p);

صحیح ordered_سیٹ_put_strdup_بھرا(Ordered_سیٹ **s, ثابت ساخت hash_ops *hash_ops, ثابت حرف *p);
#تعریف_کرو ordered_سیٹ_put_strdup(s, p) ordered_سیٹ_put_strdup_بھرا(s, &string_hash_ops_free, p)
صحیح ordered_سیٹ_put_strdupv_بھرا(Ordered_سیٹ **s, ثابت ساخت hash_ops *hash_ops, حرف **l);
#تعریف_کرو ordered_سیٹ_put_strdupv(s, l) ordered_سیٹ_put_strdupv_بھرا(s, &string_hash_ops_free, l)
صحیح ordered_سیٹ_put_string_سیٹ_بھرا(Ordered_سیٹ **s, ثابت ساخت hash_ops *hash_ops, Ordered_سیٹ *l);
#تعریف_کرو ordered_سیٹ_put_string_سیٹ(s, l) ordered_سیٹ_put_string_سیٹ_بھرا(s, &string_hash_ops_free, l)

خالی ordered_سیٹ_print(FILE *f, ثابت حرف *میدان, Ordered_سیٹ *s);

#تعریف_کرو ORDERED_سیٹ_FOREACH_2(e, s, i) \
        برائے (Iterator i = ITERATOR_پہلا; ordered_سیٹ_iterate((s), &i, (خالی**)&(e)); )
#تعریف_کرو ORDERED_سیٹ_FOREACH(e, s) \
        ORDERED_سیٹ_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Ordered_سیٹ*, ordered_سیٹ_free);

#تعریف_کرو cleanup_ordered_سیٹ_free _cleanup_(ordered_سیٹ_freep)
