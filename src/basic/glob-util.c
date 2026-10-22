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

        如果 (isempty(p))
                返回 false; /* should not happen, but for safey. */

        如果 (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                如果 (!p)
                        返回 false; /* should not happen, but for safety. */
        }

        为 (;;) {
                p += strspn(p, "/");
                如果 (*p == '\0')
                        返回 true;
                如果 (*p == '.') {
                        p++;
                        如果 (IN_SET(*p, '/', '\0'))
                                返回 false; /* refuse dot */
                        如果 (*p == '.') {
                                p++;
                                如果 (IN_SET(*p, '/', '\0'))
                                        返回 false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                如果 (*p == '\0')
                        返回 true;
        }
}

static 整数 filter_glob_result(char * const *result, const char *path, char ***ret) {
        整数 r;

        assert(path);

        如果 (strv_isempty(result))
                返回 -ENOENT;

        _cleanup_free_ char *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        如果 (r < 0 && r != -ENOENT)
                返回 r;

        _cleanup_strv_free_ char **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                如果 (!safe_glob_verify(*p, prefix))
                        continue;

                如果 (!ret)
                        返回 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                如果 (r < 0)
                        返回 r;
        }

        如果 (n_filtered == 0)
                返回 -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        返回 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, void, closedir);

整数 safe_glob_full(const char *path, 整数 flags, opendir_t opendir_func, char ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (struct dirent* (*)(void *)) readdir_no_dot,
                .gl_opendir = (void* (*)(const char *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        整数 r;

        assert(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, NULL, &g);
        如果 (r == GLOB_NOMATCH)
                返回 -ENOENT;
        如果 (r == GLOB_NOSPACE)
                返回 -ENOMEM;
        如果 (r != 0)
                返回 errno_or_else(EIO);

#ifndef __GLIBC__
        返回 filter_glob_result(g.gl_pathv, path, ret);
#endif

        如果 (strv_isempty(g.gl_pathv))
                返回 -ENOENT;

        如果 (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        返回 0;
}

整数 glob_first(const char *path, char **ret) {
        _cleanup_strv_free_ char **v = NULL;
        整数 r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        如果 (r == -ENOENT) {
                如果 (ret)
                        *ret = NULL;
                返回 false;
        }
        如果 (r < 0)
                返回 r;

        assert(!strv_isempty(v));

        如果 (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        返回 true;
}

整数 glob_extend(char ***strv, const char *path, 整数 flags) {
        char **v;
        整数 r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        如果 (r < 0)
                返回 r;

        返回 strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

整数 glob_non_glob_prefix(const char *path, char **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        如果 (path[n] != '\0')
                当 (n > 0 && path[n-1] != '/')
                        n--;

        如果 (n == 0)
                返回 -ENOENT;

        char *ans = strndup(path, n);
        如果 (!ans)
                返回 -ENOMEM;
        *ret = ans;
        返回 0;
}

bool string_is_glob(const char *p) {
        /* Check if a string contains any glob patterns. */
        返回 !!strpbrk(p, GLOB_CHARS);
}
