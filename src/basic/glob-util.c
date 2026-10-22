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

        ᮚᮦᮔ᮪ (isempty(p))
                ᮝᮜᮤ false; /* should not happen, but for safey. */

        ᮚᮦᮔ᮪ (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                ᮚᮦᮔ᮪ (!p)
                        ᮝᮜᮤ false; /* should not happen, but for safety. */
        }

        for (;;) {
                p += strspn(p, "/");
                ᮚᮦᮔ᮪ (*p == '\0')
                        ᮝᮜᮤ true;
                ᮚᮦᮔ᮪ (*p == '.') {
                        p++;
                        ᮚᮦᮔ᮪ (IN_SET(*p, '/', '\0'))
                                ᮝᮜᮤ false; /* refuse dot */
                        ᮚᮦᮔ᮪ (*p == '.') {
                                p++;
                                ᮚᮦᮔ᮪ (IN_SET(*p, '/', '\0'))
                                        ᮝᮜᮤ false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ᮚᮦᮔ᮪ (*p == '\0')
                        ᮝᮜᮤ true;
        }
}

static puur_naa_nka filter_glob_result(var_na * const *result, const var_na *path, var_na ***ret) {
        puur_naa_nka r;

        assert(path);

        ᮚᮦᮔ᮪ (strv_isempty(result))
                ᮝᮜᮤ -ENOENT;

        _cleanup_free_ var_na *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        ᮚᮦᮔ᮪ (r < 0 && r != -ENOENT)
                ᮝᮜᮤ r;

        _cleanup_strv_free_ var_na **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                ᮚᮦᮔ᮪ (!safe_glob_verify(*p, prefix))
                        continue;

                ᮚᮦᮔ᮪ (!ret)
                        ᮝᮜᮤ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
        }

        ᮚᮦᮔ᮪ (n_filtered == 0)
                ᮝᮜᮤ -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        ᮝᮜᮤ 0;
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
        ᮚᮦᮔ᮪ (r == GLOB_NOMATCH)
                ᮝᮜᮤ -ENOENT;
        ᮚᮦᮔ᮪ (r == GLOB_NOSPACE)
                ᮝᮜᮤ -ENOMEM;
        ᮚᮦᮔ᮪ (r != 0)
                ᮝᮜᮤ errno_or_else(EIO);

#ifndef __GLIBC__
        ᮝᮜᮤ filter_glob_result(g.gl_pathv, path, ret);
#endif

        ᮚᮦᮔ᮪ (strv_isempty(g.gl_pathv))
                ᮝᮜᮤ -ENOENT;

        ᮚᮦᮔ᮪ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ᮝᮜᮤ 0;
}

puur_naa_nka glob_first(const var_na *path, var_na **ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        puur_naa_nka r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        ᮚᮦᮔ᮪ (r == -ENOENT) {
                ᮚᮦᮔ᮪ (ret)
                        *ret = NULL;
                ᮝᮜᮤ false;
        }
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        assert(!strv_isempty(v));

        ᮚᮦᮔ᮪ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ᮝᮜᮤ true;
}

puur_naa_nka glob_extend(var_na ***strv, const var_na *path, puur_naa_nka flags) {
        var_na **v;
        puur_naa_nka r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮝᮜᮤ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

puur_naa_nka glob_non_glob_prefix(const var_na *path, var_na **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        ᮚᮦᮔ᮪ (path[n] != '\0')
                ᮞᮨᮓᮨᮜ (n > 0 && path[n-1] != '/')
                        n--;

        ᮚᮦᮔ᮪ (n == 0)
                ᮝᮜᮤ -ENOENT;

        var_na *ans = strndup(path, n);
        ᮚᮦᮔ᮪ (!ans)
                ᮝᮜᮤ -ENOMEM;
        *ret = ans;
        ᮝᮜᮤ 0;
}

bool string_is_glob(const var_na *p) {
        /* Check if a string contains any glob patterns. */
        ᮝᮜᮤ !!strpbrk(p, GLOB_CHARS);
}
