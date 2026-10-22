/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fnmatch.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "chase.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "glob-util.h"
#ضمن "log.h"
#ضمن "path-util.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"

bool is_path(ثابت حرف *p) {
        ܐܢ (!p) /* A NULL pointer is definitely not a path */
                ܦܢܐ false;

        ܦܢܐ strchr(p, '/');
}

صحيح path_split_and_make_absolute(ثابت حرف *p, حرف ***ret) {
        _cleanup_strv_free_ حرف **l = عدم;
        صحيح r;

        أكد(p);
        أكد(ret);

        l = strv_split(p, ":");
        ܐܢ (!l)
                ܦܢܐ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        ܐܢ (r < 0)
                ܦܢܐ r;

        *ret = TAKE_PTR(l);
        ܦܢܐ r;
}

حرف* path_make_absolute(ثابت حرف *p, ثابت حرف *prefix) {
        أكد(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ܐܢ (path_is_absolute(p) || isempty(prefix))
                ܦܢܐ strdup(p);

        ܦܢܐ path_join(prefix, p);
}

صحيح safe_getcwd(حرف **ret) {
        _cleanup_free_ حرف *cwd = عدم;

        cwd = get_current_dir_name();
        ܐܢ (!cwd)
                ܦܢܐ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ܐܢ (cwd[0] != '/')
                ܦܢܐ -ENOMEDIUM;

        ܐܢ (ret)
                *ret = TAKE_PTR(cwd);

        ܦܢܐ 0;
}

صحيح path_make_absolute_cwd(ثابت حرف *p, حرف **ret) {
        حرف *c;
        صحيح r;

        أكد(p);
        أكد(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ܐܢ (path_is_absolute(p))
                c = strdup(p);
        ܐܚܪܢܐ {
                _cleanup_free_ حرف *cwd = عدم;

                r = safe_getcwd(&cwd);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                c = path_join(cwd, p);
        }
        ܐܢ (!c)
                ܦܢܐ -ENOMEM;

        *ret = c;
        ܦܢܐ 0;
}

صحيح path_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret) {
        _cleanup_free_ حرف *result = عدم;
        unsigned n_parents;
        ثابت حرف *f, *t;
        صحيح r, k;
        حرف *p;

        أكد(from);
        أكد(to);
        أكد(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        ܐܢ (!path_is_absolute(from) || !path_is_absolute(to))
                ܦܢܐ -EINVAL;

        لكل (;;) {
                r = path_find_first_component(&from, true, &f);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                k = path_find_first_component(&to, true, &t);
                ܐܢ (k < 0)
                        ܦܢܐ k;

                ܐܢ (r == 0) {
                        /* end of 'from' */
                        ܐܢ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                ܐܢ (!result)
                                        ܦܢܐ -ENOMEM;
                        } ܐܚܪܢܐ {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                ܐܢ (r < 0)
                                        ܦܢܐ r;

                                ܐܢ (!path_is_valid(result))
                                        ܦܢܐ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ܦܢܐ 0;
                }

                ܐܢ (r != k || !strneq(f, t, r))
                        اكسر;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        لكل (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        اكسر;
        }

        ܐܢ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ܦܢܐ -EINVAL;

        result = new(حرف, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ܐܢ (!result)
                ܦܢܐ -ENOMEM;

        لكل (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ܐܢ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ܦܢܐ 0;
        }

        strcpy(p, t);

        path_simplify(result);

        ܐܢ (!path_is_valid(result))
                ܦܢܐ -EINVAL;

        *ret = TAKE_PTR(result);
        ܦܢܐ 0;
}

صحيح path_make_relative_parent(ثابت حرف *from_child, ثابت حرف *to, حرف **ret) {
        _cleanup_free_ حرف *from = عدم;
        صحيح r;

        أكد(from_child);
        أكد(to);
        أكد(ret);

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
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ path_make_relative(from, to, ret);
}

حرف* path_startswith_strv(ثابت حرف *p, حرف * ثابت *strv) {
        أكد(p);

        STRV_FOREACH(s, strv) {
                حرف *t;

                t = path_startswith(p, *s);
                ܐܢ (t)
                        ܦܢܐ t;
        }

        ܦܢܐ عدم;
}

صحيح path_strv_make_absolute_cwd(حرف **l) {
        صحيح r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                حرف *t;

                r = path_make_absolute_cwd(*s, &t);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ܦܢܐ 0;
}

حرف** path_strv_resolve(حرف **l, ثابت حرف *root) {
        unsigned k = 0;
        bool enomem = false;
        صحيح r;

        ܐܢ (strv_isempty(l))
                ܦܢܐ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ حرف *orig = عدم;
                حرف *t, *u;

                ܐܢ (root) {
                        ܐܢ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                حرر(*s);
                                استمر;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        ܐܢ (!t) {
                                enomem = true;
                                استمر;
                        }
                } ܐܚܪܢܐ
                        t = *s;

                r = chase(t, root, 0, &u, عدم);
                ܐܢ (r == -ENOENT) {
                        ܐܢ (root) {
                                u = TAKE_PTR(orig);
                                حرر(t);
                        } ܐܚܪܢܐ
                                u = t;
                } ܐܚܪܢܐ ܐܢ (r < 0) {
                        حرر(t);

                        ܐܢ (r == -ENOMEM)
                                enomem = true;

                        استمر;
                } ܐܚܪܢܐ ܐܢ (root) {
                        حرف *x;

                        حرر(t);
                        x = path_startswith(u, root);
                        ܐܢ (x) {
                                /* restore the slash if it was lost */
                                ܐܢ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                حرر(u);
                                ܐܢ (!t) {
                                        enomem = true;
                                        استمر;
                                }
                                u = t;
                        } ܐܚܪܢܐ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ܐܚܪܢܐ
                        حرر(t);

                l[k++] = u;
        }

        l[k] = عدم;

        ܐܢ (enomem)
                ܦܢܐ عدم;

        ܦܢܐ l;
}

حرف** path_strv_resolve_uniq(حرف **l, ثابت حرف *root) {

        ܐܢ (strv_isempty(l))
                ܦܢܐ l;

        ܐܢ (!path_strv_resolve(l, root))
                ܦܢܐ عدم;

        ܦܢܐ strv_uniq(l);
}

حرف* skip_leading_slash(ثابت حرف *p) {
        ܦܢܐ skip_leading_chars(p, "/");
}

حرف* path_simplify_full(حرف *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        حرف *f = path;
        صحيح r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        ܐܢ (isempty(path))
                ܦܢܐ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        لكل (ثابت حرف *p = f;;) {
                ثابت حرف *e;

                r = path_find_first_component(&p, true, &e);
                ܐܢ (r == 0)
                        اكسر;

                ܐܢ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        استمر;

                beginning = false;

                ܐܢ (add_slash)
                        *f++ = '/';

                ܐܢ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ܦܢܐ path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ܐܢ (f == path)
                *f++ = '.';

        ܐܢ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ܦܢܐ path;
}

صحيح path_simplify_alloc(ثابت حرف *path, حرف **ret) {
        أكد(ret);

        ܐܢ (!path) {
                *ret = عدم;
                ܦܢܐ 0;
        }

        حرف *t = strdup(path);
        ܐܢ (!t)
                ܦܢܐ -ENOMEM;

        *ret = path_simplify(t);
        ܦܢܐ 0;
}

حرف* path_startswith_full(ثابت حرف *original_path, ثابت حرف *prefix, PathStartWithFlags flags) {
        أكد(original_path);
        أكد(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ثابت حرف *path = original_path;

        ܐܢ ((path[0] == '/') != (prefix[0] == '/'))
                ܦܢܐ عدم;

        لكل (;;) {
                ثابت حرف *p, *q;
                صحيح m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                ܐܢ (m < 0)
                        ܦܢܐ عدم;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                ܐܢ (n < 0)
                        ܦܢܐ عدم;

                ܐܢ (n == 0) {
                        ܐܢ (!p)
                                p = path;

                        ܐܢ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ܐܢ (p <= original_path)
                                        ܦܢܐ عدم;

                                p--;

                                ܐܢ (*p != '/')
                                        ܦܢܐ عدم;
                        }

                        ܦܢܐ (حرف*) p;
                }

                ܐܢ (m != n)
                        ܦܢܐ عدم;

                ܐܢ (!strneq(p, q, m))
                        ܦܢܐ عدم;
        }
}

صحيح path_compare(ثابت حرف *a, ثابت حرف *b) {
        صحيح r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ܐܢ (r != 0)
                ܦܢܐ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        ܐܢ (r != 0)
                ܦܢܐ r;

        لكل (;;) {
                ثابت حرف *aa, *bb;
                صحيح j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                ܐܢ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ܐܢ (r != 0)
                                ܦܢܐ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ܦܢܐ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ܐܢ (j == 0) {
                        ܐܢ (k == 0)
                                ܦܢܐ 0;
                        ܦܢܐ -1;
                }
                ܐܢ (k == 0)
                        ܦܢܐ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ܐܢ (r != 0)
                        ܦܢܐ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ܐܢ (r != 0)
                        ܦܢܐ r;
        }
}

صحيح path_compare_filename(ثابت حرف *a, ثابت حرف *b) {
        _cleanup_free_ حرف *fa = عدم, *fb = عدم;
        صحيح r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ܐܢ (r != 0)
                ܦܢܐ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ܐܢ (r != 0)
                ܦܢܐ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ܐܢ (r != 0)
                ܦܢܐ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ܐܢ (j < 0)
                ܦܢܐ strcmp(a, b);

        ܦܢܐ strcmp(fa, fb);
}

صحيح path_equal_or_inode_same_full(ثابت حرف *a, ثابت حرف *b, صحيح flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ܐܢ (path_equal(a, b))
                ܦܢܐ 1;

        ܐܢ (!a || !b)
                ܦܢܐ 0;

        ܦܢܐ inode_same(a, b, flags);
}

حرف* path_extend_internal(حرف **x, ...) {
        size_t sz, old_sz;
        حرف *q, *nx;
        ثابت حرف *p;
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
        ܟܡܐ ((p = va_arg(ap, حرف*)) != POINTER_MAX) {
                size_t add;

                ܐܢ (isempty(p))
                        استمر;

                add = 1 + strlen(p);
                ܐܢ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ܦܢܐ عدم;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : عدم, GREEDY_ALLOC_ROUND_UP(sz+1));
        ܐܢ (!nx)
                ܦܢܐ عدم;
        ܐܢ (x)
                *x = nx;

        ܐܢ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ܐܚܪܢܐ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ܟܡܐ ((p = va_arg(ap, حرف*)) != POINTER_MAX) {
                ܐܢ (isempty(p))
                        استمر;

                ܐܢ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ܦܢܐ nx;
}

صحيح open_and_check_executable(ثابت حرف *name, ثابت حرف *root, حرف **ret_path, صحيح *ret_fd) {
        _cleanup_close_ صحيح fd = -EBADF;
        _cleanup_free_ حرف *resolved = عدم;
        صحيح r;

        أكد(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        ܐܢ (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                name = resolved;
        } ܐܚܪܢܐ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                ܐܢ (fd < 0)
                        ܦܢܐ -errno;
        }

        r = fd_verify_regular(fd);
        ܐܢ (r < 0)
                ܦܢܐ r;

        r = access_fd(fd, X_OK);
        ܐܢ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (ret_path) {
                ܐܢ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                ܐܚܪܢܐ {
                        r = path_make_absolute_cwd(name, ret_path);
                        ܐܢ (r < 0)
                                ܦܢܐ r;

                        path_simplify(*ret_path);
                }
        }

        ܐܢ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ܦܢܐ 0;
}

صحيح find_executable_full(
                ثابت حرف *name,
                ثابت حرف *root,
                حرف * ثابت *exec_search_path,
                bool use_path_envvar,
                حرف **ret_filename,
                صحيح *ret_fd) {

        صحيح last_error = -ENOENT, r = 0;

        أكد(name);

        ܐܢ (is_path(name))
                ܦܢܐ open_and_check_executable(name, root, ret_filename, ret_fd);

        ܐܢ (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ حرف *full_path = عدم;

                        ܐܢ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                استمر;
                        }

                        full_path = path_join(*element, name);
                        ܐܢ (!full_path)
                                ܦܢܐ -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        ܐܢ (r >= 0)
                                ܦܢܐ 0;
                        ܐܢ (r != -EACCES)
                                last_error = r;
                }
                ܦܢܐ last_error;
        }

        ثابت حرف *p = عدم;

        ܐܢ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ܐܢ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        لكل (;;) {
                _cleanup_free_ حرف *element = عدم;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        اكسر;

                ܐܢ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        استمر;
                }

                ܐܢ (!path_extend(&element, name))
                        ܦܢܐ -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                ܐܢ (r >= 0) /* Found it! */
                        ܦܢܐ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ܐܢ (r != -EACCES)
                        last_error = r;
        }

        ܦܢܐ last_error;
}

ساكن صحيح executable_is_good(ثابت حرف *executable) {
        _cleanup_free_ حرف *p = عدم, *d = عدم;
        صحيح r;

        r = find_executable(executable, &p);
        ܐܢ (r == -ENOENT)
                ܦܢܐ 0;
        ܐܢ (r < 0)
                ܦܢܐ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ܐܢ (r == -EINVAL) /* not a symlink */
                ܦܢܐ 1;
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

صحيح fsck_exists(فراغ) {
        ܦܢܐ executable_is_good("fsck");
}

صحيح fsck_exists_for_fstype(ثابت حرف *fstype) {
        ثابت حرف *checker;
        صحيح r;

        أكد(fstype);

        ܐܢ (streq(fstype, "auto"))
                ܦܢܐ -EINVAL;

        r = fsck_exists();
        ܐܢ (r <= 0)
                ܦܢܐ r;

        checker = strjoina("fsck.", fstype);
        ܦܢܐ executable_is_good(checker);
}

ساكن ثابت حرف* skip_slash_or_dot(ثابت حرف *p) {
        POINTER_MAY_BE_NULL(p);

        لكل (; !isempty(p); p++) {
                ܐܢ (*p == '/')
                        استمر;
                ܐܢ (startswith(p, "./")) {
                        p++;
                        استمر;
                }
                اكسر;
        }
        ܦܢܐ p;
}

صحيح path_find_first_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret) {
        ثابت حرف *q, *first, *end_first, *next;
        size_t len;

        أكد(p);

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
        ܐܢ (isempty(first)) {
                *p = first;
                ܐܢ (ret)
                        *ret = عدم;
                ܦܢܐ 0;
        }
        ܐܢ (streq(first, ".")) {
                *p = first + 1;
                ܐܢ (ret)
                        *ret = عدم;
                ܦܢܐ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        ܐܢ (len > NAME_MAX)
                ܦܢܐ -EINVAL;
        ܐܢ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ܦܢܐ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        ܐܢ (ret)
                *ret = first;
        ܦܢܐ len;
}

ساكن ثابت حرف* skip_slash_or_dot_backward(ثابت حرف *path, ثابت حرف *q) {
        أكد(path);
        أكد(!q || q >= path);

        لكل (; q; q = PTR_SUB1(q, path)) {
                ܐܢ (*q == '/')
                        استمر;
                ܐܢ (q > path && strneq(q - 1, "/.", 2))
                        استمر;
                ܐܢ (q == path && *q == '.')
                        استمر;
                اكسر;
        }
        ܦܢܐ q;
}

صحيح path_find_last_component(ثابت حرف *path, bool accept_dot_dot, ثابت حرف **next, ثابت حرف **ret) {
        ثابت حرف *q, *last_end, *last_begin;
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

        ܐܢ (isempty(path)) {
                ܐܢ (next)
                        *next = path;
                ܐܢ (ret)
                        *ret = عدم;
                ܦܢܐ 0;
        }

        ܐܢ (next && *next) {
                ܐܢ (*next < path || *next > path + strlen(path))
                        ܦܢܐ -EINVAL;
                ܐܢ (*next == path) {
                        ܐܢ (ret)
                                *ret = عدم;
                        ܦܢܐ 0;
                }
                ܐܢ (!IN_SET(**next, '\0', '/'))
                        ܦܢܐ -EINVAL;
                q = *next - 1;
        } ܐܚܪܢܐ
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        ܐܢ (!q) { /* the root directory, "." or "./" */
                ܐܢ (next)
                        *next = path;
                ܐܢ (ret)
                        *ret = عدم;
                ܦܢܐ 0;
        }

        last_end = q + 1;

        ܟܡܐ (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        ܐܢ (len > NAME_MAX)
                ܦܢܐ -EINVAL;
        ܐܢ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ܦܢܐ -EINVAL;

        ܐܢ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        ܐܢ (ret)
                *ret = last_begin;
        ܦܢܐ len;
}

ثابت حرف* last_path_component(ثابت حرف *path) {

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

        ܐܢ (!path)
                ܦܢܐ عدم;

        l = k = strlen(path);
        ܐܢ (l == 0) /* special case — an empty string */
                ܦܢܐ path;

        ܟܡܐ (k > 0 && path[k-1] == '/')
                k--;

        ܐܢ (k == 0) /* the root directory */
                ܦܢܐ path + l - 1;

        ܟܡܐ (k > 0 && path[k-1] != '/')
                k--;

        ܦܢܐ path + k;
}

صحيح path_split_prefix_filename(ثابت حرف *path, حرف **ret_dir, حرف **ret_filename) {
        _cleanup_free_ حرف *d = عدم;
        ثابت حرف *c, *next = عدم;
        صحيح r;

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

        ܐܢ (isempty(path))
                ܦܢܐ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (r == 0) /* root directory or "." */
                ܦܢܐ -EADDRNOTAVAIL;

        ܐܢ (ret_dir) {
                ܐܢ (next == path) {
                        ܐܢ (*path != '/') { /* filename only */
                                ܐܢ (!ret_filename)
                                        ܦܢܐ -EDESTADDRREQ;
                        } ܐܚܪܢܐ {
                                d = strdup("/");
                                ܐܢ (!d)
                                        ܦܢܐ -ENOMEM;
                        }
                } ܐܚܪܢܐ {
                        d = strndup(path, next - path);
                        ܐܢ (!d)
                                ܦܢܐ -ENOMEM;

                        path_simplify(d);

                        ܐܢ (!path_is_valid(d))
                                ܦܢܐ -EINVAL;
                }

        } ܐܚܪܢܐ ܐܢ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ܦܢܐ -EINVAL;

        ܐܢ (ret_filename) {
                حرف *fn = strndup(c, r);
                ܐܢ (!fn)
                        ܦܢܐ -ENOMEM;

                *ret_filename = fn;
        }

        ܐܢ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ܦܢܐ strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ثابت حرف *p) {
        ثابت حرف *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ܐܢ (!p)
                ܦܢܐ false;

        e = strchrnul(p, '/');
        ܐܢ (*e != 0)
                ܦܢܐ false;

        ܐܢ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ܦܢܐ false;

        ܦܢܐ true;
}

bool filename_is_valid(ثابت حرف *p) {

        ܐܢ (isempty(p))
                ܦܢܐ false;

        ܐܢ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ܦܢܐ false;

        ܦܢܐ filename_part_is_valid(p);
}

bool path_is_valid_full(ثابت حرف *p, bool accept_dot_dot) {
        ܐܢ (isempty(p))
                ܦܢܐ false;

        لكل (ثابت حرف *e = p;;) {
                صحيح r;

                r = path_find_first_component(&e, accept_dot_dot, عدم);
                ܐܢ (r < 0)
                        ܦܢܐ false;

                ܐܢ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ܦܢܐ false;
                ܐܢ (*e == 0)           /* End of string? Yay! */
                        ܦܢܐ true;
        }
}

bool path_is_normalized(ثابت حرف *p) {
        ܐܢ (!path_is_safe(p))
                ܦܢܐ false;

        ܐܢ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ܦܢܐ false;

        ܐܢ (strstr(p, "//"))
                ܦܢܐ false;

        ܦܢܐ true;
}

صحيح file_in_same_dir(ثابت حرف *path, ثابت حرف *filename, حرف **ret) {
        _cleanup_free_ حرف *b = عدم;
        صحيح r;

        أكد(path);
        أكد(filename);
        أكد(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ܐܢ (path_is_absolute(filename))
                b = strdup(filename);
        ܐܚܪܢܐ {
                _cleanup_free_ حرف *dn = عدم;

                r = path_extract_directory(path, &dn);
                ܐܢ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ܐܚܪܢܐ ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܚܪܢܐ
                        b = path_join(dn, filename);
        }
        ܐܢ (!b)
                ܦܢܐ -ENOMEM;

        *ret = TAKE_PTR(b);
        ܦܢܐ 0;
}

bool hidden_or_backup_file(ثابت حرف *filename) {
        أكد(filename);

        ܐܢ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ܦܢܐ true;

        ثابت حرف *dot = strrchr(filename, '.');
        ܐܢ (!dot)
                ܦܢܐ false;

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

        ܦܢܐ STR_IN_SET(dot + 1,
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

bool is_device_path(ثابت حرف *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ثابت حرف *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        ܦܢܐ !isempty(p);
}

bool valid_device_node_path(ثابت حرف *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ܐܢ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ܦܢܐ false;

        ܐܢ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ܦܢܐ false;

        ܦܢܐ path_is_normalized(path);
}

bool valid_device_allow_pattern(ثابت حرف *path) {
        أكد(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ܐܢ (STARTSWITH_SET(path, "block-", "char-"))
                ܦܢܐ true;

        ܦܢܐ valid_device_node_path(path);
}

bool dot_or_dot_dot(ثابت حرف *path) {
        ܐܢ (!path)
                ܦܢܐ false;
        ܐܢ (path[0] != '.')
                ܦܢܐ false;
        ܐܢ (path[1] == 0)
                ܦܢܐ true;
        ܐܢ (path[1] != '.')
                ܦܢܐ false;

        ܦܢܐ path[2] == 0;
}

bool path_implies_directory(ثابت حرف *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ܐܢ (!path)
                ܦܢܐ false;

        ܐܢ (dot_or_dot_dot(path))
                ܦܢܐ true;

        ܦܢܐ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(ثابت حرف *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ܐܢ (isempty(path))
                ܦܢܐ true;

        ܦܢܐ path_equal(path, "/");
}

ثابت حرف* empty_to_root(ثابت حرف *path) {
        ܦܢܐ isempty(path) ? "/" : path;
}

صحيح empty_or_root_harder_to_null(ثابت حرف **path) {
        صحيح r;

        أكد(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        ܐܢ (empty_or_root(*path)) {
                *path = عدم;
                ܦܢܐ 0;
        }

        r = path_is_root(*path);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (r > 0)
                *path = عدم;

        ܦܢܐ 0;
}

bool path_strv_contains(حرف * ثابت *l, ثابت حرف *path) {
        أكد(path);

        STRV_FOREACH(i, l)
                ܐܢ (path_equal(*i, path))
                        ܦܢܐ true;

        ܦܢܐ false;
}

bool prefixed_path_strv_contains(حرف * ثابت *l, ثابت حرف *path) {
        أكد(path);

        STRV_FOREACH(i, l) {
                ثابت حرف *j = *i;

                ܐܢ (*j == '-')
                        j++;
                ܐܢ (*j == '+')
                        j++;

                ܐܢ (path_equal(j, path))
                        ܦܢܐ true;
        }

        ܦܢܐ false;
}

صحيح path_glob_can_match(ثابت حرف *pattern, ثابت حرف *prefix, حرف **ret) {
        أكد(pattern);
        أكد(prefix);

        لكل (ثابت حرف *a = pattern, *b = prefix;;) {
                _cleanup_free_ حرف *g = عدم, *h = عدم;
                ثابت حرف *p, *q;
                صحيح r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                ܐܢ (s < 0)
                        ܦܢܐ s;

                ܐܢ (s == 0) {
                        /* The pattern matches the prefix. */
                        ܐܢ (ret) {
                                حرف *t;

                                t = path_join(prefix, p);
                                ܐܢ (!t)
                                        ܦܢܐ -ENOMEM;

                                *ret = t;
                        }
                        ܦܢܐ true;
                }

                ܐܢ (r == 0)
                        اكسر;

                ܐܢ (r == s && strneq(p, q, r))
                        استمر; /* common component. Check next. */

                g = strndup(p, r);
                ܐܢ (!g)
                        ܦܢܐ -ENOMEM;

                ܐܢ (!string_is_glob(g))
                        اكسر;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ܐܢ (!h)
                        ܦܢܐ -ENOMEM;

                r = fnmatch(g, h, 0);
                ܐܢ (r == FNM_NOMATCH)
                        اكسر;
                ܐܢ (r != 0) /* Failure to process pattern? */
                        ܦܢܐ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ܐܢ (ret)
                *ret = عدم;
        ܦܢܐ false;
}

#ܐܢ HAVE_SPLIT_BIN
ساكن bool dir_is_split(ثابت حرف *a, ثابت حرف *b) {
        صحيح r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ܐܢ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ܦܢܐ true;
        }
        ܦܢܐ r == 0;
}
#endif

ثابت حرف* default_PATH(فراغ) {
#ܐܢ HAVE_SPLIT_BIN
        ساكن ثابت حرف *default_path = عدم;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ܐܢ (default_path)
                ܦܢܐ default_path;

        ܐܢ (dir_is_split("/usr/sbin", "/usr/bin"))
                ܦܢܐ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        ܐܢ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ܦܢܐ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ܦܢܐ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#ܐܚܪܢܐ
        ܦܢܐ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
