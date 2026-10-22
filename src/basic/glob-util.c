/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <dirent.h>
#অন্তর্ভাৱ <sys/stat.h>

#অন্তর্ভাৱ "dirent-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "glob-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"

#ifndef __GLIBC__
স্থির bool safe_glob_verify(ধ্রুৱ ৱর্ণ *p, ধ্রুৱ ৱর্ণ *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        যদি (isempty(p))
                ঘূৰাই false; /* should not happen, but for safey. */

        যদি (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                যদি (!p)
                        ঘূৰাই false; /* should not happen, but for safety. */
        }

        বাবে (;;) {
                p += strspn(p, "/");
                যদি (*p == '\0')
                        ঘূৰাই true;
                যদি (*p == '.') {
                        p++;
                        যদি (IN_SET(*p, '/', '\0'))
                                ঘূৰাই false; /* refuse dot */
                        যদি (*p == '.') {
                                p++;
                                যদি (IN_SET(*p, '/', '\0'))
                                        ঘূৰাই false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                যদি (*p == '\0')
                        ঘূৰাই true;
        }
}

স্থির পূর্ণাঙ্ক filter_glob_result(ৱর্ণ * ধ্রুৱ *result, ধ্রুৱ ৱর্ণ *path, ৱর্ণ ***ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        যদি (strv_isempty(result))
                ঘূৰাই -ENOENT;

        _cleanup_free_ ৱর্ণ *prefix = রিক্ত;
        r = glob_non_glob_prefix(path, &prefix);
        যদি (r < 0 && r != -ENOENT)
                ঘূৰাই r;

        _cleanup_strv_free_ ৱর্ণ **filtered = রিক্ত;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                যদি (!safe_glob_verify(*p, prefix))
                        অনুৱর্ত;

                যদি (!ret)
                        ঘূৰাই 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        যদি (n_filtered == 0)
                ঘূৰাই -ENOENT;

        প্রতিজ্ঞা(ret);
        *ret = TAKE_PTR(filtered);
        ঘূৰাই 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, শূন্য, closedir);

পূর্ণাঙ্ক safe_glob_full(ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, opendir_t opendir_func, ৱর্ণ ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (সংরচনা dirent* (*)(শূন্য *)) readdir_no_dot,
                .gl_opendir = (শূন্য* (*)(ধ্রুৱ ৱর্ণ *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, রিক্ত, &g);
        যদি (r == GLOB_NOMATCH)
                ঘূৰাই -ENOENT;
        যদি (r == GLOB_NOSPACE)
                ঘূৰাই -ENOMEM;
        যদি (r != 0)
                ঘূৰাই errno_or_else(EIO);

#ifndef __GLIBC__
        ঘূৰাই filter_glob_result(g.gl_pathv, path, ret);
#endif

        যদি (strv_isempty(g.gl_pathv))
                ঘূৰাই -ENOENT;

        যদি (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক glob_first(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret) {
        _cleanup_strv_free_ ৱর্ণ **v = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        যদি (r == -ENOENT) {
                যদি (ret)
                        *ret = রিক্ত;
                ঘূৰাই false;
        }
        যদি (r < 0)
                ঘূৰাই r;

        প্রতিজ্ঞা(!strv_isempty(v));

        যদি (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ঘূৰাই true;
}

পূর্ণাঙ্ক glob_extend(ৱর্ণ ***strv, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags) {
        ৱর্ণ **v;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

পূর্ণাঙ্ক glob_non_glob_prefix(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret) {
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        যদি (path[n] != '\0')
                যেতিয়ালৈ (n > 0 && path[n-1] != '/')
                        n--;

        যদি (n == 0)
                ঘূৰাই -ENOENT;

        ৱর্ণ *ans = strndup(path, n);
        যদি (!ans)
                ঘূৰাই -ENOMEM;
        *ret = ans;
        ঘূৰাই 0;
}

bool string_is_glob(ধ্রুৱ ৱর্ণ *p) {
        /* Check if a string contains any glob patterns. */
        ঘূৰাই !!strpbrk(p, GLOB_CHARS);
}
