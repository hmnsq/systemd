/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <dirent.h>
#অন্তর্ভাব <sys/stat.h>

#অন্তর্ভাব "dirent-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "glob-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"

#ifndef __GLIBC__
স্থির bool safe_glob_verify(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        যদি (isempty(p))
                ফেরত false; /* should not happen, but for safey. */

        যদি (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                যদি (!p)
                        ফেরত false; /* should not happen, but for safety. */
        }

        জন্য (;;) {
                p += strspn(p, "/");
                যদি (*p == '\0')
                        ফেরত true;
                যদি (*p == '.') {
                        p++;
                        যদি (IN_SET(*p, '/', '\0'))
                                ফেরত false; /* refuse dot */
                        যদি (*p == '.') {
                                p++;
                                যদি (IN_SET(*p, '/', '\0'))
                                        ফেরত false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                যদি (*p == '\0')
                        ফেরত true;
        }
}

স্থির পূর্ণসংখ্যা filter_glob_ফলাফল(অক্ষর * ধ্রুবক *ফলাফল, ধ্রুবক অক্ষর *path, অক্ষর ***ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        যদি (strv_isempty(ফলাফল))
                ফেরত -ENOENT;

        _cleanup_free_ অক্ষর *prefix = ফাঁকা;
        r = glob_non_glob_prefix(path, &prefix);
        যদি (r < 0 && r != -ENOENT)
                ফেরত r;

        _cleanup_strv_free_ অক্ষর **filtered = ফাঁকা;
        size_t n_filtered = 0;
        STRV_FOREACH(p, ফলাফল) {
                যদি (!safe_glob_verify(*p, prefix))
                        চালু;

                যদি (!ret)
                        ফেরত 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_আকার(&filtered, &n_filtered, *p);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (n_filtered == 0)
                ফেরত -ENOENT;

        প্রতিজ্ঞা(ret);
        *ret = TAKE_PTR(filtered);
        ফেরত 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, শূন্য, closedir);

পূর্ণসংখ্যা safe_glob_full(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags, opendir_t opendir_func, অক্ষর ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (কাঠামো dirent* (*)(শূন্য *)) readdir_no_dot,
                .gl_opendir = (শূন্য* (*)(ধ্রুবক অক্ষর *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, ফাঁকা, &g);
        যদি (r == GLOB_NOMATCH)
                ফেরত -ENOENT;
        যদি (r == GLOB_NOSPACE)
                ফেরত -ENOMEM;
        যদি (r != 0)
                ফেরত errno_or_else(EIO);

#ifndef __GLIBC__
        ফেরত filter_glob_ফলাফল(g.gl_pathv, path, ret);
#endif

        যদি (strv_isempty(g.gl_pathv))
                ফেরত -ENOENT;

        যদি (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ফেরত 0;
}

পূর্ণসংখ্যা glob_first(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        _cleanup_strv_free_ অক্ষর **v = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        যদি (r == -ENOENT) {
                যদি (ret)
                        *ret = ফাঁকা;
                ফেরত false;
        }
        যদি (r < 0)
                ফেরত r;

        প্রতিজ্ঞা(!strv_isempty(v));

        যদি (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ফেরত true;
}

পূর্ণসংখ্যা glob_extend(অক্ষর ***strv, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags) {
        অক্ষর **v;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        যদি (r < 0)
                ফেরত r;

        ফেরত strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

পূর্ণসংখ্যা glob_non_glob_prefix(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        যদি (path[n] != '\0')
                যখন (n > 0 && path[n-1] != '/')
                        n--;

        যদি (n == 0)
                ফেরত -ENOENT;

        অক্ষর *ans = strndup(path, n);
        যদি (!ans)
                ফেরত -ENOMEM;
        *ret = ans;
        ফেরত 0;
}

bool string_is_glob(ধ্রুবক অক্ষর *p) {
        /* Check if a string contains any glob patterns. */
        ফেরত !!strpbrk(p, GLOB_CHARS);
}
