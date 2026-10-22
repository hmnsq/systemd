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

        ако (isempty(p))
                върни false; /* should not happen, but for safey. */

        ако (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                ако (!p)
                        върни false; /* should not happen, but for safety. */
        }

        за (;;) {
                p += strspn(p, "/");
                ако (*p == '\0')
                        върни true;
                ако (*p == '.') {
                        p++;
                        ако (IN_SET(*p, '/', '\0'))
                                върни false; /* refuse dot */
                        ако (*p == '.') {
                                p++;
                                ако (IN_SET(*p, '/', '\0'))
                                        върни false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ако (*p == '\0')
                        върни true;
        }
}

static цяло filter_glob_result(char * const *result, const char *path, char ***ret) {
        цяло r;

        assert(path);

        ако (strv_isempty(result))
                върни -ENOENT;

        _cleanup_free_ char *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        ако (r < 0 && r != -ENOENT)
                върни r;

        _cleanup_strv_free_ char **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                ако (!safe_glob_verify(*p, prefix))
                        continue;

                ако (!ret)
                        върни 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                ако (r < 0)
                        върни r;
        }

        ако (n_filtered == 0)
                върни -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        върни 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, void, closedir);

цяло safe_glob_full(const char *path, цяло flags, opendir_t opendir_func, char ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (struct dirent* (*)(void *)) readdir_no_dot,
                .gl_opendir = (void* (*)(const char *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        цяло r;

        assert(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, NULL, &g);
        ако (r == GLOB_NOMATCH)
                върни -ENOENT;
        ако (r == GLOB_NOSPACE)
                върни -ENOMEM;
        ако (r != 0)
                върни errno_or_else(EIO);

#ifndef __GLIBC__
        върни filter_glob_result(g.gl_pathv, path, ret);
#endif

        ако (strv_isempty(g.gl_pathv))
                върни -ENOENT;

        ако (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        върни 0;
}

цяло glob_first(const char *path, char **ret) {
        _cleanup_strv_free_ char **v = NULL;
        цяло r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        ако (r == -ENOENT) {
                ако (ret)
                        *ret = NULL;
                върни false;
        }
        ако (r < 0)
                върни r;

        assert(!strv_isempty(v));

        ако (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        върни true;
}

цяло glob_extend(char ***strv, const char *path, цяло flags) {
        char **v;
        цяло r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ако (r < 0)
                върни r;

        върни strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

цяло glob_non_glob_prefix(const char *path, char **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        ако (path[n] != '\0')
                докато (n > 0 && path[n-1] != '/')
                        n--;

        ако (n == 0)
                върни -ENOENT;

        char *ans = strndup(path, n);
        ако (!ans)
                върни -ENOMEM;
        *ret = ans;
        върни 0;
}

bool string_is_glob(const char *p) {
        /* Check if a string contains any glob patterns. */
        върни !!strpbrk(p, GLOB_CHARS);
}
