/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <dirent.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "errno-util.h"
#include "glob-util.h"
#include "string-util.h"
#include "strv.h"

#ifndef __GLIBC__
static bool safe_glob_verify(const char *p, const char *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        nếu (isempty(p))
                trả_về false; /* should not happen, but for safey. */

        nếu (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                nếu (!p)
                        trả_về false; /* should not happen, but for safety. */
        }

        cho (;;) {
                p += strspn(p, "/");
                nếu (*p == '\0')
                        trả_về true;
                nếu (*p == '.') {
                        p++;
                        nếu (IN_SET(*p, '/', '\0'))
                                trả_về false; /* refuse dot */
                        nếu (*p == '.') {
                                p++;
                                nếu (IN_SET(*p, '/', '\0'))
                                        trả_về false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                nếu (*p == '\0')
                        trả_về true;
        }
}

static số_nguyên filter_glob_result(char * const *result, const char *path, char ***ret) {
        số_nguyên r;

        assert(path);

        nếu (strv_isempty(result))
                trả_về -ENOENT;

        _cleanup_free_ char *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        nếu (r < 0 && r != -ENOENT)
                trả_về r;

        _cleanup_strv_free_ char **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                nếu (!safe_glob_verify(*p, prefix))
                        continue;

                nếu (!ret)
                        trả_về 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (n_filtered == 0)
                trả_về -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        trả_về 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, void, closedir);

số_nguyên safe_glob_full(const char *path, số_nguyên flags, opendir_t opendir_func, char ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (struct dirent* (*)(void *)) readdir_no_dot,
                .gl_opendir = (void* (*)(const char *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        số_nguyên r;

        assert(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, NULL, &g);
        nếu (r == GLOB_NOMATCH)
                trả_về -ENOENT;
        nếu (r == GLOB_NOSPACE)
                trả_về -ENOMEM;
        nếu (r != 0)
                trả_về errno_or_else(EIO);

#ifndef __GLIBC__
        trả_về filter_glob_result(g.gl_pathv, path, ret);
#endif

        nếu (strv_isempty(g.gl_pathv))
                trả_về -ENOENT;

        nếu (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        trả_về 0;
}

số_nguyên glob_first(const char *path, char **ret) {
        _cleanup_strv_free_ char **v = NULL;
        số_nguyên r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        nếu (r == -ENOENT) {
                nếu (ret)
                        *ret = NULL;
                trả_về false;
        }
        nếu (r < 0)
                trả_về r;

        assert(!strv_isempty(v));

        nếu (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        trả_về true;
}

số_nguyên glob_extend(char ***strv, const char *path, số_nguyên flags) {
        char **v;
        số_nguyên r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        nếu (r < 0)
                trả_về r;

        trả_về strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

số_nguyên glob_non_glob_prefix(const char *path, char **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        nếu (path[n] != '\0')
                trong_khi (n > 0 && path[n-1] != '/')
                        n--;

        nếu (n == 0)
                trả_về -ENOENT;

        char *ans = strndup(path, n);
        nếu (!ans)
                trả_về -ENOMEM;
        *ret = ans;
        trả_về 0;
}

bool string_is_glob(const char *p) {
        /* Check if a string contains any glob patterns. */
        trả_về !!strpbrk(p, GLOB_CHARS);
}
