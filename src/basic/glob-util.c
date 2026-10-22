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

        𐤀𐤌 (isempty(p))
                أرجع false; /* should not happen, but for safey. */

        𐤀𐤌 (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                𐤀𐤌 (!p)
                        أرجع false; /* should not happen, but for safety. */
        }

        لكل (;;) {
                p += strspn(p, "/");
                𐤀𐤌 (*p == '\0')
                        أرجع true;
                𐤀𐤌 (*p == '.') {
                        p++;
                        𐤀𐤌 (IN_SET(*p, '/', '\0'))
                                أرجع false; /* refuse dot */
                        𐤀𐤌 (*p == '.') {
                                p++;
                                𐤀𐤌 (IN_SET(*p, '/', '\0'))
                                        أرجع false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                𐤀𐤌 (*p == '\0')
                        أرجع true;
        }
}

ساكن صحيح filter_glob_result(حرف * ثابت *result, ثابت حرف *path, حرف ***ret) {
        صحيح r;

        أكد(path);

        𐤀𐤌 (strv_isempty(result))
                أرجع -ENOENT;

        _cleanup_free_ حرف *prefix = عدم;
        r = glob_non_glob_prefix(path, &prefix);
        𐤀𐤌 (r < 0 && r != -ENOENT)
                أرجع r;

        _cleanup_strv_free_ حرف **filtered = عدم;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                𐤀𐤌 (!safe_glob_verify(*p, prefix))
                        استمر;

                𐤀𐤌 (!ret)
                        أرجع 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                𐤀𐤌 (r < 0)
                        أرجع r;
        }

        𐤀𐤌 (n_filtered == 0)
                أرجع -ENOENT;

        أكد(ret);
        *ret = TAKE_PTR(filtered);
        أرجع 0;
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
        𐤀𐤌 (r == GLOB_NOMATCH)
                أرجع -ENOENT;
        𐤀𐤌 (r == GLOB_NOSPACE)
                أرجع -ENOMEM;
        𐤀𐤌 (r != 0)
                أرجع errno_or_else(EIO);

#ifndef __GLIBC__
        أرجع filter_glob_result(g.gl_pathv, path, ret);
#endif

        𐤀𐤌 (strv_isempty(g.gl_pathv))
                أرجع -ENOENT;

        𐤀𐤌 (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        أرجع 0;
}

صحيح glob_first(ثابت حرف *path, حرف **ret) {
        _cleanup_strv_free_ حرف **v = عدم;
        صحيح r;

        أكد(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        𐤀𐤌 (r == -ENOENT) {
                𐤀𐤌 (ret)
                        *ret = عدم;
                أرجع false;
        }
        𐤀𐤌 (r < 0)
                أرجع r;

        أكد(!strv_isempty(v));

        𐤀𐤌 (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        أرجع true;
}

صحيح glob_extend(حرف ***strv, ثابت حرف *path, صحيح flags) {
        حرف **v;
        صحيح r;

        أكد(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        𐤀𐤌 (r < 0)
                أرجع r;

        أرجع strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

صحيح glob_non_glob_prefix(ثابت حرف *path, حرف **ret) {
        أكد(path);
        أكد(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        𐤀𐤌 (path[n] != '\0')
                طالما (n > 0 && path[n-1] != '/')
                        n--;

        𐤀𐤌 (n == 0)
                أرجع -ENOENT;

        حرف *ans = strndup(path, n);
        𐤀𐤌 (!ans)
                أرجع -ENOMEM;
        *ret = ans;
        أرجع 0;
}

bool string_is_glob(ثابت حرف *p) {
        /* Check if a string contains any glob patterns. */
        أرجع !!strpbrk(p, GLOB_CHARS);
}
