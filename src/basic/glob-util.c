/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <dirent.h>
#અન્તર્ભાવ <sys/stat.h>

#અન્તર્ભાવ "dirent-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "glob-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"

#ifndef __GLIBC__
સ્થિર bool safe_glob_verify(અચલ અક્ષર *p, અચલ અક્ષર *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        જો (isempty(p))
                પાછા false; /* should not happen, but for safey. */

        જો (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                જો (!p)
                        પાછા false; /* should not happen, but for safety. */
        }

        માટે (;;) {
                p += strspn(p, "/");
                જો (*p == '\0')
                        પાછા true;
                જો (*p == '.') {
                        p++;
                        જો (IN_SET(*p, '/', '\0'))
                                પાછા false; /* refuse dot */
                        જો (*p == '.') {
                                p++;
                                જો (IN_SET(*p, '/', '\0'))
                                        પાછા false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                જો (*p == '\0')
                        પાછા true;
        }
}

સ્થિર પૂર્ણાંક filter_glob_result(અક્ષર * અચલ *result, અચલ અક્ષર *path, અક્ષર ***ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        જો (strv_isempty(result))
                પાછા -ENOENT;

        _cleanup_free_ અક્ષર *prefix = ખાલી;
        r = glob_non_glob_prefix(path, &prefix);
        જો (r < 0 && r != -ENOENT)
                પાછા r;

        _cleanup_strv_free_ અક્ષર **filtered = ખાલી;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                જો (!safe_glob_verify(*p, prefix))
                        ચાલુ;

                જો (!ret)
                        પાછા 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                જો (r < 0)
                        પાછા r;
        }

        જો (n_filtered == 0)
                પાછા -ENOENT;

        પ્રતિજ્ઞા(ret);
        *ret = TAKE_PTR(filtered);
        પાછા 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, શૂન્ય, closedir);

પૂર્ણાંક safe_glob_full(અચલ અક્ષર *path, પૂર્ણાંક flags, opendir_t opendir_func, અક્ષર ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (સંરચના dirent* (*)(શૂન્ય *)) readdir_no_dot,
                .gl_opendir = (શૂન્ય* (*)(અચલ અક્ષર *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, ખાલી, &g);
        જો (r == GLOB_NOMATCH)
                પાછા -ENOENT;
        જો (r == GLOB_NOSPACE)
                પાછા -ENOMEM;
        જો (r != 0)
                પાછા errno_or_else(EIO);

#ifndef __GLIBC__
        પાછા filter_glob_result(g.gl_pathv, path, ret);
#endif

        જો (strv_isempty(g.gl_pathv))
                પાછા -ENOENT;

        જો (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        પાછા 0;
}

પૂર્ણાંક glob_first(અચલ અક્ષર *path, અક્ષર **ret) {
        _cleanup_strv_free_ અક્ષર **v = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        જો (r == -ENOENT) {
                જો (ret)
                        *ret = ખાલી;
                પાછા false;
        }
        જો (r < 0)
                પાછા r;

        પ્રતિજ્ઞા(!strv_isempty(v));

        જો (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        પાછા true;
}

પૂર્ણાંક glob_extend(અક્ષર ***strv, અચલ અક્ષર *path, પૂર્ણાંક flags) {
        અક્ષર **v;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        જો (r < 0)
                પાછા r;

        પાછા strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

પૂર્ણાંક glob_non_glob_prefix(અચલ અક્ષર *path, અક્ષર **ret) {
        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        જો (path[n] != '\0')
                જ્યારે (n > 0 && path[n-1] != '/')
                        n--;

        જો (n == 0)
                પાછા -ENOENT;

        અક્ષર *ans = strndup(path, n);
        જો (!ans)
                પાછા -ENOMEM;
        *ret = ans;
        પાછા 0;
}

bool string_is_glob(અચલ અક્ષર *p) {
        /* Check if a string contains any glob patterns. */
        પાછા !!strpbrk(p, GLOB_CHARS);
}
