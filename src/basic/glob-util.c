/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <dirent.h>
#समावेश <sys/stat.h>

#समावेश "dirent-util.h"
#समावेश "errno-util.h"
#समावेश "glob-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"

#अगर_अघोषित __GLIBC__
अटल bool safe_glob_verify(स्थिर अक्षर *p, स्थिर अक्षर *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        अगर (isempty(p))
                वापस false; /* should not happen, but for safey. */

        अगर (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                अगर (!p)
                        वापस false; /* should not happen, but for safety. */
        }

        क्रम (;;) {
                p += म_अखोज(p, "/");
                अगर (*p == '\0')
                        वापस true;
                अगर (*p == '.') {
                        p++;
                        अगर (IN_SET(*p, '/', '\0'))
                                वापस false; /* refuse dot */
                        अगर (*p == '.') {
                                p++;
                                अगर (IN_SET(*p, '/', '\0'))
                                        वापस false; /* refuse dot dot */
                        }
                }

                p += म_खोज(p, "/");
                अगर (*p == '\0')
                        वापस true;
        }
}

अटल पूर्णांक filter_glob_परिणाम(अक्षर * स्थिर *परिणाम, स्थिर अक्षर *path, अक्षर ***ret) {
        पूर्णांक r;

        निश्चित(path);

        अगर (strv_isempty(परिणाम))
                वापस -ENOENT;

        _cleanup_free_ अक्षर *prefix = शून्य;
        r = glob_non_glob_prefix(path, &prefix);
        अगर (r < 0 && r != -ENOENT)
                वापस r;

        _cleanup_strv_free_ अक्षर **filtered = शून्य;
        माप_प्रकार n_filtered = 0;
        STRV_FOREACH(p, परिणाम) {
                अगर (!safe_glob_verify(*p, prefix))
                        जारी;

                अगर (!ret)
                        वापस 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_आकार(&filtered, &n_filtered, *p);
                अगर (r < 0)
                        वापस r;
        }

        अगर (n_filtered == 0)
                वापस -ENOENT;

        निश्चित(ret);
        *ret = TAKE_PTR(filtered);
        वापस 0;
}
#पूर्ण_अगर

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, व्योम, बंद_सूची);

पूर्णांक safe_glob_full(स्थिर अक्षर *path, पूर्णांक flags, opendir_t opendir_func, अक्षर ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (काष्ठा dirent* (*)(व्योम *)) readdir_no_dot,
                .gl_opendir = (व्योम* (*)(स्थिर अक्षर *)) (opendir_func ?: सूची_खोलो),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        पूर्णांक r;

        निश्चित(path);

        त्रुटि_सं = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, शून्य, &g);
        अगर (r == GLOB_NOMATCH)
                वापस -ENOENT;
        अगर (r == GLOB_NOSPACE)
                वापस -ENOMEM;
        अगर (r != 0)
                वापस errno_or_else(EIO);

#अगर_अघोषित __GLIBC__
        वापस filter_glob_परिणाम(g.gl_pathv, path, ret);
#पूर्ण_अगर

        अगर (strv_isempty(g.gl_pathv))
                वापस -ENOENT;

        अगर (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        वापस 0;
}

पूर्णांक glob_first(स्थिर अक्षर *path, अक्षर **ret) {
        _cleanup_strv_free_ अक्षर **v = शून्य;
        पूर्णांक r;

        निश्चित(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        अगर (r == -ENOENT) {
                अगर (ret)
                        *ret = शून्य;
                वापस false;
        }
        अगर (r < 0)
                वापस r;

        निश्चित(!strv_isempty(v));

        अगर (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        वापस true;
}

पूर्णांक glob_extend(अक्षर ***strv, स्थिर अक्षर *path, पूर्णांक flags) {
        अक्षर **v;
        पूर्णांक r;

        निश्चित(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        अगर (r < 0)
                वापस r;

        वापस strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

पूर्णांक glob_non_glob_prefix(स्थिर अक्षर *path, अक्षर **ret) {
        निश्चित(path);
        निश्चित(ret);

        /* Return the path of the path that has no glob characters. */

        माप_प्रकार n = म_खोज(path, GLOB_CHARS);

        अगर (path[n] != '\0')
                जबतक (n > 0 && path[n-1] != '/')
                        n--;

        अगर (n == 0)
                वापस -ENOENT;

        अक्षर *ans = strndup(path, n);
        अगर (!ans)
                वापस -ENOMEM;
        *ret = ans;
        वापस 0;
}

bool string_is_glob(स्थिर अक्षर *p) {
        /* Check if a string contains any glob patterns. */
        वापस !!strpbrk(p, GLOB_CHARS);
}
