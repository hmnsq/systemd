/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <dirent.h>
#شامل <sys/stat.h>

#شامل "dirent-util.h"
#شامل "errno-util.h"
#شامل "glob-util.h"
#شامل "string-util.h"
#شامل "strv.h"

#ifndef __GLIBC__
ایستا bool safe_glob_verify(ثابت نویسه *p, ثابت نویسه *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        اگر (isempty(p))
                بازگشت false; /* should not happen, but for safey. */

        اگر (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                اگر (!p)
                        بازگشت false; /* should not happen, but for safety. */
        }

        برای (;;) {
                p += strspn(p, "/");
                اگر (*p == '\0')
                        بازگشت true;
                اگر (*p == '.') {
                        p++;
                        اگر (IN_SET(*p, '/', '\0'))
                                بازگشت false; /* refuse dot */
                        اگر (*p == '.') {
                                p++;
                                اگر (IN_SET(*p, '/', '\0'))
                                        بازگشت false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                اگر (*p == '\0')
                        بازگشت true;
        }
}

ایستا صحیح filter_glob_result(نویسه * ثابت *result, ثابت نویسه *path, نویسه ***ret) {
        صحیح r;

        أكد(path);

        اگر (strv_isempty(result))
                بازگشت -ENOENT;

        _cleanup_free_ نویسه *prefix = پوچ;
        r = glob_non_glob_prefix(path, &prefix);
        اگر (r < 0 && r != -ENOENT)
                بازگشت r;

        _cleanup_strv_free_ نویسه **filtered = پوچ;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                اگر (!safe_glob_verify(*p, prefix))
                        ادامه;

                اگر (!ret)
                        بازگشت 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (n_filtered == 0)
                بازگشت -ENOENT;

        أكد(ret);
        *ret = TAKE_PTR(filtered);
        بازگشت 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, تهی, closedir);

صحیح safe_glob_full(ثابت نویسه *path, صحیح flags, opendir_t opendir_func, نویسه ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (ساختار dirent* (*)(تهی *)) readdir_no_dot,
                .gl_opendir = (تهی* (*)(ثابت نویسه *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        صحیح r;

        أكد(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, پوچ, &g);
        اگر (r == GLOB_NOMATCH)
                بازگشت -ENOENT;
        اگر (r == GLOB_NOSPACE)
                بازگشت -ENOMEM;
        اگر (r != 0)
                بازگشت errno_or_else(EIO);

#ifndef __GLIBC__
        بازگشت filter_glob_result(g.gl_pathv, path, ret);
#endif

        اگر (strv_isempty(g.gl_pathv))
                بازگشت -ENOENT;

        اگر (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        بازگشت 0;
}

صحیح glob_first(ثابت نویسه *path, نویسه **ret) {
        _cleanup_strv_free_ نویسه **v = پوچ;
        صحیح r;

        أكد(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        اگر (r == -ENOENT) {
                اگر (ret)
                        *ret = پوچ;
                بازگشت false;
        }
        اگر (r < 0)
                بازگشت r;

        أكد(!strv_isempty(v));

        اگر (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        بازگشت true;
}

صحیح glob_extend(نویسه ***strv, ثابت نویسه *path, صحیح flags) {
        نویسه **v;
        صحیح r;

        أكد(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        اگر (r < 0)
                بازگشت r;

        بازگشت strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

صحیح glob_non_glob_prefix(ثابت نویسه *path, نویسه **ret) {
        أكد(path);
        أكد(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        اگر (path[n] != '\0')
                وقتی (n > 0 && path[n-1] != '/')
                        n--;

        اگر (n == 0)
                بازگشت -ENOENT;

        نویسه *ans = strndup(path, n);
        اگر (!ans)
                بازگشت -ENOMEM;
        *ret = ans;
        بازگشت 0;
}

bool string_is_glob(ثابت نویسه *p) {
        /* Check if a string contains any glob patterns. */
        بازگشت !!strpbrk(p, GLOB_CHARS);
}
