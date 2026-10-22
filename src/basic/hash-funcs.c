/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>
#شامل <string.h>
#شامل <sys/sysmacros.h>

#شامل "hash-funcs.h"
#شامل "path-util.h"
#شامل "siphash24.h"
#شامل "strv.h"

خالی string_hash_func(ثابت حرف *p, ساخت siphash *state) {
        siphash24_compress(p, لڑی_لمبائی(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                حرف, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
                string_hash_ops_free,
                حرف, string_hash_func, string_compare_func, آزاد);
DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR(
                string_hash_ops_قدر_free,
                حرف, string_hash_func, string_compare_func,
                خالی, آزاد);
DEFINE_HASH_OPS_بھرا(
                string_hash_ops_free_free,
                حرف, string_hash_func, string_compare_func, آزاد,
                خالی, آزاد);
DEFINE_HASH_OPS_بھرا(
                string_hash_ops_free_strv_free,
                حرف, string_hash_func, string_compare_func, آزاد,
                حرف*, strv_free);

خالی راستہ_hash_func(ثابت حرف *p, ساخت siphash *state) {
        bool جوڑو_slash = false;

        أكد(p);
        أكد(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        اگر (راستہ_is_absolute(p))
                siphash24_compress_byte('/', state);

        برائے (;;) {
                ثابت حرف *e;
                صحیح r;

                r = راستہ_ڈھونڈو_پہلا_component(&p, true, &e);
                اگر (r == 0)
                        واپس;

                اگر (جوڑو_slash)
                        siphash24_compress_byte('/', state);

                اگر (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        واپس;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                جوڑو_slash = true;
        }
}

DEFINE_HASH_OPS(راستہ_hash_ops,
                حرف, راستہ_hash_func, راستہ_compare);
DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
                راستہ_hash_ops_free,
                حرف, راستہ_hash_func, راستہ_compare, آزاد);
DEFINE_HASH_OPS_بھرا(
                راستہ_hash_ops_free_free,
                حرف, راستہ_hash_func, راستہ_compare, آزاد,
                خالی, آزاد);

خالی trivial_hash_func(ثابت خالی *p, ساخت siphash *state) {
        siphash24_compress_typesafe(p, state);
}

صحیح trivial_compare_func(ثابت خالی *a, ثابت خالی *b) {
        واپس CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                خالی, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
                trivial_hash_ops_free,
                خالی, trivial_hash_func, trivial_compare_func, آزاد);
DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR(
                trivial_hash_ops_قدر_free,
                خالی, trivial_hash_func, trivial_compare_func,
                خالی, آزاد);
DEFINE_HASH_OPS_بھرا(
                trivial_hash_ops_free_free,
                خالی, trivial_hash_func, trivial_compare_func, آزاد,
                خالی, آزاد);

خالی uint64_hash_func(ثابت uint64_t *p, ساخت siphash *state) {
        أكد(p);

        siphash24_compress_typesafe(*p, state);
}

صحیح uint64_compare_func(ثابت uint64_t *a, ثابت uint64_t *b) {
        أكد(a);
        أكد(b);

        واپس CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR(
                uint64_hash_ops_قدر_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                خالی, آزاد);

#اگر SIZEOF_DEV_T != 8
خالی devt_hash_func(ثابت dev_t *p, ساخت siphash *state) {
        أكد(p);

        siphash24_compress_typesafe(*p, state);
}
#اگر_ختم

صحیح devt_compare_func(ثابت dev_t *a, ثابت dev_t *b) {
        صحیح r;

        أكد(a);
        أكد(b);

        r = CMP(major(*a), major(*b));
        اگر (r != 0)
                واپس r;

        واپس CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
