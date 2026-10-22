/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fnmatch.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "glob-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

bool is_path(ଅଚଳ ଅକ୍ଷର *p) {
        ଯଦି (!p) /* A NULL pointer is definitely not a path */
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ strchr(p, '/');
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_split_and_make_absolute(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର ***ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);

        l = strv_split(p, ":");
        ଯଦି (!l)
                ଫେରନ୍ତୁ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = TAKE_PTR(l);
        ଫେରନ୍ତୁ r;
}

ଅକ୍ଷର* path_make_absolute(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *prefix) {
        ପ୍ରତିଜ୍ଞା(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ଯଦି (path_is_absolute(p) || isempty(prefix))
                ଫେରନ୍ତୁ strdup(p);

        ଫେରନ୍ତୁ path_join(prefix, p);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_getcwd(ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *cwd = ଖାଲି;

        cwd = get_current_dir_name();
        ଯଦି (!cwd)
                ଫେରନ୍ତୁ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ଯଦି (cwd[0] != '/')
                ଫେରନ୍ତୁ -ENOMEDIUM;

        ଯଦି (ret)
                *ret = TAKE_PTR(cwd);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_make_absolute_cwd(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର *c;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ଯଦି (path_is_absolute(p))
                c = strdup(p);
        ଅନ୍ୟଥା {
                _cleanup_free_ ଅକ୍ଷର *cwd = ଖାଲି;

                r = safe_getcwd(&cwd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                c = path_join(cwd, p);
        }
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = c;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_make_relative(ଅଚଳ ଅକ୍ଷର *from, ଅଚଳ ଅକ୍ଷର *to, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *result = ଖାଲି;
        unsigned n_parents;
        ଅଚଳ ଅକ୍ଷର *f, *t;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, k;
        ଅକ୍ଷର *p;

        ପ୍ରତିଜ୍ଞା(from);
        ପ୍ରତିଜ୍ଞା(to);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        ଯଦି (!path_is_absolute(from) || !path_is_absolute(to))
                ଫେରନ୍ତୁ -EINVAL;

        ପାଇଁ (;;) {
                r = path_find_first_component(&from, true, &f);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                k = path_find_first_component(&to, true, &t);
                ଯଦି (k < 0)
                        ଫେରନ୍ତୁ k;

                ଯଦି (r == 0) {
                        /* end of 'from' */
                        ଯଦି (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                ଯଦି (!result)
                                        ଫେରନ୍ତୁ -ENOMEM;
                        } ଅନ୍ୟଥା {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                ଯଦି (!path_is_valid(result))
                                        ଫେରନ୍ତୁ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ଫେରନ୍ତୁ 0;
                }

                ଯଦି (r != k || !strneq(f, t, r))
                        ଭାଙ୍ଗ;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        ପାଇଁ (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;
        }

        ଯଦି (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ଫେରନ୍ତୁ -EINVAL;

        result = new(ଅକ୍ଷର, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ଯଦି (!result)
                ଫେରନ୍ତୁ -ENOMEM;

        ପାଇଁ (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ଯଦି (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ଫେରନ୍ତୁ 0;
        }

        strcpy(p, t);

        path_simplify(result);

        ଯଦି (!path_is_valid(result))
                ଫେରନ୍ତୁ -EINVAL;

        *ret = TAKE_PTR(result);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_make_relative_parent(ଅଚଳ ଅକ୍ଷର *from_child, ଅଚଳ ଅକ୍ଷର *to, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *from = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(from_child);
        ପ୍ରତିଜ୍ଞା(to);
        ପ୍ରତିଜ୍ଞା(ret);

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
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ path_make_relative(from, to, ret);
}

ଅକ୍ଷର* path_startswith_strv(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର * ଅଚଳ *strv) {
        ପ୍ରତିଜ୍ଞା(p);

        STRV_FOREACH(s, strv) {
                ଅକ୍ଷର *t;

                t = path_startswith(p, *s);
                ଯଦି (t)
                        ଫେରନ୍ତୁ t;
        }

        ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_strv_make_absolute_cwd(ଅକ୍ଷର **l) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                ଅକ୍ଷର *t;

                r = path_make_absolute_cwd(*s, &t);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର** path_strv_resolve(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *ମୂଳ) {
        unsigned k = 0;
        bool enomem = false;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (strv_isempty(l))
                ଫେରନ୍ତୁ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ ଅକ୍ଷର *orig = ଖାଲି;
                ଅକ୍ଷର *t, *u;

                ଯଦି (ମୂଳ) {
                        ଯଦି (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                ମୁକ୍ତ(*s);
                                ଜାରି;
                        }

                        orig = *s;
                        t = path_join(ମୂଳ, orig);
                        ଯଦି (!t) {
                                enomem = true;
                                ଜାରି;
                        }
                } ଅନ୍ୟଥା
                        t = *s;

                r = chase(t, ମୂଳ, 0, &u, ଖାଲି);
                ଯଦି (r == -ENOENT) {
                        ଯଦି (ମୂଳ) {
                                u = TAKE_PTR(orig);
                                ମୁକ୍ତ(t);
                        } ଅନ୍ୟଥା
                                u = t;
                } ଅନ୍ୟଥା ଯଦି (r < 0) {
                        ମୁକ୍ତ(t);

                        ଯଦି (r == -ENOMEM)
                                enomem = true;

                        ଜାରି;
                } ଅନ୍ୟଥା ଯଦି (ମୂଳ) {
                        ଅକ୍ଷର *x;

                        ମୁକ୍ତ(t);
                        x = path_startswith(u, ମୂଳ);
                        ଯଦି (x) {
                                /* restore the slash if it was lost */
                                ଯଦି (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                ମୁକ୍ତ(u);
                                ଯଦି (!t) {
                                        enomem = true;
                                        ଜାରି;
                                }
                                u = t;
                        } ଅନ୍ୟଥା {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ଅନ୍ୟଥା
                        ମୁକ୍ତ(t);

                l[k++] = u;
        }

        l[k] = ଖାଲି;

        ଯଦି (enomem)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ l;
}

ଅକ୍ଷର** path_strv_resolve_uniq(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *ମୂଳ) {

        ଯଦି (strv_isempty(l))
                ଫେରନ୍ତୁ l;

        ଯଦି (!path_strv_resolve(l, ମୂଳ))
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ strv_uniq(l);
}

ଅକ୍ଷର* skip_leading_slash(ଅଚଳ ଅକ୍ଷର *p) {
        ଫେରନ୍ତୁ skip_leading_chars(p, "/");
}

ଅକ୍ଷର* path_simplify_full(ଅକ୍ଷର *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        ଅକ୍ଷର *f = path;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        ଯଦି (isempty(path))
                ଫେରନ୍ତୁ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = f;;) {
                ଅଚଳ ଅକ୍ଷର *e;

                r = path_find_first_component(&p, true, &e);
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଯଦି (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        ଜାରି;

                beginning = false;

                ଯଦି (add_slash)
                        *f++ = '/';

                ଯଦି (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ଫେରନ୍ତୁ path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ଯଦି (f == path)
                *f++ = '.';

        ଯଦି (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ଫେରନ୍ତୁ path;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_simplify_alloc(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!path) {
                *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        ଅକ୍ଷର *t = strdup(path);
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = path_simplify(t);
        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର* path_startswith_full(ଅଚଳ ଅକ୍ଷର *original_path, ଅଚଳ ଅକ୍ଷର *prefix, PathStartWithFlags flags) {
        ପ୍ରତିଜ୍ଞା(original_path);
        ପ୍ରତିଜ୍ଞା(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ଅଚଳ ଅକ୍ଷର *path = original_path;

        ଯଦି ((path[0] == '/') != (prefix[0] == '/'))
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (;;) {
                ଅଚଳ ଅକ୍ଷର *p, *q;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                ଯଦି (m < 0)
                        ଫେରନ୍ତୁ ଖାଲି;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ ଖାଲି;

                ଯଦି (n == 0) {
                        ଯଦି (!p)
                                p = path;

                        ଯଦି (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ଯଦି (p <= original_path)
                                        ଫେରନ୍ତୁ ଖାଲି;

                                p--;

                                ଯଦି (*p != '/')
                                        ଫେରନ୍ତୁ ଖାଲି;
                        }

                        ଫେରନ୍ତୁ (ଅକ୍ଷର*) p;
                }

                ଯଦି (m != n)
                        ଫେରନ୍ତୁ ଖାଲି;

                ଯଦି (!strneq(p, q, m))
                        ଫେରନ୍ତୁ ଖାଲି;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_compare(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (;;) {
                ଅଚଳ ଅକ୍ଷର *aa, *bb;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                ଯଦି (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ଯଦି (r != 0)
                                ଫେରନ୍ତୁ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ଫେରନ୍ତୁ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ଯଦି (j == 0) {
                        ଯଦି (k == 0)
                                ଫେରନ୍ତୁ 0;
                        ଫେରନ୍ତୁ -1;
                }
                ଯଦି (k == 0)
                        ଫେରନ୍ତୁ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ଯଦି (r != 0)
                        ଫେରନ୍ତୁ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ଯଦି (r != 0)
                        ଫେରନ୍ତୁ r;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_compare_filename(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
        _cleanup_free_ ଅକ୍ଷର *fa = ଖାଲି, *fb = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ଯଦି (j < 0)
                ଫେରନ୍ତୁ strcmp(a, b);

        ଫେରନ୍ତୁ strcmp(fa, fb);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_equal_or_inode_same_full(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ଯଦି (path_equal(a, b))
                ଫେରନ୍ତୁ 1;

        ଯଦି (!a || !b)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ inode_same(a, b, flags);
}

ଅକ୍ଷର* path_extend_internal(ଅକ୍ଷର **x, ...) {
        size_t sz, old_sz;
        ଅକ୍ଷର *q, *nx;
        ଅଚଳ ଅକ୍ଷର *p;
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
        ଯେତେବେଳେ ((p = va_arg(ap, ଅକ୍ଷର*)) != POINTER_MAX) {
                size_t add;

                ଯଦି (isempty(p))
                        ଜାରି;

                add = 1 + strlen(p);
                ଯଦି (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ଫେରନ୍ତୁ ଖାଲି;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : ଖାଲି, GREEDY_ALLOC_ROUND_UP(sz+1));
        ଯଦି (!nx)
                ଫେରନ୍ତୁ ଖାଲି;
        ଯଦି (x)
                *x = nx;

        ଯଦି (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ଅନ୍ୟଥା {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ଯେତେବେଳେ ((p = va_arg(ap, ଅକ୍ଷର*)) != POINTER_MAX) {
                ଯଦି (isempty(p))
                        ଜାରି;

                ଯଦି (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ଫେରନ୍ତୁ nx;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_and_check_executable(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *resolved = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        ଯଦି (ମୂଳ) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, ମୂଳ, CHASE_PREFIX_ମୂଳ, &resolved, &fd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                name = resolved;
        } ଅନ୍ୟଥା {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        r = fd_verify_regular(fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = access_fd(fd, X_OK);
        ଯଦି (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path) {
                ଯଦି (resolved)
                        *ret_path = TAKE_PTR(resolved);
                ଅନ୍ୟଥା {
                        r = path_make_absolute_cwd(name, ret_path);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        path_simplify(*ret_path);
                }
        }

        ଯଦି (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_executable_full(
                ଅଚଳ ଅକ୍ଷର *name,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅକ୍ଷର * ଅଚଳ *exec_ସନ୍ଧାନ_path,
                bool use_path_envvar,
                ଅକ୍ଷର **ret_filename,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_error = -ENOENT, r = 0;

        ପ୍ରତିଜ୍ଞା(name);

        ଯଦି (is_path(name))
                ଫେରନ୍ତୁ open_and_check_executable(name, ମୂଳ, ret_filename, ret_fd);

        ଯଦି (exec_ସନ୍ଧାନ_path) {
                STRV_FOREACH(element, exec_ସନ୍ଧାନ_path) {
                        _cleanup_free_ ଅକ୍ଷର *full_path = ଖାଲି;

                        ଯଦି (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                ଜାରି;
                        }

                        full_path = path_join(*element, name);
                        ଯଦି (!full_path)
                                ଫେରନ୍ତୁ -ENOMEM;

                        r = open_and_check_executable(full_path, ମୂଳ, ret_filename, ret_fd);
                        ଯଦି (r >= 0)
                                ଫେରନ୍ତୁ 0;
                        ଯଦି (r != -EACCES)
                                last_error = r;
                }
                ଫେରନ୍ତୁ last_error;
        }

        ଅଚଳ ଅକ୍ଷର *p = ଖାଲି;

        ଯଦି (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ଯଦି (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *element = ଖାଲି;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଯଦି (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        ଜାରି;
                }

                ଯଦି (!path_extend(&element, name))
                        ଫେରନ୍ତୁ -ENOMEM;

                r = open_and_check_executable(element, ମୂଳ, ret_filename, ret_fd);
                ଯଦି (r >= 0) /* Found it! */
                        ଫେରନ୍ତୁ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ଯଦି (r != -EACCES)
                        last_error = r;
        }

        ଫେରନ୍ତୁ last_error;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା executable_is_good(ଅଚଳ ଅକ୍ଷର *executable) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି, *d = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = find_executable(executable, &p);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ 0;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ଯଦି (r == -EINVAL) /* not a symlink */
                ଫେରନ୍ତୁ 1;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsck_exists(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ executable_is_good("fsck");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsck_exists_for_fstype(ଅଚଳ ଅକ୍ଷର *fstype) {
        ଅଚଳ ଅକ୍ଷର *checker;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fstype);

        ଯଦି (streq(fstype, "auto"))
                ଫେରନ୍ତୁ -EINVAL;

        r = fsck_exists();
        ଯଦି (r <= 0)
                ଫେରନ୍ତୁ r;

        checker = strjoina("fsck.", fstype);
        ଫେରନ୍ତୁ executable_is_good(checker);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* skip_slash_or_dot(ଅଚଳ ଅକ୍ଷର *p) {
        POINTER_MAY_BE_NULL(p);

        ପାଇଁ (; !isempty(p); p++) {
                ଯଦି (*p == '/')
                        ଜାରି;
                ଯଦି (startswith(p, "./")) {
                        p++;
                        ଜାରି;
                }
                ଭାଙ୍ଗ;
        }
        ଫେରନ୍ତୁ p;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_find_first_component(ଅଚଳ ଅକ୍ଷର **p, bool accept_dot_dot, ଅଚଳ ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *q, *first, *end_first, *next;
        size_t len;

        ପ୍ରତିଜ୍ଞା(p);

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
        ଯଦି (isempty(first)) {
                *p = first;
                ଯଦି (ret)
                        *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }
        ଯଦି (streq(first, ".")) {
                *p = first + 1;
                ଯଦି (ret)
                        *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        ଯଦି (len > NAME_MAX)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ଫେରନ୍ତୁ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        ଯଦି (ret)
                *ret = first;
        ଫେରନ୍ତୁ len;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* skip_slash_or_dot_backward(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *q) {
        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(!q || q >= path);

        ପାଇଁ (; q; q = PTR_SUB1(q, path)) {
                ଯଦି (*q == '/')
                        ଜାରି;
                ଯଦି (q > path && strneq(q - 1, "/.", 2))
                        ଜାରି;
                ଯଦି (q == path && *q == '.')
                        ଜାରି;
                ଭାଙ୍ଗ;
        }
        ଫେରନ୍ତୁ q;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_find_last_component(ଅଚଳ ଅକ୍ଷର *path, bool accept_dot_dot, ଅଚଳ ଅକ୍ଷର **next, ଅଚଳ ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *q, *last_end, *last_begin;
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

        ଯଦି (isempty(path)) {
                ଯଦି (next)
                        *next = path;
                ଯଦି (ret)
                        *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (next && *next) {
                ଯଦି (*next < path || *next > path + strlen(path))
                        ଫେରନ୍ତୁ -EINVAL;
                ଯଦି (*next == path) {
                        ଯଦି (ret)
                                *ret = ଖାଲି;
                        ଫେରନ୍ତୁ 0;
                }
                ଯଦି (!IN_SET(**next, '\0', '/'))
                        ଫେରନ୍ତୁ -EINVAL;
                q = *next - 1;
        } ଅନ୍ୟଥା
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        ଯଦି (!q) { /* the root directory, "." or "./" */
                ଯଦି (next)
                        *next = path;
                ଯଦି (ret)
                        *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        last_end = q + 1;

        ଯେତେବେଳେ (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        ଯଦି (len > NAME_MAX)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        ଯଦି (ret)
                *ret = last_begin;
        ଫେରନ୍ତୁ len;
}

ଅଚଳ ଅକ୍ଷର* last_path_component(ଅଚଳ ଅକ୍ଷର *path) {

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

        ଯଦି (!path)
                ଫେରନ୍ତୁ ଖାଲି;

        l = k = strlen(path);
        ଯଦି (l == 0) /* special case — an empty string */
                ଫେରନ୍ତୁ path;

        ଯେତେବେଳେ (k > 0 && path[k-1] == '/')
                k--;

        ଯଦି (k == 0) /* the root directory */
                ଫେରନ୍ତୁ path + l - 1;

        ଯେତେବେଳେ (k > 0 && path[k-1] != '/')
                k--;

        ଫେରନ୍ତୁ path + k;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_split_prefix_filename(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_dir, ଅକ୍ଷର **ret_filename) {
        _cleanup_free_ ଅକ୍ଷର *d = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *c, *next = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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

        ଯଦି (isempty(path))
                ଫେରନ୍ତୁ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0) /* root directory or "." */
                ଫେରନ୍ତୁ -EADDRNOTAVAIL;

        ଯଦି (ret_dir) {
                ଯଦି (next == path) {
                        ଯଦି (*path != '/') { /* filename only */
                                ଯଦି (!ret_filename)
                                        ଫେରନ୍ତୁ -EDESTADDRREQ;
                        } ଅନ୍ୟଥା {
                                d = strdup("/");
                                ଯଦି (!d)
                                        ଫେରନ୍ତୁ -ENOMEM;
                        }
                } ଅନ୍ୟଥା {
                        d = strndup(path, next - path);
                        ଯଦି (!d)
                                ଫେରନ୍ତୁ -ENOMEM;

                        path_simplify(d);

                        ଯଦି (!path_is_valid(d))
                                ଫେରନ୍ତୁ -EINVAL;
                }

        } ଅନ୍ୟଥା ଯଦି (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret_filename) {
                ଅକ୍ଷର *fn = strndup(c, r);
                ଯଦି (!fn)
                        ଫେରନ୍ତୁ -ENOMEM;

                *ret_filename = fn;
        }

        ଯଦି (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ଫେରନ୍ତୁ strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ଅଚଳ ଅକ୍ଷର *p) {
        ଅଚଳ ଅକ୍ଷର *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ଯଦି (!p)
                ଫେରନ୍ତୁ false;

        e = strchrnul(p, '/');
        ଯଦି (*e != 0)
                ଫେରନ୍ତୁ false;

        ଯଦି (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

bool filename_is_valid(ଅଚଳ ଅକ୍ଷର *p) {

        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ false;

        ଯଦି (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ filename_part_is_valid(p);
}

bool path_is_valid_full(ଅଚଳ ଅକ୍ଷର *p, bool accept_dot_dot) {
        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ false;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *e = p;;) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

                r = path_find_first_component(&e, accept_dot_dot, ଖାଲି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ false;

                ଯଦି (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ଫେରନ୍ତୁ false;
                ଯଦି (*e == 0)           /* End of string? Yay! */
                        ଫେରନ୍ତୁ true;
        }
}

bool path_is_normalized(ଅଚଳ ଅକ୍ଷର *p) {
        ଯଦି (!path_is_safe(p))
                ଫେରନ୍ତୁ false;

        ଯଦି (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ଫେରନ୍ତୁ false;

        ଯଦି (strstr(p, "//"))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା file_in_same_dir(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *filename, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *b = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(filename);
        ପ୍ରତିଜ୍ଞା(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ଯଦି (path_is_absolute(filename))
                b = strdup(filename);
        ଅନ୍ୟଥା {
                _cleanup_free_ ଅକ୍ଷର *dn = ଖାଲି;

                r = path_extract_directory(path, &dn);
                ଯଦି (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ଅନ୍ୟଥା ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଅନ୍ୟଥା
                        b = path_join(dn, filename);
        }
        ଯଦି (!b)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = TAKE_PTR(b);
        ଫେରନ୍ତୁ 0;
}

bool hidden_or_backup_file(ଅଚଳ ଅକ୍ଷର *filename) {
        ପ୍ରତିଜ୍ଞା(filename);

        ଯଦି (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ଫେରନ୍ତୁ true;

        ଅଚଳ ଅକ୍ଷର *dot = strrchr(filename, '.');
        ଯଦି (!dot)
                ଫେରନ୍ତୁ false;

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

        ଫେରନ୍ତୁ STR_IN_SET(dot + 1,
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

bool is_device_path(ଅଚଳ ଅକ୍ଷର *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ଅଚଳ ଅକ୍ଷର *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        ଫେରନ୍ତୁ !isempty(p);
}

bool valid_device_ଗଣ୍ଠି_path(ଅଚଳ ଅକ୍ଷର *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ଯଦି (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ଫେରନ୍ତୁ false;

        ଯଦି (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ path_is_normalized(path);
}

bool valid_device_allow_pattern(ଅଚଳ ଅକ୍ଷର *path) {
        ପ୍ରତିଜ୍ଞା(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ଯଦି (STARTSWITH_SET(path, "block-", "char-"))
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ valid_device_ଗଣ୍ଠି_path(path);
}

bool dot_or_dot_dot(ଅଚଳ ଅକ୍ଷର *path) {
        ଯଦି (!path)
                ଫେରନ୍ତୁ false;
        ଯଦି (path[0] != '.')
                ଫେରନ୍ତୁ false;
        ଯଦି (path[1] == 0)
                ଫେରନ୍ତୁ true;
        ଯଦି (path[1] != '.')
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ path[2] == 0;
}

bool path_implies_directory(ଅଚଳ ଅକ୍ଷର *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ଯଦି (!path)
                ଫେରନ୍ତୁ false;

        ଯଦି (dot_or_dot_dot(path))
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_ମୂଳ(ଅଚଳ ଅକ୍ଷର *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ଯଦି (isempty(path))
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ path_equal(path, "/");
}

ଅଚଳ ଅକ୍ଷର* empty_to_ମୂଳ(ଅଚଳ ଅକ୍ଷର *path) {
        ଫେରନ୍ତୁ isempty(path) ? "/" : path;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା empty_or_ମୂଳ_harder_to_null(ଅଚଳ ଅକ୍ଷର **path) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        ଯଦି (empty_or_ମୂଳ(*path)) {
                *path = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        r = path_is_ମୂଳ(*path);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r > 0)
                *path = ଖାଲି;

        ଫେରନ୍ତୁ 0;
}

bool path_strv_contains(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *path) {
        ପ୍ରତିଜ୍ଞା(path);

        STRV_FOREACH(i, l)
                ଯଦି (path_equal(*i, path))
                        ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ false;
}

bool prefixed_path_strv_contains(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *path) {
        ପ୍ରତିଜ୍ଞା(path);

        STRV_FOREACH(i, l) {
                ଅଚଳ ଅକ୍ଷର *j = *i;

                ଯଦି (*j == '-')
                        j++;
                ଯଦି (*j == '+')
                        j++;

                ଯଦି (path_equal(j, path))
                        ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_glob_can_match(ଅଚଳ ଅକ୍ଷର *pattern, ଅଚଳ ଅକ୍ଷର *prefix, ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(pattern);
        ପ୍ରତିଜ୍ଞା(prefix);

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *a = pattern, *b = prefix;;) {
                _cleanup_free_ ଅକ୍ଷର *g = ଖାଲି, *h = ଖାଲି;
                ଅଚଳ ଅକ୍ଷର *p, *q;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                ଯଦି (s < 0)
                        ଫେରନ୍ତୁ s;

                ଯଦି (s == 0) {
                        /* The pattern matches the prefix. */
                        ଯଦି (ret) {
                                ଅକ୍ଷର *t;

                                t = path_join(prefix, p);
                                ଯଦି (!t)
                                        ଫେରନ୍ତୁ -ENOMEM;

                                *ret = t;
                        }
                        ଫେରନ୍ତୁ true;
                }

                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଯଦି (r == s && strneq(p, q, r))
                        ଜାରି; /* common component. Check next. */

                g = strndup(p, r);
                ଯଦି (!g)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (!string_is_glob(g))
                        ଭାଙ୍ଗ;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ଯଦି (!h)
                        ଫେରନ୍ତୁ -ENOMEM;

                r = fnmatch(g, h, 0);
                ଯଦି (r == FNM_NOMATCH)
                        ଭାଙ୍ଗ;
                ଯଦି (r != 0) /* Failure to process pattern? */
                        ଫେରନ୍ତୁ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ଯଦି (ret)
                *ret = ଖାଲି;
        ଫେରନ୍ତୁ false;
}

#ଯଦି HAVE_SPLIT_BIN
ସ୍ଥିର bool dir_is_split(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ଯଦି (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ଫେରନ୍ତୁ true;
        }
        ଫେରନ୍ତୁ r == 0;
}
#endif

ଅଚଳ ଅକ୍ଷର* default_PATH(ଶୂନ୍ୟ) {
#ଯଦି HAVE_SPLIT_BIN
        ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *default_path = ଖାଲି;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ଯଦି (default_path)
                ଫେରନ୍ତୁ default_path;

        ଯଦି (dir_is_split("/usr/sbin", "/usr/bin"))
                ଫେରନ୍ତୁ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        ଯଦି (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ଫେରନ୍ତୁ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ଫେରନ୍ତୁ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
