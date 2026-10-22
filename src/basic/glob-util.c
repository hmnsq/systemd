/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <dirent.h>
#অন্তর্ভাব <sys/stat.h>

#অন্তর্ভাব "dirent-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "glob-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"

#ifndef __GLIBC__
স্থির bool safe_glob_verify(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        যদি (isempty(p))
                প্রত্যাগম false; /* should not happen, but for safey. */

        যদি (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                যদি (!p)
                        প্রত্যাগম false; /* should not happen, but for safety. */
        }

        প্রতি (;;) {
                p += strspn(p, "/");
                যদি (*p == '\0')
                        প্রত্যাগম true;
                যদি (*p == '.') {
                        p++;
                        যদি (IN_SET(*p, '/', '\0'))
                                প্রত্যাগম false; /* refuse dot */
                        যদি (*p == '.') {
                                p++;
                                যদি (IN_SET(*p, '/', '\0'))
                                        প্রত্যাগম false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                যদি (*p == '\0')
                        প্রত্যাগম true;
        }
}

স্থির পূর্ণাঙ্ক filter_glob_result(বর্ণ * ধ্রুব *result, ধ্রুব বর্ণ *path, বর্ণ ***ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        যদি (strv_isempty(result))
                প্রত্যাগম -ENOENT;

        _cleanup_free_ বর্ণ *prefix = রিক্ত;
        r = glob_non_glob_prefix(path, &prefix);
        যদি (r < 0 && r != -ENOENT)
                প্রত্যাগম r;

        _cleanup_strv_free_ বর্ণ **filtered = রিক্ত;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                যদি (!safe_glob_verify(*p, prefix))
                        অনুবর্ত;

                যদি (!ret)
                        প্রত্যাগম 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (n_filtered == 0)
                প্রত্যাগম -ENOENT;

        প্রতিজ্ঞা(ret);
        *ret = TAKE_PTR(filtered);
        প্রত্যাগম 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, শূন্য, closedir);

পূর্ণাঙ্ক safe_glob_full(ধ্রুব বর্ণ *path, পূর্ণাঙ্ক flags, opendir_t opendir_func, বর্ণ ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (সংরচনা dirent* (*)(শূন্য *)) readdir_no_dot,
                .gl_opendir = (শূন্য* (*)(ধ্রুব বর্ণ *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, রিক্ত, &g);
        যদি (r == GLOB_NOMATCH)
                প্রত্যাগম -ENOENT;
        যদি (r == GLOB_NOSPACE)
                প্রত্যাগম -ENOMEM;
        যদি (r != 0)
                প্রত্যাগম errno_or_else(EIO);

#ifndef __GLIBC__
        প্রত্যাগম filter_glob_result(g.gl_pathv, path, ret);
#endif

        যদি (strv_isempty(g.gl_pathv))
                প্রত্যাগম -ENOENT;

        যদি (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক glob_first(ধ্রুব বর্ণ *path, বর্ণ **ret) {
        _cleanup_strv_free_ বর্ণ **v = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        যদি (r == -ENOENT) {
                যদি (ret)
                        *ret = রিক্ত;
                প্রত্যাগম false;
        }
        যদি (r < 0)
                প্রত্যাগম r;

        প্রতিজ্ঞা(!strv_isempty(v));

        যদি (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        প্রত্যাগম true;
}

পূর্ণাঙ্ক glob_extend(বর্ণ ***strv, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক flags) {
        বর্ণ **v;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

পূর্ণাঙ্ক glob_non_glob_prefix(ধ্রুব বর্ণ *path, বর্ণ **ret) {
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        যদি (path[n] != '\0')
                যাবত্ (n > 0 && path[n-1] != '/')
                        n--;

        যদি (n == 0)
                প্রত্যাগম -ENOENT;

        বর্ণ *ans = strndup(path, n);
        যদি (!ans)
                প্রত্যাগম -ENOMEM;
        *ret = ans;
        প্রত্যাগম 0;
}

bool string_is_glob(ধ্রুব বর্ণ *p) {
        /* Check if a string contains any glob patterns. */
        প্রত্যাগম !!strpbrk(p, GLOB_CHARS);
}
