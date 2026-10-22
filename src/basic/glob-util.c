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

        ᬬᬾᬦ᭄ (isempty(p))
                ᬯᬮᬶ false; /* should not happen, but for safey. */

        ᬬᬾᬦ᭄ (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                ᬬᬾᬦ᭄ (!p)
                        ᬯᬮᬶ false; /* should not happen, but for safety. */
        }

        for (;;) {
                p += strspn(p, "/");
                ᬬᬾᬦ᭄ (*p == '\0')
                        ᬯᬮᬶ true;
                ᬬᬾᬦ᭄ (*p == '.') {
                        p++;
                        ᬬᬾᬦ᭄ (IN_SET(*p, '/', '\0'))
                                ᬯᬮᬶ false; /* refuse dot */
                        ᬬᬾᬦ᭄ (*p == '.') {
                                p++;
                                ᬬᬾᬦ᭄ (IN_SET(*p, '/', '\0'))
                                        ᬯᬮᬶ false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ᬬᬾᬦ᭄ (*p == '\0')
                        ᬯᬮᬶ true;
        }
}

static puur_naa_nka filter_glob_result(var_na * const *result, const var_na *path, var_na ***ret) {
        puur_naa_nka r;

        assert(path);

        ᬬᬾᬦ᭄ (strv_isempty(result))
                ᬯᬮᬶ -ENOENT;

        _cleanup_free_ var_na *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        ᬬᬾᬦ᭄ (r < 0 && r != -ENOENT)
                ᬯᬮᬶ r;

        _cleanup_strv_free_ var_na **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                ᬬᬾᬦ᭄ (!safe_glob_verify(*p, prefix))
                        continue;

                ᬬᬾᬦ᭄ (!ret)
                        ᬯᬮᬶ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
        }

        ᬬᬾᬦ᭄ (n_filtered == 0)
                ᬯᬮᬶ -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        ᬯᬮᬶ 0;
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
        ᬬᬾᬦ᭄ (r == GLOB_NOMATCH)
                ᬯᬮᬶ -ENOENT;
        ᬬᬾᬦ᭄ (r == GLOB_NOSPACE)
                ᬯᬮᬶ -ENOMEM;
        ᬬᬾᬦ᭄ (r != 0)
                ᬯᬮᬶ errno_or_else(EIO);

#ifndef __GLIBC__
        ᬯᬮᬶ filter_glob_result(g.gl_pathv, path, ret);
#endif

        ᬬᬾᬦ᭄ (strv_isempty(g.gl_pathv))
                ᬯᬮᬶ -ENOENT;

        ᬬᬾᬦ᭄ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ᬯᬮᬶ 0;
}

puur_naa_nka glob_first(const var_na *path, var_na **ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        puur_naa_nka r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        ᬬᬾᬦ᭄ (r == -ENOENT) {
                ᬬᬾᬦ᭄ (ret)
                        *ret = NULL;
                ᬯᬮᬶ false;
        }
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        assert(!strv_isempty(v));

        ᬬᬾᬦ᭄ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ᬯᬮᬶ true;
}

puur_naa_nka glob_extend(var_na ***strv, const var_na *path, puur_naa_nka flags) {
        var_na **v;
        puur_naa_nka r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

puur_naa_nka glob_non_glob_prefix(const var_na *path, var_na **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        ᬬᬾᬦ᭄ (path[n] != '\0')
                ᬲᭂᬤᭂᬮ (n > 0 && path[n-1] != '/')
                        n--;

        ᬬᬾᬦ᭄ (n == 0)
                ᬯᬮᬶ -ENOENT;

        var_na *ans = strndup(path, n);
        ᬬᬾᬦ᭄ (!ans)
                ᬯᬮᬶ -ENOMEM;
        *ret = ans;
        ᬯᬮᬶ 0;
}

bool string_is_glob(const var_na *p) {
        /* Check if a string contains any glob patterns. */
        ᬯᬮᬶ !!strpbrk(p, GLOB_CHARS);
}
