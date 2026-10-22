/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/sysmacros.h>

#અન્તર્ભાવ "hash-funcs.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "siphash24.h"
#અન્તર્ભાવ "strv.h"

શૂન્ય string_hash_func(અચલ અક્ષર *p, સંરચના siphash *state) {
        siphash24_compress(p, strlen(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                અક્ષર, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(
                string_hash_ops_free,
                અક્ષર, string_hash_func, string_compare_func, મુક્ત);
DEFINE_HASH_OPS_WITH_કિંમત_DESTRUCTOR(
                string_hash_ops_કિંમત_free,
                અક્ષર, string_hash_func, string_compare_func,
                શૂન્ય, મુક્ત);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_free,
                અક્ષર, string_hash_func, string_compare_func, મુક્ત,
                શૂન્ય, મુક્ત);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_strv_free,
                અક્ષર, string_hash_func, string_compare_func, મુક્ત,
                અક્ષર*, strv_free);

શૂન્ય path_hash_func(અચલ અક્ષર *p, સંરચના siphash *state) {
        bool add_slash = false;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        જો (path_is_absolute(p))
                siphash24_compress_byte('/', state);

        માટે (;;) {
                અચલ અક્ષર *e;
                પૂર્ણાંક r;

                r = path_find_first_component(&p, true, &e);
                જો (r == 0)
                        પાછા;

                જો (add_slash)
                        siphash24_compress_byte('/', state);

                જો (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        પાછા;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                add_slash = true;
        }
}

DEFINE_HASH_OPS(path_hash_ops,
                અક્ષર, path_hash_func, path_compare);
DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(
                path_hash_ops_free,
                અક્ષર, path_hash_func, path_compare, મુક્ત);
DEFINE_HASH_OPS_FULL(
                path_hash_ops_free_free,
                અક્ષર, path_hash_func, path_compare, મુક્ત,
                શૂન્ય, મુક્ત);

શૂન્ય trivial_hash_func(અચલ શૂન્ય *p, સંરચના siphash *state) {
        siphash24_compress_typesafe(p, state);
}

પૂર્ણાંક trivial_compare_func(અચલ શૂન્ય *a, અચલ શૂન્ય *b) {
        પાછા CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                શૂન્ય, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(
                trivial_hash_ops_free,
                શૂન્ય, trivial_hash_func, trivial_compare_func, મુક્ત);
DEFINE_HASH_OPS_WITH_કિંમત_DESTRUCTOR(
                trivial_hash_ops_કિંમત_free,
                શૂન્ય, trivial_hash_func, trivial_compare_func,
                શૂન્ય, મુક્ત);
DEFINE_HASH_OPS_FULL(
                trivial_hash_ops_free_free,
                શૂન્ય, trivial_hash_func, trivial_compare_func, મુક્ત,
                શૂન્ય, મુક્ત);

શૂન્ય uint64_hash_func(અચલ uint64_t *p, સંરચના siphash *state) {
        પ્રતિજ્ઞા(p);

        siphash24_compress_typesafe(*p, state);
}

પૂર્ણાંક uint64_compare_func(અચલ uint64_t *a, અચલ uint64_t *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        પાછા CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_કિંમત_DESTRUCTOR(
                uint64_hash_ops_કિંમત_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                શૂન્ય, મુક્ત);

#જો SIZEOF_DEV_T != 8
શૂન્ય devt_hash_func(અચલ dev_t *p, સંરચના siphash *state) {
        પ્રતિજ્ઞા(p);

        siphash24_compress_typesafe(*p, state);
}
#endif

પૂર્ણાંક devt_compare_func(અચલ dev_t *a, અચલ dev_t *b) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        r = CMP(major(*a), major(*b));
        જો (r != 0)
                પાછા r;

        પાછા CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
