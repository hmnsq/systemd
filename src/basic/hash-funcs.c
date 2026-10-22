/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <sys/sysmacros.h>

#ضمن "hash-funcs.h"
#ضمن "path-util.h"
#ضمن "siphash24.h"
#ضمن "strv.h"

فراغ string_hash_func(ثابت حرف *p, بنية siphash *state) {
        siphash24_compress(p, strlen(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                حرف, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(
                string_hash_ops_free,
                حرف, string_hash_func, string_compare_func, حرر);
DEFINE_HASH_OPS_WITH_VALUE_DESTRUCTOR(
                string_hash_ops_value_free,
                حرف, string_hash_func, string_compare_func,
                فراغ, حرر);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_free,
                حرف, string_hash_func, string_compare_func, حرر,
                فراغ, حرر);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_strv_free,
                حرف, string_hash_func, string_compare_func, حرر,
                حرف*, strv_free);

فراغ path_hash_func(ثابت حرف *p, بنية siphash *state) {
        bool add_slash = false;

        أكد(p);
        أكد(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        𐤀𐤌 (path_is_absolute(p))
                siphash24_compress_byte('/', state);

        لكل (;;) {
                ثابت حرف *e;
                صحيح r;

                r = path_find_first_component(&p, true, &e);
                𐤀𐤌 (r == 0)
                        أرجع;

                𐤀𐤌 (add_slash)
                        siphash24_compress_byte('/', state);

                𐤀𐤌 (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        أرجع;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                add_slash = true;
        }
}

DEFINE_HASH_OPS(path_hash_ops,
                حرف, path_hash_func, path_compare);
DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(
                path_hash_ops_free,
                حرف, path_hash_func, path_compare, حرر);
DEFINE_HASH_OPS_FULL(
                path_hash_ops_free_free,
                حرف, path_hash_func, path_compare, حرر,
                فراغ, حرر);

فراغ trivial_hash_func(ثابت فراغ *p, بنية siphash *state) {
        siphash24_compress_typesafe(p, state);
}

صحيح trivial_compare_func(ثابت فراغ *a, ثابت فراغ *b) {
        أرجع CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                فراغ, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(
                trivial_hash_ops_free,
                فراغ, trivial_hash_func, trivial_compare_func, حرر);
DEFINE_HASH_OPS_WITH_VALUE_DESTRUCTOR(
                trivial_hash_ops_value_free,
                فراغ, trivial_hash_func, trivial_compare_func,
                فراغ, حرر);
DEFINE_HASH_OPS_FULL(
                trivial_hash_ops_free_free,
                فراغ, trivial_hash_func, trivial_compare_func, حرر,
                فراغ, حرر);

فراغ uint64_hash_func(ثابت uint64_t *p, بنية siphash *state) {
        أكد(p);

        siphash24_compress_typesafe(*p, state);
}

صحيح uint64_compare_func(ثابت uint64_t *a, ثابت uint64_t *b) {
        أكد(a);
        أكد(b);

        أرجع CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_VALUE_DESTRUCTOR(
                uint64_hash_ops_value_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                فراغ, حرر);

#𐤀𐤌 SIZEOF_DEV_T != 8
فراغ devt_hash_func(ثابت dev_t *p, بنية siphash *state) {
        أكد(p);

        siphash24_compress_typesafe(*p, state);
}
#endif

صحيح devt_compare_func(ثابت dev_t *a, ثابت dev_t *b) {
        صحيح r;

        أكد(a);
        أكد(b);

        r = CMP(major(*a), major(*b));
        𐤀𐤌 (r != 0)
                أرجع r;

        أرجع CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
