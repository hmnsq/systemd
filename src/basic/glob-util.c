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

        ꦪꦼꦤ꧀ (isempty(p))
                ꦮꦭꦶ false; /* should not happen, but for safey. */

        ꦪꦼꦤ꧀ (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                ꦪꦼꦤ꧀ (!p)
                        ꦮꦭꦶ false; /* should not happen, but for safety. */
        }

        ꦏꦁꦒꦺꦴ (;;) {
                p += strspn(p, "/");
                ꦪꦼꦤ꧀ (*p == '\0')
                        ꦮꦭꦶ true;
                ꦪꦼꦤ꧀ (*p == '.') {
                        p++;
                        ꦪꦼꦤ꧀ (IN_SET(*p, '/', '\0'))
                                ꦮꦭꦶ false; /* refuse dot */
                        ꦪꦼꦤ꧀ (*p == '.') {
                                p++;
                                ꦪꦼꦤ꧀ (IN_SET(*p, '/', '\0'))
                                        ꦮꦭꦶ false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ꦪꦼꦤ꧀ (*p == '\0')
                        ꦮꦭꦶ true;
        }
}

static puur_naa_nka filter_glob_result(var_na * const *result, const var_na *path, var_na ***ret) {
        puur_naa_nka r;

        assert(path);

        ꦪꦼꦤ꧀ (strv_isempty(result))
                ꦮꦭꦶ -ENOENT;

        _cleanup_free_ var_na *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        ꦪꦼꦤ꧀ (r < 0 && r != -ENOENT)
                ꦮꦭꦶ r;

        _cleanup_strv_free_ var_na **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                ꦪꦼꦤ꧀ (!safe_glob_verify(*p, prefix))
                        continue;

                ꦪꦼꦤ꧀ (!ret)
                        ꦮꦭꦶ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        }

        ꦪꦼꦤ꧀ (n_filtered == 0)
                ꦮꦭꦶ -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        ꦮꦭꦶ 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, void, closedir);

puur_naa_nka safe_glob_full(const var_na *path, puur_naa_nka flags, opendir_t opendir_func, var_na ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (sa_nracanaa dirent* (*)(void *)) readdir_no_dot,
                .gl_opendir = (void* (*)(const var_na *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        puur_naa_nka r;

        assert(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, NULL, &g);
        ꦪꦼꦤ꧀ (r == GLOB_NOMATCH)
                ꦮꦭꦶ -ENOENT;
        ꦪꦼꦤ꧀ (r == GLOB_NOSPACE)
                ꦮꦭꦶ -ENOMEM;
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ errno_or_else(EIO);

#ifndef __GLIBC__
        ꦮꦭꦶ filter_glob_result(g.gl_pathv, path, ret);
#endif

        ꦪꦼꦤ꧀ (strv_isempty(g.gl_pathv))
                ꦮꦭꦶ -ENOENT;

        ꦪꦼꦤ꧀ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ꦮꦭꦶ 0;
}

puur_naa_nka glob_first(const var_na *path, var_na **ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        puur_naa_nka r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        ꦪꦼꦤ꧀ (r == -ENOENT) {
                ꦪꦼꦤ꧀ (ret)
                        *ret = NULL;
                ꦮꦭꦶ false;
        }
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        assert(!strv_isempty(v));

        ꦪꦼꦤ꧀ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ꦮꦭꦶ true;
}

puur_naa_nka glob_extend(var_na ***strv, const var_na *path, puur_naa_nka flags) {
        var_na **v;
        puur_naa_nka r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

puur_naa_nka glob_non_glob_prefix(const var_na *path, var_na **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        ꦪꦼꦤ꧀ (path[n] != '\0')
                ꦱꦼꦢꦼꦭ (n > 0 && path[n-1] != '/')
                        n--;

        ꦪꦼꦤ꧀ (n == 0)
                ꦮꦭꦶ -ENOENT;

        var_na *ans = strndup(path, n);
        ꦪꦼꦤ꧀ (!ans)
                ꦮꦭꦶ -ENOMEM;
        *ret = ans;
        ꦮꦭꦶ 0;
}

bool string_is_glob(const var_na *p) {
        /* Check if a string contains any glob patterns. */
        ꦮꦭꦶ !!strpbrk(p, GLOB_CHARS);
}
