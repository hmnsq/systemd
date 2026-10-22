/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/sysmacros.h>

#अन्तर्भाव "hash-funcs.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "siphash24.h"
#अन्तर्भाव "strv.h"

शून्य string_hash_func(अचर वर्ण *p, संरचना siphash *state) {
        siphash24_compress(p, म_माप(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                वर्ण, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_साँचो_DESTRUCTOR(
                string_hash_ops_free,
                वर्ण, string_hash_func, string_compare_func, मुक्त);
DEFINE_HASH_OPS_WITH_मान_DESTRUCTOR(
                string_hash_ops_मान_free,
                वर्ण, string_hash_func, string_compare_func,
                शून्य, मुक्त);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_free,
                वर्ण, string_hash_func, string_compare_func, मुक्त,
                शून्य, मुक्त);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_strv_free,
                वर्ण, string_hash_func, string_compare_func, मुक्त,
                वर्ण*, strv_free);

शून्य path_hash_func(अचर वर्ण *p, संरचना siphash *state) {
        bool add_slash = false;

        प्रतिज्ञा(p);
        प्रतिज्ञा(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        यदि (path_is_absolute(p))
                siphash24_compress_byte('/', state);

        लागि (;;) {
                अचर वर्ण *e;
                पूर्णाङ्क r;

                r = path_find_first_component(&p, true, &e);
                यदि (r == 0)
                        फर्काऊ;

                यदि (add_slash)
                        siphash24_compress_byte('/', state);

                यदि (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        फर्काऊ;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                add_slash = true;
        }
}

DEFINE_HASH_OPS(path_hash_ops,
                वर्ण, path_hash_func, path_compare);
DEFINE_HASH_OPS_WITH_साँचो_DESTRUCTOR(
                path_hash_ops_free,
                वर्ण, path_hash_func, path_compare, मुक्त);
DEFINE_HASH_OPS_FULL(
                path_hash_ops_free_free,
                वर्ण, path_hash_func, path_compare, मुक्त,
                शून्य, मुक्त);

शून्य trivial_hash_func(अचर शून्य *p, संरचना siphash *state) {
        siphash24_compress_typesafe(p, state);
}

पूर्णाङ्क trivial_compare_func(अचर शून्य *a, अचर शून्य *b) {
        फर्काऊ CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                शून्य, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_साँचो_DESTRUCTOR(
                trivial_hash_ops_free,
                शून्य, trivial_hash_func, trivial_compare_func, मुक्त);
DEFINE_HASH_OPS_WITH_मान_DESTRUCTOR(
                trivial_hash_ops_मान_free,
                शून्य, trivial_hash_func, trivial_compare_func,
                शून्य, मुक्त);
DEFINE_HASH_OPS_FULL(
                trivial_hash_ops_free_free,
                शून्य, trivial_hash_func, trivial_compare_func, मुक्त,
                शून्य, मुक्त);

शून्य uint64_hash_func(अचर uint64_t *p, संरचना siphash *state) {
        प्रतिज्ञा(p);

        siphash24_compress_typesafe(*p, state);
}

पूर्णाङ्क uint64_compare_func(अचर uint64_t *a, अचर uint64_t *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        फर्काऊ CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_मान_DESTRUCTOR(
                uint64_hash_ops_मान_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                शून्य, मुक्त);

#यदि SIZEOF_DEV_T != 8
शून्य devt_hash_func(अचर dev_t *p, संरचना siphash *state) {
        प्रतिज्ञा(p);

        siphash24_compress_typesafe(*p, state);
}
#endif

पूर्णाङ्क devt_compare_func(अचर dev_t *a, अचर dev_t *b) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = CMP(major(*a), major(*b));
        यदि (r != 0)
                फर्काऊ r;

        फर्काऊ CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
