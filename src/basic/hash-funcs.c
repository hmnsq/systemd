/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/sysmacros.h>

#ଅନ୍ତର୍ଭାଵ "hash-funcs.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "siphash24.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

ଶୂନ୍ୟ string_hash_func(ଅଚଳ ଅକ୍ଷର *p, ସଂରଚନା siphash *state) {
        siphash24_compress(p, strlen(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                ଅକ୍ଷର, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(
                string_hash_ops_free,
                ଅକ୍ଷର, string_hash_func, string_compare_func, ମୁକ୍ତ);
DEFINE_HASH_OPS_WITH_ମୂଲ୍ୟ_DESTRUCTOR(
                string_hash_ops_ମୂଲ୍ୟ_free,
                ଅକ୍ଷର, string_hash_func, string_compare_func,
                ଶୂନ୍ୟ, ମୁକ୍ତ);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_free,
                ଅକ୍ଷର, string_hash_func, string_compare_func, ମୁକ୍ତ,
                ଶୂନ୍ୟ, ମୁକ୍ତ);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_strv_free,
                ଅକ୍ଷର, string_hash_func, string_compare_func, ମୁକ୍ତ,
                ଅକ୍ଷର*, strv_free);

ଶୂନ୍ୟ path_hash_func(ଅଚଳ ଅକ୍ଷର *p, ସଂରଚନା siphash *state) {
        bool add_slash = false;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        ଯଦି (path_is_absolute(p))
                siphash24_compress_byte('/', state);

        ପାଇଁ (;;) {
                ଅଚଳ ଅକ୍ଷର *e;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

                r = path_find_first_component(&p, true, &e);
                ଯଦି (r == 0)
                        ଫେରନ୍ତୁ;

                ଯଦି (add_slash)
                        siphash24_compress_byte('/', state);

                ଯଦି (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        ଫେରନ୍ତୁ;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                add_slash = true;
        }
}

DEFINE_HASH_OPS(path_hash_ops,
                ଅକ୍ଷର, path_hash_func, path_compare);
DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(
                path_hash_ops_free,
                ଅକ୍ଷର, path_hash_func, path_compare, ମୁକ୍ତ);
DEFINE_HASH_OPS_FULL(
                path_hash_ops_free_free,
                ଅକ୍ଷର, path_hash_func, path_compare, ମୁକ୍ତ,
                ଶୂନ୍ୟ, ମୁକ୍ତ);

ଶୂନ୍ୟ trivial_hash_func(ଅଚଳ ଶୂନ୍ୟ *p, ସଂରଚନା siphash *state) {
        siphash24_compress_typesafe(p, state);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା trivial_compare_func(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b) {
        ଫେରନ୍ତୁ CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                ଶୂନ୍ୟ, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(
                trivial_hash_ops_free,
                ଶୂନ୍ୟ, trivial_hash_func, trivial_compare_func, ମୁକ୍ତ);
DEFINE_HASH_OPS_WITH_ମୂଲ୍ୟ_DESTRUCTOR(
                trivial_hash_ops_ମୂଲ୍ୟ_free,
                ଶୂନ୍ୟ, trivial_hash_func, trivial_compare_func,
                ଶୂନ୍ୟ, ମୁକ୍ତ);
DEFINE_HASH_OPS_FULL(
                trivial_hash_ops_free_free,
                ଶୂନ୍ୟ, trivial_hash_func, trivial_compare_func, ମୁକ୍ତ,
                ଶୂନ୍ୟ, ମୁକ୍ତ);

ଶୂନ୍ୟ uint64_hash_func(ଅଚଳ uint64_t *p, ସଂରଚନା siphash *state) {
        ପ୍ରତିଜ୍ଞା(p);

        siphash24_compress_typesafe(*p, state);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uint64_compare_func(ଅଚଳ uint64_t *a, ଅଚଳ uint64_t *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଫେରନ୍ତୁ CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_ମୂଲ୍ୟ_DESTRUCTOR(
                uint64_hash_ops_ମୂଲ୍ୟ_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                ଶୂନ୍ୟ, ମୁକ୍ତ);

#ଯଦି SIZEOF_DEV_T != 8
ଶୂନ୍ୟ devt_hash_func(ଅଚଳ dev_t *p, ସଂରଚନା siphash *state) {
        ପ୍ରତିଜ୍ଞା(p);

        siphash24_compress_typesafe(*p, state);
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା devt_compare_func(ଅଚଳ dev_t *a, ଅଚଳ dev_t *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        r = CMP(major(*a), major(*b));
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
