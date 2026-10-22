/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <dirent.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "glob-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

#ifndef __GLIBC__
स्थिर bool safe_glob_verify(स्थिरांक अक्षर *p, स्थिरांक अक्षर *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        जर (isempty(p))
                परत false; /* should not happen, but for safey. */

        जर (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                जर (!p)
                        परत false; /* should not happen, but for safety. */
        }

        साठी (;;) {
                p += म_अखोज(p, "/");
                जर (*p == '\0')
                        परत true;
                जर (*p == '.') {
                        p++;
                        जर (IN_SET(*p, '/', '\0'))
                                परत false; /* refuse dot */
                        जर (*p == '.') {
                                p++;
                                जर (IN_SET(*p, '/', '\0'))
                                        परत false; /* refuse dot dot */
                        }
                }

                p += म_खोज(p, "/");
                जर (*p == '\0')
                        परत true;
        }
}

स्थिर पूर्णांक filter_glob_result(अक्षर * स्थिरांक *result, स्थिरांक अक्षर *path, अक्षर ***ret) {
        पूर्णांक r;

        प्रतिज्ञा(path);

        जर (strv_isempty(result))
                परत -ENOENT;

        _cleanup_free_ अक्षर *prefix = रिक्त;
        r = glob_non_glob_prefix(path, &prefix);
        जर (r < 0 && r != -ENOENT)
                परत r;

        _cleanup_strv_free_ अक्षर **filtered = रिक्त;
        माप_प्रकार n_filtered = 0;
        STRV_FOREACH(p, result) {
                जर (!safe_glob_verify(*p, prefix))
                        सुरूठेवा;

                जर (!ret)
                        परत 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                जर (r < 0)
                        परत r;
        }

        जर (n_filtered == 0)
                परत -ENOENT;

        प्रतिज्ञा(ret);
        *ret = TAKE_PTR(filtered);
        परत 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, शून्य, बंद_सूची);

पूर्णांक safe_glob_full(स्थिरांक अक्षर *path, पूर्णांक flags, opendir_t opendir_func, अक्षर ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (संरचना dirent* (*)(शून्य *)) readdir_no_dot,
                .gl_opendir = (शून्य* (*)(स्थिरांक अक्षर *)) (opendir_func ?: सूची_खोलो),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        पूर्णांक r;

        प्रतिज्ञा(path);

        त्रुटि_सं = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, रिक्त, &g);
        जर (r == GLOB_NOMATCH)
                परत -ENOENT;
        जर (r == GLOB_NOSPACE)
                परत -ENOMEM;
        जर (r != 0)
                परत errno_or_else(EIO);

#ifndef __GLIBC__
        परत filter_glob_result(g.gl_pathv, path, ret);
#endif

        जर (strv_isempty(g.gl_pathv))
                परत -ENOENT;

        जर (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        परत 0;
}

पूर्णांक glob_first(स्थिरांक अक्षर *path, अक्षर **ret) {
        _cleanup_strv_free_ अक्षर **v = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        जर (r == -ENOENT) {
                जर (ret)
                        *ret = रिक्त;
                परत false;
        }
        जर (r < 0)
                परत r;

        प्रतिज्ञा(!strv_isempty(v));

        जर (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        परत true;
}

पूर्णांक glob_extend(अक्षर ***strv, स्थिरांक अक्षर *path, पूर्णांक flags) {
        अक्षर **v;
        पूर्णांक r;

        प्रतिज्ञा(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        जर (r < 0)
                परत r;

        परत strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

पूर्णांक glob_non_glob_prefix(स्थिरांक अक्षर *path, अक्षर **ret) {
        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        /* Return the path of the path that has no glob characters. */

        माप_प्रकार n = म_खोज(path, GLOB_CHARS);

        जर (path[n] != '\0')
                पर्यंत (n > 0 && path[n-1] != '/')
                        n--;

        जर (n == 0)
                परत -ENOENT;

        अक्षर *ans = strndup(path, n);
        जर (!ans)
                परत -ENOMEM;
        *ret = ans;
        परत 0;
}

bool string_is_glob(स्थिरांक अक्षर *p) {
        /* Check if a string contains any glob patterns. */
        परत !!strpbrk(p, GLOB_CHARS);
}
