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

        အကယ်၍ (isempty(p))
                ပြန်ပေး false; /* should not happen, but for safey. */

        အကယ်၍ (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                အကယ်၍ (!p)
                        ပြန်ပေး false; /* should not happen, but for safety. */
        }

        အတွက် (;;) {
                p += strspn(p, "/");
                အကယ်၍ (*p == '\0')
                        ပြန်ပေး true;
                အကယ်၍ (*p == '.') {
                        p++;
                        အကယ်၍ (IN_SET(*p, '/', '\0'))
                                ပြန်ပေး false; /* refuse dot */
                        အကယ်၍ (*p == '.') {
                                p++;
                                အကယ်၍ (IN_SET(*p, '/', '\0'))
                                        ပြန်ပေး false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                အကယ်၍ (*p == '\0')
                        ပြန်ပေး true;
        }
}

static ကိန်းပြည့် filter_glob_result(var_na * const *result, const var_na *path, var_na ***ret) {
        ကိန်းပြည့် r;

        assert(path);

        အကယ်၍ (strv_isempty(result))
                ပြန်ပေး -ENOENT;

        _cleanup_free_ var_na *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        အကယ်၍ (r < 0 && r != -ENOENT)
                ပြန်ပေး r;

        _cleanup_strv_free_ var_na **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                အကယ်၍ (!safe_glob_verify(*p, prefix))
                        continue;

                အကယ်၍ (!ret)
                        ပြန်ပေး 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        အကယ်၍ (n_filtered == 0)
                ပြန်ပေး -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        ပြန်ပေး 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, void, closedir);

ကိန်းပြည့် safe_glob_full(const var_na *path, ကိန်းပြည့် flags, opendir_t opendir_func, var_na ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (sa_nracanaa dirent* (*)(void *)) readdir_no_dot,
                .gl_opendir = (void* (*)(const var_na *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        ကိန်းပြည့် r;

        assert(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, NULL, &g);
        အကယ်၍ (r == GLOB_NOMATCH)
                ပြန်ပေး -ENOENT;
        အကယ်၍ (r == GLOB_NOSPACE)
                ပြန်ပေး -ENOMEM;
        အကယ်၍ (r != 0)
                ပြန်ပေး errno_or_else(EIO);

#ifndef __GLIBC__
        ပြန်ပေး filter_glob_result(g.gl_pathv, path, ret);
#endif

        အကယ်၍ (strv_isempty(g.gl_pathv))
                ပြန်ပေး -ENOENT;

        အကယ်၍ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် glob_first(const var_na *path, var_na **ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        ကိန်းပြည့် r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        အကယ်၍ (r == -ENOENT) {
                အကယ်၍ (ret)
                        *ret = NULL;
                ပြန်ပေး false;
        }
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        assert(!strv_isempty(v));

        အကယ်၍ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ပြန်ပေး true;
}

ကိန်းပြည့် glob_extend(var_na ***strv, const var_na *path, ကိန်းပြည့် flags) {
        var_na **v;
        ကိန်းပြည့် r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

ကိန်းပြည့် glob_non_glob_prefix(const var_na *path, var_na **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        အကယ်၍ (path[n] != '\0')
                နေစဉ် (n > 0 && path[n-1] != '/')
                        n--;

        အကယ်၍ (n == 0)
                ပြန်ပေး -ENOENT;

        var_na *ans = strndup(path, n);
        အကယ်၍ (!ans)
                ပြန်ပေး -ENOMEM;
        *ret = ans;
        ပြန်ပေး 0;
}

bool string_is_glob(const var_na *p) {
        /* Check if a string contains any glob patterns. */
        ပြန်ပေး !!strpbrk(p, GLOB_CHARS);
}
