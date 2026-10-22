/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <dirent.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glob-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __GLIBC__
ਸਥਿਰ bool ਸੁਰੱਖਿਅਤ_glob_verify(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ) {
        ਸੂਚਕ_MAY_BE_NULL(p);
        ਸੂਚਕ_MAY_BE_NULL(ਅਗੇਤਰ);

        ਜੇ (isempty(p))
                ਵਾਪਸ false; /* should not happen, but for safey. */

        ਜੇ (ਅਗੇਤਰ) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, ਅਗੇਤਰ);
                ਜੇ (!p)
                        ਵਾਪਸ false; /* should not happen, but for safety. */
        }

        ਲਈ (;;) {
                p += strspn(p, "/");
                ਜੇ (*p == '\0')
                        ਵਾਪਸ true;
                ਜੇ (*p == '.') {
                        p++;
                        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(*p, '/', '\0'))
                                ਵਾਪਸ false; /* refuse dot */
                        ਜੇ (*p == '.') {
                                p++;
                                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(*p, '/', '\0'))
                                        ਵਾਪਸ false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ਜੇ (*p == '\0')
                        ਵਾਪਸ true;
        }
}

ਸਥਿਰ ਪੂਰਨ filter_glob_ਨਤੀਜਾ(ਅੱਖਰ * ਅਚਲ *ਨਤੀਜਾ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ ***ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        ਜੇ (strv_isempty(ਨਤੀਜਾ))
                ਵਾਪਸ -ENOENT;

        _cleanup_free_ ਅੱਖਰ *ਅਗੇਤਰ = ਨਿਰਾ;
        r = glob_non_glob_ਅਗੇਤਰ(ਰਸਤਾ, &ਅਗੇਤਰ);
        ਜੇ (r < 0 && r != -ENOENT)
                ਵਾਪਸ r;

        _cleanup_strv_free_ ਅੱਖਰ **filtered = ਨਿਰਾ;
        size_t n_filtered = 0;
        STRV_FOREACH(p, ਨਤੀਜਾ) {
                ਜੇ (!ਸੁਰੱਖਿਅਤ_glob_verify(*p, ਅਗੇਤਰ))
                        ਜਾਰੀ;

                ਜੇ (!ret)
                        ਵਾਪਸ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_ਮਾਪ(&filtered, &n_filtered, *p);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (n_filtered == 0)
                ਵਾਪਸ -ENOENT;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        *ret = TAKE_PTR(filtered);
        ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, ਖਾਲੀ, closedir);

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_glob_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, opendir_t opendir_func, ਅੱਖਰ ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (ਬਣਤਰ dirent* (*)(ਖਾਲੀ *)) readdir_no_dot,
                .gl_opendir = (ਖਾਲੀ* (*)(ਅਚਲ ਅੱਖਰ *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        errno = 0;
        r = glob(ਰਸਤਾ, flags | GLOB_ALTDIRFUNC, ਨਿਰਾ, &g);
        ਜੇ (r == GLOB_NOMATCH)
                ਵਾਪਸ -ENOENT;
        ਜੇ (r == GLOB_NOSPACE)
                ਵਾਪਸ -ENOMEM;
        ਜੇ (r != 0)
                ਵਾਪਸ errno_or_else(EIO);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __GLIBC__
        ਵਾਪਸ filter_glob_ਨਤੀਜਾ(g.gl_pathv, ਰਸਤਾ, ret);
#ਜੇ_ਅੰਤ

        ਜੇ (strv_isempty(g.gl_pathv))
                ਵਾਪਸ -ENOENT;

        ਜੇ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ glob_ਪਹਿਲਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        _cleanup_strv_free_ ਅੱਖਰ **v = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        r = ਸੁਰੱਖਿਅਤ_glob(ਰਸਤਾ, GLOB_NOSORT|GLOB_BRACE, &v);
        ਜੇ (r == -ENOENT) {
                ਜੇ (ret)
                        *ret = ਨਿਰਾ;
                ਵਾਪਸ false;
        }
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!strv_isempty(v));

        ਜੇ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ਵਾਪਸ true;
}

ਪੂਰਨ glob_extend(ਅੱਖਰ ***strv, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags) {
        ਅੱਖਰ **v;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        r = ਸੁਰੱਖਿਅਤ_glob(ਰਸਤਾ, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

ਪੂਰਨ glob_non_glob_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(ਰਸਤਾ, GLOB_CHARS);

        ਜੇ (ਰਸਤਾ[n] != '\0')
                ਜਦੋਂ (n > 0 && ਰਸਤਾ[n-1] != '/')
                        n--;

        ਜੇ (n == 0)
                ਵਾਪਸ -ENOENT;

        ਅੱਖਰ *ans = strndup(ਰਸਤਾ, n);
        ਜੇ (!ans)
                ਵਾਪਸ -ENOMEM;
        *ret = ans;
        ਵਾਪਸ 0;
}

bool string_is_glob(ਅਚਲ ਅੱਖਰ *p) {
        /* Check if a string contains any glob patterns. */
        ਵਾਪਸ !!strpbrk(p, GLOB_CHARS);
}
