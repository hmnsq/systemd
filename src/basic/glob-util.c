/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <dirent.h>
#شامل <sys/stat.h>

#شامل "dirent-util.h"
#شامل "errno-util.h"
#شامل "glob-util.h"
#شامل "string-util.h"
#شامل "strv.h"

#اگر_غیر_تعریف_شدہ __GLIBC__
ساکن bool محفوظ_glob_verify(ثابت حرف *p, ثابت حرف *سابقہ) {
        اشارہ_گر_MAY_BE_NULL(p);
        اشارہ_گر_MAY_BE_NULL(سابقہ);

        اگر (isempty(p))
                واپس false; /* should not happen, but for safey. */

        اگر (سابقہ) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, سابقہ);
                اگر (!p)
                        واپس false; /* should not happen, but for safety. */
        }

        برائے (;;) {
                p += strspn(p, "/");
                اگر (*p == '\0')
                        واپس true;
                اگر (*p == '.') {
                        p++;
                        اگر (اندر_سیٹ(*p, '/', '\0'))
                                واپس false; /* refuse dot */
                        اگر (*p == '.') {
                                p++;
                                اگر (اندر_سیٹ(*p, '/', '\0'))
                                        واپس false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                اگر (*p == '\0')
                        واپس true;
        }
}

ساکن صحیح filter_glob_نتیجہ(حرف * ثابت *نتیجہ, ثابت حرف *راستہ, حرف ***ret) {
        صحیح r;

        أكد(راستہ);

        اگر (strv_isempty(نتیجہ))
                واپس -ENOENT;

        _cleanup_free_ حرف *سابقہ = تہی;
        r = glob_non_glob_سابقہ(راستہ, &سابقہ);
        اگر (r < 0 && r != -ENOENT)
                واپس r;

        _cleanup_strv_free_ حرف **filtered = تہی;
        size_t n_filtered = 0;
        STRV_FOREACH(p, نتیجہ) {
                اگر (!محفوظ_glob_verify(*p, سابقہ))
                        جاری;

                اگر (!ret)
                        واپس 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_سائز(&filtered, &n_filtered, *p);
                اگر (r < 0)
                        واپس r;
        }

        اگر (n_filtered == 0)
                واپس -ENOENT;

        أكد(ret);
        *ret = TAKE_PTR(filtered);
        واپس 0;
}
#اگر_ختم

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, خالی, closedir);

صحیح محفوظ_glob_بھرا(ثابت حرف *راستہ, صحیح flags, opendir_t opendir_func, حرف ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (ساخت dirent* (*)(خالی *)) readdir_no_dot,
                .gl_opendir = (خالی* (*)(ثابت حرف *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        صحیح r;

        أكد(راستہ);

        errno = 0;
        r = glob(راستہ, flags | GLOB_ALTDIRFUNC, تہی, &g);
        اگر (r == GLOB_NOMATCH)
                واپس -ENOENT;
        اگر (r == GLOB_NOSPACE)
                واپس -ENOMEM;
        اگر (r != 0)
                واپس errno_or_else(EIO);

#اگر_غیر_تعریف_شدہ __GLIBC__
        واپس filter_glob_نتیجہ(g.gl_pathv, راستہ, ret);
#اگر_ختم

        اگر (strv_isempty(g.gl_pathv))
                واپس -ENOENT;

        اگر (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        واپس 0;
}

صحیح glob_پہلا(ثابت حرف *راستہ, حرف **ret) {
        _cleanup_strv_free_ حرف **v = تہی;
        صحیح r;

        أكد(راستہ);

        r = محفوظ_glob(راستہ, GLOB_NOSORT|GLOB_BRACE, &v);
        اگر (r == -ENOENT) {
                اگر (ret)
                        *ret = تہی;
                واپس false;
        }
        اگر (r < 0)
                واپس r;

        أكد(!strv_isempty(v));

        اگر (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        واپس true;
}

صحیح glob_extend(حرف ***strv, ثابت حرف *راستہ, صحیح flags) {
        حرف **v;
        صحیح r;

        أكد(راستہ);

        r = محفوظ_glob(راستہ, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        اگر (r < 0)
                واپس r;

        واپس strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

صحیح glob_non_glob_سابقہ(ثابت حرف *راستہ, حرف **ret) {
        أكد(راستہ);
        أكد(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(راستہ, GLOB_CHARS);

        اگر (راستہ[n] != '\0')
                جب (n > 0 && راستہ[n-1] != '/')
                        n--;

        اگر (n == 0)
                واپس -ENOENT;

        حرف *ans = strndup(راستہ, n);
        اگر (!ans)
                واپس -ENOMEM;
        *ret = ans;
        واپس 0;
}

bool string_is_glob(ثابت حرف *p) {
        /* Check if a string contains any glob patterns. */
        واپس !!strpbrk(p, GLOB_CHARS);
}
