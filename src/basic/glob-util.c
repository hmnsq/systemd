/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <dirent.h>
#അന്തര്ഭാവ <sys/stat.h>

#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "glob-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"

#ifndef __GLIBC__
സ്ഥിതം bool safe_glob_verify(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *prefix) {
        POINTER_MAY_BE_NULL(p);
        POINTER_MAY_BE_NULL(prefix);

        എങ്കിൽ (isempty(p))
                തിരികെ false; /* should not happen, but for safey. */

        എങ്കിൽ (prefix) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, prefix);
                എങ്കിൽ (!p)
                        തിരികെ false; /* should not happen, but for safety. */
        }

        വേണ്ടി (;;) {
                p += strspn(p, "/");
                എങ്കിൽ (*p == '\0')
                        തിരികെ true;
                എങ്കിൽ (*p == '.') {
                        p++;
                        എങ്കിൽ (IN_SET(*p, '/', '\0'))
                                തിരികെ false; /* refuse dot */
                        എങ്കിൽ (*p == '.') {
                                p++;
                                എങ്കിൽ (IN_SET(*p, '/', '\0'))
                                        തിരികെ false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                എങ്കിൽ (*p == '\0')
                        തിരികെ true;
        }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ filter_glob_result(അക്ഷരം * സ്ഥിരം *result, സ്ഥിരം അക്ഷരം *path, അക്ഷരം ***ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        എങ്കിൽ (strv_isempty(result))
                തിരികെ -ENOENT;

        _cleanup_free_ അക്ഷരം *prefix = ശൂന്യ;
        r = glob_non_glob_prefix(path, &prefix);
        എങ്കിൽ (r < 0 && r != -ENOENT)
                തിരികെ r;

        _cleanup_strv_free_ അക്ഷരം **filtered = ശൂന്യ;
        size_t n_filtered = 0;
        STRV_FOREACH(p, result) {
                എങ്കിൽ (!safe_glob_verify(*p, prefix))
                        തുടരുക;

                എങ്കിൽ (!ret)
                        തിരികെ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_size(&filtered, &n_filtered, *p);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (n_filtered == 0)
                തിരികെ -ENOENT;

        പ്രതിജ്ഞാ(ret);
        *ret = TAKE_PTR(filtered);
        തിരികെ 0;
}
#endif

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, ശൂന്യം, closedir);

പൂർണ്ണസംഖ്യ safe_glob_full(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, opendir_t opendir_func, അക്ഷരം ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (ഘടന dirent* (*)(ശൂന്യം *)) readdir_no_dot,
                .gl_opendir = (ശൂന്യം* (*)(സ്ഥിരം അക്ഷരം *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        errno = 0;
        r = glob(path, flags | GLOB_ALTDIRFUNC, ശൂന്യ, &g);
        എങ്കിൽ (r == GLOB_NOMATCH)
                തിരികെ -ENOENT;
        എങ്കിൽ (r == GLOB_NOSPACE)
                തിരികെ -ENOMEM;
        എങ്കിൽ (r != 0)
                തിരികെ errno_or_else(EIO);

#ifndef __GLIBC__
        തിരികെ filter_glob_result(g.gl_pathv, path, ret);
#endif

        എങ്കിൽ (strv_isempty(g.gl_pathv))
                തിരികെ -ENOENT;

        എങ്കിൽ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ glob_first(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        _cleanup_strv_free_ അക്ഷരം **v = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE, &v);
        എങ്കിൽ (r == -ENOENT) {
                എങ്കിൽ (ret)
                        *ret = ശൂന്യ;
                തിരികെ false;
        }
        എങ്കിൽ (r < 0)
                തിരികെ r;

        പ്രതിജ്ഞാ(!strv_isempty(v));

        എങ്കിൽ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ glob_extend(അക്ഷരം ***strv, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags) {
        അക്ഷരം **v;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        r = safe_glob(path, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

പൂർണ്ണസംഖ്യ glob_non_glob_prefix(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(path, GLOB_CHARS);

        എങ്കിൽ (path[n] != '\0')
                ആകുമ്പോൾ (n > 0 && path[n-1] != '/')
                        n--;

        എങ്കിൽ (n == 0)
                തിരികെ -ENOENT;

        അക്ഷരം *ans = strndup(path, n);
        എങ്കിൽ (!ans)
                തിരികെ -ENOMEM;
        *ret = ans;
        തിരികെ 0;
}

bool string_is_glob(സ്ഥിരം അക്ഷരം *p) {
        /* Check if a string contains any glob patterns. */
        തിരികെ !!strpbrk(p, GLOB_CHARS);
}
