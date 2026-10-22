/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/sysmacros.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "hash-funcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "siphash24.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

ਖਾਲੀ string_hash_func(ਅਚਲ ਅੱਖਰ *p, ਬਣਤਰ siphash *state) {
        siphash24_compress(p, ਲੜੀ_ਲੰਬਾਈ(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                ਅੱਖਰ, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(
                string_hash_ops_free,
                ਅੱਖਰ, string_hash_func, string_compare_func, ਮੁਕਤ);
DEFINE_HASH_OPS_WITH_ਮੁੱਲ_DESTRUCTOR(
                string_hash_ops_ਮੁੱਲ_free,
                ਅੱਖਰ, string_hash_func, string_compare_func,
                ਖਾਲੀ, ਮੁਕਤ);
DEFINE_HASH_OPS_ਭਰਿਆ(
                string_hash_ops_free_free,
                ਅੱਖਰ, string_hash_func, string_compare_func, ਮੁਕਤ,
                ਖਾਲੀ, ਮੁਕਤ);
DEFINE_HASH_OPS_ਭਰਿਆ(
                string_hash_ops_free_strv_free,
                ਅੱਖਰ, string_hash_func, string_compare_func, ਮੁਕਤ,
                ਅੱਖਰ*, strv_free);

ਖਾਲੀ ਰਸਤਾ_hash_func(ਅਚਲ ਅੱਖਰ *p, ਬਣਤਰ siphash *state) {
        bool ਜੋੜੋ_slash = false;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        ਜੇ (ਰਸਤਾ_is_absolute(p))
                siphash24_compress_byte('/', state);

        ਲਈ (;;) {
                ਅਚਲ ਅੱਖਰ *e;
                ਪੂਰਨ r;

                r = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&p, true, &e);
                ਜੇ (r == 0)
                        ਵਾਪਸ;

                ਜੇ (ਜੋੜੋ_slash)
                        siphash24_compress_byte('/', state);

                ਜੇ (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        ਵਾਪਸ;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                ਜੋੜੋ_slash = true;
        }
}

DEFINE_HASH_OPS(ਰਸਤਾ_hash_ops,
                ਅੱਖਰ, ਰਸਤਾ_hash_func, ਰਸਤਾ_compare);
DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(
                ਰਸਤਾ_hash_ops_free,
                ਅੱਖਰ, ਰਸਤਾ_hash_func, ਰਸਤਾ_compare, ਮੁਕਤ);
DEFINE_HASH_OPS_ਭਰਿਆ(
                ਰਸਤਾ_hash_ops_free_free,
                ਅੱਖਰ, ਰਸਤਾ_hash_func, ਰਸਤਾ_compare, ਮੁਕਤ,
                ਖਾਲੀ, ਮੁਕਤ);

ਖਾਲੀ trivial_hash_func(ਅਚਲ ਖਾਲੀ *p, ਬਣਤਰ siphash *state) {
        siphash24_compress_typesafe(p, state);
}

ਪੂਰਨ trivial_compare_func(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b) {
        ਵਾਪਸ CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                ਖਾਲੀ, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(
                trivial_hash_ops_free,
                ਖਾਲੀ, trivial_hash_func, trivial_compare_func, ਮੁਕਤ);
DEFINE_HASH_OPS_WITH_ਮੁੱਲ_DESTRUCTOR(
                trivial_hash_ops_ਮੁੱਲ_free,
                ਖਾਲੀ, trivial_hash_func, trivial_compare_func,
                ਖਾਲੀ, ਮੁਕਤ);
DEFINE_HASH_OPS_ਭਰਿਆ(
                trivial_hash_ops_free_free,
                ਖਾਲੀ, trivial_hash_func, trivial_compare_func, ਮੁਕਤ,
                ਖਾਲੀ, ਮੁਕਤ);

ਖਾਲੀ uint64_hash_func(ਅਚਲ uint64_t *p, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        siphash24_compress_typesafe(*p, state);
}

ਪੂਰਨ uint64_compare_func(ਅਚਲ uint64_t *a, ਅਚਲ uint64_t *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਵਾਪਸ CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_ਮੁੱਲ_DESTRUCTOR(
                uint64_hash_ops_ਮੁੱਲ_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                ਖਾਲੀ, ਮੁਕਤ);

#ਜੇ SIZEOF_DEV_T != 8
ਖਾਲੀ devt_hash_func(ਅਚਲ dev_t *p, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        siphash24_compress_typesafe(*p, state);
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ devt_compare_func(ਅਚਲ dev_t *a, ਅਚਲ dev_t *b) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        r = CMP(major(*a), major(*b));
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        ਵਾਪਸ CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
