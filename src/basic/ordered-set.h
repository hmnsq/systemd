/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "hashmap.h"

प्रकारनिर्देश संरचना Ordered_समुच्चयः Ordered_समुच्चयः;

स्थिर अन्तःस्थ Ordered_समुच्चयः* ordered_समुच्चयः_new(ध्रुव संरचना hash_ops *ops) {
        प्रत्यागम (Ordered_समुच्चयः*) ordered_hashmap_new(ops);
}

पूर्णाङ्क ordered_समुच्चयः_ensure_allocated(Ordered_समुच्चयः **s, ध्रुव संरचना hash_ops *ops);

पूर्णाङ्क ordered_समुच्चयः_ensure_put(Ordered_समुच्चयः **s, ध्रुव संरचना hash_ops *ops, शून्य *p);

स्थिर अन्तःस्थ शून्य ordered_समुच्चयः_clear(Ordered_समुच्चयः *s) {
        प्रत्यागम ordered_hashmap_clear((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ Ordered_समुच्चयः* ordered_समुच्चयः_free(Ordered_समुच्चयः *s) {
        प्रत्यागम (Ordered_समुच्चयः*) ordered_hashmap_free((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ पूर्णाङ्क ordered_समुच्चयः_अन्तर्भवति(Ordered_समुच्चयः *s, ध्रुव शून्य *p) {
        प्रत्यागम ordered_hashmap_अन्तर्भवति((OrderedHashmap*) s, p);
}

स्थिर अन्तःस्थ पूर्णाङ्क ordered_समुच्चयः_put(Ordered_समुच्चयः *s, शून्य *p) {
        प्रत्यागम ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

स्थिर अन्तःस्थ शून्य *ordered_समुच्चयः_get(Ordered_समुच्चयः *s, ध्रुव शून्य *p) {
        प्रत्यागम ordered_hashmap_get((OrderedHashmap*) s, p);
}

स्थिर अन्तःस्थ अचिह्नित ordered_समुच्चयः_आकार(Ordered_समुच्चयः *s) {
        प्रत्यागम ordered_hashmap_आकार((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ bool ordered_समुच्चयः_isempty(Ordered_समुच्चयः *s) {
        प्रत्यागम ordered_hashmap_isempty((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ bool ordered_समुच्चयः_iterate(Ordered_समुच्चयः *s, Iterator *i, शून्य **मानम्) {
        प्रत्यागम ordered_hashmap_iterate((OrderedHashmap*) s, i, मानम्, रिक्त);
}

स्थिर अन्तःस्थ शून्य* ordered_समुच्चयः_remove(Ordered_समुच्चयः *s, शून्य *p) {
        प्रत्यागम ordered_hashmap_remove((OrderedHashmap*) s, p);
}

स्थिर अन्तःस्थ शून्य* ordered_समुच्चयः_first(Ordered_समुच्चयः *s) {
        प्रत्यागम ordered_hashmap_first((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ शून्य* ordered_समुच्चयः_steal_first(Ordered_समुच्चयः *s) {
        प्रत्यागम ordered_hashmap_steal_first((OrderedHashmap*) s);
}

स्थिर अन्तःस्थ वर्ण** ordered_समुच्चयः_get_strv(Ordered_समुच्चयः *s) {
        प्रत्यागम _hashmap_get_strv(HASHMAP_आधारः((OrderedHashmap*) s));
}

स्थिर अन्तःस्थ पूर्णाङ्क ordered_समुच्चयः_reserve(Ordered_समुच्चयः *s, अचिह्नित entries_सङ्कलय) {
        प्रत्यागम ordered_hashmap_reserve((OrderedHashmap*) s, entries_सङ्कलय);
}

पूर्णाङ्क ordered_समुच्चयः_consume(Ordered_समुच्चयः *s, शून्य *p);

पूर्णाङ्क ordered_समुच्चयः_put_strdup_पूर्णम्(Ordered_समुच्चयः **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *p);
#परिभाषय ordered_समुच्चयः_put_strdup(s, p) ordered_समुच्चयः_put_strdup_पूर्णम्(s, &string_hash_ops_free, p)
पूर्णाङ्क ordered_समुच्चयः_put_strdupv_पूर्णम्(Ordered_समुच्चयः **s, ध्रुव संरचना hash_ops *hash_ops, वर्ण **l);
#परिभाषय ordered_समुच्चयः_put_strdupv(s, l) ordered_समुच्चयः_put_strdupv_पूर्णम्(s, &string_hash_ops_free, l)
पूर्णाङ्क ordered_समुच्चयः_put_string_समुच्चयः_पूर्णम्(Ordered_समुच्चयः **s, ध्रुव संरचना hash_ops *hash_ops, Ordered_समुच्चयः *l);
#परिभाषय ordered_समुच्चयः_put_string_समुच्चयः(s, l) ordered_समुच्चयः_put_string_समुच्चयः_पूर्णम्(s, &string_hash_ops_free, l)

शून्य ordered_समुच्चयः_print(FILE *f, ध्रुव वर्ण *क्षेत्रम्, Ordered_समुच्चयः *s);

#परिभाषय ORDERED_समुच्चयः_FOREACH_2(e, s, i) \
        प्रति (Iterator i = ITERATOR_FIRST; ordered_समुच्चयः_iterate((s), &i, (शून्य**)&(e)); )
#परिभाषय ORDERED_समुच्चयः_FOREACH(e, s) \
        ORDERED_समुच्चयः_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(Ordered_समुच्चयः*, ordered_समुच्चयः_free);

#परिभाषय cleanup_ordered_समुच्चयः_free _cleanup_(ordered_समुच्चयः_freep)
