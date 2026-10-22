/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/sysmacros.h>

#ಅನ್ತರ್ಭಾವ "hash-funcs.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "siphash24.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಶೂನ್ಯ string_hash_func(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ರಚನೆ siphash *state) {
        siphash24_compress(p, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                ಅಕ್ಷರ, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(
                string_hash_ops_free,
                ಅಕ್ಷರ, string_hash_func, string_compare_func, ಬಿಡುಗಡೆ);
DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(
                string_hash_ops_ಮೌಲ್ಯ_free,
                ಅಕ್ಷರ, string_hash_func, string_compare_func,
                ಶೂನ್ಯ, ಬಿಡುಗಡೆ);
DEFINE_HASH_OPS_ತುಂಬಿದ(
                string_hash_ops_free_free,
                ಅಕ್ಷರ, string_hash_func, string_compare_func, ಬಿಡುಗಡೆ,
                ಶೂನ್ಯ, ಬಿಡುಗಡೆ);
DEFINE_HASH_OPS_ತುಂಬಿದ(
                string_hash_ops_free_strv_free,
                ಅಕ್ಷರ, string_hash_func, string_compare_func, ಬಿಡುಗಡೆ,
                ಅಕ್ಷರ*, strv_free);

ಶೂನ್ಯ ಮಾರ್ಗ_hash_func(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ರಚನೆ siphash *state) {
        bool ಕೂಡಿಸು_slash = false;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        ಆದರೆ (ಮಾರ್ಗ_is_absolute(p))
                siphash24_compress_byte('/', state);

        ಗಾಗಿ (;;) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
                ಪೂರ್ಣಾಂಕ r;

                r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&p, true, &e);
                ಆದರೆ (r == 0)
                        ಹಿಂತಿರುಗಿ;

                ಆದರೆ (ಕೂಡಿಸು_slash)
                        siphash24_compress_byte('/', state);

                ಆದರೆ (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        ಹಿಂತಿರುಗಿ;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                ಕೂಡಿಸು_slash = true;
        }
}

DEFINE_HASH_OPS(ಮಾರ್ಗ_hash_ops,
                ಅಕ್ಷರ, ಮಾರ್ಗ_hash_func, ಮಾರ್ಗ_compare);
DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(
                ಮಾರ್ಗ_hash_ops_free,
                ಅಕ್ಷರ, ಮಾರ್ಗ_hash_func, ಮಾರ್ಗ_compare, ಬಿಡುಗಡೆ);
DEFINE_HASH_OPS_ತುಂಬಿದ(
                ಮಾರ್ಗ_hash_ops_free_free,
                ಅಕ್ಷರ, ಮಾರ್ಗ_hash_func, ಮಾರ್ಗ_compare, ಬಿಡುಗಡೆ,
                ಶೂನ್ಯ, ಬಿಡುಗಡೆ);

ಶೂನ್ಯ trivial_hash_func(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, ರಚನೆ siphash *state) {
        siphash24_compress_typesafe(p, state);
}

ಪೂರ್ಣಾಂಕ trivial_compare_func(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b) {
        ಹಿಂತಿರುಗಿ CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                ಶೂನ್ಯ, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(
                trivial_hash_ops_free,
                ಶೂನ್ಯ, trivial_hash_func, trivial_compare_func, ಬಿಡುಗಡೆ);
DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(
                trivial_hash_ops_ಮೌಲ್ಯ_free,
                ಶೂನ್ಯ, trivial_hash_func, trivial_compare_func,
                ಶೂನ್ಯ, ಬಿಡುಗಡೆ);
DEFINE_HASH_OPS_ತುಂಬಿದ(
                trivial_hash_ops_free_free,
                ಶೂನ್ಯ, trivial_hash_func, trivial_compare_func, ಬಿಡುಗಡೆ,
                ಶೂನ್ಯ, ಬಿಡುಗಡೆ);

ಶೂನ್ಯ uint64_hash_func(ಸ್ಥಿರಾಂಕ uint64_t *p, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(p);

        siphash24_compress_typesafe(*p, state);
}

ಪೂರ್ಣಾಂಕ uint64_compare_func(ಸ್ಥಿರಾಂಕ uint64_t *a, ಸ್ಥಿರಾಂಕ uint64_t *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಹಿಂತಿರುಗಿ CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(
                uint64_hash_ops_ಮೌಲ್ಯ_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                ಶೂನ್ಯ, ಬಿಡುಗಡೆ);

#ಆದರೆ SIZEOF_DEV_T != 8
ಶೂನ್ಯ devt_hash_func(ಸ್ಥಿರಾಂಕ dev_t *p, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(p);

        siphash24_compress_typesafe(*p, state);
}
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ devt_compare_func(ಸ್ಥಿರಾಂಕ dev_t *a, ಸ್ಥಿರಾಂಕ dev_t *b) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = CMP(major(*a), major(*b));
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
