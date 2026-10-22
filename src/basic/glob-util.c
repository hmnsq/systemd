/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <dirent.h>
#ضمن <sys/stat.h>

#ضمن "dirent-util.h"
#ضمن "errno-util.h"
#ضمن "glob-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"

#ifndef __GLIBC__
ساكن bool safe_glob_verify(ثابت حرف *p, ثابت حرف *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        ܐܢ (isempty(p))
                ܦܢܐ false; /* should not happen, but for safey. */

        ܐܢ (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                ܐܢ (!p)
                        ܦܢܐ false; /* should not happen, but for safety. */
        }

        لكل (;;) {
                p += strspn(p, "/");
                ܐܢ (*p == '\0')
                        ܦܢܐ true;
                ܐܢ (*p == '.') {
                        p++;
                        ܐܢ (IN_SET(*p, '/', '\0'))
                                ܦܢܐ false; /* refuse dot */
                        ܐܢ (*p == '.') {
                                p++;
                                ܐܢ (IN_SET(*p, '/', '\0'))
                                        ܦܢܐ false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ܐܢ (*p == '\0')
                        ܦܢܐ true;
        }
}

ساكن صحيح filter_glob_result(حرف * ثابت *result, ثابت حرف *path, حرف ***ret) {
        صحيح r;

        أكد(path);

        ܐܢ (strv_isempty(result))
                ܦܢܐ -ENOENT;

        _cleanup_free_ حرف *prefix = عدم;
        r = glob_non_glob_prefix(path, &prefix);
        ܐܢ (r < 0 && r != -ENOENT)
                ܦܢܐ r;

        _cleanup_strv_free_ حرف **filtered = عدم;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                ܐܢ (!safe_glob_verify(*p, prefix))
                        استمر;

                ܐܢ (!ret)
                        ܦܢܐ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        ܐܢ (n_filtered == 0)
                ܦܢܐ -ENOENT;

        أكد(ret);
        *ret = TAKE_PTR(filtered);
        ܦܢܐ 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, فراغ, closedir);

صحيح safe_glob_full(ثابت حرف *path, صحيح flags, opendir_t opendir_func, حرف ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (بنية dirent* (*)(فراغ *)) readdir_no_dot,
                .gl_opendir = (فراغ* (*)(ثابت حرف *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        صحيح r;

        أكد(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, عدم, &g);
        ܐܢ (r == GLOB_NOMATCH)
                ܦܢܐ -ENOENT;
        ܐܢ (r == GLOB_NOSPACE)
                ܦܢܐ -ENOMEM;
        ܐܢ (r != 0)
                ܦܢܐ errno_or_else(EIO);

#ifndef __GLIBC__
        ܦܢܐ filter_glob_result(g.gl_pathv, path, ret);
#endif

        ܐܢ (strv_isempty(g.gl_pathv))
                ܦܢܐ -ENOENT;

        ܐܢ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ܦܢܐ 0;
}

صحيح glob_first(ثابت حرف *path, حرف **ret) {
        _cleanup_strv_free_ حرف **v = عدم;
        صحيح r;

        أكد(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        ܐܢ (r == -ENOENT) {
                ܐܢ (ret)
                        *ret = عدم;
                ܦܢܐ false;
        }
        ܐܢ (r < 0)
                ܦܢܐ r;

        أكد(!strv_isempty(v));

        ܐܢ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ܦܢܐ true;
}

صحيح glob_extend(حرف ***strv, ثابت حرف *path, صحيح flags) {
        حرف **v;
        صحيح r;

        أكد(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

صحيح glob_non_glob_prefix(ثابت حرف *path, حرف **ret) {
        أكد(path);
        أكد(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        ܐܢ (path[n] != '\0')
                ܟܡܐ (n > 0 && path[n-1] != '/')
                        n--;

        ܐܢ (n == 0)
                ܦܢܐ -ENOENT;

        حرف *ans = strndup(path, n);
        ܐܢ (!ans)
                ܦܢܐ -ENOMEM;
        *ret = ans;
        ܦܢܐ 0;
}

bool string_is_glob(ثابت حرف *p) {
        /* Check if a string contains any glob patterns. */
        ܦܢܐ !!strpbrk(p, GLOB_CHARS);
}
