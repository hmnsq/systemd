/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>
#شامل <string.h>
#شامل <sys/sysmacros.h>

#شامل "hash-funcs.h"
#شامل "path-util.h"
#شامل "siphash24.h"
#شامل "strv.h"

تهی string_hash_func(ثابت نویسه *p, ساختار siphash *state) {
        siphash24_compress(p, strlen(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                نویسه, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
                string_hash_ops_free,
                نویسه, string_hash_func, string_compare_func, آزاد);
DEFINE_HASH_OPS_WITH_مقدار_DESTRUCTOR(
                string_hash_ops_مقدار_free,
                نویسه, string_hash_func, string_compare_func,
                تهی, آزاد);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_free,
                نویسه, string_hash_func, string_compare_func, آزاد,
                تهی, آزاد);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_strv_free,
                نویسه, string_hash_func, string_compare_func, آزاد,
                نویسه*, strv_free);

تهی path_hash_func(ثابت نویسه *p, ساختار siphash *state) {
        bool add_slash = false;

        أكد(p);
        أكد(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        اگر (path_is_absolute(p))
                siphash24_compress_byte('/', state);

        برای (;;) {
                ثابت نویسه *e;
                صحیح r;

                r = path_find_first_component(&p, true, &e);
                اگر (r == 0)
                        بازگشت;

                اگر (add_slash)
                        siphash24_compress_byte('/', state);

                اگر (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        بازگشت;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                add_slash = true;
        }
}

DEFINE_HASH_OPS(path_hash_ops,
                نویسه, path_hash_func, path_compare);
DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
                path_hash_ops_free,
                نویسه, path_hash_func, path_compare, آزاد);
DEFINE_HASH_OPS_FULL(
                path_hash_ops_free_free,
                نویسه, path_hash_func, path_compare, آزاد,
                تهی, آزاد);

تهی trivial_hash_func(ثابت تهی *p, ساختار siphash *state) {
        siphash24_compress_typesafe(p, state);
}

صحیح trivial_compare_func(ثابت تهی *a, ثابت تهی *b) {
        بازگشت CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                تهی, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(
                trivial_hash_ops_free,
                تهی, trivial_hash_func, trivial_compare_func, آزاد);
DEFINE_HASH_OPS_WITH_مقدار_DESTRUCTOR(
                trivial_hash_ops_مقدار_free,
                تهی, trivial_hash_func, trivial_compare_func,
                تهی, آزاد);
DEFINE_HASH_OPS_FULL(
                trivial_hash_ops_free_free,
                تهی, trivial_hash_func, trivial_compare_func, آزاد,
                تهی, آزاد);

تهی uint64_hash_func(ثابت uint64_t *p, ساختار siphash *state) {
        أكد(p);

        siphash24_compress_typesafe(*p, state);
}

صحیح uint64_compare_func(ثابت uint64_t *a, ثابت uint64_t *b) {
        أكد(a);
        أكد(b);

        بازگشت CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_مقدار_DESTRUCTOR(
                uint64_hash_ops_مقدار_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                تهی, آزاد);

#اگر SIZEOF_DEV_T != 8
تهی devt_hash_func(ثابت dev_t *p, ساختار siphash *state) {
        أكد(p);

        siphash24_compress_typesafe(*p, state);
}
#endif

صحیح devt_compare_func(ثابت dev_t *a, ثابت dev_t *b) {
        صحیح r;

        أكد(a);
        أكد(b);

        r = CMP(major(*a), major(*b));
        اگر (r != 0)
                بازگشت r;

        بازگشت CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
