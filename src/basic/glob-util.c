/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <dirent.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "glob-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

#ifndef __GLIBC__
ಸ್ಥಿರ bool safe_glob_verify(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ false; /* should not happen, but for safey. */

        ಯದಿ (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ false; /* should not happen, but for safety. */
        }

        ಪ್ರತಿ (;;) {
                p += strspn(p, "/");
                ಯದಿ (*p == '\0')
                        ಪ್ರತ್ಯಾಗಮ true;
                ಯದಿ (*p == '.') {
                        p++;
                        ಯದಿ (IN_SET(*p, '/', '\0'))
                                ಪ್ರತ್ಯಾಗಮ false; /* refuse dot */
                        ಯದಿ (*p == '.') {
                                p++;
                                ಯದಿ (IN_SET(*p, '/', '\0'))
                                        ಪ್ರತ್ಯಾಗಮ false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ಯದಿ (*p == '\0')
                        ಪ್ರತ್ಯಾಗಮ true;
        }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ filter_glob_result(ವರ್ಣ * ಧ್ರುವ *result, ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ ***ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        ಯದಿ (strv_isempty(result))
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        _cleanup_free_ ವರ್ಣ *prefix = ರಿಕ್ತ;
        r = glob_non_glob_prefix(path, &prefix);
        ಯದಿ (r < 0 && r != -ENOENT)
                ಪ್ರತ್ಯಾಗಮ r;

        _cleanup_strv_free_ ವರ್ಣ **filtered = ರಿಕ್ತ;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                ಯದಿ (!safe_glob_verify(*p, prefix))
                        ಅನುವರ್ತ;

                ಯದಿ (!ret)
                        ಪ್ರತ್ಯಾಗಮ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (n_filtered == 0)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        ಪ್ರತಿಜ್ಞಾ(ret);
        *ret = TAKE_PTR(filtered);
        ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, ಶೂನ್ಯ, closedir);

ಪೂರ್ಣಾಙ್ಕ safe_glob_full(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, opendir_t opendir_func, ವರ್ಣ ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (ಸಂರಚನಾ dirent* (*)(ಶೂನ್ಯ *)) readdir_no_dot,
                .gl_opendir = (ಶೂನ್ಯ* (*)(ಧ್ರುವ ವರ್ಣ *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, ರಿಕ್ತ, &g);
        ಯದಿ (r == GLOB_NOMATCH)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;
        ಯದಿ (r == GLOB_NOSPACE)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ errno_or_else(EIO);

#ifndef __GLIBC__
        ಪ್ರತ್ಯಾಗಮ filter_glob_result(g.gl_pathv, path, ret);
#endif

        ಯದಿ (strv_isempty(g.gl_pathv))
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        ಯದಿ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ glob_first(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        _cleanup_strv_free_ ವರ್ಣ **v = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        ಯದಿ (r == -ENOENT) {
                ಯದಿ (ret)
                        *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ false;
        }
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿಜ್ಞಾ(!strv_isempty(v));

        ಯದಿ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ glob_extend(ವರ್ಣ ***strv, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags) {
        ವರ್ಣ **v;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

ಪೂರ್ಣಾಙ್ಕ glob_non_glob_prefix(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        ಯದಿ (path[n] != '\0')
                ಯಾವತ್ (n > 0 && path[n-1] != '/')
                        n--;

        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        ವರ್ಣ *ans = strndup(path, n);
        ಯದಿ (!ans)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        *ret = ans;
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool string_is_glob(ಧ್ರುವ ವರ್ಣ *p) {
        /* Check if a string contains any glob patterns. */
        ಪ್ರತ್ಯಾಗಮ !!strpbrk(p, GLOB_CHARS);
}
