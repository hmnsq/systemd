/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <dirent.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "glob-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

#ifndef __GLIBC__
स्थिर bool safe_glob_verify(अचर वर्ण *p, अचर वर्ण *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        यदि (isempty(p))
                फर्काऊ false; /* should not happen, but for safey. */

        यदि (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                यदि (!p)
                        फर्काऊ false; /* should not happen, but for safety. */
        }

        लागि (;;) {
                p += म_अखोज(p, "/");
                यदि (*p == '\0')
                        फर्काऊ true;
                यदि (*p == '.') {
                        p++;
                        यदि (IN_SET(*p, '/', '\0'))
                                फर्काऊ false; /* refuse dot */
                        यदि (*p == '.') {
                                p++;
                                यदि (IN_SET(*p, '/', '\0'))
                                        फर्काऊ false; /* refuse dot dot */
                        }
                }

                p += म_खोज(p, "/");
                यदि (*p == '\0')
                        फर्काऊ true;
        }
}

स्थिर पूर्णाङ्क filter_glob_result(वर्ण * अचर *result, अचर वर्ण *path, वर्ण ***ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        यदि (strv_isempty(result))
                फर्काऊ -ENOENT;

        _cleanup_free_ वर्ण *prefix = रिक्त;
        r = glob_non_glob_prefix(path, &prefix);
        यदि (r < 0 && r != -ENOENT)
                फर्काऊ r;

        _cleanup_strv_free_ वर्ण **filtered = रिक्त;
        माप_प्रकार n_filtered = 0;
        STRV_FOREACH(p, result) {
                यदि (!safe_glob_verify(*p, prefix))
                        जारी;

                यदि (!ret)
                        फर्काऊ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                यदि (r < 0)
                        फर्काऊ r;
        }

        यदि (n_filtered == 0)
                फर्काऊ -ENOENT;

        प्रतिज्ञा(ret);
        *ret = TAKE_PTR(filtered);
        फर्काऊ 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, शून्य, बंद_सूची);

पूर्णाङ्क safe_glob_full(अचर वर्ण *path, पूर्णाङ्क flags, opendir_t opendir_func, वर्ण ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (संरचना dirent* (*)(शून्य *)) readdir_no_dot,
                .gl_opendir = (शून्य* (*)(अचर वर्ण *)) (opendir_func ?: सूची_खोलो),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        त्रुटि_सं = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, रिक्त, &g);
        यदि (r == GLOB_NOMATCH)
                फर्काऊ -ENOENT;
        यदि (r == GLOB_NOSPACE)
                फर्काऊ -ENOMEM;
        यदि (r != 0)
                फर्काऊ errno_or_else(EIO);

#ifndef __GLIBC__
        फर्काऊ filter_glob_result(g.gl_pathv, path, ret);
#endif

        यदि (strv_isempty(g.gl_pathv))
                फर्काऊ -ENOENT;

        यदि (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        फर्काऊ 0;
}

पूर्णाङ्क glob_first(अचर वर्ण *path, वर्ण **ret) {
        _cleanup_strv_free_ वर्ण **v = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        यदि (r == -ENOENT) {
                यदि (ret)
                        *ret = रिक्त;
                फर्काऊ false;
        }
        यदि (r < 0)
                फर्काऊ r;

        प्रतिज्ञा(!strv_isempty(v));

        यदि (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        फर्काऊ true;
}

पूर्णाङ्क glob_extend(वर्ण ***strv, अचर वर्ण *path, पूर्णाङ्क flags) {
        वर्ण **v;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

पूर्णाङ्क glob_non_glob_prefix(अचर वर्ण *path, वर्ण **ret) {
        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        /* Return the path of the path that has no glob characters. */

        माप_प्रकार n = म_खोज(path, GLOB_CHARS);

        यदि (path[n] != '\0')
                जबसम्म (n > 0 && path[n-1] != '/')
                        n--;

        यदि (n == 0)
                फर्काऊ -ENOENT;

        वर्ण *ans = strndup(path, n);
        यदि (!ans)
                फर्काऊ -ENOMEM;
        *ret = ans;
        फर्काऊ 0;
}

bool string_is_glob(अचर वर्ण *p) {
        /* Check if a string contains any glob patterns. */
        फर्काऊ !!strpbrk(p, GLOB_CHARS);
}
