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

        якщо (isempty(p))
                повернути false; /* should not happen, but for safey. */

        якщо (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                якщо (!p)
                        повернути false; /* should not happen, but for safety. */
        }

        для (;;) {
                p += strspn(p, "/");
                якщо (*p == '\0')
                        повернути true;
                якщо (*p == '.') {
                        p++;
                        якщо (IN_SET(*p, '/', '\0'))
                                повернути false; /* refuse dot */
                        якщо (*p == '.') {
                                p++;
                                якщо (IN_SET(*p, '/', '\0'))
                                        повернути false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                якщо (*p == '\0')
                        повернути true;
        }
}

static ціле filter_glob_result(char * const *result, const char *path, char ***ret) {
        ціле r;

        assert(path);

        якщо (strv_isempty(result))
                повернути -ENOENT;

        _cleanup_free_ char *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        якщо (r < 0 && r != -ENOENT)
                повернути r;

        _cleanup_strv_free_ char **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                якщо (!safe_glob_verify(*p, prefix))
                        continue;

                якщо (!ret)
                        повернути 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                якщо (r < 0)
                        повернути r;
        }

        якщо (n_filtered == 0)
                повернути -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        повернути 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, void, closedir);

ціле safe_glob_full(const char *path, ціле flags, opendir_t opendir_func, char ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (struct dirent* (*)(void *)) readdir_no_dot,
                .gl_opendir = (void* (*)(const char *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        ціле r;

        assert(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, NULL, &g);
        якщо (r == GLOB_NOMATCH)
                повернути -ENOENT;
        якщо (r == GLOB_NOSPACE)
                повернути -ENOMEM;
        якщо (r != 0)
                повернути errno_or_else(EIO);

#ifndef __GLIBC__
        повернути filter_glob_result(g.gl_pathv, path, ret);
#endif

        якщо (strv_isempty(g.gl_pathv))
                повернути -ENOENT;

        якщо (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        повернути 0;
}

ціле glob_first(const char *path, char **ret) {
        _cleanup_strv_free_ char **v = NULL;
        ціле r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        якщо (r == -ENOENT) {
                якщо (ret)
                        *ret = NULL;
                повернути false;
        }
        якщо (r < 0)
                повернути r;

        assert(!strv_isempty(v));

        якщо (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        повернути true;
}

ціле glob_extend(char ***strv, const char *path, ціле flags) {
        char **v;
        ціле r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        якщо (r < 0)
                повернути r;

        повернути strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

ціле glob_non_glob_prefix(const char *path, char **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        якщо (path[n] != '\0')
                поки (n > 0 && path[n-1] != '/')
                        n--;

        якщо (n == 0)
                повернути -ENOENT;

        char *ans = strndup(path, n);
        якщо (!ans)
                повернути -ENOMEM;
        *ret = ans;
        повернути 0;
}

bool string_is_glob(const char *p) {
        /* Check if a string contains any glob patterns. */
        повернути !!strpbrk(p, GLOB_CHARS);
}
