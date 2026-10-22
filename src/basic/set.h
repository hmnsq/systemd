/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "hashmap.h"

#परिभाषय समुच्चयः_free_and_replace(a, b)              \
        free_and_replace_पूर्णम्(a, b, समुच्चयः_free)

समुच्चयः_2* समुच्चयः_new(ध्रुव संरचना hash_ops *hash_ops);

स्थिर अन्तःस्थ समुच्चयः_2* समुच्चयः_free(समुच्चयः_2 *s) {
        प्रत्यागम (समुच्चयः_2*) _hashmap_free(HASHMAP_आधारः(s));
}

#परिभाषय समुच्चयः_copy(s) ((समुच्चयः_2*) _hashmap_copy(HASHMAP_आधारः(s)))

पूर्णाङ्क समुच्चयः_ensure_allocated(समुच्चयः_2 **s, ध्रुव संरचना hash_ops *hash_ops);

पूर्णाङ्क समुच्चयः_put(समुच्चयः_2 *s, ध्रुव शून्य *कुञ्चिका);
/* no set_update */
/* no set_replace */
स्थिर अन्तःस्थ शून्य *समुच्चयः_get(ध्रुव समुच्चयः_2 *s, ध्रुव शून्य *कुञ्चिका) {
        प्रत्यागम _hashmap_get(HASHMAP_आधारः((समुच्चयः_2 *) s), कुञ्चिका);
}
/* no set_get2 */

स्थिर अन्तःस्थ bool समुच्चयः_अन्तर्भवति(ध्रुव समुच्चयः_2 *s, ध्रुव शून्य *कुञ्चिका) {
        प्रत्यागम hashmap_अन्तर्भवति_2(HASHMAP_आधारः((समुच्चयः_2 *) s), कुञ्चिका);
}

स्थिर अन्तःस्थ शून्य *समुच्चयः_remove(समुच्चयः_2 *s, ध्रुव शून्य *कुञ्चिका) {
        प्रत्यागम _hashmap_remove(HASHMAP_आधारः(s), कुञ्चिका);
}

/* no set_remove2 */
/* no set_remove_value */
पूर्णाङ्क समुच्चयः_remove_and_put(समुच्चयः_2 *s, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका);
/* no set_remove_and_replace */
पूर्णाङ्क समुच्चयः_merge(समुच्चयः_2 *s, समुच्चयः_2 *other);

स्थिर अन्तःस्थ पूर्णाङ्क समुच्चयः_reserve(समुच्चयः_2 *h, अचिह्नित entries_सङ्कलय) {
        प्रत्यागम _hashmap_reserve(HASHMAP_आधारः(h), entries_सङ्कलय);
}

स्थिर अन्तःस्थ पूर्णाङ्क समुच्चयः_move(समुच्चयः_2 *s, समुच्चयः_2 *other) {
        प्रत्यागम _hashmap_move(HASHMAP_आधारः(s), HASHMAP_आधारः(other));
}

स्थिर अन्तःस्थ पूर्णाङ्क समुच्चयः_move_one(समुच्चयः_2 *s, समुच्चयः_2 *other, ध्रुव शून्य *कुञ्चिका) {
        प्रत्यागम _hashmap_move_one(HASHMAP_आधारः(s), HASHMAP_आधारः(other), कुञ्चिका);
}

स्थिर अन्तःस्थ अचिह्नित समुच्चयः_आकार(ध्रुव समुच्चयः_2 *s) {
        प्रत्यागम hashmap_आकार(HASHMAP_आधारः((समुच्चयः_2 *) s));
}

स्थिर अन्तःस्थ bool समुच्चयः_isempty(ध्रुव समुच्चयः_2 *s) {
        प्रत्यागम समुच्चयः_आकार(s) == 0;
}

स्थिर अन्तःस्थ अचिह्नित समुच्चयः_buckets(ध्रुव समुच्चयः_2 *s) {
        प्रत्यागम _hashmap_buckets(HASHMAP_आधारः((समुच्चयः_2 *) s));
}

स्थिर अन्तःस्थ bool समुच्चयः_iterate(ध्रुव समुच्चयः_2 *s, Iterator *i, शून्य **मानम्) {
        प्रत्यागम _hashmap_iterate(HASHMAP_आधारः((समुच्चयः_2*) s), i, मानम्, रिक्त);
}

स्थिर अन्तःस्थ शून्य समुच्चयः_clear(समुच्चयः_2 *s) {
        _hashmap_clear(HASHMAP_आधारः(s));
}

स्थिर अन्तःस्थ शून्य *समुच्चयः_steal_first(समुच्चयः_2 *s) {
        प्रत्यागम hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_आधारः(s), true, रिक्त);
}

/* no set_steal_first_key */
/* no set_first_key */

स्थिर अन्तःस्थ शून्य *समुच्चयः_first(ध्रुव समुच्चयः_2 *s) {
        प्रत्यागम hashmap_first_कुञ्चिका_and_मानम्(HASHMAP_आधारः((समुच्चयः_2 *) s), false, रिक्त);
}

/* no set_next */

स्थिर अन्तःस्थ वर्ण **समुच्चयः_get_strv(समुच्चयः_2 *s) {
        प्रत्यागम _hashmap_get_strv(HASHMAP_आधारः(s));
}

वर्ण** समुच्चयः_गन्तव्यम्_strv(समुच्चयः_2 **s);

पूर्णाङ्क समुच्चयः_ensure_put(समुच्चयः_2 **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका);

पूर्णाङ्क समुच्चयः_ensure_consume(समुच्चयः_2 **s, ध्रुव संरचना hash_ops *hash_ops, शून्य *कुञ्चिका);

पूर्णाङ्क समुच्चयः_consume(समुच्चयः_2 *s, शून्य *मानम्);

पूर्णाङ्क समुच्चयः_put_strndup_पूर्णम्(समुच्चयः_2 **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *p, size_t n);
#परिभाषय समुच्चयः_put_strdup_पूर्णम्(s, hash_ops, p) समुच्चयः_put_strndup_पूर्णम्(s, hash_ops, p, SIZE_MAX)
#परिभाषय समुच्चयः_put_strndup(s, p, n) समुच्चयः_put_strndup_पूर्णम्(s, &string_hash_ops_free, p, n)
#परिभाषय समुच्चयः_put_strdup(s, p) समुच्चयः_put_strndup(s, p, SIZE_MAX)

पूर्णाङ्क समुच्चयः_put_strdupv_पूर्णम्(समुच्चयः_2 **s, ध्रुव संरचना hash_ops *hash_ops, वर्ण **l);
#परिभाषय समुच्चयः_put_strdupv(s, l) समुच्चयः_put_strdupv_पूर्णम्(s, &string_hash_ops_free, l)

पूर्णाङ्क समुच्चयः_put_strsplit(समुच्चयः_2 *s, ध्रुव वर्ण *v, ध्रुव वर्ण *separators, ExtractFlags flags);

#परिभाषय समुच्चयः_FOREACH_2(e, s, i) \
        प्रति (Iterator i = ITERATOR_FIRST; समुच्चयः_iterate((s), &i, (शून्य**)&(e)); )
#परिभाषय समुच्चयः_FOREACH(e, s) \
        समुच्चयः_FOREACH_2(e, s, UNIQ_T(i, UNIQ))

#परिभाषय समुच्चयः_FOREACH_MOVE(e, d, s)                                       \
        प्रति (; ({ e = समुच्चयः_first(s); assert_se(!e || समुच्चयः_move_one(d, s, e) >= 0); e; }); )

DEFINE_TRIVIAL_CLEANUP_FUNC(समुच्चयः_2*, समुच्चयः_free);

#परिभाषय cleanup_समुच्चयः_free _cleanup_(समुच्चयः_freep)

पूर्णाङ्क समुच्चयः_strjoin(समुच्चयः_2 *s, ध्रुव वर्ण *separator, bool wrap_with_separator, वर्ण **ret);

bool समुच्चयः_equal(समुच्चयः_2 *a, समुच्चयः_2 *b);

bool समुच्चयः_fnmatch(समुच्चयः_2 *include_patterns, समुच्चयः_2 *exclude_patterns, ध्रुव वर्ण *needle);
