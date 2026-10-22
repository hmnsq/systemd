/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <dirent.h>
#அந்தர்பாவ <sys/stat.h>

#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "glob-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"

#ifndef __GLIBC__
நிலையான bool safe_glob_verify(மாறிலி எழுத்து *p, மாறிலி எழுத்து *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        எனில் (isempty(p))
                திரும்பு false; /* should not happen, but for safey. */

        எனில் (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                எனில் (!p)
                        திரும்பு false; /* should not happen, but for safety. */
        }

        க்கு (;;) {
                p += strspn(p, "/");
                எனில் (*p == '\0')
                        திரும்பு true;
                எனில் (*p == '.') {
                        p++;
                        எனில் (IN_SET(*p, '/', '\0'))
                                திரும்பு false; /* refuse dot */
                        எனில் (*p == '.') {
                                p++;
                                எனில் (IN_SET(*p, '/', '\0'))
                                        திரும்பு false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                எனில் (*p == '\0')
                        திரும்பு true;
        }
}

நிலையான முழு filter_glob_result(எழுத்து * மாறிலி *result, மாறிலி எழுத்து *path, எழுத்து ***ret) {
        முழு r;

        ப்ரதிஜ்ஞா(path);

        எனில் (strv_isempty(result))
                திரும்பு -ENOENT;

        _cleanup_free_ எழுத்து *prefix = வெற்று;
        r = glob_non_glob_prefix(path, &prefix);
        எனில் (r < 0 && r != -ENOENT)
                திரும்பு r;

        _cleanup_strv_free_ எழுத்து **filtered = வெற்று;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                எனில் (!safe_glob_verify(*p, prefix))
                        தொடர்;

                எனில் (!ret)
                        திரும்பு 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (n_filtered == 0)
                திரும்பு -ENOENT;

        ப்ரதிஜ்ஞா(ret);
        *ret = TAKE_PTR(filtered);
        திரும்பு 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, வெறுமை, closedir);

முழு safe_glob_full(மாறிலி எழுத்து *path, முழு flags, opendir_t opendir_func, எழுத்து ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (கட்டமைப்பு dirent* (*)(வெறுமை *)) readdir_no_dot,
                .gl_opendir = (வெறுமை* (*)(மாறிலி எழுத்து *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        முழு r;

        ப்ரதிஜ்ஞா(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, வெற்று, &g);
        எனில் (r == GLOB_NOMATCH)
                திரும்பு -ENOENT;
        எனில் (r == GLOB_NOSPACE)
                திரும்பு -ENOMEM;
        எனில் (r != 0)
                திரும்பு errno_or_else(EIO);

#ifndef __GLIBC__
        திரும்பு filter_glob_result(g.gl_pathv, path, ret);
#endif

        எனில் (strv_isempty(g.gl_pathv))
                திரும்பு -ENOENT;

        எனில் (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        திரும்பு 0;
}

முழு glob_first(மாறிலி எழுத்து *path, எழுத்து **ret) {
        _cleanup_strv_free_ எழுத்து **v = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        எனில் (r == -ENOENT) {
                எனில் (ret)
                        *ret = வெற்று;
                திரும்பு false;
        }
        எனில் (r < 0)
                திரும்பு r;

        ப்ரதிஜ்ஞா(!strv_isempty(v));

        எனில் (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        திரும்பு true;
}

முழு glob_extend(எழுத்து ***strv, மாறிலி எழுத்து *path, முழு flags) {
        எழுத்து **v;
        முழு r;

        ப்ரதிஜ்ஞா(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

முழு glob_non_glob_prefix(மாறிலி எழுத்து *path, எழுத்து **ret) {
        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        எனில் (path[n] != '\0')
                போது (n > 0 && path[n-1] != '/')
                        n--;

        எனில் (n == 0)
                திரும்பு -ENOENT;

        எழுத்து *ans = strndup(path, n);
        எனில் (!ans)
                திரும்பு -ENOMEM;
        *ret = ans;
        திரும்பு 0;
}

bool string_is_glob(மாறிலி எழுத்து *p) {
        /* Check if a string contains any glob patterns. */
        திரும்பு !!strpbrk(p, GLOB_CHARS);
}
