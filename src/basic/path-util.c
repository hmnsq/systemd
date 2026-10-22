/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fnmatch.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "chase.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "glob-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"

bool is_path(ধ্রুব বর্ণ *p) {
        যদি (!p) /* A NULL pointer is definitely not a path */
                প্রত্যাগম false;

        প্রত্যাগম strchr(p, '/');
}

পূর্ণাঙ্ক path_split_and_make_absolute(ধ্রুব বর্ণ *p, বর্ণ ***ret) {
        _cleanup_strv_free_ বর্ণ **l = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        l = strv_split(p, ":");
        যদি (!l)
                প্রত্যাগম -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        যদি (r < 0)
                প্রত্যাগম r;

        *ret = TAKE_PTR(l);
        প্রত্যাগম r;
}

বর্ণ* path_make_absolute(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *prefix) {
        প্রতিজ্ঞা(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        যদি (path_is_absolute(p) || isempty(prefix))
                প্রত্যাগম strdup(p);

        প্রত্যাগম path_join(prefix, p);
}

পূর্ণাঙ্ক safe_getcwd(বর্ণ **ret) {
        _cleanup_free_ বর্ণ *cwd = রিক্ত;

        cwd = get_current_dir_name();
        যদি (!cwd)
                প্রত্যাগম negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        যদি (cwd[0] != '/')
                প্রত্যাগম -ENOMEDIUM;

        যদি (ret)
                *ret = TAKE_PTR(cwd);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক path_make_absolute_cwd(ধ্রুব বর্ণ *p, বর্ণ **ret) {
        বর্ণ *c;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        যদি (path_is_absolute(p))
                c = strdup(p);
        অন্যথা {
                _cleanup_free_ বর্ণ *cwd = রিক্ত;

                r = safe_getcwd(&cwd);
                যদি (r < 0)
                        প্রত্যাগম r;

                c = path_join(cwd, p);
        }
        যদি (!c)
                প্রত্যাগম -ENOMEM;

        *ret = c;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক path_make_relative(ধ্রুব বর্ণ *from, ধ্রুব বর্ণ *to, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *result = রিক্ত;
        unsigned n_parents;
        ধ্রুব বর্ণ *f, *t;
        পূর্ণাঙ্ক r, k;
        বর্ণ *p;

        প্রতিজ্ঞা(from);
        প্রতিজ্ঞা(to);
        প্রতিজ্ঞা(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        যদি (!path_is_absolute(from) || !path_is_absolute(to))
                প্রত্যাগম -EINVAL;

        প্রতি (;;) {
                r = path_find_first_component(&from, true, &f);
                যদি (r < 0)
                        প্রত্যাগম r;

                k = path_find_first_component(&to, true, &t);
                যদি (k < 0)
                        প্রত্যাগম k;

                যদি (r == 0) {
                        /* end of 'from' */
                        যদি (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                যদি (!result)
                                        প্রত্যাগম -ENOMEM;
                        } অন্যথা {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                যদি (r < 0)
                                        প্রত্যাগম r;

                                যদি (!path_is_valid(result))
                                        প্রত্যাগম -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        প্রত্যাগম 0;
                }

                যদি (r != k || !strneq(f, t, r))
                        ভঙ্গ;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        প্রতি (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        ভঙ্গ;
        }

        যদি (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                প্রত্যাগম -EINVAL;

        result = new(বর্ণ, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        যদি (!result)
                প্রত্যাগম -ENOMEM;

        প্রতি (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        যদি (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                প্রত্যাগম 0;
        }

        strcpy(p, t);

        path_simplify(result);

        যদি (!path_is_valid(result))
                প্রত্যাগম -EINVAL;

        *ret = TAKE_PTR(result);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক path_make_relative_parent(ধ্রুব বর্ণ *from_child, ধ্রুব বর্ণ *to, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *from = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(from_child);
        প্রতিজ্ঞা(to);
        প্রতিজ্ঞা(ret);

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
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম path_make_relative(from, to, ret);
}

বর্ণ* path_startswith_strv(ধ্রুব বর্ণ *p, বর্ণ * ধ্রুব *strv) {
        প্রতিজ্ঞা(p);

        STRV_FOREACH(s, strv) {
                বর্ণ *t;

                t = path_startswith(p, *s);
                যদি (t)
                        প্রত্যাগম t;
        }

        প্রত্যাগম রিক্ত;
}

পূর্ণাঙ্ক path_strv_make_absolute_cwd(বর্ণ **l) {
        পূর্ণাঙ্ক r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                বর্ণ *t;

                r = path_make_absolute_cwd(*s, &t);
                যদি (r < 0)
                        প্রত্যাগম r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        প্রত্যাগম 0;
}

বর্ণ** path_strv_resolve(বর্ণ **l, ধ্রুব বর্ণ *মূল) {
        unsigned k = 0;
        bool enomem = false;
        পূর্ণাঙ্ক r;

        যদি (strv_isempty(l))
                প্রত্যাগম l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ বর্ণ *orig = রিক্ত;
                বর্ণ *t, *u;

                যদি (মূল) {
                        যদি (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                মুক্ত(*s);
                                অনুবর্ত;
                        }

                        orig = *s;
                        t = path_join(মূল, orig);
                        যদি (!t) {
                                enomem = true;
                                অনুবর্ত;
                        }
                } অন্যথা
                        t = *s;

                r = chase(t, মূল, 0, &u, রিক্ত);
                যদি (r == -ENOENT) {
                        যদি (মূল) {
                                u = TAKE_PTR(orig);
                                মুক্ত(t);
                        } অন্যথা
                                u = t;
                } অন্যথা যদি (r < 0) {
                        মুক্ত(t);

                        যদি (r == -ENOMEM)
                                enomem = true;

                        অনুবর্ত;
                } অন্যথা যদি (মূল) {
                        বর্ণ *x;

                        মুক্ত(t);
                        x = path_startswith(u, মূল);
                        যদি (x) {
                                /* restore the slash if it was lost */
                                যদি (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                মুক্ত(u);
                                যদি (!t) {
                                        enomem = true;
                                        অনুবর্ত;
                                }
                                u = t;
                        } অন্যথা {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } অন্যথা
                        মুক্ত(t);

                l[k++] = u;
        }

        l[k] = রিক্ত;

        যদি (enomem)
                প্রত্যাগম রিক্ত;

        প্রত্যাগম l;
}

বর্ণ** path_strv_resolve_uniq(বর্ণ **l, ধ্রুব বর্ণ *মূল) {

        যদি (strv_isempty(l))
                প্রত্যাগম l;

        যদি (!path_strv_resolve(l, মূল))
                প্রত্যাগম রিক্ত;

        প্রত্যাগম strv_uniq(l);
}

বর্ণ* skip_leading_slash(ধ্রুব বর্ণ *p) {
        প্রত্যাগম skip_leading_chars(p, "/");
}

বর্ণ* path_simplify_full(বর্ণ *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        বর্ণ *f = path;
        পূর্ণাঙ্ক r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        যদি (isempty(path))
                প্রত্যাগম path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        প্রতি (ধ্রুব বর্ণ *p = f;;) {
                ধ্রুব বর্ণ *e;

                r = path_find_first_component(&p, true, &e);
                যদি (r == 0)
                        ভঙ্গ;

                যদি (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        অনুবর্ত;

                beginning = false;

                যদি (add_slash)
                        *f++ = '/';

                যদি (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        প্রত্যাগম path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        যদি (f == path)
                *f++ = '.';

        যদি (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        প্রত্যাগম path;
}

পূর্ণাঙ্ক path_simplify_alloc(ধ্রুব বর্ণ *path, বর্ণ **ret) {
        প্রতিজ্ঞা(ret);

        যদি (!path) {
                *ret = রিক্ত;
                প্রত্যাগম 0;
        }

        বর্ণ *t = strdup(path);
        যদি (!t)
                প্রত্যাগম -ENOMEM;

        *ret = path_simplify(t);
        প্রত্যাগম 0;
}

বর্ণ* path_startswith_full(ধ্রুব বর্ণ *original_path, ধ্রুব বর্ণ *prefix, PathStartWithFlags flags) {
        প্রতিজ্ঞা(original_path);
        প্রতিজ্ঞা(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ধ্রুব বর্ণ *path = original_path;

        যদি ((path[0] == '/') != (prefix[0] == '/'))
                প্রত্যাগম রিক্ত;

        প্রতি (;;) {
                ধ্রুব বর্ণ *p, *q;
                পূর্ণাঙ্ক m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                যদি (m < 0)
                        প্রত্যাগম রিক্ত;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                যদি (n < 0)
                        প্রত্যাগম রিক্ত;

                যদি (n == 0) {
                        যদি (!p)
                                p = path;

                        যদি (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                যদি (p <= original_path)
                                        প্রত্যাগম রিক্ত;

                                p--;

                                যদি (*p != '/')
                                        প্রত্যাগম রিক্ত;
                        }

                        প্রত্যাগম (বর্ণ*) p;
                }

                যদি (m != n)
                        প্রত্যাগম রিক্ত;

                যদি (!strneq(p, q, m))
                        প্রত্যাগম রিক্ত;
        }
}

পূর্ণাঙ্ক path_compare(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b) {
        পূর্ণাঙ্ক r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        যদি (r != 0)
                প্রত্যাগম r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        যদি (r != 0)
                প্রত্যাগম r;

        প্রতি (;;) {
                ধ্রুব বর্ণ *aa, *bb;
                পূর্ণাঙ্ক j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                যদি (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        যদি (r != 0)
                                প্রত্যাগম r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        প্রত্যাগম strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                যদি (j == 0) {
                        যদি (k == 0)
                                প্রত্যাগম 0;
                        প্রত্যাগম -1;
                }
                যদি (k == 0)
                        প্রত্যাগম 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                যদি (r != 0)
                        প্রত্যাগম r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                যদি (r != 0)
                        প্রত্যাগম r;
        }
}

পূর্ণাঙ্ক path_compare_filename(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b) {
        _cleanup_free_ বর্ণ *fa = রিক্ত, *fb = রিক্ত;
        পূর্ণাঙ্ক r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        যদি (r != 0)
                প্রত্যাগম r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        যদি (r != 0)
                প্রত্যাগম r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        যদি (r != 0)
                প্রত্যাগম r;

        /* fallback to use strcmp() if both paths are invalid. */
        যদি (j < 0)
                প্রত্যাগম strcmp(a, b);

        প্রত্যাগম strcmp(fa, fb);
}

পূর্ণাঙ্ক path_equal_or_inode_same_full(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b, পূর্ণাঙ্ক flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        যদি (path_equal(a, b))
                প্রত্যাগম 1;

        যদি (!a || !b)
                প্রত্যাগম 0;

        প্রত্যাগম inode_same(a, b, flags);
}

বর্ণ* path_extend_internal(বর্ণ **x, ...) {
        size_t sz, old_sz;
        বর্ণ *q, *nx;
        ধ্রুব বর্ণ *p;
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
        যাবত্ ((p = va_arg(ap, বর্ণ*)) != POINTER_MAX) {
                size_t add;

                যদি (isempty(p))
                        অনুবর্ত;

                add = 1 + strlen(p);
                যদি (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        প্রত্যাগম রিক্ত;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : রিক্ত, GREEDY_ALLOC_ROUND_UP(sz+1));
        যদি (!nx)
                প্রত্যাগম রিক্ত;
        যদি (x)
                *x = nx;

        যদি (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        অন্যথা {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        যাবত্ ((p = va_arg(ap, বর্ণ*)) != POINTER_MAX) {
                যদি (isempty(p))
                        অনুবর্ত;

                যদি (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        প্রত্যাগম nx;
}

পূর্ণাঙ্ক open_and_check_executable(ধ্রুব বর্ণ *name, ধ্রুব বর্ণ *মূল, বর্ণ **ret_path, পূর্ণাঙ্ক *ret_fd) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        _cleanup_free_ বর্ণ *resolved = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        যদি (মূল) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, মূল, CHASE_PREFIX_মূল, &resolved, &fd);
                যদি (r < 0)
                        প্রত্যাগম r;

                name = resolved;
        } অন্যথা {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                যদি (fd < 0)
                        প্রত্যাগম -errno;
        }

        r = fd_verify_regular(fd);
        যদি (r < 0)
                প্রত্যাগম r;

        r = access_fd(fd, X_OK);
        যদি (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (ret_path) {
                যদি (resolved)
                        *ret_path = TAKE_PTR(resolved);
                অন্যথা {
                        r = path_make_absolute_cwd(name, ret_path);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        path_simplify(*ret_path);
                }
        }

        যদি (ret_fd)
                *ret_fd = TAKE_FD(fd);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক find_executable_full(
                ধ্রুব বর্ণ *name,
                ধ্রুব বর্ণ *মূল,
                বর্ণ * ধ্রুব *exec_অনুসন্ধান_path,
                bool use_path_envvar,
                বর্ণ **ret_filename,
                পূর্ণাঙ্ক *ret_fd) {

        পূর্ণাঙ্ক last_error = -ENOENT, r = 0;

        প্রতিজ্ঞা(name);

        যদি (is_path(name))
                প্রত্যাগম open_and_check_executable(name, মূল, ret_filename, ret_fd);

        যদি (exec_অনুসন্ধান_path) {
                STRV_FOREACH(element, exec_অনুসন্ধান_path) {
                        _cleanup_free_ বর্ণ *full_path = রিক্ত;

                        যদি (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                অনুবর্ত;
                        }

                        full_path = path_join(*element, name);
                        যদি (!full_path)
                                প্রত্যাগম -ENOMEM;

                        r = open_and_check_executable(full_path, মূল, ret_filename, ret_fd);
                        যদি (r >= 0)
                                প্রত্যাগম 0;
                        যদি (r != -EACCES)
                                last_error = r;
                }
                প্রত্যাগম last_error;
        }

        ধ্রুব বর্ণ *p = রিক্ত;

        যদি (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        যদি (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        প্রতি (;;) {
                _cleanup_free_ বর্ণ *element = রিক্ত;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        ভঙ্গ;

                যদি (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        অনুবর্ত;
                }

                যদি (!path_extend(&element, name))
                        প্রত্যাগম -ENOMEM;

                r = open_and_check_executable(element, মূল, ret_filename, ret_fd);
                যদি (r >= 0) /* Found it! */
                        প্রত্যাগম 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                যদি (r != -EACCES)
                        last_error = r;
        }

        প্রত্যাগম last_error;
}

স্থির পূর্ণাঙ্ক executable_is_good(ধ্রুব বর্ণ *executable) {
        _cleanup_free_ বর্ণ *p = রিক্ত, *d = রিক্ত;
        পূর্ণাঙ্ক r;

        r = find_executable(executable, &p);
        যদি (r == -ENOENT)
                প্রত্যাগম 0;
        যদি (r < 0)
                প্রত্যাগম r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        যদি (r == -EINVAL) /* not a symlink */
                প্রত্যাগম 1;
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

পূর্ণাঙ্ক fsck_exists(শূন্য) {
        প্রত্যাগম executable_is_good("fsck");
}

পূর্ণাঙ্ক fsck_exists_for_fstype(ধ্রুব বর্ণ *fstype) {
        ধ্রুব বর্ণ *checker;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fstype);

        যদি (streq(fstype, "auto"))
                প্রত্যাগম -EINVAL;

        r = fsck_exists();
        যদি (r <= 0)
                প্রত্যাগম r;

        checker = strjoina("fsck.", fstype);
        প্রত্যাগম executable_is_good(checker);
}

স্থির ধ্রুব বর্ণ* skip_slash_or_dot(ধ্রুব বর্ণ *p) {
        POINTER_MAY_BE_NULL(p);

        প্রতি (; !isempty(p); p++) {
                যদি (*p == '/')
                        অনুবর্ত;
                যদি (startswith(p, "./")) {
                        p++;
                        অনুবর্ত;
                }
                ভঙ্গ;
        }
        প্রত্যাগম p;
}

পূর্ণাঙ্ক path_find_first_component(ধ্রুব বর্ণ **p, bool accept_dot_dot, ধ্রুব বর্ণ **ret) {
        ধ্রুব বর্ণ *q, *first, *end_first, *next;
        size_t len;

        প্রতিজ্ঞা(p);

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
        যদি (isempty(first)) {
                *p = first;
                যদি (ret)
                        *ret = রিক্ত;
                প্রত্যাগম 0;
        }
        যদি (streq(first, ".")) {
                *p = first + 1;
                যদি (ret)
                        *ret = রিক্ত;
                প্রত্যাগম 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        যদি (len > NAME_MAX)
                প্রত্যাগম -EINVAL;
        যদি (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                প্রত্যাগম -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        যদি (ret)
                *ret = first;
        প্রত্যাগম len;
}

স্থির ধ্রুব বর্ণ* skip_slash_or_dot_backward(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *q) {
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(!q || q >= path);

        প্রতি (; q; q = PTR_SUB1(q, path)) {
                যদি (*q == '/')
                        অনুবর্ত;
                যদি (q > path && strneq(q - 1, "/.", 2))
                        অনুবর্ত;
                যদি (q == path && *q == '.')
                        অনুবর্ত;
                ভঙ্গ;
        }
        প্রত্যাগম q;
}

পূর্ণাঙ্ক path_find_last_component(ধ্রুব বর্ণ *path, bool accept_dot_dot, ধ্রুব বর্ণ **next, ধ্রুব বর্ণ **ret) {
        ধ্রুব বর্ণ *q, *last_end, *last_begin;
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

        যদি (isempty(path)) {
                যদি (next)
                        *next = path;
                যদি (ret)
                        *ret = রিক্ত;
                প্রত্যাগম 0;
        }

        যদি (next && *next) {
                যদি (*next < path || *next > path + strlen(path))
                        প্রত্যাগম -EINVAL;
                যদি (*next == path) {
                        যদি (ret)
                                *ret = রিক্ত;
                        প্রত্যাগম 0;
                }
                যদি (!IN_SET(**next, '\0', '/'))
                        প্রত্যাগম -EINVAL;
                q = *next - 1;
        } অন্যথা
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        যদি (!q) { /* the root directory, "." or "./" */
                যদি (next)
                        *next = path;
                যদি (ret)
                        *ret = রিক্ত;
                প্রত্যাগম 0;
        }

        last_end = q + 1;

        যাবত্ (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        যদি (len > NAME_MAX)
                প্রত্যাগম -EINVAL;
        যদি (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                প্রত্যাগম -EINVAL;

        যদি (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        যদি (ret)
                *ret = last_begin;
        প্রত্যাগম len;
}

ধ্রুব বর্ণ* last_path_component(ধ্রুব বর্ণ *path) {

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

        যদি (!path)
                প্রত্যাগম রিক্ত;

        l = k = strlen(path);
        যদি (l == 0) /* special case — an empty string */
                প্রত্যাগম path;

        যাবত্ (k > 0 && path[k-1] == '/')
                k--;

        যদি (k == 0) /* the root directory */
                প্রত্যাগম path + l - 1;

        যাবত্ (k > 0 && path[k-1] != '/')
                k--;

        প্রত্যাগম path + k;
}

পূর্ণাঙ্ক path_split_prefix_filename(ধ্রুব বর্ণ *path, বর্ণ **ret_dir, বর্ণ **ret_filename) {
        _cleanup_free_ বর্ণ *d = রিক্ত;
        ধ্রুব বর্ণ *c, *next = রিক্ত;
        পূর্ণাঙ্ক r;

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

        যদি (isempty(path))
                প্রত্যাগম -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r == 0) /* root directory or "." */
                প্রত্যাগম -EADDRNOTAVAIL;

        যদি (ret_dir) {
                যদি (next == path) {
                        যদি (*path != '/') { /* filename only */
                                যদি (!ret_filename)
                                        প্রত্যাগম -EDESTADDRREQ;
                        } অন্যথা {
                                d = strdup("/");
                                যদি (!d)
                                        প্রত্যাগম -ENOMEM;
                        }
                } অন্যথা {
                        d = strndup(path, next - path);
                        যদি (!d)
                                প্রত্যাগম -ENOMEM;

                        path_simplify(d);

                        যদি (!path_is_valid(d))
                                প্রত্যাগম -EINVAL;
                }

        } অন্যথা যদি (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                প্রত্যাগম -EINVAL;

        যদি (ret_filename) {
                বর্ণ *fn = strndup(c, r);
                যদি (!fn)
                        প্রত্যাগম -ENOMEM;

                *ret_filename = fn;
        }

        যদি (ret_dir)
                *ret_dir = TAKE_PTR(d);

        প্রত্যাগম strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ধ্রুব বর্ণ *p) {
        ধ্রুব বর্ণ *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        যদি (!p)
                প্রত্যাগম false;

        e = strchrnul(p, '/');
        যদি (*e != 0)
                প্রত্যাগম false;

        যদি (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                প্রত্যাগম false;

        প্রত্যাগম true;
}

bool filename_is_valid(ধ্রুব বর্ণ *p) {

        যদি (isempty(p))
                প্রত্যাগম false;

        যদি (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                প্রত্যাগম false;

        প্রত্যাগম filename_part_is_valid(p);
}

bool path_is_valid_full(ধ্রুব বর্ণ *p, bool accept_dot_dot) {
        যদি (isempty(p))
                প্রত্যাগম false;

        প্রতি (ধ্রুব বর্ণ *e = p;;) {
                পূর্ণাঙ্ক r;

                r = path_find_first_component(&e, accept_dot_dot, রিক্ত);
                যদি (r < 0)
                        প্রত্যাগম false;

                যদি (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        প্রত্যাগম false;
                যদি (*e == 0)           /* End of string? Yay! */
                        প্রত্যাগম true;
        }
}

bool path_is_normalized(ধ্রুব বর্ণ *p) {
        যদি (!path_is_safe(p))
                প্রত্যাগম false;

        যদি (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                প্রত্যাগম false;

        যদি (strstr(p, "//"))
                প্রত্যাগম false;

        প্রত্যাগম true;
}

পূর্ণাঙ্ক file_in_same_dir(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *filename, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *b = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(filename);
        প্রতিজ্ঞা(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        যদি (path_is_absolute(filename))
                b = strdup(filename);
        অন্যথা {
                _cleanup_free_ বর্ণ *dn = রিক্ত;

                r = path_extract_directory(path, &dn);
                যদি (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                অন্যথা যদি (r < 0)
                        প্রত্যাগম r;
                অন্যথা
                        b = path_join(dn, filename);
        }
        যদি (!b)
                প্রত্যাগম -ENOMEM;

        *ret = TAKE_PTR(b);
        প্রত্যাগম 0;
}

bool hidden_or_backup_file(ধ্রুব বর্ণ *filename) {
        প্রতিজ্ঞা(filename);

        যদি (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                প্রত্যাগম true;

        ধ্রুব বর্ণ *dot = strrchr(filename, '.');
        যদি (!dot)
                প্রত্যাগম false;

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

        প্রত্যাগম STR_IN_SET(dot + 1,
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

bool is_device_path(ধ্রুব বর্ণ *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ধ্রুব বর্ণ *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        প্রত্যাগম !isempty(p);
}

bool valid_device_গ্রন্থি_path(ধ্রুব বর্ণ *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        যদি (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                প্রত্যাগম false;

        যদি (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                প্রত্যাগম false;

        প্রত্যাগম path_is_normalized(path);
}

bool valid_device_allow_pattern(ধ্রুব বর্ণ *path) {
        প্রতিজ্ঞা(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        যদি (STARTSWITH_SET(path, "block-", "char-"))
                প্রত্যাগম true;

        প্রত্যাগম valid_device_গ্রন্থি_path(path);
}

bool dot_or_dot_dot(ধ্রুব বর্ণ *path) {
        যদি (!path)
                প্রত্যাগম false;
        যদি (path[0] != '.')
                প্রত্যাগম false;
        যদি (path[1] == 0)
                প্রত্যাগম true;
        যদি (path[1] != '.')
                প্রত্যাগম false;

        প্রত্যাগম path[2] == 0;
}

bool path_implies_directory(ধ্রুব বর্ণ *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        যদি (!path)
                প্রত্যাগম false;

        যদি (dot_or_dot_dot(path))
                প্রত্যাগম true;

        প্রত্যাগম ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_মূল(ধ্রুব বর্ণ *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        যদি (isempty(path))
                প্রত্যাগম true;

        প্রত্যাগম path_equal(path, "/");
}

ধ্রুব বর্ণ* empty_to_মূল(ধ্রুব বর্ণ *path) {
        প্রত্যাগম isempty(path) ? "/" : path;
}

পূর্ণাঙ্ক empty_or_মূল_harder_to_null(ধ্রুব বর্ণ **path) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        যদি (empty_or_মূল(*path)) {
                *path = রিক্ত;
                প্রত্যাগম 0;
        }

        r = path_is_মূল(*path);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r > 0)
                *path = রিক্ত;

        প্রত্যাগম 0;
}

bool path_strv_contains(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *path) {
        প্রতিজ্ঞা(path);

        STRV_FOREACH(i, l)
                যদি (path_equal(*i, path))
                        প্রত্যাগম true;

        প্রত্যাগম false;
}

bool prefixed_path_strv_contains(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *path) {
        প্রতিজ্ঞা(path);

        STRV_FOREACH(i, l) {
                ধ্রুব বর্ণ *j = *i;

                যদি (*j == '-')
                        j++;
                যদি (*j == '+')
                        j++;

                যদি (path_equal(j, path))
                        প্রত্যাগম true;
        }

        প্রত্যাগম false;
}

পূর্ণাঙ্ক path_glob_can_match(ধ্রুব বর্ণ *pattern, ধ্রুব বর্ণ *prefix, বর্ণ **ret) {
        প্রতিজ্ঞা(pattern);
        প্রতিজ্ঞা(prefix);

        প্রতি (ধ্রুব বর্ণ *a = pattern, *b = prefix;;) {
                _cleanup_free_ বর্ণ *g = রিক্ত, *h = রিক্ত;
                ধ্রুব বর্ণ *p, *q;
                পূর্ণাঙ্ক r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                যদি (r < 0)
                        প্রত্যাগম r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                যদি (s < 0)
                        প্রত্যাগম s;

                যদি (s == 0) {
                        /* The pattern matches the prefix. */
                        যদি (ret) {
                                বর্ণ *t;

                                t = path_join(prefix, p);
                                যদি (!t)
                                        প্রত্যাগম -ENOMEM;

                                *ret = t;
                        }
                        প্রত্যাগম true;
                }

                যদি (r == 0)
                        ভঙ্গ;

                যদি (r == s && strneq(p, q, r))
                        অনুবর্ত; /* common component. Check next. */

                g = strndup(p, r);
                যদি (!g)
                        প্রত্যাগম -ENOMEM;

                যদি (!string_is_glob(g))
                        ভঙ্গ;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                যদি (!h)
                        প্রত্যাগম -ENOMEM;

                r = fnmatch(g, h, 0);
                যদি (r == FNM_NOMATCH)
                        ভঙ্গ;
                যদি (r != 0) /* Failure to process pattern? */
                        প্রত্যাগম -EINVAL;
        }

        /* The pattern does not match the prefix. */
        যদি (ret)
                *ret = রিক্ত;
        প্রত্যাগম false;
}

#যদি HAVE_SPLIT_BIN
স্থির bool dir_is_split(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b) {
        পূর্ণাঙ্ক r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        যদি (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                প্রত্যাগম true;
        }
        প্রত্যাগম r == 0;
}
#endif

ধ্রুব বর্ণ* default_PATH(শূন্য) {
#যদি HAVE_SPLIT_BIN
        স্থির ধ্রুব বর্ণ *default_path = রিক্ত;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        যদি (default_path)
                প্রত্যাগম default_path;

        যদি (dir_is_split("/usr/sbin", "/usr/bin"))
                প্রত্যাগম (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        যদি (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                প্রত্যাগম (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        প্রত্যাগম (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#অন্যথা
        প্রত্যাগম DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
