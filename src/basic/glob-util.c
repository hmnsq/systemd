/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <dirent.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "glob-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __GLIBC__
ಸ್ಥಿರ bool ಸುರಕ್ಷಿತ_glob_verify(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ) {
        ಸೂಚಕ_MAY_BE_NULL(p);
        ಸೂಚಕ_MAY_BE_NULL(ಪೂರ್ವಪ್ರತ್ಯಯ);

        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ false; /* should not happen, but for safey. */

        ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, ಪೂರ್ವಪ್ರತ್ಯಯ);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ false; /* should not happen, but for safety. */
        }

        ಗಾಗಿ (;;) {
                p += strspn(p, "/");
                ಆದರೆ (*p == '\0')
                        ಹಿಂತಿರುಗಿ true;
                ಆದರೆ (*p == '.') {
                        p++;
                        ಆದರೆ (ಒಳ_ಗಣ(*p, '/', '\0'))
                                ಹಿಂತಿರುಗಿ false; /* refuse dot */
                        ಆದರೆ (*p == '.') {
                                p++;
                                ಆದರೆ (ಒಳ_ಗಣ(*p, '/', '\0'))
                                        ಹಿಂತಿರುಗಿ false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ಆದರೆ (*p == '\0')
                        ಹಿಂತಿರುಗಿ true;
        }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ filter_glob_ಫಲಿತಾಂಶ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ ***ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        ಆದರೆ (strv_isempty(ಫಲಿತಾಂಶ))
                ಹಿಂತಿರುಗಿ -ENOENT;

        _cleanup_free_ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;
        r = glob_non_glob_ಪೂರ್ವಪ್ರತ್ಯಯ(ಮಾರ್ಗ, &ಪೂರ್ವಪ್ರತ್ಯಯ);
        ಆದರೆ (r < 0 && r != -ENOENT)
                ಹಿಂತಿರುಗಿ r;

        _cleanup_strv_free_ ಅಕ್ಷರ **filtered = ಖಾಲಿ;
        size_t n_filtered = 0;
        STRV_FOREACH(p, ಫಲಿತಾಂಶ) {
                ಆದರೆ (!ಸುರಕ್ಷಿತ_glob_verify(*p, ಪೂರ್ವಪ್ರತ್ಯಯ))
                        ಮುಂದುವರಿ;

                ಆದರೆ (!ret)
                        ಹಿಂತಿರುಗಿ 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_ಗಾತ್ರ(&filtered, &n_filtered, *p);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (n_filtered == 0)
                ಹಿಂತಿರುಗಿ -ENOENT;

        ಪ್ರತಿಜ್ಞಾ(ret);
        *ret = TAKE_PTR(filtered);
        ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, ಶೂನ್ಯ, closedir);

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_glob_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, opendir_t opendir_func, ಅಕ್ಷರ ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (ರಚನೆ dirent* (*)(ಶೂನ್ಯ *)) readdir_no_dot,
                .gl_opendir = (ಶೂನ್ಯ* (*)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        errno = 0;
        r = glob(ಮಾರ್ಗ, flags | GLOB_ALTDIRFUNC, ಖಾಲಿ, &g);
        ಆದರೆ (r == GLOB_NOMATCH)
                ಹಿಂತಿರುಗಿ -ENOENT;
        ಆದರೆ (r == GLOB_NOSPACE)
                ಹಿಂತಿರುಗಿ -ENOMEM;
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ errno_or_else(EIO);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __GLIBC__
        ಹಿಂತಿರುಗಿ filter_glob_ಫಲಿತಾಂಶ(g.gl_pathv, ಮಾರ್ಗ, ret);
#ಆದರೆ_ಅಂತ್ಯ

        ಆದರೆ (strv_isempty(g.gl_pathv))
                ಹಿಂತಿರುಗಿ -ENOENT;

        ಆದರೆ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ glob_ಮೊದಲ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **v = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        r = ಸುರಕ್ಷಿತ_glob(ಮಾರ್ಗ, GLOB_NOSORT|GLOB_BRACE, &v);
        ಆದರೆ (r == -ENOENT) {
                ಆದರೆ (ret)
                        *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ false;
        }
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಪ್ರತಿಜ್ಞಾ(!strv_isempty(v));

        ಆದರೆ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ glob_extend(ಅಕ್ಷರ ***strv, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags) {
        ಅಕ್ಷರ **v;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        r = ಸುರಕ್ಷಿತ_glob(ಮಾರ್ಗ, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

ಪೂರ್ಣಾಂಕ glob_non_glob_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(ಮಾರ್ಗ, GLOB_CHARS);

        ಆದರೆ (ಮಾರ್ಗ[n] != '\0')
                ಆಗ (n > 0 && ಮಾರ್ಗ[n-1] != '/')
                        n--;

        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ -ENOENT;

        ಅಕ್ಷರ *ans = strndup(ಮಾರ್ಗ, n);
        ಆದರೆ (!ans)
                ಹಿಂತಿರುಗಿ -ENOMEM;
        *ret = ans;
        ಹಿಂತಿರುಗಿ 0;
}

bool string_is_glob(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        /* Check if a string contains any glob patterns. */
        ಹಿಂತಿರುಗಿ !!strpbrk(p, GLOB_CHARS);
}
