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

        ถ้า (isempty(p))
                คืน false; /* should not happen, but for safey. */

        ถ้า (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                ถ้า (!p)
                        คืน false; /* should not happen, but for safety. */
        }

        สำหรับ (;;) {
                p += strspn(p, "/");
                ถ้า (*p == '\0')
                        คืน true;
                ถ้า (*p == '.') {
                        p++;
                        ถ้า (IN_SET(*p, '/', '\0'))
                                คืน false; /* refuse dot */
                        ถ้า (*p == '.') {
                                p++;
                                ถ้า (IN_SET(*p, '/', '\0'))
                                        คืน false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ถ้า (*p == '\0')
                        คืน true;
        }
}

static จำนวนเต็ม filter_glob_result(var_na * const *result, const var_na *path, var_na ***ret) {
        จำนวนเต็ม r;

        assert(path);

        ถ้า (strv_isempty(result))
                คืน -ENOENT;

        _cleanup_free_ var_na *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        ถ้า (r < 0 && r != -ENOENT)
                คืน r;

        _cleanup_strv_free_ var_na **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                ถ้า (!safe_glob_verify(*p, prefix))
                        continue;

                ถ้า (!ret)
                        คืน 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                ถ้า (r < 0)
                        คืน r;
        }

        ถ้า (n_filtered == 0)
                คืน -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        คืน 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, void, closedir);

จำนวนเต็ม safe_glob_full(const var_na *path, จำนวนเต็ม flags, opendir_t opendir_func, var_na ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (sa_nracanaa dirent* (*)(void *)) readdir_no_dot,
                .gl_opendir = (void* (*)(const var_na *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        จำนวนเต็ม r;

        assert(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, NULL, &g);
        ถ้า (r == GLOB_NOMATCH)
                คืน -ENOENT;
        ถ้า (r == GLOB_NOSPACE)
                คืน -ENOMEM;
        ถ้า (r != 0)
                คืน errno_or_else(EIO);

#ifndef __GLIBC__
        คืน filter_glob_result(g.gl_pathv, path, ret);
#endif

        ถ้า (strv_isempty(g.gl_pathv))
                คืน -ENOENT;

        ถ้า (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        คืน 0;
}

จำนวนเต็ม glob_first(const var_na *path, var_na **ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        จำนวนเต็ม r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        ถ้า (r == -ENOENT) {
                ถ้า (ret)
                        *ret = NULL;
                คืน false;
        }
        ถ้า (r < 0)
                คืน r;

        assert(!strv_isempty(v));

        ถ้า (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        คืน true;
}

จำนวนเต็ม glob_extend(var_na ***strv, const var_na *path, จำนวนเต็ม flags) {
        var_na **v;
        จำนวนเต็ม r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ถ้า (r < 0)
                คืน r;

        คืน strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

จำนวนเต็ม glob_non_glob_prefix(const var_na *path, var_na **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        ถ้า (path[n] != '\0')
                ขณะ (n > 0 && path[n-1] != '/')
                        n--;

        ถ้า (n == 0)
                คืน -ENOENT;

        var_na *ans = strndup(path, n);
        ถ้า (!ans)
                คืน -ENOMEM;
        *ret = ans;
        คืน 0;
}

bool string_is_glob(const var_na *p) {
        /* Check if a string contains any glob patterns. */
        คืน !!strpbrk(p, GLOB_CHARS);
}
