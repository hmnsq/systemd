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

        නම් (isempty(p))
                ආපසු false; /* should not happen, but for safey. */

        නම් (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                නම් (!p)
                        ආපසු false; /* should not happen, but for safety. */
        }

        සඳහා (;;) {
                p += strspn(p, "/");
                නම් (*p == '\0')
                        ආපසු true;
                නම් (*p == '.') {
                        p++;
                        නම් (IN_SET(*p, '/', '\0'))
                                ආපසු false; /* refuse dot */
                        නම් (*p == '.') {
                                p++;
                                නම් (IN_SET(*p, '/', '\0'))
                                        ආපසු false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                නම් (*p == '\0')
                        ආපසු true;
        }
}

static පූර්ණ_සංඛ්‍යා filter_glob_result(var_na * const *result, const var_na *path, var_na ***ret) {
        පූර්ණ_සංඛ්‍යා r;

        assert(path);

        නම් (strv_isempty(result))
                ආපසු -ENOENT;

        _cleanup_free_ var_na *prefix = NULL;
        r = glob_non_glob_prefix(path, &prefix);
        නම් (r < 0 && r != -ENOENT)
                ආපසු r;

        _cleanup_strv_free_ var_na **filtered = NULL;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                නම් (!safe_glob_verify(*p, prefix))
                        continue;

                නම් (!ret)
                        ආපසු 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                නම් (r < 0)
                        ආපසු r;
        }

        නම් (n_filtered == 0)
                ආපසු -ENOENT;

        assert(ret);
        *ret = TAKE_PTR(filtered);
        ආපසු 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, void, closedir);

පූර්ණ_සංඛ්‍යා safe_glob_full(const var_na *path, පූර්ණ_සංඛ්‍යා flags, opendir_t opendir_func, var_na ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (sa_nracanaa dirent* (*)(void *)) readdir_no_dot,
                .gl_opendir = (void* (*)(const var_na *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        පූර්ණ_සංඛ්‍යා r;

        assert(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, NULL, &g);
        නම් (r == GLOB_NOMATCH)
                ආපසු -ENOENT;
        නම් (r == GLOB_NOSPACE)
                ආපසු -ENOMEM;
        නම් (r != 0)
                ආපසු errno_or_else(EIO);

#ifndef __GLIBC__
        ආපසු filter_glob_result(g.gl_pathv, path, ret);
#endif

        නම් (strv_isempty(g.gl_pathv))
                ආපසු -ENOENT;

        නම් (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා glob_first(const var_na *path, var_na **ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        නම් (r == -ENOENT) {
                නම් (ret)
                        *ret = NULL;
                ආපසු false;
        }
        නම් (r < 0)
                ආපසු r;

        assert(!strv_isempty(v));

        නම් (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ආපසු true;
}

පූර්ණ_සංඛ්‍යා glob_extend(var_na ***strv, const var_na *path, පූර්ණ_සංඛ්‍යා flags) {
        var_na **v;
        පූර්ණ_සංඛ්‍යා r;

        assert(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        නම් (r < 0)
                ආපසු r;

        ආපසු strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

පූර්ණ_සංඛ්‍යා glob_non_glob_prefix(const var_na *path, var_na **ret) {
        assert(path);
        assert(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        නම් (path[n] != '\0')
                අතරතුර (n > 0 && path[n-1] != '/')
                        n--;

        නම් (n == 0)
                ආපසු -ENOENT;

        var_na *ans = strndup(path, n);
        නම් (!ans)
                ආපසු -ENOMEM;
        *ret = ans;
        ආපසු 0;
}

bool string_is_glob(const var_na *p) {
        /* Check if a string contains any glob patterns. */
        ආපසු !!strpbrk(p, GLOB_CHARS);
}
