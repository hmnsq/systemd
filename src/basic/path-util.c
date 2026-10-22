/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fnmatch.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "glob-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

bool is_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಆದರೆ (!p) /* A NULL pointer is definitely not a path */
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ strchr(p, '/');
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ವಿಭಜಿಸು_and_make_absolute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ ***ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        l = strv_ವಿಭಜಿಸು(p, ":");
        ಆದರೆ (!l)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        r = ಮಾರ್ಗ_strv_make_absolute_cwd(l);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = TAKE_PTR(l);
        ಹಿಂತಿರುಗಿ r;
}

ಅಕ್ಷರ* ಮಾರ್ಗ_make_absolute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ಆದರೆ (ಮಾರ್ಗ_is_absolute(p) || isempty(ಪೂರ್ವಪ್ರತ್ಯಯ))
                ಹಿಂತಿರುಗಿ strdup(p);

        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_join(ಪೂರ್ವಪ್ರತ್ಯಯ, p);
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_getcwd(ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *cwd = ಖಾಲಿ;

        cwd = get_current_dir_name();
        ಆದರೆ (!cwd)
                ಹಿಂತಿರುಗಿ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ಆದರೆ (cwd[0] != '/')
                ಹಿಂತಿರುಗಿ -ENOMEDIUM;

        ಆದರೆ (ret)
                *ret = TAKE_PTR(cwd);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_make_absolute_cwd(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ *c;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ಆದರೆ (ಮಾರ್ಗ_is_absolute(p))
                c = strdup(p);
        ಇಲ್ಲದಿದ್ದರೆ {
                _cleanup_free_ ಅಕ್ಷರ *cwd = ಖಾಲಿ;

                r = ಸುರಕ್ಷಿತ_getcwd(&cwd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                c = ಮಾರ್ಗ_join(cwd, p);
        }
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = c;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_make_relative(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *from, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *to, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಫಲಿತಾಂಶ = ಖಾಲಿ;
        ಚಿಹ್ನೆರಹಿತ n_parents;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, *t;
        ಪೂರ್ಣಾಂಕ r, k;
        ಅಕ್ಷರ *p;

        ಪ್ರತಿಜ್ಞಾ(from);
        ಪ್ರತಿಜ್ಞಾ(to);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        ಆದರೆ (!ಮಾರ್ಗ_is_absolute(from) || !ಮಾರ್ಗ_is_absolute(to))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಗಾಗಿ (;;) {
                r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&from, true, &f);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                k = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&to, true, &t);
                ಆದರೆ (k < 0)
                        ಹಿಂತಿರುಗಿ k;

                ಆದರೆ (r == 0) {
                        /* end of 'from' */
                        ಆದರೆ (k == 0) {
                                /* from and to are equivalent. */
                                ಫಲಿತಾಂಶ = strdup(".");
                                ಆದರೆ (!ಫಲಿತಾಂಶ)
                                        ಹಿಂತಿರುಗಿ -ENOMEM;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                /* 'to' is inside of 'from'. */
                                r = ಮಾರ್ಗ_simplify_alloc(t, &ಫಲಿತಾಂಶ);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                ಆದರೆ (!ಮಾರ್ಗ_is_valid(ಫಲಿತಾಂಶ))
                                        ಹಿಂತಿರುಗಿ -EINVAL;
                        }

                        *ret = TAKE_PTR(ಫಲಿತಾಂಶ);
                        ಹಿಂತಿರುಗಿ 0;
                }

                ಆದರೆ (r != k || !strneq(f, t, r))
                        ನಿಲ್ಲಿಸು;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        ಗಾಗಿ (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&from, false, &f);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;
        }

        ಆದರೆ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಫಲಿತಾಂಶ = new(ಅಕ್ಷರ, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ಆದರೆ (!ಫಲಿತಾಂಶ)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಗಾಗಿ (p = ಫಲಿತಾಂಶ; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ಆದರೆ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(ಫಲಿತಾಂಶ);
                ಹಿಂತಿರುಗಿ 0;
        }

        strcpy(p, t);

        ಮಾರ್ಗ_simplify(ಫಲಿತಾಂಶ);

        ಆದರೆ (!ಮಾರ್ಗ_is_valid(ಫಲಿತಾಂಶ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = TAKE_PTR(ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_make_relative_ಪೋಷಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *from_ಮಗು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *to, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *from = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(from_ಮಗು);
        ಪ್ರತಿಜ್ಞಾ(to);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Similar to path_make_relative(), but provides the relative path from the parent directory of
         * 'from_child'. This may be useful when creating relative symlink.
         *
         * E.g.
         * - from = "/path/to/aaa", to = "/path/to/bbb"
         *      path_make_relative(from, to) = "../bbb"
         *      path_make_relative_parent(from, to) = "bbb"
         *
         * - from = "/path/to/aaa/bbb", to = "/path/to/ccc/ddd"
         *      path_make_relative(from, to) = "../../ccc/ddd"
         *      path_make_relative_parent(from, to) = "../ccc/ddd"
         */

        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_directory(from_ಮಗು, &from);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_make_relative(from, to, ret);
}

ಅಕ್ಷರ* ಮಾರ್ಗ_startswith_strv(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *strv) {
        ಪ್ರತಿಜ್ಞಾ(p);

        STRV_FOREACH(s, strv) {
                ಅಕ್ಷರ *t;

                t = ಮಾರ್ಗ_startswith(p, *s);
                ಆದರೆ (t)
                        ಹಿಂತಿರುಗಿ t;
        }

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_strv_make_absolute_cwd(ಅಕ್ಷರ **l) {
        ಪೂರ್ಣಾಂಕ r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                ಅಕ್ಷರ *t;

                r = ಮಾರ್ಗ_make_absolute_cwd(*s, &t);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಮಾರ್ಗ_simplify(t);
                free_and_replace(*s, t);
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ** ಮಾರ್ಗ_strv_resolve(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು) {
        ಚಿಹ್ನೆರಹಿತ k = 0;
        bool enomem = false;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (strv_isempty(l))
                ಹಿಂತಿರುಗಿ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ ಅಕ್ಷರ *orig = ಖಾಲಿ;
                ಅಕ್ಷರ *t, *u;

                ಆದರೆ (ಬೇರು) {
                        ಆದರೆ (!ಮಾರ್ಗ_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                ಬಿಡುಗಡೆ(*s);
                                ಮುಂದುವರಿ;
                        }

                        orig = *s;
                        t = ಮಾರ್ಗ_join(ಬೇರು, orig);
                        ಆದರೆ (!t) {
                                enomem = true;
                                ಮುಂದುವರಿ;
                        }
                } ಇಲ್ಲದಿದ್ದರೆ
                        t = *s;

                r = chase(t, ಬೇರು, 0, &u, ಖಾಲಿ);
                ಆದರೆ (r == -ENOENT) {
                        ಆದರೆ (ಬೇರು) {
                                u = TAKE_PTR(orig);
                                ಬಿಡುಗಡೆ(t);
                        } ಇಲ್ಲದಿದ್ದರೆ
                                u = t;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < 0) {
                        ಬಿಡುಗಡೆ(t);

                        ಆದರೆ (r == -ENOMEM)
                                enomem = true;

                        ಮುಂದುವರಿ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬೇರು) {
                        ಅಕ್ಷರ *x;

                        ಬಿಡುಗಡೆ(t);
                        x = ಮಾರ್ಗ_startswith(u, ಬೇರು);
                        ಆದರೆ (x) {
                                /* restore the slash if it was lost */
                                ಆದರೆ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                ಬಿಡುಗಡೆ(u);
                                ಆದರೆ (!t) {
                                        enomem = true;
                                        ಮುಂದುವರಿ;
                                }
                                u = t;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ಇಲ್ಲದಿದ್ದರೆ
                        ಬಿಡುಗಡೆ(t);

                l[k++] = u;
        }

        l[k] = ಖಾಲಿ;

        ಆದರೆ (enomem)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ l;
}

ಅಕ್ಷರ** ಮಾರ್ಗ_strv_resolve_uniq(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು) {

        ಆದರೆ (strv_isempty(l))
                ಹಿಂತಿರುಗಿ l;

        ಆದರೆ (!ಮಾರ್ಗ_strv_resolve(l, ಬೇರು))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ strv_uniq(l);
}

ಅಕ್ಷರ* skip_leading_slash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಹಿಂತಿರುಗಿ skip_leading_chars(p, "/");
}

ಅಕ್ಷರ* ಮಾರ್ಗ_simplify_ತುಂಬಿದ(ಅಕ್ಷರ *ಮಾರ್ಗ, ಮಾರ್ಗ_Simplify_Flags flags) {
        bool ಕೂಡಿಸು_slash = false, keep_trailing_slash, absolute, beginning = true;
        ಅಕ್ಷರ *f = ಮಾರ್ಗ;
        ಪೂರ್ಣಾಂಕ r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        ಆದರೆ (isempty(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;

        keep_trailing_slash = FLAGS_ಗಣ(flags, ಮಾರ್ಗ_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(ಮಾರ್ಗ, "/");

        absolute = ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ);
        f += absolute;  /* Keep leading /, if present. */

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = f;;) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

                r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&p, true, &e);
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (r > 0 && absolute && beginning && ಮಾರ್ಗ_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        ಮುಂದುವರಿ;

                beginning = false;

                ಆದರೆ (ಕೂಡಿಸು_slash)
                        *f++ = '/';

                ಆದರೆ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p) + 1);
                        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;
                }

                memmove(f, e, r);
                f += r;

                ಕೂಡಿಸು_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ಆದರೆ (f == ಮಾರ್ಗ)
                *f++ = '.';

        ಆದರೆ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_simplify_alloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!ಮಾರ್ಗ) {
                *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಅಕ್ಷರ *t = strdup(ಮಾರ್ಗ);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = ಮಾರ್ಗ_simplify(t);
        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ* ಮಾರ್ಗ_startswith_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *original_ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ_ಆರಂಭ_With_Flags flags) {
        ಪ್ರತಿಜ್ಞಾ(original_ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = original_ಮಾರ್ಗ;

        ಆದರೆ ((ಮಾರ್ಗ[0] == '/') != (ಪೂರ್ವಪ್ರತ್ಯಯ[0] == '/'))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (;;) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *q;
                ಪೂರ್ಣಾಂಕ m, n;

                m = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&ಮಾರ್ಗ, !FLAGS_ಗಣ(flags, ಮಾರ್ಗ_STARTSWITH_REFUSE_DOT_DOT), &p);
                ಆದರೆ (m < 0)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                n = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&ಪೂರ್ವಪ್ರತ್ಯಯ, !FLAGS_ಗಣ(flags, ಮಾರ್ಗ_STARTSWITH_REFUSE_DOT_DOT), &q);
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                ಆದರೆ (n == 0) {
                        ಆದರೆ (!p)
                                p = ಮಾರ್ಗ;

                        ಆದರೆ (FLAGS_ಗಣ(flags, ಮಾರ್ಗ_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ಆದರೆ (p <= original_ಮಾರ್ಗ)
                                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                                p--;

                                ಆದರೆ (*p != '/')
                                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                        }

                        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) p;
                }

                ಆದರೆ (m != n)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                ಆದರೆ (!strneq(p, q, m))
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_compare(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
        ಪೂರ್ಣಾಂಕ r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(ಮಾರ್ಗ_is_absolute(a), ಮಾರ್ಗ_is_absolute(b));
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (;;) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *aa, *bb;
                ಪೂರ್ಣಾಂಕ j, k;

                j = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&a, true, &aa);
                k = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&b, true, &bb);

                ಆದರೆ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ಆದರೆ (r != 0)
                                ಹಿಂತಿರುಗಿ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ಹಿಂತಿರುಗಿ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ಆದರೆ (j == 0) {
                        ಆದರೆ (k == 0)
                                ಹಿಂತಿರುಗಿ 0;
                        ಹಿಂತಿರುಗಿ -1;
                }
                ಆದರೆ (k == 0)
                        ಹಿಂತಿರುಗಿ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ಆದರೆ (r != 0)
                        ಹಿಂತಿರುಗಿ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ಆದರೆ (r != 0)
                        ಹಿಂತಿರುಗಿ r;
        }
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_compare_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
        _cleanup_free_ ಅಕ್ಷರ *fa = ಖಾಲಿ, *fb = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        j = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(a, &fa);
        k = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ಆದರೆ (j < 0)
                ಹಿಂತಿರುಗಿ strcmp(a, b);

        ಹಿಂತಿರುಗಿ strcmp(fa, fb);
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_equal_or_inode_same_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, ಪೂರ್ಣಾಂಕ flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ಆದರೆ (ಮಾರ್ಗ_equal(a, b))
                ಹಿಂತಿರುಗಿ 1;

        ಆದರೆ (!a || !b)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ inode_same(a, b, flags);
}

ಅಕ್ಷರ* ಮಾರ್ಗ_extend_ಆಂತರಿಕ(ಅಕ್ಷರ **x, ...) {
        size_t sz, old_sz;
        ಅಕ್ಷರ *q, *nx;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        va_list ap;
        bool slash;

        ಸೂಚಕ_MAY_BE_NULL(x);

        /* Joins all listed strings until the sentinel and places a "/" between them unless the strings
         * end/begin already with one so that it is unnecessary. Note that slashes which are already
         * duplicate won't be removed. The string returned is hence always equal to or longer than the sum of
         * the lengths of the individual strings.
         *
         * The first argument may be an already allocated string that is extended via realloc() if
         * non-NULL. path_extend() and path_join() are macro wrappers around this function, making use of the
         * first parameter to distinguish the two operations.
         *
         * Note: any listed empty string is simply skipped. This can be useful for concatenating strings of
         * which some are optional.
         *
         * Examples:
         *
         * path_join("foo", "bar") → "foo/bar"
         * path_join("foo/", "bar") → "foo/bar"
         * path_join("", "foo", "", "bar", "") → "foo/bar" */

        sz = old_sz = x ? strlen_ptr(*x) : 0;
        va_start(ap, x);
        ಆಗ ((p = va_arg(ap, ಅಕ್ಷರ*)) != ಸೂಚಕ_MAX) {
                size_t ಕೂಡಿಸು;

                ಆದರೆ (isempty(p))
                        ಮುಂದುವರಿ;

                ಕೂಡಿಸು = 1 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p);
                ಆದರೆ (sz > SIZE_MAX - ಕೂಡಿಸು) { /* overflow check */
                        va_end(ap);
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }

                sz += ಕೂಡಿಸು;
        }
        va_end(ap);

        nx = realloc(x ? *x : ಖಾಲಿ, GREEDY_ALLOC_ಸುತ್ತು_ಮೇಲೆ(sz+1));
        ಆದರೆ (!nx)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಆದರೆ (x)
                *x = nx;

        ಆದರೆ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ಇಲ್ಲದಿದ್ದರೆ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ಆಗ ((p = va_arg(ap, ಅಕ್ಷರ*)) != ಸೂಚಕ_MAX) {
                ಆದರೆ (isempty(p))
                        ಮುಂದುವರಿ;

                ಆದರೆ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ಹಿಂತಿರುಗಿ nx;
}

ಪೂರ್ಣಾಂಕ open_and_check_executable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret_fd) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *resolved = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        ಆದರೆ (ಬೇರು) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, ಬೇರು, CHASE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು, &resolved, &fd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                name = resolved;
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_ಮಾರ್ಗ|O_CLOEXEC);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        r = fd_verify_regular(fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = access_fd(fd, X_OK);
        ಆದರೆ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ) {
                ಆದರೆ (resolved)
                        *ret_ಮಾರ್ಗ = TAKE_PTR(resolved);
                ಇಲ್ಲದಿದ್ದರೆ {
                        r = ಮಾರ್ಗ_make_absolute_cwd(name, ret_ಮಾರ್ಗ);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        ಮಾರ್ಗ_simplify(*ret_ಮಾರ್ಗ);
                }
        }

        ಆದರೆ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_executable_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *exec_ಹುಡುಕು_ಮಾರ್ಗ,
                bool use_ಮಾರ್ಗ_envvar,
                ಅಕ್ಷರ **ret_filename,
                ಪೂರ್ಣಾಂಕ *ret_fd) {

        ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_error = -ENOENT, r = 0;

        ಪ್ರತಿಜ್ಞಾ(name);

        ಆದರೆ (is_ಮಾರ್ಗ(name))
                ಹಿಂತಿರುಗಿ open_and_check_executable(name, ಬೇರು, ret_filename, ret_fd);

        ಆದರೆ (exec_ಹುಡುಕು_ಮಾರ್ಗ) {
                STRV_FOREACH(element, exec_ಹುಡುಕು_ಮಾರ್ಗ) {
                        _cleanup_free_ ಅಕ್ಷರ *ತುಂಬಿದ_ಮಾರ್ಗ = ಖಾಲಿ;

                        ಆದರೆ (!ಮಾರ್ಗ_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                ಮುಂದುವರಿ;
                        }

                        ತುಂಬಿದ_ಮಾರ್ಗ = ಮಾರ್ಗ_join(*element, name);
                        ಆದರೆ (!ತುಂಬಿದ_ಮಾರ್ಗ)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        r = open_and_check_executable(ತುಂಬಿದ_ಮಾರ್ಗ, ಬೇರು, ret_filename, ret_fd);
                        ಆದರೆ (r >= 0)
                                ಹಿಂತಿರುಗಿ 0;
                        ಆದರೆ (r != -EACCES)
                                ಕೊನೆಯ_error = r;
                }
                ಹಿಂತಿರುಗಿ ಕೊನೆಯ_error;
        }

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಖಾಲಿ;

        ಆದರೆ (use_ಮಾರ್ಗ_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ಆದರೆ (!p)
                p = default_ಮಾರ್ಗ();

        /* Resolve a single-component name to a full path */
        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *element = ಖಾಲಿ;

                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&p, &element, ":", ಹೊರತೆಗೆ_RELAX|ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (!ಮಾರ್ಗ_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        ಮುಂದುವರಿ;
                }

                ಆದರೆ (!ಮಾರ್ಗ_extend(&element, name))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                r = open_and_check_executable(element, ಬೇರು, ret_filename, ret_fd);
                ಆದರೆ (r >= 0) /* Found it! */
                        ಹಿಂತಿರುಗಿ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ಆದರೆ (r != -EACCES)
                        ಕೊನೆಯ_error = r;
        }

        ಹಿಂತಿರುಗಿ ಕೊನೆಯ_error;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ executable_is_good(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *executable) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ, *d = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = ಕಂಡುಹಿಡಿ_executable(executable, &p);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ಆದರೆ (r == -EINVAL) /* not a symlink */
                ಹಿಂತಿರುಗಿ 1;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ !ಮಾರ್ಗ_ಒಳ_ಗಣ(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

ಪೂರ್ಣಾಂಕ fsck_exists(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ executable_is_good("fsck");
}

ಪೂರ್ಣಾಂಕ fsck_exists_for_fstype(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *checker;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fstype);

        ಆದರೆ (streq(fstype, "auto"))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = fsck_exists();
        ಆದರೆ (r <= 0)
                ಹಿಂತಿರುಗಿ r;

        checker = strjoina("fsck.", fstype);
        ಹಿಂತಿರುಗಿ executable_is_good(checker);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* skip_slash_or_dot(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಸೂಚಕ_MAY_BE_NULL(p);

        ಗಾಗಿ (; !isempty(p); p++) {
                ಆದರೆ (*p == '/')
                        ಮುಂದುವರಿ;
                ಆದರೆ (startswith(p, "./")) {
                        p++;
                        ಮುಂದುವರಿ;
                }
                ನಿಲ್ಲಿಸು;
        }
        ಹಿಂತಿರುಗಿ p;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, bool accept_dot_dot, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q, *ಮೊದಲ, *ಕೊನೆ_ಮೊದಲ, *ಮುಂದಿನ_2;
        size_t len;

        ಪ್ರತಿಜ್ಞಾ(p);

        /* When a path is input, then returns the pointer to the first component and its length, and
         * move the input pointer to the next component or nul. This skips both over any '/'
         * immediately *before* and *after* the first component before returning.
         *
         * Examples
         *   Input:  p: "//.//aaa///bbbbb/cc"
         *   Output: p: "bbbbb///cc"
         *           ret: "aaa///bbbbb/cc"
         *           return value: 3 (== strlen("aaa"))
         *
         *   Input:  p: "aaa//"
         *   Output: p: (pointer to NUL)
         *           ret: "aaa//"
         *           return value: 3 (== strlen("aaa"))
         *
         *   Input:  p: "/", ".", ""
         *   Output: p: (pointer to NUL)
         *           ret: NULL
         *           return value: 0
         *
         *   Input:  p: NULL
         *   Output: p: NULL
         *           ret: NULL
         *           return value: 0
         *
         *   Input:  p: "(too long component)"
         *   Output: return value: -EINVAL
         *
         *   (when accept_dot_dot is false)
         *   Input:  p: "//..//aaa///bbbbb/cc"
         *   Output: return value: -EINVAL
         */

        q = *p;

        ಮೊದಲ = skip_slash_or_dot(q);
        ಆದರೆ (isempty(ಮೊದಲ)) {
                *p = ಮೊದಲ;
                ಆದರೆ (ret)
                        *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ (streq(ಮೊದಲ, ".")) {
                *p = ಮೊದಲ + 1;
                ಆದರೆ (ret)
                        *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಕೊನೆ_ಮೊದಲ = strchrnul(ಮೊದಲ, '/');
        len = ಕೊನೆ_ಮೊದಲ - ಮೊದಲ;

        ಆದರೆ (len > NAME_MAX)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (!accept_dot_dot && len == 2 && ಮೊದಲ[0] == '.' && ಮೊದಲ[1] == '.')
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಮುಂದಿನ_2 = skip_slash_or_dot(ಕೊನೆ_ಮೊದಲ);

        *p = ಮುಂದಿನ_2 + streq(ಮುಂದಿನ_2, ".");
        ಆದರೆ (ret)
                *ret = ಮೊದಲ;
        ಹಿಂತಿರುಗಿ len;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* skip_slash_or_dot_backward(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q) {
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(!q || q >= ಮಾರ್ಗ);

        ಗಾಗಿ (; q; q = PTR_SUB1(q, ಮಾರ್ಗ)) {
                ಆದರೆ (*q == '/')
                        ಮುಂದುವರಿ;
                ಆದರೆ (q > ಮಾರ್ಗ && strneq(q - 1, "/.", 2))
                        ಮುಂದುವರಿ;
                ಆದರೆ (q == ಮಾರ್ಗ && *q == '.')
                        ಮುಂದುವರಿ;
                ನಿಲ್ಲಿಸು;
        }
        ಹಿಂತಿರುಗಿ q;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_component(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool accept_dot_dot, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮುಂದಿನ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q, *ಕೊನೆಯ_ಕೊನೆ, *ಕೊನೆಯ_begin;
        size_t len;

        ಸೂಚಕ_MAY_BE_NULL(ಮುಂದಿನ_2);
        ಸೂಚಕ_MAY_BE_NULL(ret);

        /* Similar to path_find_first_component(), but search components from the end.
        *
        * Examples
        *   Input:  path: "//.//aaa///bbbbb/cc//././"
        *           next: NULL
        *   Output: next: "/cc//././"
        *           ret: "cc//././"
        *           return value: 2 (== strlen("cc"))
        *
        *   Input:  path: "//.//aaa///bbbbb/cc//././"
        *           next: "/cc//././"
        *   Output: next: "///bbbbb/cc//././"
        *           ret: "bbbbb/cc//././"
        *           return value: 5 (== strlen("bbbbb"))
        *
        *   Input:  path: "//.//aaa///bbbbb/cc//././"
        *           next: "///bbbbb/cc//././"
        *   Output: next: "//.//aaa///bbbbb/cc//././" (next == path)
        *           ret: "aaa///bbbbb/cc//././"
        *           return value: 3 (== strlen("aaa"))
        *
        *   Input:  path: "/", ".", "", or NULL
        *   Output: next: equivalent to path
        *           ret: NULL
        *           return value: 0
        *
        *   Input:  path: "(too long component)"
        *   Output: return value: -EINVAL
        *
        *   (when accept_dot_dot is false)
        *   Input:  path: "//..//aaa///bbbbb/cc/..//"
        *   Output: return value: -EINVAL
        */

        ಆದರೆ (isempty(ಮಾರ್ಗ)) {
                ಆದರೆ (ಮುಂದಿನ_2)
                        *ಮುಂದಿನ_2 = ಮಾರ್ಗ;
                ಆದರೆ (ret)
                        *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (ಮುಂದಿನ_2 && *ಮುಂದಿನ_2) {
                ಆದರೆ (*ಮುಂದಿನ_2 < ಮಾರ್ಗ || *ಮುಂದಿನ_2 > ಮಾರ್ಗ + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ))
                        ಹಿಂತಿರುಗಿ -EINVAL;
                ಆದರೆ (*ಮುಂದಿನ_2 == ಮಾರ್ಗ) {
                        ಆದರೆ (ret)
                                *ret = ಖಾಲಿ;
                        ಹಿಂತಿರುಗಿ 0;
                }
                ಆದರೆ (!ಒಳ_ಗಣ(**ಮುಂದಿನ_2, '\0', '/'))
                        ಹಿಂತಿರುಗಿ -EINVAL;
                q = *ಮುಂದಿನ_2 - 1;
        } ಇಲ್ಲದಿದ್ದರೆ
                q = ಮಾರ್ಗ + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ) - 1;

        q = skip_slash_or_dot_backward(ಮಾರ್ಗ, q);
        ಆದರೆ (!q) { /* the root directory, "." or "./" */
                ಆದರೆ (ಮುಂದಿನ_2)
                        *ಮುಂದಿನ_2 = ಮಾರ್ಗ;
                ಆದರೆ (ret)
                        *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಕೊನೆಯ_ಕೊನೆ = q + 1;

        ಆಗ (q && *q != '/')
                q = PTR_SUB1(q, ಮಾರ್ಗ);

        ಕೊನೆಯ_begin = q ? q + 1 : ಮಾರ್ಗ;
        len = ಕೊನೆಯ_ಕೊನೆ - ಕೊನೆಯ_begin;

        ಆದರೆ (len > NAME_MAX)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (!accept_dot_dot && len == 2 && strneq(ಕೊನೆಯ_begin, "..", 2))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ಮುಂದಿನ_2) {
                q = skip_slash_or_dot_backward(ಮಾರ್ಗ, q);
                *ಮುಂದಿನ_2 = q ? q + 1 : ಮಾರ್ಗ;
        }

        ಆದರೆ (ret)
                *ret = ಕೊನೆಯ_begin;
        ಹಿಂತಿರುಗಿ len;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೊನೆಯ_ಮಾರ್ಗ_component(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {

        /* Finds the last component of the path, preserving the optional trailing slash that signifies a directory.
         *
         *    a/b/c → c
         *    a/b/c/ → c/
         *    x → x
         *    x/ → x/
         *    /y → y
         *    /y/ → y/
         *    / → /
         *    // → /
         *    /foo/a → a
         *    /foo/a/ → a/
         *
         *    Also, the empty string is mapped to itself.
         *
         * This is different than basename(), which returns "" when a trailing slash is present.
         *
         * This always succeeds (except if you pass NULL in which case it returns NULL, too).
         */

        ಚಿಹ್ನೆರಹಿತ l, k;

        ಆದರೆ (!ಮಾರ್ಗ)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        l = k = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ);
        ಆದರೆ (l == 0) /* special case — an empty string */
                ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;

        ಆಗ (k > 0 && ಮಾರ್ಗ[k-1] == '/')
                k--;

        ಆದರೆ (k == 0) /* the root directory */
                ಹಿಂತಿರುಗಿ ಮಾರ್ಗ + l - 1;

        ಆಗ (k > 0 && ಮಾರ್ಗ[k-1] != '/')
                k--;

        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ + k;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ವಿಭಜಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_dir, ಅಕ್ಷರ **ret_filename) {
        _cleanup_free_ ಅಕ್ಷರ *d = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *c, *ಮುಂದಿನ_2 = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಸೂಚಕ_MAY_BE_NULL(ಮಾರ್ಗ);

        /* Split the path into dir prefix/filename pair. Returns:
         *
         * -EINVAL        → if the path is not valid
         * -EADDRNOTAVAIL → if the path refers to the uppermost directory in hierarchy (i.e. has neither
         *                  dir prefix nor filename - the root dir itself or ".")
         * -EDESTADDRREQ  → if only a filename was passed, and caller only specifies ret_dir
         * -ENOMEM        → no memory
         *
         * Returns >= 0 on success. If the input path has a trailing slash, returns O_DIRECTORY, to
         * indicate the referenced file must be a directory.
         *
         * This function guarantees to return a fully valid filename, i.e. one that passes
         * filename_is_valid() – this means "." and ".." are not accepted. */

        ಆದರೆ (isempty(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_component(ಮಾರ್ಗ, /* accept_dot_dot = */ false, &ಮುಂದಿನ_2, &c);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0) /* root directory or "." */
                ಹಿಂತಿರುಗಿ -EADDRNOTAVAIL;

        ಆದರೆ (ret_dir) {
                ಆದರೆ (ಮುಂದಿನ_2 == ಮಾರ್ಗ) {
                        ಆದರೆ (*ಮಾರ್ಗ != '/') { /* filename only */
                                ಆದರೆ (!ret_filename)
                                        ಹಿಂತಿರುಗಿ -EDESTADDRREQ;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                d = strdup("/");
                                ಆದರೆ (!d)
                                        ಹಿಂತಿರುಗಿ -ENOMEM;
                        }
                } ಇಲ್ಲದಿದ್ದರೆ {
                        d = strndup(ಮಾರ್ಗ, ಮುಂದಿನ_2 - ಮಾರ್ಗ);
                        ಆದರೆ (!d)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        ಮಾರ್ಗ_simplify(d);

                        ಆದರೆ (!ಮಾರ್ಗ_is_valid(d))
                                ಹಿಂತಿರುಗಿ -EINVAL;
                }

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಮಾರ್ಗ_is_valid(ಮಾರ್ಗ))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret_filename) {
                ಅಕ್ಷರ *fn = strndup(c, r);
                ಆದರೆ (!fn)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                *ret_filename = fn;
        }

        ಆದರೆ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ false;

        e = strchrnul(p, '/');
        ಆದರೆ (*e != 0)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

bool filename_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {

        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ filename_part_is_valid(p);
}

bool ಮಾರ್ಗ_is_valid_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, bool accept_dot_dot) {
        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ false;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = p;;) {
                ಪೂರ್ಣಾಂಕ r;

                r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&e, accept_dot_dot, ಖಾಲಿ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ಹಿಂತಿರುಗಿ false;
                ಆದರೆ (*e == 0)           /* End of string? Yay! */
                        ಹಿಂತಿರುಗಿ true;
        }
}

bool ಮಾರ್ಗ_is_normalized(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಆದರೆ (!ಮಾರ್ಗ_is_ಸುರಕ್ಷಿತ(p))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(p, "/./"))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(p, "//"))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ file_ಒಳ_same_dir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *b = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(filename);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ಆದರೆ (ಮಾರ್ಗ_is_absolute(filename))
                b = strdup(filename);
        ಇಲ್ಲದಿದ್ದರೆ {
                _cleanup_free_ ಅಕ್ಷರ *dn = ಖಾಲಿ;

                r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_directory(ಮಾರ್ಗ, &dn);
                ಆದರೆ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಇಲ್ಲದಿದ್ದರೆ
                        b = ಮಾರ್ಗ_join(dn, filename);
        }
        ಆದರೆ (!b)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = TAKE_PTR(b);
        ಹಿಂತಿರುಗಿ 0;
}

bool hidden_or_backup_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename) {
        ಪ್ರತಿಜ್ಞಾ(filename);

        ಆದರೆ (filename[0] == '.' ||
            STR_ಒಳ_ಗಣ(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ಹಿಂತಿರುಗಿ true;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dot = strrchr(filename, '.');
        ಆದರೆ (!dot)
                ಹಿಂತಿರುಗಿ false;

        /* Please, let's not add more entries to the list below. If external projects think it's a good idea
         * to come up with always new suffixes and that everybody else should just adjust to that, then it
         * really should be on them. Hence, in future, let's not add any more entries. Instead, let's ask
         * those packages to instead adopt one of the generic suffixes/prefixes for hidden files or backups,
         * possibly augmented with an additional string. Specifically: there's now:
         *
         *    The generic suffixes "~" and ".bak" for backup files
         *    The generic prefix "." for hidden files
         *
         * Thus, if a new package manager "foopkg" wants its own set of ".foopkg-new", ".foopkg-old",
         * ".foopkg-dist" or so registered, let's refuse that and ask them to use ".foopkg.new",
         * ".foopkg.old" or ".foopkg~" instead.
         */

        ಹಿಂತಿರುಗಿ STR_ಒಳ_ಗಣ(dot + 1,
                          "ignore",
                          "rpmnew",
                          "rpmsave",
                          "rpmorig",
                          "dpkg-old",
                          "dpkg-new",
                          "dpkg-tmp",
                          "dpkg-dist",
                          "dpkg-bak",
                          "dpkg-backup",
                          "dpkg-remove",
                          "ucf-new",
                          "ucf-old",
                          "ucf-dist",
                          "swp",
                          "bak",
                          "old",
                          "new");
}

bool is_device_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಮಾರ್ಗ_STARTSWITH_ಗಣ(ASSERT_PTR(ಮಾರ್ಗ), "/dev/", "/sys/");
        ಹಿಂತಿರುಗಿ !isempty(p);
}

bool valid_device_ಗಂಟು_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ಆದರೆ (!ಮಾರ್ಗ_STARTSWITH_ಗಣ(ಮಾರ್ಗ, "/dev/", "/run/systemd/inaccessible/"))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (endswith(ಮಾರ್ಗ, "/")) /* can't be a device node if it ends in a slash */
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_is_normalized(ಮಾರ್ಗ);
}

bool valid_device_allow_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ಆದರೆ (STARTSWITH_ಗಣ(ಮಾರ್ಗ, "block-", "char-"))
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ valid_device_ಗಂಟು_ಮಾರ್ಗ(ಮಾರ್ಗ);
}

bool dot_or_dot_dot(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಆದರೆ (!ಮಾರ್ಗ)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (ಮಾರ್ಗ[0] != '.')
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (ಮಾರ್ಗ[1] == 0)
                ಹಿಂತಿರುಗಿ true;
        ಆದರೆ (ಮಾರ್ಗ[1] != '.')
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ[2] == 0;
}

bool ಮಾರ್ಗ_ಸೂಚಿಸುತ್ತದೆ_directory(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ಆದರೆ (!ಮಾರ್ಗ)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (dot_or_dot_dot(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ ENDSWITH_ಗಣ(ಮಾರ್ಗ, "/", "/.", "/..");
}

bool ಖಾಲಿಗಳು_or_ಬೇರು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ಆದರೆ (isempty(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_equal(ಮಾರ್ಗ, "/");
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಹಿಂತಿರುಗಿ isempty(ಮಾರ್ಗ) ? "/" : ಮಾರ್ಗ;
}

ಪೂರ್ಣಾಂಕ ಖಾಲಿಗಳು_or_ಬೇರು_harder_ಗಮ್ಯ_null(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮಾರ್ಗ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* This nullifies the input path when the path is empty or points to "/". */

        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(*ಮಾರ್ಗ)) {
                *ಮಾರ್ಗ = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        r = ಮಾರ್ಗ_is_ಬೇರು(*ಮಾರ್ಗ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r > 0)
                *ಮಾರ್ಗ = ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ 0;
}

bool ಮಾರ್ಗ_strv_ಒಳಗೊಂಡಿದೆ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        STRV_FOREACH(i, l)
                ಆದರೆ (ಮಾರ್ಗ_equal(*i, ಮಾರ್ಗ))
                        ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ false;
}

bool prefixed_ಮಾರ್ಗ_strv_ಒಳಗೊಂಡಿದೆ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        STRV_FOREACH(i, l) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *j = *i;

                ಆದರೆ (*j == '-')
                        j++;
                ಆದರೆ (*j == '+')
                        j++;

                ಆದರೆ (ಮಾರ್ಗ_equal(j, ಮಾರ್ಗ))
                        ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_glob_can_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂನೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ನಮೂನೆ);
        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a = ನಮೂನೆ, *b = ಪೂರ್ವಪ್ರತ್ಯಯ;;) {
                _cleanup_free_ ಅಕ್ಷರ *g = ಖಾಲಿ, *h = ಖಾಲಿ;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *q;
                ಪೂರ್ಣಾಂಕ r, s;

                r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&a, /* accept_dot_dot= */ false, &p);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                s = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&b, /* accept_dot_dot= */ false, &q);
                ಆದರೆ (s < 0)
                        ಹಿಂತಿರುಗಿ s;

                ಆದರೆ (s == 0) {
                        /* The pattern matches the prefix. */
                        ಆದರೆ (ret) {
                                ಅಕ್ಷರ *t;

                                t = ಮಾರ್ಗ_join(ಪೂರ್ವಪ್ರತ್ಯಯ, p);
                                ಆದರೆ (!t)
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                *ret = t;
                        }
                        ಹಿಂತಿರುಗಿ true;
                }

                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (r == s && strneq(p, q, r))
                        ಮುಂದುವರಿ; /* common component. Check next. */

                g = strndup(p, r);
                ಆದರೆ (!g)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (!string_is_glob(g))
                        ನಿಲ್ಲಿಸು;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ಆದರೆ (!h)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                r = fnmatch(g, h, 0);
                ಆದರೆ (r == FNM_NOMATCH)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (r != 0) /* Failure to process pattern? */
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ಆದರೆ (ret)
                *ret = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ false;
}

#ಆದರೆ HAVE_ವಿಭಜಿಸು_BIN
ಸ್ಥಿರ bool dir_is_ವಿಭಜಿಸು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
        ಪೂರ್ಣಾಂಕ r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ಆದರೆ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ಹಿಂತಿರುಗಿ true;
        }
        ಹಿಂತಿರುಗಿ r == 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* default_ಮಾರ್ಗ(ಶೂನ್ಯ) {
#ಆದರೆ HAVE_ವಿಭಜಿಸು_BIN
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_ಮಾರ್ಗ_2 = ಖಾಲಿ;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ಆದರೆ (default_ಮಾರ್ಗ_2)
                ಹಿಂತಿರುಗಿ default_ಮಾರ್ಗ_2;

        ಆದರೆ (dir_is_ವಿಭಜಿಸು("/usr/sbin", "/usr/bin"))
                ಹಿಂತಿರುಗಿ (default_ಮಾರ್ಗ_2 = DEFAULT_ಮಾರ್ಗ_WITH_ತುಂಬಿದ_SBIN);  /* a */
        ಆದರೆ (dir_is_ವಿಭಜಿಸು("/usr/local/sbin", "/usr/local/bin"))
                ಹಿಂತಿರುಗಿ (default_ಮಾರ್ಗ_2 = DEFAULT_ಮಾರ್ಗ_WITH_LOCAL_SBIN); /* b */
        ಹಿಂತಿರುಗಿ (default_ಮಾರ್ಗ_2 = DEFAULT_ಮಾರ್ಗ_WITHOUT_SBIN);            /* c */
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ DEFAULT_ಮಾರ್ಗ_WITHOUT_SBIN;
#ಆದರೆ_ಅಂತ್ಯ
}
