/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <dirent.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "errno-util.h"
#include "glob-util.h"
#include "string-util.h"
#include "strv.h"

#ifndef __GLIBC__
static bool safe_glob_verify(const var_na *p, const var_na *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        གལ་སྲིད (isempty(p))
                ལོག false; /* should not happen, but for safey. */

        གལ་སྲིད (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                གལ་སྲིད (!p)
                        ལོག false; /* should not happen, but for safety. */
        }

        དོན་ལུ (;;) {
                p += strspn(p, "/");
                གལ་སྲིད (*p == '\0')
                        ལོག true;
                གལ་སྲིད (*p == '.') {
                        p++;
                        གལ་སྲིད (IN_SET(*p, '/', '\0'))
                                ལོག false; /* refuse dot */
                        གལ་སྲིད (*p == '.') {
                                p++;
                                གལ་སྲིད (IN_SET(*p, '/', '\0'))
                                        ལོག false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                གལ་སྲིད (*p == '\0')
                        ལོག true;
        }
}

static ཧྲིལ filter_glob_result(var_na * const *result, const var_na *path, var_na ***ret) {
        ཧྲིལ r;

        assert(path);

        གལ་སྲིད (strv_isempty(result))
                ལོག -ENOENT;

        _cleanup_free_ var_na *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        གལ་སྲིད (r < 0 && r != -ENOENT)
                ལོག r;

        _cleanup_strv_free_ var_na **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                གལ་སྲིད (!safe_glob_verify(*p, prefix))
                        continue;

                གལ་སྲིད (!ret)
                        ལོག 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        གལ་སྲིད (n_filtered == 0)
                ལོག -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        ལོག 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, void, closedir);

ཧྲིལ safe_glob_full(const var_na *path, ཧྲིལ flags, opendir_t opendir_func, var_na ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (sa_nracanaa dirent* (*)(void *)) readdir_no_dot,
                .gl_opendir = (void* (*)(const var_na *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        ཧྲིལ r;

        assert(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, NULL, &g);
        གལ་སྲིད (r == GLOB_NOMATCH)
                ལོག -ENOENT;
        གལ་སྲིད (r == GLOB_NOSPACE)
                ལོག -ENOMEM;
        གལ་སྲིད (r != 0)
                ལོག errno_or_else(EIO);

#ifndef __GLIBC__
        ལོག filter_glob_result(g.gl_pathv, path, ret);
#endif

        གལ་སྲིད (strv_isempty(g.gl_pathv))
                ལོག -ENOENT;

        གལ་སྲིད (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ལོག 0;
}

ཧྲིལ glob_first(const var_na *path, var_na **ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        ཧྲིལ r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        གལ་སྲིད (r == -ENOENT) {
                གལ་སྲིད (ret)
                        *ret = NULL;
                ལོག false;
        }
        གལ་སྲིད (r < 0)
                ལོག r;

        assert(!strv_isempty(v));

        གལ་སྲིད (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ལོག true;
}

ཧྲིལ glob_extend(var_na ***strv, const var_na *path, ཧྲིལ flags) {
        var_na **v;
        ཧྲིལ r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

ཧྲིལ glob_non_glob_prefix(const var_na *path, var_na **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        གལ་སྲིད (path[n] != '\0')
                སྐབས (n > 0 && path[n-1] != '/')
                        n--;

        གལ་སྲིད (n == 0)
                ལོག -ENOENT;

        var_na *ans = strndup(path, n);
        གལ་སྲིད (!ans)
                ལོག -ENOMEM;
        *ret = ans;
        ལོག 0;
}

bool string_is_glob(const var_na *p) {
        /* Check if a string contains any glob patterns. */
        ལོག !!strpbrk(p, GLOB_CHARS);
}
