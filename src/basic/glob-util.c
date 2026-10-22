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

        ប្រសិនបើ (isempty(p))
                ត្រឡប់ false; /* should not happen, but for safey. */

        ប្រសិនបើ (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                ប្រសិនបើ (!p)
                        ត្រឡប់ false; /* should not happen, but for safety. */
        }

        សម្រាប់ (;;) {
                p += strspn(p, "/");
                ប្រសិនបើ (*p == '\0')
                        ត្រឡប់ true;
                ប្រសិនបើ (*p == '.') {
                        p++;
                        ប្រសិនបើ (IN_SET(*p, '/', '\0'))
                                ត្រឡប់ false; /* refuse dot */
                        ប្រសិនបើ (*p == '.') {
                                p++;
                                ប្រសិនបើ (IN_SET(*p, '/', '\0'))
                                        ត្រឡប់ false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ប្រសិនបើ (*p == '\0')
                        ត្រឡប់ true;
        }
}

static ចំនួនគត់ filter_glob_result(var_na * const *result, const var_na *path, var_na ***ret) {
        ចំនួនគត់ r;

        assert(path);

        ប្រសិនបើ (strv_isempty(result))
                ត្រឡប់ -ENOENT;

        _cleanup_free_ var_na *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        ប្រសិនបើ (r < 0 && r != -ENOENT)
                ត្រឡប់ r;

        _cleanup_strv_free_ var_na **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                ប្រសិនបើ (!safe_glob_verify(*p, prefix))
                        continue;

                ប្រសិនបើ (!ret)
                        ត្រឡប់ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        ប្រសិនបើ (n_filtered == 0)
                ត្រឡប់ -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        ត្រឡប់ 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, void, closedir);

ចំនួនគត់ safe_glob_full(const var_na *path, ចំនួនគត់ flags, opendir_t opendir_func, var_na ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (sa_nracanaa dirent* (*)(void *)) readdir_no_dot,
                .gl_opendir = (void* (*)(const var_na *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        ចំនួនគត់ r;

        assert(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, NULL, &g);
        ប្រសិនបើ (r == GLOB_NOMATCH)
                ត្រឡប់ -ENOENT;
        ប្រសិនបើ (r == GLOB_NOSPACE)
                ត្រឡប់ -ENOMEM;
        ប្រសិនបើ (r != 0)
                ត្រឡប់ errno_or_else(EIO);

#ifndef __GLIBC__
        ត្រឡប់ filter_glob_result(g.gl_pathv, path, ret);
#endif

        ប្រសិនបើ (strv_isempty(g.gl_pathv))
                ត្រឡប់ -ENOENT;

        ប្រសិនបើ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ត្រឡប់ 0;
}

ចំនួនគត់ glob_first(const var_na *path, var_na **ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        ចំនួនគត់ r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        ប្រសិនបើ (r == -ENOENT) {
                ប្រសិនបើ (ret)
                        *ret = NULL;
                ត្រឡប់ false;
        }
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        assert(!strv_isempty(v));

        ប្រសិនបើ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ត្រឡប់ true;
}

ចំនួនគត់ glob_extend(var_na ***strv, const var_na *path, ចំនួនគត់ flags) {
        var_na **v;
        ចំនួនគត់ r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

ចំនួនគត់ glob_non_glob_prefix(const var_na *path, var_na **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        ប្រសិនបើ (path[n] != '\0')
                ខណៈពេល (n > 0 && path[n-1] != '/')
                        n--;

        ប្រសិនបើ (n == 0)
                ត្រឡប់ -ENOENT;

        var_na *ans = strndup(path, n);
        ប្រសិនបើ (!ans)
                ត្រឡប់ -ENOMEM;
        *ret = ans;
        ត្រឡប់ 0;
}

bool string_is_glob(const var_na *p) {
        /* Check if a string contains any glob patterns. */
        ត្រឡប់ !!strpbrk(p, GLOB_CHARS);
}
