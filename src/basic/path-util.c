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

bool is_path(ಧ್ರುವ ವರ್ಣ *p) {
        ಯದಿ (!p) /* A NULL pointer is definitely not a path */
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ strchr(p, '/');
}

ಪೂರ್ಣಾಙ್ಕ path_split_and_make_absolute(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ ***ret) {
        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        l = strv_split(p, ":");
        ಯದಿ (!l)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = TAKE_PTR(l);
        ಪ್ರತ್ಯಾಗಮ r;
}

ವರ್ಣ* path_make_absolute(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *prefix) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ಯದಿ (path_is_absolute(p) || isempty(prefix))
                ಪ್ರತ್ಯಾಗಮ strdup(p);

        ಪ್ರತ್ಯಾಗಮ path_join(prefix, p);
}

ಪೂರ್ಣಾಙ್ಕ safe_getcwd(ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *cwd = ರಿಕ್ತ;

        cwd = get_current_dir_name();
        ಯದಿ (!cwd)
                ಪ್ರತ್ಯಾಗಮ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ಯದಿ (cwd[0] != '/')
                ಪ್ರತ್ಯಾಗಮ -ENOMEDIUM;

        ಯದಿ (ret)
                *ret = TAKE_PTR(cwd);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ path_make_absolute_cwd(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret) {
        ವರ್ಣ *c;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ಯದಿ (path_is_absolute(p))
                c = strdup(p);
        ಅನ್ಯಥಾ {
                _cleanup_free_ ವರ್ಣ *cwd = ರಿಕ್ತ;

                r = safe_getcwd(&cwd);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                c = path_join(cwd, p);
        }
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = c;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ path_make_relative(ಧ್ರುವ ವರ್ಣ *from, ಧ್ರುವ ವರ್ಣ *to, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *result = ರಿಕ್ತ;
        unsigned n_parents;
        ಧ್ರುವ ವರ್ಣ *f, *t;
        ಪೂರ್ಣಾಙ್ಕ r, k;
        ವರ್ಣ *p;

        ಪ್ರತಿಜ್ಞಾ(from);
        ಪ್ರತಿಜ್ಞಾ(to);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        ಯದಿ (!path_is_absolute(from) || !path_is_absolute(to))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತಿ (;;) {
                r = path_find_first_component(&from, true, &f);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                k = path_find_first_component(&to, true, &t);
                ಯದಿ (k < 0)
                        ಪ್ರತ್ಯಾಗಮ k;

                ಯದಿ (r == 0) {
                        /* end of 'from' */
                        ಯದಿ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                ಯದಿ (!result)
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                        } ಅನ್ಯಥಾ {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                ಯದಿ (!path_is_valid(result))
                                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ಪ್ರತ್ಯಾಗಮ 0;
                }

                ಯದಿ (r != k || !strneq(f, t, r))
                        ಭಙ್ಗ;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        ಪ್ರತಿ (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;
        }

        ಯದಿ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        result = new(ವರ್ಣ, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ಯದಿ (!result)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತಿ (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ಯದಿ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        strcpy(p, t);

        path_simplify(result);

        ಯದಿ (!path_is_valid(result))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = TAKE_PTR(result);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ path_make_relative_parent(ಧ್ರುವ ವರ್ಣ *from_child, ಧ್ರುವ ವರ್ಣ *to, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *from = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(from_child);
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

        r = path_extract_directory(from_child, &from);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ path_make_relative(from, to, ret);
}

ವರ್ಣ* path_startswith_strv(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ * ಧ್ರುವ *strv) {
        ಪ್ರತಿಜ್ಞಾ(p);

        STRV_FOREACH(s, strv) {
                ವರ್ಣ *t;

                t = path_startswith(p, *s);
                ಯದಿ (t)
                        ಪ್ರತ್ಯಾಗಮ t;
        }

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ path_strv_make_absolute_cwd(ವರ್ಣ **l) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                ವರ್ಣ *t;

                r = path_make_absolute_cwd(*s, &t);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ** path_strv_resolve(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *ಬೇರು) {
        unsigned k = 0;
        bool enomem = false;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (strv_isempty(l))
                ಪ್ರತ್ಯಾಗಮ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ ವರ್ಣ *orig = ರಿಕ್ತ;
                ವರ್ಣ *t, *u;

                ಯದಿ (ಬೇರು) {
                        ಯದಿ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                ಮುಕ್ತ(*s);
                                ಅನುವರ್ತ;
                        }

                        orig = *s;
                        t = path_join(ಬೇರು, orig);
                        ಯದಿ (!t) {
                                enomem = true;
                                ಅನುವರ್ತ;
                        }
                } ಅನ್ಯಥಾ
                        t = *s;

                r = chase(t, ಬೇರು, 0, &u, ರಿಕ್ತ);
                ಯದಿ (r == -ENOENT) {
                        ಯದಿ (ಬೇರು) {
                                u = TAKE_PTR(orig);
                                ಮುಕ್ತ(t);
                        } ಅನ್ಯಥಾ
                                u = t;
                } ಅನ್ಯಥಾ ಯದಿ (r < 0) {
                        ಮುಕ್ತ(t);

                        ಯದಿ (r == -ENOMEM)
                                enomem = true;

                        ಅನುವರ್ತ;
                } ಅನ್ಯಥಾ ಯದಿ (ಬೇರು) {
                        ವರ್ಣ *x;

                        ಮುಕ್ತ(t);
                        x = path_startswith(u, ಬೇರು);
                        ಯದಿ (x) {
                                /* restore the slash if it was lost */
                                ಯದಿ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                ಮುಕ್ತ(u);
                                ಯದಿ (!t) {
                                        enomem = true;
                                        ಅನುವರ್ತ;
                                }
                                u = t;
                        } ಅನ್ಯಥಾ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ಅನ್ಯಥಾ
                        ಮುಕ್ತ(t);

                l[k++] = u;
        }

        l[k] = ರಿಕ್ತ;

        ಯದಿ (enomem)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ l;
}

ವರ್ಣ** path_strv_resolve_uniq(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *ಬೇರು) {

        ಯದಿ (strv_isempty(l))
                ಪ್ರತ್ಯಾಗಮ l;

        ಯದಿ (!path_strv_resolve(l, ಬೇರು))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ strv_uniq(l);
}

ವರ್ಣ* skip_leading_slash(ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತ್ಯಾಗಮ skip_leading_chars(p, "/");
}

ವರ್ಣ* path_simplify_full(ವರ್ಣ *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        ವರ್ಣ *f = path;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        ಯದಿ (isempty(path))
                ಪ್ರತ್ಯಾಗಮ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = f;;) {
                ಧ್ರುವ ವರ್ಣ *e;

                r = path_find_first_component(&p, true, &e);
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                ಯದಿ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        ಅನುವರ್ತ;

                beginning = false;

                ಯದಿ (add_slash)
                        *f++ = '/';

                ಯದಿ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ಪ್ರತ್ಯಾಗಮ path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ಯದಿ (f == path)
                *f++ = '.';

        ಯದಿ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ಪ್ರತ್ಯಾಗಮ path;
}

ಪೂರ್ಣಾಙ್ಕ path_simplify_alloc(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!path) {
                *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ವರ್ಣ *t = strdup(path);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = path_simplify(t);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ* path_startswith_full(ಧ್ರುವ ವರ್ಣ *original_path, ಧ್ರುವ ವರ್ಣ *prefix, PathStartWithFlags flags) {
        ಪ್ರತಿಜ್ಞಾ(original_path);
        ಪ್ರತಿಜ್ಞಾ(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ಧ್ರುವ ವರ್ಣ *path = original_path;

        ಯದಿ ((path[0] == '/') != (prefix[0] == '/'))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (;;) {
                ಧ್ರುವ ವರ್ಣ *p, *q;
                ಪೂರ್ಣಾಙ್ಕ m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                ಯದಿ (m < 0)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                ಯದಿ (n == 0) {
                        ಯದಿ (!p)
                                p = path;

                        ಯದಿ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ಯದಿ (p <= original_path)
                                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                                p--;

                                ಯದಿ (*p != '/')
                                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                        }

                        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) p;
                }

                ಯದಿ (m != n)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                ಯದಿ (!strneq(p, q, m))
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
}

ಪೂರ್ಣಾಙ್ಕ path_compare(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (;;) {
                ಧ್ರುವ ವರ್ಣ *aa, *bb;
                ಪೂರ್ಣಾಙ್ಕ j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                ಯದಿ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ಯದಿ (r != 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ಪ್ರತ್ಯಾಗಮ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ಯದಿ (j == 0) {
                        ಯದಿ (k == 0)
                                ಪ್ರತ್ಯಾಗಮ 0;
                        ಪ್ರತ್ಯಾಗಮ -1;
                }
                ಯದಿ (k == 0)
                        ಪ್ರತ್ಯಾಗಮ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ಯದಿ (r != 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ಯದಿ (r != 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }
}

ಪೂರ್ಣಾಙ್ಕ path_compare_filename(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
        _cleanup_free_ ವರ್ಣ *fa = ರಿಕ್ತ, *fb = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ಯದಿ (j < 0)
                ಪ್ರತ್ಯಾಗಮ strcmp(a, b);

        ಪ್ರತ್ಯಾಗಮ strcmp(fa, fb);
}

ಪೂರ್ಣಾಙ್ಕ path_equal_or_inode_same_full(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b, ಪೂರ್ಣಾಙ್ಕ flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ಯದಿ (path_equal(a, b))
                ಪ್ರತ್ಯಾಗಮ 1;

        ಯದಿ (!a || !b)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ inode_same(a, b, flags);
}

ವರ್ಣ* path_extend_internal(ವರ್ಣ **x, ...) {
        size_t sz, old_sz;
        ವರ್ಣ *q, *nx;
        ಧ್ರುವ ವರ್ಣ *p;
        va_list ap;
        bool slash;

        POINTER_MAY_BE_NULL(x);

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
        ಯಾವತ್ ((p = va_arg(ap, ವರ್ಣ*)) != POINTER_MAX) {
                size_t add;

                ಯದಿ (isempty(p))
                        ಅನುವರ್ತ;

                add = 1 + strlen(p);
                ಯದಿ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : ರಿಕ್ತ, GREEDY_ALLOC_ROUND_UP(sz+1));
        ಯದಿ (!nx)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಯದಿ (x)
                *x = nx;

        ಯದಿ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ಅನ್ಯಥಾ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ಯಾವತ್ ((p = va_arg(ap, ವರ್ಣ*)) != POINTER_MAX) {
                ಯದಿ (isempty(p))
                        ಅನುವರ್ತ;

                ಯದಿ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ nx;
}

ಪೂರ್ಣಾಙ್ಕ open_and_check_executable(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *resolved = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        ಯದಿ (ಬೇರು) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, ಬೇರು, CHASE_PREFIX_ಬೇರು, &resolved, &fd);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                name = resolved;
        } ಅನ್ಯಥಾ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        r = fd_verify_regular(fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = access_fd(fd, X_OK);
        ಯದಿ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path) {
                ಯದಿ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                ಅನ್ಯಥಾ {
                        r = path_make_absolute_cwd(name, ret_path);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        path_simplify(*ret_path);
                }
        }

        ಯದಿ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ find_executable_full(
                ಧ್ರುವ ವರ್ಣ *name,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ವರ್ಣ * ಧ್ರುವ *exec_ಹುಡುಕು_path,
                bool use_path_envvar,
                ವರ್ಣ **ret_filename,
                ಪೂರ್ಣಾಙ್ಕ *ret_fd) {

        ಪೂರ್ಣಾಙ್ಕ last_error = -ENOENT, r = 0;

        ಪ್ರತಿಜ್ಞಾ(name);

        ಯದಿ (is_path(name))
                ಪ್ರತ್ಯಾಗಮ open_and_check_executable(name, ಬೇರು, ret_filename, ret_fd);

        ಯದಿ (exec_ಹುಡುಕು_path) {
                STRV_FOREACH(element, exec_ಹುಡುಕು_path) {
                        _cleanup_free_ ವರ್ಣ *full_path = ರಿಕ್ತ;

                        ಯದಿ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                ಅನುವರ್ತ;
                        }

                        full_path = path_join(*element, name);
                        ಯದಿ (!full_path)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        r = open_and_check_executable(full_path, ಬೇರು, ret_filename, ret_fd);
                        ಯದಿ (r >= 0)
                                ಪ್ರತ್ಯಾಗಮ 0;
                        ಯದಿ (r != -EACCES)
                                last_error = r;
                }
                ಪ್ರತ್ಯಾಗಮ last_error;
        }

        ಧ್ರುವ ವರ್ಣ *p = ರಿಕ್ತ;

        ಯದಿ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ಯದಿ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *element = ರಿಕ್ತ;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                ಯದಿ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        ಅನುವರ್ತ;
                }

                ಯದಿ (!path_extend(&element, name))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                r = open_and_check_executable(element, ಬೇರು, ret_filename, ret_fd);
                ಯದಿ (r >= 0) /* Found it! */
                        ಪ್ರತ್ಯಾಗಮ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ಯದಿ (r != -EACCES)
                        last_error = r;
        }

        ಪ್ರತ್ಯಾಗಮ last_error;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ executable_is_good(ಧ್ರುವ ವರ್ಣ *executable) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ, *d = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = find_executable(executable, &p);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ಯದಿ (r == -EINVAL) /* not a symlink */
                ಪ್ರತ್ಯಾಗಮ 1;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

ಪೂರ್ಣಾಙ್ಕ fsck_exists(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ executable_is_good("fsck");
}

ಪೂರ್ಣಾಙ್ಕ fsck_exists_for_fstype(ಧ್ರುವ ವರ್ಣ *fstype) {
        ಧ್ರುವ ವರ್ಣ *checker;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fstype);

        ಯದಿ (streq(fstype, "auto"))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = fsck_exists();
        ಯದಿ (r <= 0)
                ಪ್ರತ್ಯಾಗಮ r;

        checker = strjoina("fsck.", fstype);
        ಪ್ರತ್ಯಾಗಮ executable_is_good(checker);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* skip_slash_or_dot(ಧ್ರುವ ವರ್ಣ *p) {
        POINTER_MAY_BE_NULL(p);

        ಪ್ರತಿ (; !isempty(p); p++) {
                ಯದಿ (*p == '/')
                        ಅನುವರ್ತ;
                ಯದಿ (startswith(p, "./")) {
                        p++;
                        ಅನುವರ್ತ;
                }
                ಭಙ್ಗ;
        }
        ಪ್ರತ್ಯಾಗಮ p;
}

ಪೂರ್ಣಾಙ್ಕ path_find_first_component(ಧ್ರುವ ವರ್ಣ **p, bool accept_dot_dot, ಧ್ರುವ ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *q, *first, *end_first, *next;
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

        first = skip_slash_or_dot(q);
        ಯದಿ (isempty(first)) {
                *p = first;
                ಯದಿ (ret)
                        *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ (streq(first, ".")) {
                *p = first + 1;
                ಯದಿ (ret)
                        *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        ಯದಿ (len > NAME_MAX)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        ಯದಿ (ret)
                *ret = first;
        ಪ್ರತ್ಯಾಗಮ len;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* skip_slash_or_dot_backward(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *q) {
        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(!q || q >= path);

        ಪ್ರತಿ (; q; q = PTR_SUB1(q, path)) {
                ಯದಿ (*q == '/')
                        ಅನುವರ್ತ;
                ಯದಿ (q > path && strneq(q - 1, "/.", 2))
                        ಅನುವರ್ತ;
                ಯದಿ (q == path && *q == '.')
                        ಅನುವರ್ತ;
                ಭಙ್ಗ;
        }
        ಪ್ರತ್ಯಾಗಮ q;
}

ಪೂರ್ಣಾಙ್ಕ path_find_last_component(ಧ್ರುವ ವರ್ಣ *path, bool accept_dot_dot, ಧ್ರುವ ವರ್ಣ **next, ಧ್ರುವ ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *q, *last_end, *last_begin;
        size_t len;

        POINTER_MAY_BE_NULL(next);
        POINTER_MAY_BE_NULL(ret);

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

        ಯದಿ (isempty(path)) {
                ಯದಿ (next)
                        *next = path;
                ಯದಿ (ret)
                        *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (next && *next) {
                ಯದಿ (*next < path || *next > path + strlen(path))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                ಯದಿ (*next == path) {
                        ಯದಿ (ret)
                                *ret = ರಿಕ್ತ;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಯದಿ (!IN_SET(**next, '\0', '/'))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                q = *next - 1;
        } ಅನ್ಯಥಾ
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        ಯದಿ (!q) { /* the root directory, "." or "./" */
                ಯದಿ (next)
                        *next = path;
                ಯದಿ (ret)
                        *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        last_end = q + 1;

        ಯಾವತ್ (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        ಯದಿ (len > NAME_MAX)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        ಯದಿ (ret)
                *ret = last_begin;
        ಪ್ರತ್ಯಾಗಮ len;
}

ಧ್ರುವ ವರ್ಣ* last_path_component(ಧ್ರುವ ವರ್ಣ *path) {

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

        unsigned l, k;

        ಯದಿ (!path)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        l = k = strlen(path);
        ಯದಿ (l == 0) /* special case — an empty string */
                ಪ್ರತ್ಯಾಗಮ path;

        ಯಾವತ್ (k > 0 && path[k-1] == '/')
                k--;

        ಯದಿ (k == 0) /* the root directory */
                ಪ್ರತ್ಯಾಗಮ path + l - 1;

        ಯಾವತ್ (k > 0 && path[k-1] != '/')
                k--;

        ಪ್ರತ್ಯಾಗಮ path + k;
}

ಪೂರ್ಣಾಙ್ಕ path_split_prefix_filename(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_dir, ವರ್ಣ **ret_filename) {
        _cleanup_free_ ವರ್ಣ *d = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *c, *next = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        POINTER_MAY_BE_NULL(path);

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

        ಯದಿ (isempty(path))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0) /* root directory or "." */
                ಪ್ರತ್ಯಾಗಮ -EADDRNOTAVAIL;

        ಯದಿ (ret_dir) {
                ಯದಿ (next == path) {
                        ಯದಿ (*path != '/') { /* filename only */
                                ಯದಿ (!ret_filename)
                                        ಪ್ರತ್ಯಾಗಮ -EDESTADDRREQ;
                        } ಅನ್ಯಥಾ {
                                d = strdup("/");
                                ಯದಿ (!d)
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                        }
                } ಅನ್ಯಥಾ {
                        d = strndup(path, next - path);
                        ಯದಿ (!d)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        path_simplify(d);

                        ಯದಿ (!path_is_valid(d))
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;
                }

        } ಅನ್ಯಥಾ ಯದಿ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (ret_filename) {
                ವರ್ಣ *fn = strndup(c, r);
                ಯದಿ (!fn)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                *ret_filename = fn;
        }

        ಯದಿ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ಪ್ರತ್ಯಾಗಮ strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ಧ್ರುವ ವರ್ಣ *p) {
        ಧ್ರುವ ವರ್ಣ *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ false;

        e = strchrnul(p, '/');
        ಯದಿ (*e != 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

bool filename_is_valid(ಧ್ರುವ ವರ್ಣ *p) {

        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ filename_part_is_valid(p);
}

bool path_is_valid_full(ಧ್ರುವ ವರ್ಣ *p, bool accept_dot_dot) {
        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *e = p;;) {
                ಪೂರ್ಣಾಙ್ಕ r;

                r = path_find_first_component(&e, accept_dot_dot, ರಿಕ್ತ);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ಪ್ರತ್ಯಾಗಮ false;
                ಯದಿ (*e == 0)           /* End of string? Yay! */
                        ಪ್ರತ್ಯಾಗಮ true;
        }
}

bool path_is_normalized(ಧ್ರುವ ವರ್ಣ *p) {
        ಯದಿ (!path_is_safe(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (strstr(p, "//"))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ file_in_same_dir(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *filename, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *b = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(filename);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ಯದಿ (path_is_absolute(filename))
                b = strdup(filename);
        ಅನ್ಯಥಾ {
                _cleanup_free_ ವರ್ಣ *dn = ರಿಕ್ತ;

                r = path_extract_directory(path, &dn);
                ಯದಿ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ಅನ್ಯಥಾ ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಅನ್ಯಥಾ
                        b = path_join(dn, filename);
        }
        ಯದಿ (!b)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = TAKE_PTR(b);
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool hidden_or_backup_file(ಧ್ರುವ ವರ್ಣ *filename) {
        ಪ್ರತಿಜ್ಞಾ(filename);

        ಯದಿ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ಪ್ರತ್ಯಾಗಮ true;

        ಧ್ರುವ ವರ್ಣ *dot = strrchr(filename, '.');
        ಯದಿ (!dot)
                ಪ್ರತ್ಯಾಗಮ false;

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

        ಪ್ರತ್ಯಾಗಮ STR_IN_SET(dot + 1,
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

bool is_device_path(ಧ್ರುವ ವರ್ಣ *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ಧ್ರುವ ವರ್ಣ *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        ಪ್ರತ್ಯಾಗಮ !isempty(p);
}

bool valid_device_ಗಂಟು_path(ಧ್ರುವ ವರ್ಣ *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ಯದಿ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ path_is_normalized(path);
}

bool valid_device_allow_pattern(ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತಿಜ್ಞಾ(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ಯದಿ (STARTSWITH_SET(path, "block-", "char-"))
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ valid_device_ಗಂಟು_path(path);
}

bool dot_or_dot_dot(ಧ್ರುವ ವರ್ಣ *path) {
        ಯದಿ (!path)
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (path[0] != '.')
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (path[1] == 0)
                ಪ್ರತ್ಯಾಗಮ true;
        ಯದಿ (path[1] != '.')
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ path[2] == 0;
}

bool path_implies_directory(ಧ್ರುವ ವರ್ಣ *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ಯದಿ (!path)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (dot_or_dot_dot(path))
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_ಬೇರು(ಧ್ರುವ ವರ್ಣ *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ಯದಿ (isempty(path))
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ path_equal(path, "/");
}

ಧ್ರುವ ವರ್ಣ* empty_to_ಬೇರು(ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತ್ಯಾಗಮ isempty(path) ? "/" : path;
}

ಪೂರ್ಣಾಙ್ಕ empty_or_ಬೇರು_harder_to_null(ಧ್ರುವ ವರ್ಣ **path) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        ಯದಿ (empty_or_ಬೇರು(*path)) {
                *path = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        r = path_is_ಬೇರು(*path);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r > 0)
                *path = ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ 0;
}

bool path_strv_contains(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತಿಜ್ಞಾ(path);

        STRV_FOREACH(i, l)
                ಯದಿ (path_equal(*i, path))
                        ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ false;
}

bool prefixed_path_strv_contains(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತಿಜ್ಞಾ(path);

        STRV_FOREACH(i, l) {
                ಧ್ರುವ ವರ್ಣ *j = *i;

                ಯದಿ (*j == '-')
                        j++;
                ಯದಿ (*j == '+')
                        j++;

                ಯದಿ (path_equal(j, path))
                        ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ path_glob_can_match(ಧ್ರುವ ವರ್ಣ *pattern, ಧ್ರುವ ವರ್ಣ *prefix, ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(pattern);
        ಪ್ರತಿಜ್ಞಾ(prefix);

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *a = pattern, *b = prefix;;) {
                _cleanup_free_ ವರ್ಣ *g = ರಿಕ್ತ, *h = ರಿಕ್ತ;
                ಧ್ರುವ ವರ್ಣ *p, *q;
                ಪೂರ್ಣಾಙ್ಕ r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                ಯದಿ (s < 0)
                        ಪ್ರತ್ಯಾಗಮ s;

                ಯದಿ (s == 0) {
                        /* The pattern matches the prefix. */
                        ಯದಿ (ret) {
                                ವರ್ಣ *t;

                                t = path_join(prefix, p);
                                ಯದಿ (!t)
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                *ret = t;
                        }
                        ಪ್ರತ್ಯಾಗಮ true;
                }

                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                ಯದಿ (r == s && strneq(p, q, r))
                        ಅನುವರ್ತ; /* common component. Check next. */

                g = strndup(p, r);
                ಯದಿ (!g)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (!string_is_glob(g))
                        ಭಙ್ಗ;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ಯದಿ (!h)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                r = fnmatch(g, h, 0);
                ಯದಿ (r == FNM_NOMATCH)
                        ಭಙ್ಗ;
                ಯದಿ (r != 0) /* Failure to process pattern? */
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ಯದಿ (ret)
                *ret = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ false;
}

#ಯದಿ HAVE_SPLIT_BIN
ಸ್ಥಿರ bool dir_is_split(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ಯದಿ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ಪ್ರತ್ಯಾಗಮ true;
        }
        ಪ್ರತ್ಯಾಗಮ r == 0;
}
#endif

ಧ್ರುವ ವರ್ಣ* default_PATH(ಶೂನ್ಯ) {
#ಯದಿ HAVE_SPLIT_BIN
        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *default_path = ರಿಕ್ತ;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ಯದಿ (default_path)
                ಪ್ರತ್ಯಾಗಮ default_path;

        ಯದಿ (dir_is_split("/usr/sbin", "/usr/bin"))
                ಪ್ರತ್ಯಾಗಮ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        ಯದಿ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ಪ್ರತ್ಯಾಗಮ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ಪ್ರತ್ಯಾಗಮ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
