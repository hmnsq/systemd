/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/sysmacros.h>

#অন্তর্ভাব "hash-funcs.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "siphash24.h"
#অন্তর্ভাব "strv.h"

শূন্য string_hash_func(ধ্রুবক অক্ষর *p, কাঠামো siphash *state) {
        siphash24_compress(p, strlen(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                অক্ষর, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(
                string_hash_ops_free,
                অক্ষর, string_hash_func, string_compare_func, মুক্ত);
DEFINE_HASH_OPS_WITH_মান_DESTRUCTOR(
                string_hash_ops_মান_free,
                অক্ষর, string_hash_func, string_compare_func,
                শূন্য, মুক্ত);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_free,
                অক্ষর, string_hash_func, string_compare_func, মুক্ত,
                শূন্য, মুক্ত);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_strv_free,
                অক্ষর, string_hash_func, string_compare_func, মুক্ত,
                অক্ষর*, strv_free);

শূন্য path_hash_func(ধ্রুবক অক্ষর *p, কাঠামো siphash *state) {
        bool add_slash = false;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        যদি (path_is_absolute(p))
                siphash24_compress_byte('/', state);

        জন্য (;;) {
                ধ্রুবক অক্ষর *e;
                পূর্ণসংখ্যা r;

                r = path_find_first_component(&p, true, &e);
                যদি (r == 0)
                        ফেরত;

                যদি (add_slash)
                        siphash24_compress_byte('/', state);

                যদি (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        ফেরত;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                add_slash = true;
        }
}

DEFINE_HASH_OPS(path_hash_ops,
                অক্ষর, path_hash_func, path_compare);
DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(
                path_hash_ops_free,
                অক্ষর, path_hash_func, path_compare, মুক্ত);
DEFINE_HASH_OPS_FULL(
                path_hash_ops_free_free,
                অক্ষর, path_hash_func, path_compare, মুক্ত,
                শূন্য, মুক্ত);

শূন্য trivial_hash_func(ধ্রুবক শূন্য *p, কাঠামো siphash *state) {
        siphash24_compress_typesafe(p, state);
}

পূর্ণসংখ্যা trivial_compare_func(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b) {
        ফেরত CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                শূন্য, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(
                trivial_hash_ops_free,
                শূন্য, trivial_hash_func, trivial_compare_func, মুক্ত);
DEFINE_HASH_OPS_WITH_মান_DESTRUCTOR(
                trivial_hash_ops_মান_free,
                শূন্য, trivial_hash_func, trivial_compare_func,
                শূন্য, মুক্ত);
DEFINE_HASH_OPS_FULL(
                trivial_hash_ops_free_free,
                শূন্য, trivial_hash_func, trivial_compare_func, মুক্ত,
                শূন্য, মুক্ত);

শূন্য uint64_hash_func(ধ্রুবক uint64_t *p, কাঠামো siphash *state) {
        প্রতিজ্ঞা(p);

        siphash24_compress_typesafe(*p, state);
}

পূর্ণসংখ্যা uint64_compare_func(ধ্রুবক uint64_t *a, ধ্রুবক uint64_t *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ফেরত CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_মান_DESTRUCTOR(
                uint64_hash_ops_মান_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                শূন্য, মুক্ত);

#যদি SIZEOF_DEV_T != 8
শূন্য devt_hash_func(ধ্রুবক dev_t *p, কাঠামো siphash *state) {
        প্রতিজ্ঞা(p);

        siphash24_compress_typesafe(*p, state);
}
#endif

পূর্ণসংখ্যা devt_compare_func(ধ্রুবক dev_t *a, ধ্রুবক dev_t *b) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        r = CMP(major(*a), major(*b));
        যদি (r != 0)
                ফেরত r;

        ফেরত CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
