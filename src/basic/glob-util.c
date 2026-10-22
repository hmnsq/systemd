/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <dirent.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "glob-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

#यदि_अपरिभाषितम् __GLIBC__
स्थिर bool safe_glob_verify(ध्रुव वर्ण *p, ध्रुव वर्ण *उपसर्गः) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(उपसर्गः);

        यदि (isempty(p))
                प्रत्यागम false; /* should not happen, but for safey. */

        यदि (उपसर्गः) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, उपसर्गः);
                यदि (!p)
                        प्रत्यागम false; /* should not happen, but for safety. */
        }

        प्रति (;;) {
                p += strspn(p, "/");
                यदि (*p == '\0')
                        प्रत्यागम true;
                यदि (*p == '.') {
                        p++;
                        यदि (अन्तर्_समुच्चयः(*p, '/', '\0'))
                                प्रत्यागम false; /* refuse dot */
                        यदि (*p == '.') {
                                p++;
                                यदि (अन्तर्_समुच्चयः(*p, '/', '\0'))
                                        प्रत्यागम false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                यदि (*p == '\0')
                        प्रत्यागम true;
        }
}

स्थिर पूर्णाङ्क filter_glob_परिणाम(वर्ण * ध्रुव *परिणाम, ध्रुव वर्ण *मार्गः, वर्ण ***ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        यदि (strv_isempty(परिणाम))
                प्रत्यागम -ENOENT;

        _cleanup_free_ वर्ण *उपसर्गः = रिक्त;
        r = glob_non_glob_उपसर्गः(मार्गः, &उपसर्गः);
        यदि (r < 0 && r != -ENOENT)
                प्रत्यागम r;

        _cleanup_strv_free_ वर्ण **filtered = रिक्त;
        size_t n_filtered = 0;
        STRV_FOREACH(p, परिणाम) {
                यदि (!safe_glob_verify(*p, उपसर्गः))
                        अनुवर्त;

                यदि (!ret)
                        प्रत्यागम 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_आकार(&filtered, &n_filtered, *p);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (n_filtered == 0)
                प्रत्यागम -ENOENT;

        प्रतिज्ञा(ret);
        *ret = TAKE_PTR(filtered);
        प्रत्यागम 0;
}
#यदि_अन्तः

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, शून्य, closedir);

पूर्णाङ्क safe_glob_पूर्णम्(ध्रुव वर्ण *मार्गः, पूर्णाङ्क flags, opendir_t opendir_func, वर्ण ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (संरचना dirent* (*)(शून्य *)) readdir_no_dot,
                .gl_opendir = (शून्य* (*)(ध्रुव वर्ण *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        errno = 0;
        r = glob(मार्गः, flags | GLOB_ALTDIRFUNC, रिक्त, &g);
        यदि (r == GLOB_NOMATCH)
                प्रत्यागम -ENOENT;
        यदि (r == GLOB_NOSPACE)
                प्रत्यागम -ENOMEM;
        यदि (r != 0)
                प्रत्यागम errno_or_else(EIO);

#यदि_अपरिभाषितम् __GLIBC__
        प्रत्यागम filter_glob_परिणाम(g.gl_pathv, मार्गः, ret);
#यदि_अन्तः

        यदि (strv_isempty(g.gl_pathv))
                प्रत्यागम -ENOENT;

        यदि (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        प्रत्यागम 0;
}

पूर्णाङ्क glob_first(ध्रुव वर्ण *मार्गः, वर्ण **ret) {
        _cleanup_strv_free_ वर्ण **v = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        r = safe_glob(मार्गः, GLOB_NOSORT|GLOB_BRACE, &v);
        यदि (r == -ENOENT) {
                यदि (ret)
                        *ret = रिक्त;
                प्रत्यागम false;
        }
        यदि (r < 0)
                प्रत्यागम r;

        प्रतिज्ञा(!strv_isempty(v));

        यदि (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        प्रत्यागम true;
}

पूर्णाङ्क glob_extend(वर्ण ***strv, ध्रुव वर्ण *मार्गः, पूर्णाङ्क flags) {
        वर्ण **v;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        r = safe_glob(मार्गः, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

पूर्णाङ्क glob_non_glob_उपसर्गः(ध्रुव वर्ण *मार्गः, वर्ण **ret) {
        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(मार्गः, GLOB_CHARS);

        यदि (मार्गः[n] != '\0')
                यावत् (n > 0 && मार्गः[n-1] != '/')
                        n--;

        यदि (n == 0)
                प्रत्यागम -ENOENT;

        वर्ण *ans = strndup(मार्गः, n);
        यदि (!ans)
                प्रत्यागम -ENOMEM;
        *ret = ans;
        प्रत्यागम 0;
}

bool string_is_glob(ध्रुव वर्ण *p) {
        /* Check if a string contains any glob patterns. */
        प्रत्यागम !!strpbrk(p, GLOB_CHARS);
}
