/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/sysmacros.h>

#ಅನ್ತರ್ಭಾವ "hash-funcs.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "siphash24.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಶೂನ್ಯ string_hash_func(ಧ್ರುವ ವರ್ಣ *p, ಸಂರಚನಾ siphash *state) {
        siphash24_compress(p, strlen(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                ವರ್ಣ, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(
                string_hash_ops_free,
                ವರ್ಣ, string_hash_func, string_compare_func, ಮುಕ್ತ);
DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(
                string_hash_ops_ಮೌಲ್ಯ_free,
                ವರ್ಣ, string_hash_func, string_compare_func,
                ಶೂನ್ಯ, ಮುಕ್ತ);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_free,
                ವರ್ಣ, string_hash_func, string_compare_func, ಮುಕ್ತ,
                ಶೂನ್ಯ, ಮುಕ್ತ);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_strv_free,
                ವರ್ಣ, string_hash_func, string_compare_func, ಮುಕ್ತ,
                ವರ್ಣ*, strv_free);

ಶೂನ್ಯ path_hash_func(ಧ್ರುವ ವರ್ಣ *p, ಸಂರಚನಾ siphash *state) {
        bool add_slash = false;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        ಯದಿ (path_is_absolute(p))
                siphash24_compress_byte('/', state);

        ಪ್ರತಿ (;;) {
                ಧ್ರುವ ವರ್ಣ *e;
                ಪೂರ್ಣಾಙ್ಕ r;

                r = path_find_first_component(&p, true, &e);
                ಯದಿ (r == 0)
                        ಪ್ರತ್ಯಾಗಮ;

                ಯದಿ (add_slash)
                        siphash24_compress_byte('/', state);

                ಯದಿ (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        ಪ್ರತ್ಯಾಗಮ;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                add_slash = true;
        }
}

DEFINE_HASH_OPS(path_hash_ops,
                ವರ್ಣ, path_hash_func, path_compare);
DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(
                path_hash_ops_free,
                ವರ್ಣ, path_hash_func, path_compare, ಮುಕ್ತ);
DEFINE_HASH_OPS_FULL(
                path_hash_ops_free_free,
                ವರ್ಣ, path_hash_func, path_compare, ಮುಕ್ತ,
                ಶೂನ್ಯ, ಮುಕ್ತ);

ಶೂನ್ಯ trivial_hash_func(ಧ್ರುವ ಶೂನ್ಯ *p, ಸಂರಚನಾ siphash *state) {
        siphash24_compress_typesafe(p, state);
}

ಪೂರ್ಣಾಙ್ಕ trivial_compare_func(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b) {
        ಪ್ರತ್ಯಾಗಮ CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                ಶೂನ್ಯ, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(
                trivial_hash_ops_free,
                ಶೂನ್ಯ, trivial_hash_func, trivial_compare_func, ಮುಕ್ತ);
DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(
                trivial_hash_ops_ಮೌಲ್ಯ_free,
                ಶೂನ್ಯ, trivial_hash_func, trivial_compare_func,
                ಶೂನ್ಯ, ಮುಕ್ತ);
DEFINE_HASH_OPS_FULL(
                trivial_hash_ops_free_free,
                ಶೂನ್ಯ, trivial_hash_func, trivial_compare_func, ಮುಕ್ತ,
                ಶೂನ್ಯ, ಮುಕ್ತ);

ಶೂನ್ಯ uint64_hash_func(ಧ್ರುವ uint64_t *p, ಸಂರಚನಾ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(p);

        siphash24_compress_typesafe(*p, state);
}

ಪೂರ್ಣಾಙ್ಕ uint64_compare_func(ಧ್ರುವ uint64_t *a, ಧ್ರುವ uint64_t *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತ್ಯಾಗಮ CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(
                uint64_hash_ops_ಮೌಲ್ಯ_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                ಶೂನ್ಯ, ಮುಕ್ತ);

#ಯದಿ SIZEOF_DEV_T != 8
ಶೂನ್ಯ devt_hash_func(ಧ್ರುವ dev_t *p, ಸಂರಚನಾ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(p);

        siphash24_compress_typesafe(*p, state);
}
#endif

ಪೂರ್ಣಾಙ್ಕ devt_compare_func(ಧ್ರುವ dev_t *a, ಧ್ರುವ dev_t *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = CMP(major(*a), major(*b));
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
