/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/sysmacros.h>

#அந்தர்பாவ "hash-funcs.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "siphash24.h"
#அந்தர்பாவ "strv.h"

வெறுமை string_hash_func(மாறிலி எழுத்து *p, கட்டமைப்பு siphash *state) {
        siphash24_compress(p, strlen(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                எழுத்து, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(
                string_hash_ops_free,
                எழுத்து, string_hash_func, string_compare_func, விடுவி);
DEFINE_HASH_OPS_WITH_மதிப்பு_DESTRUCTOR(
                string_hash_ops_மதிப்பு_free,
                எழுத்து, string_hash_func, string_compare_func,
                வெறுமை, விடுவி);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_free,
                எழுத்து, string_hash_func, string_compare_func, விடுவி,
                வெறுமை, விடுவி);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_strv_free,
                எழுத்து, string_hash_func, string_compare_func, விடுவி,
                எழுத்து*, strv_free);

வெறுமை path_hash_func(மாறிலி எழுத்து *p, கட்டமைப்பு siphash *state) {
        bool add_slash = false;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        எனில் (path_is_absolute(p))
                siphash24_compress_byte('/', state);

        க்கு (;;) {
                மாறிலி எழுத்து *e;
                முழு r;

                r = path_find_first_component(&p, true, &e);
                எனில் (r == 0)
                        திரும்பு;

                எனில் (add_slash)
                        siphash24_compress_byte('/', state);

                எனில் (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        திரும்பு;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                add_slash = true;
        }
}

DEFINE_HASH_OPS(path_hash_ops,
                எழுத்து, path_hash_func, path_compare);
DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(
                path_hash_ops_free,
                எழுத்து, path_hash_func, path_compare, விடுவி);
DEFINE_HASH_OPS_FULL(
                path_hash_ops_free_free,
                எழுத்து, path_hash_func, path_compare, விடுவி,
                வெறுமை, விடுவி);

வெறுமை trivial_hash_func(மாறிலி வெறுமை *p, கட்டமைப்பு siphash *state) {
        siphash24_compress_typesafe(p, state);
}

முழு trivial_compare_func(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b) {
        திரும்பு CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                வெறுமை, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(
                trivial_hash_ops_free,
                வெறுமை, trivial_hash_func, trivial_compare_func, விடுவி);
DEFINE_HASH_OPS_WITH_மதிப்பு_DESTRUCTOR(
                trivial_hash_ops_மதிப்பு_free,
                வெறுமை, trivial_hash_func, trivial_compare_func,
                வெறுமை, விடுவி);
DEFINE_HASH_OPS_FULL(
                trivial_hash_ops_free_free,
                வெறுமை, trivial_hash_func, trivial_compare_func, விடுவி,
                வெறுமை, விடுவி);

வெறுமை uint64_hash_func(மாறிலி uint64_t *p, கட்டமைப்பு siphash *state) {
        ப்ரதிஜ்ஞா(p);

        siphash24_compress_typesafe(*p, state);
}

முழு uint64_compare_func(மாறிலி uint64_t *a, மாறிலி uint64_t *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        திரும்பு CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_மதிப்பு_DESTRUCTOR(
                uint64_hash_ops_மதிப்பு_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                வெறுமை, விடுவி);

#எனில் SIZEOF_DEV_T != 8
வெறுமை devt_hash_func(மாறிலி dev_t *p, கட்டமைப்பு siphash *state) {
        ப்ரதிஜ்ஞா(p);

        siphash24_compress_typesafe(*p, state);
}
#endif

முழு devt_compare_func(மாறிலி dev_t *a, மாறிலி dev_t *b) {
        முழு r;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        r = CMP(major(*a), major(*b));
        எனில் (r != 0)
                திரும்பு r;

        திரும்பு CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
