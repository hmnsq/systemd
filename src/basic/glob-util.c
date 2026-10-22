/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <dirent.h>
#ضمن <sys/stat.h>

#ضمن "dirent-util.h"
#ضمن "errno-util.h"
#ضمن "glob-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"

#جے_بنا_تعریف __GLIBC__
ساكن bool محفوظ_glob_verify(ثابت حرف *p, ثابت حرف *اگیتر) {
        اشارہ_گر_MAY_BE_NULL(p);
        اشارہ_گر_MAY_BE_NULL(اگیتر);

        إذا (isempty(p))
                أرجع false; /* should not happen, but for safey. */

        إذا (اگیتر) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, اگیتر);
                إذا (!p)
                        أرجع false; /* should not happen, but for safety. */
        }

        لكل (;;) {
                p += strspn(p, "/");
                إذا (*p == '\0')
                        أرجع true;
                إذا (*p == '.') {
                        p++;
                        إذا (وچ_سیٹ(*p, '/', '\0'))
                                أرجع false; /* refuse dot */
                        إذا (*p == '.') {
                                p++;
                                إذا (وچ_سیٹ(*p, '/', '\0'))
                                        أرجع false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                إذا (*p == '\0')
                        أرجع true;
        }
}

ساكن صحيح filter_glob_نتیجہ(حرف * ثابت *نتیجہ, ثابت حرف *رستہ, حرف ***ret) {
        صحيح r;

        أكد(رستہ);

        إذا (strv_isempty(نتیجہ))
                أرجع -ENOENT;

        _cleanup_free_ حرف *اگیتر = عدم;
        r = glob_non_glob_اگیتر(رستہ, &اگیتر);
        إذا (r < 0 && r != -ENOENT)
                أرجع r;

        _cleanup_strv_free_ حرف **filtered = عدم;
        size_t n_filtered = 0;
        STRV_FOREACH(p, نتیجہ) {
                إذا (!محفوظ_glob_verify(*p, اگیتر))
                        استمر;

                إذا (!ret)
                        أرجع 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_ناپ(&filtered, &n_filtered, *p);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (n_filtered == 0)
                أرجع -ENOENT;

        أكد(ret);
        *ret = TAKE_PTR(filtered);
        أرجع 0;
}
#جے_انت

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, فراغ, closedir);

صحيح محفوظ_glob_بھریا(ثابت حرف *رستہ, صحيح flags, opendir_t opendir_func, حرف ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (بنية dirent* (*)(فراغ *)) readdir_no_dot,
                .gl_opendir = (فراغ* (*)(ثابت حرف *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        صحيح r;

        أكد(رستہ);

        errno = 0;
        r = glob(رستہ, flags | GLOB_ALTDIRFUNC, عدم, &g);
        إذا (r == GLOB_NOMATCH)
                أرجع -ENOENT;
        إذا (r == GLOB_NOSPACE)
                أرجع -ENOMEM;
        إذا (r != 0)
                أرجع errno_or_else(EIO);

#جے_بنا_تعریف __GLIBC__
        أرجع filter_glob_نتیجہ(g.gl_pathv, رستہ, ret);
#جے_انت

        إذا (strv_isempty(g.gl_pathv))
                أرجع -ENOENT;

        إذا (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        أرجع 0;
}

صحيح glob_پہلا(ثابت حرف *رستہ, حرف **ret) {
        _cleanup_strv_free_ حرف **v = عدم;
        صحيح r;

        أكد(رستہ);

        r = محفوظ_glob(رستہ, GLOB_NOSORT|GLOB_BRACE, &v);
        إذا (r == -ENOENT) {
                إذا (ret)
                        *ret = عدم;
                أرجع false;
        }
        إذا (r < 0)
                أرجع r;

        أكد(!strv_isempty(v));

        إذا (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        أرجع true;
}

صحيح glob_extend(حرف ***strv, ثابت حرف *رستہ, صحيح flags) {
        حرف **v;
        صحيح r;

        أكد(رستہ);

        r = محفوظ_glob(رستہ, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        إذا (r < 0)
                أرجع r;

        أرجع strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

صحيح glob_non_glob_اگیتر(ثابت حرف *رستہ, حرف **ret) {
        أكد(رستہ);
        أكد(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(رستہ, GLOB_CHARS);

        إذا (رستہ[n] != '\0')
                طالما (n > 0 && رستہ[n-1] != '/')
                        n--;

        إذا (n == 0)
                أرجع -ENOENT;

        حرف *ans = strndup(رستہ, n);
        إذا (!ans)
                أرجع -ENOMEM;
        *ret = ans;
        أرجع 0;
}

bool string_is_glob(ثابت حرف *p) {
        /* Check if a string contains any glob patterns. */
        أرجع !!strpbrk(p, GLOB_CHARS);
}
