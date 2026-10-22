/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/sysmacros.h>

#അന്തര്ഭാവ "hash-funcs.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "siphash24.h"
#അന്തര്ഭാവ "strv.h"

ശൂന്യം string_hash_func(സ്ഥിരം അക്ഷരം *p, ഘടന siphash *state) {
        siphash24_compress(p, strlen(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                അക്ഷരം, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(
                string_hash_ops_free,
                അക്ഷരം, string_hash_func, string_compare_func, സ്വതന്ത്രം);
DEFINE_HASH_OPS_WITH_മൂല്യം_DESTRUCTOR(
                string_hash_ops_മൂല്യം_free,
                അക്ഷരം, string_hash_func, string_compare_func,
                ശൂന്യം, സ്വതന്ത്രം);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_free,
                അക്ഷരം, string_hash_func, string_compare_func, സ്വതന്ത്രം,
                ശൂന്യം, സ്വതന്ത്രം);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_strv_free,
                അക്ഷരം, string_hash_func, string_compare_func, സ്വതന്ത്രം,
                അക്ഷരം*, strv_free);

ശൂന്യം path_hash_func(സ്ഥിരം അക്ഷരം *p, ഘടന siphash *state) {
        bool add_slash = false;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        എങ്കിൽ (path_is_absolute(p))
                siphash24_compress_byte('/', state);

        വേണ്ടി (;;) {
                സ്ഥിരം അക്ഷരം *e;
                പൂർണ്ണസംഖ്യ r;

                r = path_find_first_component(&p, true, &e);
                എങ്കിൽ (r == 0)
                        തിരികെ;

                എങ്കിൽ (add_slash)
                        siphash24_compress_byte('/', state);

                എങ്കിൽ (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        തിരികെ;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                add_slash = true;
        }
}

DEFINE_HASH_OPS(path_hash_ops,
                അക്ഷരം, path_hash_func, path_compare);
DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(
                path_hash_ops_free,
                അക്ഷരം, path_hash_func, path_compare, സ്വതന്ത്രം);
DEFINE_HASH_OPS_FULL(
                path_hash_ops_free_free,
                അക്ഷരം, path_hash_func, path_compare, സ്വതന്ത്രം,
                ശൂന്യം, സ്വതന്ത്രം);

ശൂന്യം trivial_hash_func(സ്ഥിരം ശൂന്യം *p, ഘടന siphash *state) {
        siphash24_compress_typesafe(p, state);
}

പൂർണ്ണസംഖ്യ trivial_compare_func(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b) {
        തിരികെ CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                ശൂന്യം, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(
                trivial_hash_ops_free,
                ശൂന്യം, trivial_hash_func, trivial_compare_func, സ്വതന്ത്രം);
DEFINE_HASH_OPS_WITH_മൂല്യം_DESTRUCTOR(
                trivial_hash_ops_മൂല്യം_free,
                ശൂന്യം, trivial_hash_func, trivial_compare_func,
                ശൂന്യം, സ്വതന്ത്രം);
DEFINE_HASH_OPS_FULL(
                trivial_hash_ops_free_free,
                ശൂന്യം, trivial_hash_func, trivial_compare_func, സ്വതന്ത്രം,
                ശൂന്യം, സ്വതന്ത്രം);

ശൂന്യം uint64_hash_func(സ്ഥിരം uint64_t *p, ഘടന siphash *state) {
        പ്രതിജ്ഞാ(p);

        siphash24_compress_typesafe(*p, state);
}

പൂർണ്ണസംഖ്യ uint64_compare_func(സ്ഥിരം uint64_t *a, സ്ഥിരം uint64_t *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        തിരികെ CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_മൂല്യം_DESTRUCTOR(
                uint64_hash_ops_മൂല്യം_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                ശൂന്യം, സ്വതന്ത്രം);

#എങ്കിൽ SIZEOF_DEV_T != 8
ശൂന്യം devt_hash_func(സ്ഥിരം dev_t *p, ഘടന siphash *state) {
        പ്രതിജ്ഞാ(p);

        siphash24_compress_typesafe(*p, state);
}
#endif

പൂർണ്ണസംഖ്യ devt_compare_func(സ്ഥിരം dev_t *a, സ്ഥിരം dev_t *b) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        r = CMP(major(*a), major(*b));
        എങ്കിൽ (r != 0)
                തിരികെ r;

        തിരികെ CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
