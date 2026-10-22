/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <sys/sysmacros.h>

#समावेश "hash-funcs.h"
#समावेश "path-util.h"
#समावेश "siphash24.h"
#समावेश "strv.h"

व्योम string_hash_func(स्थिर अक्षर *p, काष्ठा siphash *state) {
        siphash24_compress(p, म_माप(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                अक्षर, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(
                string_hash_ops_free,
                अक्षर, string_hash_func, string_compare_func, मुक्त);
DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR(
                string_hash_ops_मूल्य_free,
                अक्षर, string_hash_func, string_compare_func,
                व्योम, मुक्त);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_free,
                अक्षर, string_hash_func, string_compare_func, मुक्त,
                व्योम, मुक्त);
DEFINE_HASH_OPS_FULL(
                string_hash_ops_free_strv_free,
                अक्षर, string_hash_func, string_compare_func, मुक्त,
                अक्षर*, strv_free);

व्योम path_hash_func(स्थिर अक्षर *p, काष्ठा siphash *state) {
        bool add_slash = false;

        निश्चित(p);
        निश्चित(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        अगर (path_is_absolute(p))
                siphash24_compress_byte('/', state);

        क्रम (;;) {
                स्थिर अक्षर *e;
                पूर्णांक r;

                r = path_find_first_component(&p, true, &e);
                अगर (r == 0)
                        वापस;

                अगर (add_slash)
                        siphash24_compress_byte('/', state);

                अगर (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        वापस;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                add_slash = true;
        }
}

DEFINE_HASH_OPS(path_hash_ops,
                अक्षर, path_hash_func, path_compare);
DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(
                path_hash_ops_free,
                अक्षर, path_hash_func, path_compare, मुक्त);
DEFINE_HASH_OPS_FULL(
                path_hash_ops_free_free,
                अक्षर, path_hash_func, path_compare, मुक्त,
                व्योम, मुक्त);

व्योम trivial_hash_func(स्थिर व्योम *p, काष्ठा siphash *state) {
        siphash24_compress_typesafe(p, state);
}

पूर्णांक trivial_compare_func(स्थिर व्योम *a, स्थिर व्योम *b) {
        वापस CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                व्योम, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(
                trivial_hash_ops_free,
                व्योम, trivial_hash_func, trivial_compare_func, मुक्त);
DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR(
                trivial_hash_ops_मूल्य_free,
                व्योम, trivial_hash_func, trivial_compare_func,
                व्योम, मुक्त);
DEFINE_HASH_OPS_FULL(
                trivial_hash_ops_free_free,
                व्योम, trivial_hash_func, trivial_compare_func, मुक्त,
                व्योम, मुक्त);

व्योम uint64_hash_func(स्थिर uint64_t *p, काष्ठा siphash *state) {
        निश्चित(p);

        siphash24_compress_typesafe(*p, state);
}

पूर्णांक uint64_compare_func(स्थिर uint64_t *a, स्थिर uint64_t *b) {
        निश्चित(a);
        निश्चित(b);

        वापस CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR(
                uint64_hash_ops_मूल्य_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                व्योम, मुक्त);

#अगर SIZEOF_DEV_T != 8
व्योम devt_hash_func(स्थिर dev_t *p, काष्ठा siphash *state) {
        निश्चित(p);

        siphash24_compress_typesafe(*p, state);
}
#पूर्ण_अगर

पूर्णांक devt_compare_func(स्थिर dev_t *a, स्थिर dev_t *b) {
        पूर्णांक r;

        निश्चित(a);
        निश्चित(b);

        r = CMP(major(*a), major(*b));
        अगर (r != 0)
                वापस r;

        वापस CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
