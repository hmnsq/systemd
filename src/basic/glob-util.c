/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <dirent.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "glob-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

#ifndef __GLIBC__
स्थिर bool safe_glob_verify(ध्रुव वर्ण *p, ध्रुव वर्ण *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        जइ (isempty(p))
                प्रत्यागम false; /* should not happen, but for safey. */

        जइ (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                जइ (!p)
                        प्रत्यागम false; /* should not happen, but for safety. */
        }

        प्रति (;;) {
                p += strspn(p, "/");
                जइ (*p == '\0')
                        प्रत्यागम true;
                जइ (*p == '.') {
                        p++;
                        जइ (IN_SET(*p, '/', '\0'))
                                प्रत्यागम false; /* refuse dot */
                        जइ (*p == '.') {
                                p++;
                                जइ (IN_SET(*p, '/', '\0'))
                                        प्रत्यागम false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                जइ (*p == '\0')
                        प्रत्यागम true;
        }
}

स्थिर पूर्णाङ्क filter_glob_result(वर्ण * ध्रुव *result, ध्रुव वर्ण *path, वर्ण ***ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        जइ (strv_isempty(result))
                प्रत्यागम -ENOENT;

        _cleanup_free_ वर्ण *prefix = रिक्त;
        r = glob_non_glob_prefix(path, &prefix);
        जइ (r < 0 && r != -ENOENT)
                प्रत्यागम r;

        _cleanup_strv_free_ वर्ण **filtered = रिक्त;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                जइ (!safe_glob_verify(*p, prefix))
                        अनुवर्त;

                जइ (!ret)
                        प्रत्यागम 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                जइ (r < 0)
                        प्रत्यागम r;
        }

        जइ (n_filtered == 0)
                प्रत्यागम -ENOENT;

        प्रतिज्ञा(ret);
        *ret = TAKE_PTR(filtered);
        प्रत्यागम 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, शून्य, closedir);

पूर्णाङ्क safe_glob_full(ध्रुव वर्ण *path, पूर्णाङ्क flags, opendir_t opendir_func, वर्ण ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (संरचना dirent* (*)(शून्य *)) readdir_no_dot,
                .gl_opendir = (शून्य* (*)(ध्रुव वर्ण *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, रिक्त, &g);
        जइ (r == GLOB_NOMATCH)
                प्रत्यागम -ENOENT;
        जइ (r == GLOB_NOSPACE)
                प्रत्यागम -ENOMEM;
        जइ (r != 0)
                प्रत्यागम errno_or_else(EIO);

#ifndef __GLIBC__
        प्रत्यागम filter_glob_result(g.gl_pathv, path, ret);
#endif

        जइ (strv_isempty(g.gl_pathv))
                प्रत्यागम -ENOENT;

        जइ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        प्रत्यागम 0;
}

पूर्णाङ्क glob_first(ध्रुव वर्ण *path, वर्ण **ret) {
        _cleanup_strv_free_ वर्ण **v = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        जइ (r == -ENOENT) {
                जइ (ret)
                        *ret = रिक्त;
                प्रत्यागम false;
        }
        जइ (r < 0)
                प्रत्यागम r;

        प्रतिज्ञा(!strv_isempty(v));

        जइ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        प्रत्यागम true;
}

पूर्णाङ्क glob_extend(वर्ण ***strv, ध्रुव वर्ण *path, पूर्णाङ्क flags) {
        वर्ण **v;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

पूर्णाङ्क glob_non_glob_prefix(ध्रुव वर्ण *path, वर्ण **ret) {
        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        जइ (path[n] != '\0')
                जाव (n > 0 && path[n-1] != '/')
                        n--;

        जइ (n == 0)
                प्रत्यागम -ENOENT;

        वर्ण *ans = strndup(path, n);
        जइ (!ans)
                प्रत्यागम -ENOMEM;
        *ret = ans;
        प्रत्यागम 0;
}

bool string_is_glob(ध्रुव वर्ण *p) {
        /* Check if a string contains any glob patterns. */
        प्रत्यागम !!strpbrk(p, GLOB_CHARS);
}
