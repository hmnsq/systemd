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

bool is_path(ধ্রুবক অক্ষর *p) {
        যদি (!p) /* A NULL pointer is definitely not a path */
                ফেরত false;

        ফেরত strchr(p, '/');
}

পূর্ণসংখ্যা path_split_and_make_absolute(ধ্রুবক অক্ষর *p, অক্ষর ***ret) {
        _cleanup_strv_free_ অক্ষর **l = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        l = strv_split(p, ":");
        যদি (!l)
                ফেরত -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        যদি (r < 0)
                ফেরত r;

        *ret = TAKE_PTR(l);
        ফেরত r;
}

অক্ষর* path_make_absolute(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *prefix) {
        প্রতিজ্ঞা(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        যদি (path_is_absolute(p) || isempty(prefix))
                ফেরত strdup(p);

        ফেরত path_join(prefix, p);
}

পূর্ণসংখ্যা safe_getcwd(অক্ষর **ret) {
        _cleanup_free_ অক্ষর *cwd = ফাঁকা;

        cwd = get_current_dir_name();
        যদি (!cwd)
                ফেরত negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        যদি (cwd[0] != '/')
                ফেরত -ENOMEDIUM;

        যদি (ret)
                *ret = TAKE_PTR(cwd);

        ফেরত 0;
}

পূর্ণসংখ্যা path_make_absolute_cwd(ধ্রুবক অক্ষর *p, অক্ষর **ret) {
        অক্ষর *c;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        যদি (path_is_absolute(p))
                c = strdup(p);
        অন্যথা {
                _cleanup_free_ অক্ষর *cwd = ফাঁকা;

                r = safe_getcwd(&cwd);
                যদি (r < 0)
                        ফেরত r;

                c = path_join(cwd, p);
        }
        যদি (!c)
                ফেরত -ENOMEM;

        *ret = c;
        ফেরত 0;
}

পূর্ণসংখ্যা path_make_relative(ধ্রুবক অক্ষর *from, ধ্রুবক অক্ষর *to, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *ফলাফল = ফাঁকা;
        unsigned n_parents;
        ধ্রুবক অক্ষর *f, *t;
        পূর্ণসংখ্যা r, k;
        অক্ষর *p;

        প্রতিজ্ঞা(from);
        প্রতিজ্ঞা(to);
        প্রতিজ্ঞা(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        যদি (!path_is_absolute(from) || !path_is_absolute(to))
                ফেরত -EINVAL;

        জন্য (;;) {
                r = path_find_first_component(&from, true, &f);
                যদি (r < 0)
                        ফেরত r;

                k = path_find_first_component(&to, true, &t);
                যদি (k < 0)
                        ফেরত k;

                যদি (r == 0) {
                        /* end of 'from' */
                        যদি (k == 0) {
                                /* from and to are equivalent. */
                                ফলাফল = strdup(".");
                                যদি (!ফলাফল)
                                        ফেরত -ENOMEM;
                        } অন্যথা {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &ফলাফল);
                                যদি (r < 0)
                                        ফেরত r;

                                যদি (!path_is_valid(ফলাফল))
                                        ফেরত -EINVAL;
                        }

                        *ret = TAKE_PTR(ফলাফল);
                        ফেরত 0;
                }

                যদি (r != k || !strneq(f, t, r))
                        বিরতি;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        জন্য (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;
        }

        যদি (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ফেরত -EINVAL;

        ফলাফল = new(অক্ষর, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        যদি (!ফলাফল)
                ফেরত -ENOMEM;

        জন্য (p = ফলাফল; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        যদি (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(ফলাফল);
                ফেরত 0;
        }

        strcpy(p, t);

        path_simplify(ফলাফল);

        যদি (!path_is_valid(ফলাফল))
                ফেরত -EINVAL;

        *ret = TAKE_PTR(ফলাফল);
        ফেরত 0;
}

পূর্ণসংখ্যা path_make_relative_parent(ধ্রুবক অক্ষর *from_child, ধ্রুবক অক্ষর *to, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *from = ফাঁকা;
        পূর্ণসংখ্যা r;

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
                ফেরত r;

        ফেরত path_make_relative(from, to, ret);
}

অক্ষর* path_startswith_strv(ধ্রুবক অক্ষর *p, অক্ষর * ধ্রুবক *strv) {
        প্রতিজ্ঞা(p);

        STRV_FOREACH(s, strv) {
                অক্ষর *t;

                t = path_startswith(p, *s);
                যদি (t)
                        ফেরত t;
        }

        ফেরত ফাঁকা;
}

পূর্ণসংখ্যা path_strv_make_absolute_cwd(অক্ষর **l) {
        পূর্ণসংখ্যা r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                অক্ষর *t;

                r = path_make_absolute_cwd(*s, &t);
                যদি (r < 0)
                        ফেরত r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ফেরত 0;
}

অক্ষর** path_strv_resolve(অক্ষর **l, ধ্রুবক অক্ষর *মূল) {
        unsigned k = 0;
        bool enomem = false;
        পূর্ণসংখ্যা r;

        যদি (strv_isempty(l))
                ফেরত l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ অক্ষর *orig = ফাঁকা;
                অক্ষর *t, *u;

                যদি (মূল) {
                        যদি (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                মুক্ত(*s);
                                চালু;
                        }

                        orig = *s;
                        t = path_join(মূল, orig);
                        যদি (!t) {
                                enomem = true;
                                চালু;
                        }
                } অন্যথা
                        t = *s;

                r = chase(t, মূল, 0, &u, ফাঁকা);
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

                        চালু;
                } অন্যথা যদি (মূল) {
                        অক্ষর *x;

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
                                        চালু;
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

        l[k] = ফাঁকা;

        যদি (enomem)
                ফেরত ফাঁকা;

        ফেরত l;
}

অক্ষর** path_strv_resolve_uniq(অক্ষর **l, ধ্রুবক অক্ষর *মূল) {

        যদি (strv_isempty(l))
                ফেরত l;

        যদি (!path_strv_resolve(l, মূল))
                ফেরত ফাঁকা;

        ফেরত strv_uniq(l);
}

অক্ষর* skip_leading_slash(ধ্রুবক অক্ষর *p) {
        ফেরত skip_leading_chars(p, "/");
}

অক্ষর* path_simplify_full(অক্ষর *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        অক্ষর *f = path;
        পূর্ণসংখ্যা r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        যদি (isempty(path))
                ফেরত path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        জন্য (ধ্রুবক অক্ষর *p = f;;) {
                ধ্রুবক অক্ষর *e;

                r = path_find_first_component(&p, true, &e);
                যদি (r == 0)
                        বিরতি;

                যদি (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        চালু;

                beginning = false;

                যদি (add_slash)
                        *f++ = '/';

                যদি (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ফেরত path;
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
        ফেরত path;
}

পূর্ণসংখ্যা path_simplify_alloc(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        প্রতিজ্ঞা(ret);

        যদি (!path) {
                *ret = ফাঁকা;
                ফেরত 0;
        }

        অক্ষর *t = strdup(path);
        যদি (!t)
                ফেরত -ENOMEM;

        *ret = path_simplify(t);
        ফেরত 0;
}

অক্ষর* path_startswith_full(ধ্রুবক অক্ষর *original_path, ধ্রুবক অক্ষর *prefix, PathStartWithFlags flags) {
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

        ধ্রুবক অক্ষর *path = original_path;

        যদি ((path[0] == '/') != (prefix[0] == '/'))
                ফেরত ফাঁকা;

        জন্য (;;) {
                ধ্রুবক অক্ষর *p, *q;
                পূর্ণসংখ্যা m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                যদি (m < 0)
                        ফেরত ফাঁকা;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                যদি (n < 0)
                        ফেরত ফাঁকা;

                যদি (n == 0) {
                        যদি (!p)
                                p = path;

                        যদি (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                যদি (p <= original_path)
                                        ফেরত ফাঁকা;

                                p--;

                                যদি (*p != '/')
                                        ফেরত ফাঁকা;
                        }

                        ফেরত (অক্ষর*) p;
                }

                যদি (m != n)
                        ফেরত ফাঁকা;

                যদি (!strneq(p, q, m))
                        ফেরত ফাঁকা;
        }
}

পূর্ণসংখ্যা path_compare(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
        পূর্ণসংখ্যা r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        যদি (r != 0)
                ফেরত r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        যদি (r != 0)
                ফেরত r;

        জন্য (;;) {
                ধ্রুবক অক্ষর *aa, *bb;
                পূর্ণসংখ্যা j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                যদি (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        যদি (r != 0)
                                ফেরত r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ফেরত strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                যদি (j == 0) {
                        যদি (k == 0)
                                ফেরত 0;
                        ফেরত -1;
                }
                যদি (k == 0)
                        ফেরত 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                যদি (r != 0)
                        ফেরত r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                যদি (r != 0)
                        ফেরত r;
        }
}

পূর্ণসংখ্যা path_compare_filename(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
        _cleanup_free_ অক্ষর *fa = ফাঁকা, *fb = ফাঁকা;
        পূর্ণসংখ্যা r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        যদি (r != 0)
                ফেরত r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        যদি (r != 0)
                ফেরত r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        যদি (r != 0)
                ফেরত r;

        /* fallback to use strcmp() if both paths are invalid. */
        যদি (j < 0)
                ফেরত strcmp(a, b);

        ফেরত strcmp(fa, fb);
}

পূর্ণসংখ্যা path_equal_or_inode_same_full(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b, পূর্ণসংখ্যা flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        যদি (path_equal(a, b))
                ফেরত 1;

        যদি (!a || !b)
                ফেরত 0;

        ফেরত inode_same(a, b, flags);
}

অক্ষর* path_extend_internal(অক্ষর **x, ...) {
        size_t sz, old_sz;
        অক্ষর *q, *nx;
        ধ্রুবক অক্ষর *p;
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
        যখন ((p = va_arg(ap, অক্ষর*)) != POINTER_MAX) {
                size_t add;

                যদি (isempty(p))
                        চালু;

                add = 1 + strlen(p);
                যদি (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ফেরত ফাঁকা;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : ফাঁকা, GREEDY_ALLOC_ROUND_UP(sz+1));
        যদি (!nx)
                ফেরত ফাঁকা;
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
        যখন ((p = va_arg(ap, অক্ষর*)) != POINTER_MAX) {
                যদি (isempty(p))
                        চালু;

                যদি (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ফেরত nx;
}

পূর্ণসংখ্যা open_and_check_executable(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *মূল, অক্ষর **ret_path, পূর্ণসংখ্যা *ret_fd) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        _cleanup_free_ অক্ষর *resolved = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        যদি (মূল) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, মূল, CHASE_PREFIX_মূল, &resolved, &fd);
                যদি (r < 0)
                        ফেরত r;

                name = resolved;
        } অন্যথা {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                যদি (fd < 0)
                        ফেরত -errno;
        }

        r = fd_verify_regular(fd);
        যদি (r < 0)
                ফেরত r;

        r = access_fd(fd, X_OK);
        যদি (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path) {
                যদি (resolved)
                        *ret_path = TAKE_PTR(resolved);
                অন্যথা {
                        r = path_make_absolute_cwd(name, ret_path);
                        যদি (r < 0)
                                ফেরত r;

                        path_simplify(*ret_path);
                }
        }

        যদি (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ফেরত 0;
}

পূর্ণসংখ্যা find_executable_full(
                ধ্রুবক অক্ষর *name,
                ধ্রুবক অক্ষর *মূল,
                অক্ষর * ধ্রুবক *exec_অনুসন্ধান_path,
                bool use_path_envvar,
                অক্ষর **ret_filename,
                পূর্ণসংখ্যা *ret_fd) {

        পূর্ণসংখ্যা last_error = -ENOENT, r = 0;

        প্রতিজ্ঞা(name);

        যদি (is_path(name))
                ফেরত open_and_check_executable(name, মূল, ret_filename, ret_fd);

        যদি (exec_অনুসন্ধান_path) {
                STRV_FOREACH(element, exec_অনুসন্ধান_path) {
                        _cleanup_free_ অক্ষর *full_path = ফাঁকা;

                        যদি (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                চালু;
                        }

                        full_path = path_join(*element, name);
                        যদি (!full_path)
                                ফেরত -ENOMEM;

                        r = open_and_check_executable(full_path, মূল, ret_filename, ret_fd);
                        যদি (r >= 0)
                                ফেরত 0;
                        যদি (r != -EACCES)
                                last_error = r;
                }
                ফেরত last_error;
        }

        ধ্রুবক অক্ষর *p = ফাঁকা;

        যদি (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        যদি (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        জন্য (;;) {
                _cleanup_free_ অক্ষর *element = ফাঁকা;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                যদি (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        চালু;
                }

                যদি (!path_extend(&element, name))
                        ফেরত -ENOMEM;

                r = open_and_check_executable(element, মূল, ret_filename, ret_fd);
                যদি (r >= 0) /* Found it! */
                        ফেরত 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                যদি (r != -EACCES)
                        last_error = r;
        }

        ফেরত last_error;
}

স্থির পূর্ণসংখ্যা executable_is_good(ধ্রুবক অক্ষর *executable) {
        _cleanup_free_ অক্ষর *p = ফাঁকা, *d = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = find_executable(executable, &p);
        যদি (r == -ENOENT)
                ফেরত 0;
        যদি (r < 0)
                ফেরত r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        যদি (r == -EINVAL) /* not a symlink */
                ফেরত 1;
        যদি (r < 0)
                ফেরত r;

        ফেরত !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

পূর্ণসংখ্যা fsck_exists(শূন্য) {
        ফেরত executable_is_good("fsck");
}

পূর্ণসংখ্যা fsck_exists_for_fstype(ধ্রুবক অক্ষর *fstype) {
        ধ্রুবক অক্ষর *checker;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fstype);

        যদি (streq(fstype, "auto"))
                ফেরত -EINVAL;

        r = fsck_exists();
        যদি (r <= 0)
                ফেরত r;

        checker = strjoina("fsck.", fstype);
        ফেরত executable_is_good(checker);
}

স্থির ধ্রুবক অক্ষর* skip_slash_or_dot(ধ্রুবক অক্ষর *p) {
        POINTER_MAY_BE_NULL(p);

        জন্য (; !isempty(p); p++) {
                যদি (*p == '/')
                        চালু;
                যদি (startswith(p, "./")) {
                        p++;
                        চালু;
                }
                বিরতি;
        }
        ফেরত p;
}

পূর্ণসংখ্যা path_find_first_component(ধ্রুবক অক্ষর **p, bool accept_dot_dot, ধ্রুবক অক্ষর **ret) {
        ধ্রুবক অক্ষর *q, *first, *end_first, *পরবর্তী_2;
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
                        *ret = ফাঁকা;
                ফেরত 0;
        }
        যদি (streq(first, ".")) {
                *p = first + 1;
                যদি (ret)
                        *ret = ফাঁকা;
                ফেরত 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        যদি (len > NAME_MAX)
                ফেরত -EINVAL;
        যদি (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ফেরত -EINVAL;

        পরবর্তী_2 = skip_slash_or_dot(end_first);

        *p = পরবর্তী_2 + streq(পরবর্তী_2, ".");
        যদি (ret)
                *ret = first;
        ফেরত len;
}

স্থির ধ্রুবক অক্ষর* skip_slash_or_dot_backward(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *q) {
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(!q || q >= path);

        জন্য (; q; q = PTR_SUB1(q, path)) {
                যদি (*q == '/')
                        চালু;
                যদি (q > path && strneq(q - 1, "/.", 2))
                        চালু;
                যদি (q == path && *q == '.')
                        চালু;
                বিরতি;
        }
        ফেরত q;
}

পূর্ণসংখ্যা path_find_last_component(ধ্রুবক অক্ষর *path, bool accept_dot_dot, ধ্রুবক অক্ষর **পরবর্তী_2, ধ্রুবক অক্ষর **ret) {
        ধ্রুবক অক্ষর *q, *last_end, *last_begin;
        size_t len;

        POINTER_MAY_BE_NULL(পরবর্তী_2);
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
                যদি (পরবর্তী_2)
                        *পরবর্তী_2 = path;
                যদি (ret)
                        *ret = ফাঁকা;
                ফেরত 0;
        }

        যদি (পরবর্তী_2 && *পরবর্তী_2) {
                যদি (*পরবর্তী_2 < path || *পরবর্তী_2 > path + strlen(path))
                        ফেরত -EINVAL;
                যদি (*পরবর্তী_2 == path) {
                        যদি (ret)
                                *ret = ফাঁকা;
                        ফেরত 0;
                }
                যদি (!IN_SET(**পরবর্তী_2, '\0', '/'))
                        ফেরত -EINVAL;
                q = *পরবর্তী_2 - 1;
        } অন্যথা
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        যদি (!q) { /* the root directory, "." or "./" */
                যদি (পরবর্তী_2)
                        *পরবর্তী_2 = path;
                যদি (ret)
                        *ret = ফাঁকা;
                ফেরত 0;
        }

        last_end = q + 1;

        যখন (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        যদি (len > NAME_MAX)
                ফেরত -EINVAL;
        যদি (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ফেরত -EINVAL;

        যদি (পরবর্তী_2) {
                q = skip_slash_or_dot_backward(path, q);
                *পরবর্তী_2 = q ? q + 1 : path;
        }

        যদি (ret)
                *ret = last_begin;
        ফেরত len;
}

ধ্রুবক অক্ষর* last_path_component(ধ্রুবক অক্ষর *path) {

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
                ফেরত ফাঁকা;

        l = k = strlen(path);
        যদি (l == 0) /* special case — an empty string */
                ফেরত path;

        যখন (k > 0 && path[k-1] == '/')
                k--;

        যদি (k == 0) /* the root directory */
                ফেরত path + l - 1;

        যখন (k > 0 && path[k-1] != '/')
                k--;

        ফেরত path + k;
}

পূর্ণসংখ্যা path_split_prefix_filename(ধ্রুবক অক্ষর *path, অক্ষর **ret_dir, অক্ষর **ret_filename) {
        _cleanup_free_ অক্ষর *d = ফাঁকা;
        ধ্রুবক অক্ষর *c, *পরবর্তী_2 = ফাঁকা;
        পূর্ণসংখ্যা r;

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
                ফেরত -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &পরবর্তী_2, &c);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0) /* root directory or "." */
                ফেরত -EADDRNOTAVAIL;

        যদি (ret_dir) {
                যদি (পরবর্তী_2 == path) {
                        যদি (*path != '/') { /* filename only */
                                যদি (!ret_filename)
                                        ফেরত -EDESTADDRREQ;
                        } অন্যথা {
                                d = strdup("/");
                                যদি (!d)
                                        ফেরত -ENOMEM;
                        }
                } অন্যথা {
                        d = strndup(path, পরবর্তী_2 - path);
                        যদি (!d)
                                ফেরত -ENOMEM;

                        path_simplify(d);

                        যদি (!path_is_valid(d))
                                ফেরত -EINVAL;
                }

        } অন্যথা যদি (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ফেরত -EINVAL;

        যদি (ret_filename) {
                অক্ষর *fn = strndup(c, r);
                যদি (!fn)
                        ফেরত -ENOMEM;

                *ret_filename = fn;
        }

        যদি (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ফেরত strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ধ্রুবক অক্ষর *p) {
        ধ্রুবক অক্ষর *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        যদি (!p)
                ফেরত false;

        e = strchrnul(p, '/');
        যদি (*e != 0)
                ফেরত false;

        যদি (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ফেরত false;

        ফেরত true;
}

bool filename_is_valid(ধ্রুবক অক্ষর *p) {

        যদি (isempty(p))
                ফেরত false;

        যদি (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ফেরত false;

        ফেরত filename_part_is_valid(p);
}

bool path_is_valid_full(ধ্রুবক অক্ষর *p, bool accept_dot_dot) {
        যদি (isempty(p))
                ফেরত false;

        জন্য (ধ্রুবক অক্ষর *e = p;;) {
                পূর্ণসংখ্যা r;

                r = path_find_first_component(&e, accept_dot_dot, ফাঁকা);
                যদি (r < 0)
                        ফেরত false;

                যদি (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ফেরত false;
                যদি (*e == 0)           /* End of string? Yay! */
                        ফেরত true;
        }
}

bool path_is_normalized(ধ্রুবক অক্ষর *p) {
        যদি (!path_is_safe(p))
                ফেরত false;

        যদি (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ফেরত false;

        যদি (strstr(p, "//"))
                ফেরত false;

        ফেরত true;
}

পূর্ণসংখ্যা file_in_same_dir(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *filename, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *b = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(filename);
        প্রতিজ্ঞা(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        যদি (path_is_absolute(filename))
                b = strdup(filename);
        অন্যথা {
                _cleanup_free_ অক্ষর *dn = ফাঁকা;

                r = path_extract_directory(path, &dn);
                যদি (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                অন্যথা যদি (r < 0)
                        ফেরত r;
                অন্যথা
                        b = path_join(dn, filename);
        }
        যদি (!b)
                ফেরত -ENOMEM;

        *ret = TAKE_PTR(b);
        ফেরত 0;
}

bool hidden_or_backup_file(ধ্রুবক অক্ষর *filename) {
        প্রতিজ্ঞা(filename);

        যদি (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ফেরত true;

        ধ্রুবক অক্ষর *dot = strrchr(filename, '.');
        যদি (!dot)
                ফেরত false;

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

        ফেরত STR_IN_SET(dot + 1,
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

bool is_device_path(ধ্রুবক অক্ষর *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ধ্রুবক অক্ষর *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        ফেরত !isempty(p);
}

bool valid_device_গ্রন্থি_path(ধ্রুবক অক্ষর *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        যদি (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ফেরত false;

        যদি (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ফেরত false;

        ফেরত path_is_normalized(path);
}

bool valid_device_allow_pattern(ধ্রুবক অক্ষর *path) {
        প্রতিজ্ঞা(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        যদি (STARTSWITH_SET(path, "block-", "char-"))
                ফেরত true;

        ফেরত valid_device_গ্রন্থি_path(path);
}

bool dot_or_dot_dot(ধ্রুবক অক্ষর *path) {
        যদি (!path)
                ফেরত false;
        যদি (path[0] != '.')
                ফেরত false;
        যদি (path[1] == 0)
                ফেরত true;
        যদি (path[1] != '.')
                ফেরত false;

        ফেরত path[2] == 0;
}

bool path_implies_directory(ধ্রুবক অক্ষর *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        যদি (!path)
                ফেরত false;

        যদি (dot_or_dot_dot(path))
                ফেরত true;

        ফেরত ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_মূল(ধ্রুবক অক্ষর *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        যদি (isempty(path))
                ফেরত true;

        ফেরত path_equal(path, "/");
}

ধ্রুবক অক্ষর* empty_to_মূল(ধ্রুবক অক্ষর *path) {
        ফেরত isempty(path) ? "/" : path;
}

পূর্ণসংখ্যা empty_or_মূল_harder_to_null(ধ্রুবক অক্ষর **path) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        যদি (empty_or_মূল(*path)) {
                *path = ফাঁকা;
                ফেরত 0;
        }

        r = path_is_মূল(*path);
        যদি (r < 0)
                ফেরত r;
        যদি (r > 0)
                *path = ফাঁকা;

        ফেরত 0;
}

bool path_strv_contains(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *path) {
        প্রতিজ্ঞা(path);

        STRV_FOREACH(i, l)
                যদি (path_equal(*i, path))
                        ফেরত true;

        ফেরত false;
}

bool prefixed_path_strv_contains(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *path) {
        প্রতিজ্ঞা(path);

        STRV_FOREACH(i, l) {
                ধ্রুবক অক্ষর *j = *i;

                যদি (*j == '-')
                        j++;
                যদি (*j == '+')
                        j++;

                যদি (path_equal(j, path))
                        ফেরত true;
        }

        ফেরত false;
}

পূর্ণসংখ্যা path_glob_can_match(ধ্রুবক অক্ষর *pattern, ধ্রুবক অক্ষর *prefix, অক্ষর **ret) {
        প্রতিজ্ঞা(pattern);
        প্রতিজ্ঞা(prefix);

        জন্য (ধ্রুবক অক্ষর *a = pattern, *b = prefix;;) {
                _cleanup_free_ অক্ষর *g = ফাঁকা, *h = ফাঁকা;
                ধ্রুবক অক্ষর *p, *q;
                পূর্ণসংখ্যা r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                যদি (r < 0)
                        ফেরত r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                যদি (s < 0)
                        ফেরত s;

                যদি (s == 0) {
                        /* The pattern matches the prefix. */
                        যদি (ret) {
                                অক্ষর *t;

                                t = path_join(prefix, p);
                                যদি (!t)
                                        ফেরত -ENOMEM;

                                *ret = t;
                        }
                        ফেরত true;
                }

                যদি (r == 0)
                        বিরতি;

                যদি (r == s && strneq(p, q, r))
                        চালু; /* common component. Check next. */

                g = strndup(p, r);
                যদি (!g)
                        ফেরত -ENOMEM;

                যদি (!string_is_glob(g))
                        বিরতি;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                যদি (!h)
                        ফেরত -ENOMEM;

                r = fnmatch(g, h, 0);
                যদি (r == FNM_NOMATCH)
                        বিরতি;
                যদি (r != 0) /* Failure to process pattern? */
                        ফেরত -EINVAL;
        }

        /* The pattern does not match the prefix. */
        যদি (ret)
                *ret = ফাঁকা;
        ফেরত false;
}

#যদি HAVE_SPLIT_BIN
স্থির bool dir_is_split(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
        পূর্ণসংখ্যা r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        যদি (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ফেরত true;
        }
        ফেরত r == 0;
}
#endif

ধ্রুবক অক্ষর* default_PATH(শূন্য) {
#যদি HAVE_SPLIT_BIN
        স্থির ধ্রুবক অক্ষর *default_path = ফাঁকা;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        যদি (default_path)
                ফেরত default_path;

        যদি (dir_is_split("/usr/sbin", "/usr/bin"))
                ফেরত (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        যদি (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ফেরত (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ফেরত (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#অন্যথা
        ফেরত DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
