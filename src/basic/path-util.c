/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "alloc-util.h"
#include "chase.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fs-util.h"
#include "glob-util.h"
#include "log.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"
#include "strv.h"

bool is_path(const var_na *p) {
        ប្រសិនបើ (!p) /* A NULL pointer is definitely not a path */
                ត្រឡប់ false;

        ត្រឡប់ strchr(p, '/');
}

ចំនួនគត់ path_split_and_make_absolute(const var_na *p, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        ចំនួនគត់ r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        ប្រសិនបើ (!l)
                ត្រឡប់ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        *ret = TAKE_PTR(l);
        ត្រឡប់ r;
}

var_na* path_make_absolute(const var_na *p, const var_na *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ប្រសិនបើ (path_is_absolute(p) || isempty(prefix))
                ត្រឡប់ strdup(p);

        ត្រឡប់ path_join(prefix, p);
}

ចំនួនគត់ safe_getcwd(var_na **ret) {
        _cleanup_free_ var_na *cwd = NULL;

        cwd = get_current_dir_name();
        ប្រសិនបើ (!cwd)
                ត្រឡប់ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ប្រសិនបើ (cwd[0] != '/')
                ត្រឡប់ -ENOMEDIUM;

        ប្រសិនបើ (ret)
                *ret = TAKE_PTR(cwd);

        ត្រឡប់ 0;
}

ចំនួនគត់ path_make_absolute_cwd(const var_na *p, var_na **ret) {
        var_na *c;
        ចំនួនគត់ r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ប្រសិនបើ (path_is_absolute(p))
                c = strdup(p);
        បើមិនដូច្នេះ {
                _cleanup_free_ var_na *cwd = NULL;

                r = safe_getcwd(&cwd);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                c = path_join(cwd, p);
        }
        ប្រសិនបើ (!c)
                ត្រឡប់ -ENOMEM;

        *ret = c;
        ត្រឡប់ 0;
}

ចំនួនគត់ path_make_relative(const var_na *from, const var_na *to, var_na **ret) {
        _cleanup_free_ var_na *result = NULL;
        unsigned n_parents;
        const var_na *f, *t;
        ចំនួនគត់ r, k;
        var_na *p;

        assert(from);
        assert(to);
        assert(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        ប្រសិនបើ (!path_is_absolute(from) || !path_is_absolute(to))
                ត្រឡប់ -EINVAL;

        សម្រាប់ (;;) {
                r = path_find_first_component(&from, true, &f);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                k = path_find_first_component(&to, true, &t);
                ប្រសិនបើ (k < 0)
                        ត្រឡប់ k;

                ប្រសិនបើ (r == 0) {
                        /* end of 'from' */
                        ប្រសិនបើ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                ប្រសិនបើ (!result)
                                        ត្រឡប់ -ENOMEM;
                        } បើមិនដូច្នេះ {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                ប្រសិនបើ (r < 0)
                                        ត្រឡប់ r;

                                ប្រសិនបើ (!path_is_valid(result))
                                        ត្រឡប់ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ត្រឡប់ 0;
                }

                ប្រសិនបើ (r != k || !strneq(f, t, r))
                        bha_nga;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        សម្រាប់ (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
                ប្រសិនបើ (r == 0)
                        bha_nga;
        }

        ប្រសិនបើ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ត្រឡប់ -EINVAL;

        result = new(var_na, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ប្រសិនបើ (!result)
                ត្រឡប់ -ENOMEM;

        សម្រាប់ (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ប្រសិនបើ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ត្រឡប់ 0;
        }

        strcpy(p, t);

        path_simplify(result);

        ប្រសិនបើ (!path_is_valid(result))
                ត្រឡប់ -EINVAL;

        *ret = TAKE_PTR(result);
        ត្រឡប់ 0;
}

ចំនួនគត់ path_make_relative_parent(const var_na *from_child, const var_na *to, var_na **ret) {
        _cleanup_free_ var_na *from = NULL;
        ចំនួនគត់ r;

        assert(from_child);
        assert(to);
        assert(ret);

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
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ path_make_relative(from, to, ret);
}

var_na* path_startswith_strv(const var_na *p, var_na * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                var_na *t;

                t = path_startswith(p, *s);
                ប្រសិនបើ (t)
                        ត្រឡប់ t;
        }

        ត្រឡប់ NULL;
}

ចំនួនគត់ path_strv_make_absolute_cwd(var_na **l) {
        ចំនួនគត់ r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                var_na *t;

                r = path_make_absolute_cwd(*s, &t);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ត្រឡប់ 0;
}

var_na** path_strv_resolve(var_na **l, const var_na *root) {
        unsigned k = 0;
        bool enomem = false;
        ចំនួនគត់ r;

        ប្រសិនបើ (strv_isempty(l))
                ត្រឡប់ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ var_na *orig = NULL;
                var_na *t, *u;

                ប្រសិនបើ (root) {
                        ប្រសិនបើ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        ប្រសិនបើ (!t) {
                                enomem = true;
                                continue;
                        }
                } បើមិនដូច្នេះ
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                ប្រសិនបើ (r == -ENOENT) {
                        ប្រសិនបើ (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } បើមិនដូច្នេះ
                                u = t;
                } បើមិនដូច្នេះ ប្រសិនបើ (r < 0) {
                        free(t);

                        ប្រសិនបើ (r == -ENOMEM)
                                enomem = true;

                        continue;
                } បើមិនដូច្នេះ ប្រសិនបើ (root) {
                        var_na *x;

                        free(t);
                        x = path_startswith(u, root);
                        ប្រសិនបើ (x) {
                                /* restore the slash if it was lost */
                                ប្រសិនបើ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                ប្រសិនបើ (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } បើមិនដូច្នេះ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } បើមិនដូច្នេះ
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        ប្រសិនបើ (enomem)
                ត្រឡប់ NULL;

        ត្រឡប់ l;
}

var_na** path_strv_resolve_uniq(var_na **l, const var_na *root) {

        ប្រសិនបើ (strv_isempty(l))
                ត្រឡប់ l;

        ប្រសិនបើ (!path_strv_resolve(l, root))
                ត្រឡប់ NULL;

        ត្រឡប់ strv_uniq(l);
}

var_na* skip_leading_slash(const var_na *p) {
        ត្រឡប់ skip_leading_chars(p, "/");
}

var_na* path_simplify_full(var_na *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        var_na *f = path;
        ចំនួនគត់ r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        ប្រសិនបើ (isempty(path))
                ត្រឡប់ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        សម្រាប់ (const var_na *p = f;;) {
                const var_na *e;

                r = path_find_first_component(&p, true, &e);
                ប្រសិនបើ (r == 0)
                        bha_nga;

                ប្រសិនបើ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                ប្រសិនបើ (add_slash)
                        *f++ = '/';

                ប្រសិនបើ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ត្រឡប់ path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ប្រសិនបើ (f == path)
                *f++ = '.';

        ប្រសិនបើ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ត្រឡប់ path;
}

ចំនួនគត់ path_simplify_alloc(const var_na *path, var_na **ret) {
        assert(ret);

        ប្រសិនបើ (!path) {
                *ret = NULL;
                ត្រឡប់ 0;
        }

        var_na *t = strdup(path);
        ប្រសិនបើ (!t)
                ត្រឡប់ -ENOMEM;

        *ret = path_simplify(t);
        ត្រឡប់ 0;
}

var_na* path_startswith_full(const var_na *original_path, const var_na *prefix, PathStartWithFlags flags) {
        assert(original_path);
        assert(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        const var_na *path = original_path;

        ប្រសិនបើ ((path[0] == '/') != (prefix[0] == '/'))
                ត្រឡប់ NULL;

        សម្រាប់ (;;) {
                const var_na *p, *q;
                ចំនួនគត់ m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                ប្រសិនបើ (m < 0)
                        ត្រឡប់ NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                ប្រសិនបើ (n < 0)
                        ត្រឡប់ NULL;

                ប្រសិនបើ (n == 0) {
                        ប្រសិនបើ (!p)
                                p = path;

                        ប្រសិនបើ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ប្រសិនបើ (p <= original_path)
                                        ត្រឡប់ NULL;

                                p--;

                                ប្រសិនបើ (*p != '/')
                                        ត្រឡប់ NULL;
                        }

                        ត្រឡប់ (var_na*) p;
                }

                ប្រសិនបើ (m != n)
                        ត្រឡប់ NULL;

                ប្រសិនបើ (!strneq(p, q, m))
                        ត្រឡប់ NULL;
        }
}

ចំនួនគត់ path_compare(const var_na *a, const var_na *b) {
        ចំនួនគត់ r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        សម្រាប់ (;;) {
                const var_na *aa, *bb;
                ចំនួនគត់ j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                ប្រសិនបើ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ប្រសិនបើ (r != 0)
                                ត្រឡប់ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ត្រឡប់ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ប្រសិនបើ (j == 0) {
                        ប្រសិនបើ (k == 0)
                                ត្រឡប់ 0;
                        ត្រឡប់ -1;
                }
                ប្រសិនបើ (k == 0)
                        ត្រឡប់ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ប្រសិនបើ (r != 0)
                        ត្រឡប់ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ប្រសិនបើ (r != 0)
                        ត្រឡប់ r;
        }
}

ចំនួនគត់ path_compare_filename(const var_na *a, const var_na *b) {
        _cleanup_free_ var_na *fa = NULL, *fb = NULL;
        ចំនួនគត់ r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ប្រសិនបើ (j < 0)
                ត្រឡប់ strcmp(a, b);

        ត្រឡប់ strcmp(fa, fb);
}

ចំនួនគត់ path_equal_or_inode_same_full(const var_na *a, const var_na *b, ចំនួនគត់ flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ប្រសិនបើ (path_equal(a, b))
                ត្រឡប់ 1;

        ប្រសិនបើ (!a || !b)
                ត្រឡប់ 0;

        ត្រឡប់ inode_same(a, b, flags);
}

var_na* path_extend_internal(var_na **x, ...) {
        size_t sz, old_sz;
        var_na *q, *nx;
        const var_na *p;
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
        ខណៈពេល ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                size_t add;

                ប្រសិនបើ (isempty(p))
                        continue;

                add = 1 + strlen(p);
                ប្រសិនបើ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ត្រឡប់ NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        ប្រសិនបើ (!nx)
                ត្រឡប់ NULL;
        ប្រសិនបើ (x)
                *x = nx;

        ប្រសិនបើ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        បើមិនដូច្នេះ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ខណៈពេល ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                ប្រសិនបើ (isempty(p))
                        continue;

                ប្រសិនបើ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ត្រឡប់ nx;
}

ចំនួនគត់ open_and_check_executable(const var_na *name, const var_na *root, var_na **ret_path, ចំនួនគត់ *ret_fd) {
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;
        _cleanup_free_ var_na *resolved = NULL;
        ចំនួនគត់ r;

        assert(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        ប្រសិនបើ (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                name = resolved;
        } បើមិនដូច្នេះ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                ប្រសិនបើ (fd < 0)
                        ត្រឡប់ -errno;
        }

        r = fd_verify_regular(fd);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = access_fd(fd, X_OK);
        ប្រសិនបើ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (ret_path) {
                ប្រសិនបើ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                បើមិនដូច្នេះ {
                        r = path_make_absolute_cwd(name, ret_path);
                        ប្រសិនបើ (r < 0)
                                ត្រឡប់ r;

                        path_simplify(*ret_path);
                }
        }

        ប្រសិនបើ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ត្រឡប់ 0;
}

ចំនួនគត់ find_executable_full(
                const var_na *name,
                const var_na *root,
                var_na * const *exec_search_path,
                bool use_path_envvar,
                var_na **ret_filename,
                ចំនួនគត់ *ret_fd) {

        ចំនួនគត់ last_error = -ENOENT, r = 0;

        assert(name);

        ប្រសិនបើ (is_path(name))
                ត្រឡប់ open_and_check_executable(name, root, ret_filename, ret_fd);

        ប្រសិនបើ (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ var_na *full_path = NULL;

                        ប្រសិនបើ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        ប្រសិនបើ (!full_path)
                                ត្រឡប់ -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        ប្រសិនបើ (r >= 0)
                                ត្រឡប់ 0;
                        ប្រសិនបើ (r != -EACCES)
                                last_error = r;
                }
                ត្រឡប់ last_error;
        }

        const var_na *p = NULL;

        ប្រសិនបើ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ប្រសិនបើ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        សម្រាប់ (;;) {
                _cleanup_free_ var_na *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
                ប្រសិនបើ (r == 0)
                        bha_nga;

                ប្រសិនបើ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                ប្រសិនបើ (!path_extend(&element, name))
                        ត្រឡប់ -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                ប្រសិនបើ (r >= 0) /* Found it! */
                        ត្រឡប់ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ប្រសិនបើ (r != -EACCES)
                        last_error = r;
        }

        ត្រឡប់ last_error;
}

static ចំនួនគត់ executable_is_good(const var_na *executable) {
        _cleanup_free_ var_na *p = NULL, *d = NULL;
        ចំនួនគត់ r;

        r = find_executable(executable, &p);
        ប្រសិនបើ (r == -ENOENT)
                ត្រឡប់ 0;
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ប្រសិនបើ (r == -EINVAL) /* not a symlink */
                ត្រឡប់ 1;
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

ចំនួនគត់ fsck_exists(void) {
        ត្រឡប់ executable_is_good("fsck");
}

ចំនួនគត់ fsck_exists_for_fstype(const var_na *fstype) {
        const var_na *checker;
        ចំនួនគត់ r;

        assert(fstype);

        ប្រសិនបើ (streq(fstype, "auto"))
                ត្រឡប់ -EINVAL;

        r = fsck_exists();
        ប្រសិនបើ (r <= 0)
                ត្រឡប់ r;

        checker = strjoina("fsck.", fstype);
        ត្រឡប់ executable_is_good(checker);
}

static const var_na* skip_slash_or_dot(const var_na *p) {
        POINTER_MAY_BE_NULL(p);

        សម្រាប់ (; !isempty(p); p++) {
                ប្រសិនបើ (*p == '/')
                        continue;
                ប្រសិនបើ (startswith(p, "./")) {
                        p++;
                        continue;
                }
                bha_nga;
        }
        ត្រឡប់ p;
}

ចំនួនគត់ path_find_first_component(const var_na **p, bool accept_dot_dot, const var_na **ret) {
        const var_na *q, *first, *end_first, *next;
        size_t len;

        assert(p);

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
        ប្រសិនបើ (isempty(first)) {
                *p = first;
                ប្រសិនបើ (ret)
                        *ret = NULL;
                ត្រឡប់ 0;
        }
        ប្រសិនបើ (streq(first, ".")) {
                *p = first + 1;
                ប្រសិនបើ (ret)
                        *ret = NULL;
                ត្រឡប់ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        ប្រសិនបើ (len > NAME_MAX)
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ត្រឡប់ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        ប្រសិនបើ (ret)
                *ret = first;
        ត្រឡប់ len;
}

static const var_na* skip_slash_or_dot_backward(const var_na *path, const var_na *q) {
        assert(path);
        assert(!q || q >= path);

        សម្រាប់ (; q; q = PTR_SUB1(q, path)) {
                ប្រសិនបើ (*q == '/')
                        continue;
                ប្រសិនបើ (q > path && strneq(q - 1, "/.", 2))
                        continue;
                ប្រសិនបើ (q == path && *q == '.')
                        continue;
                bha_nga;
        }
        ត្រឡប់ q;
}

ចំនួនគត់ path_find_last_component(const var_na *path, bool accept_dot_dot, const var_na **next, const var_na **ret) {
        const var_na *q, *last_end, *last_begin;
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

        ប្រសិនបើ (isempty(path)) {
                ប្រសិនបើ (next)
                        *next = path;
                ប្រសិនបើ (ret)
                        *ret = NULL;
                ត្រឡប់ 0;
        }

        ប្រសិនបើ (next && *next) {
                ប្រសិនបើ (*next < path || *next > path + strlen(path))
                        ត្រឡប់ -EINVAL;
                ប្រសិនបើ (*next == path) {
                        ប្រសិនបើ (ret)
                                *ret = NULL;
                        ត្រឡប់ 0;
                }
                ប្រសិនបើ (!IN_SET(**next, '\0', '/'))
                        ត្រឡប់ -EINVAL;
                q = *next - 1;
        } បើមិនដូច្នេះ
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        ប្រសិនបើ (!q) { /* the root directory, "." or "./" */
                ប្រសិនបើ (next)
                        *next = path;
                ប្រសិនបើ (ret)
                        *ret = NULL;
                ត្រឡប់ 0;
        }

        last_end = q + 1;

        ខណៈពេល (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        ប្រសិនបើ (len > NAME_MAX)
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        ប្រសិនបើ (ret)
                *ret = last_begin;
        ត្រឡប់ len;
}

const var_na* last_path_component(const var_na *path) {

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

        ប្រសិនបើ (!path)
                ត្រឡប់ NULL;

        l = k = strlen(path);
        ប្រសិនបើ (l == 0) /* special case — an empty string */
                ត្រឡប់ path;

        ខណៈពេល (k > 0 && path[k-1] == '/')
                k--;

        ប្រសិនបើ (k == 0) /* the root directory */
                ត្រឡប់ path + l - 1;

        ខណៈពេល (k > 0 && path[k-1] != '/')
                k--;

        ត្រឡប់ path + k;
}

ចំនួនគត់ path_split_prefix_filename(const var_na *path, var_na **ret_dir, var_na **ret_filename) {
        _cleanup_free_ var_na *d = NULL;
        const var_na *c, *next = NULL;
        ចំនួនគត់ r;

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

        ប្រសិនបើ (isempty(path))
                ត្រឡប់ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (r == 0) /* root directory or "." */
                ត្រឡប់ -EADDRNOTAVAIL;

        ប្រសិនបើ (ret_dir) {
                ប្រសិនបើ (next == path) {
                        ប្រសិនបើ (*path != '/') { /* filename only */
                                ប្រសិនបើ (!ret_filename)
                                        ត្រឡប់ -EDESTADDRREQ;
                        } បើមិនដូច្នេះ {
                                d = strdup("/");
                                ប្រសិនបើ (!d)
                                        ត្រឡប់ -ENOMEM;
                        }
                } បើមិនដូច្នេះ {
                        d = strndup(path, next - path);
                        ប្រសិនបើ (!d)
                                ត្រឡប់ -ENOMEM;

                        path_simplify(d);

                        ប្រសិនបើ (!path_is_valid(d))
                                ត្រឡប់ -EINVAL;
                }

        } បើមិនដូច្នេះ ប្រសិនបើ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (ret_filename) {
                var_na *fn = strndup(c, r);
                ប្រសិនបើ (!fn)
                        ត្រឡប់ -ENOMEM;

                *ret_filename = fn;
        }

        ប្រសិនបើ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ត្រឡប់ strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const var_na *p) {
        const var_na *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ប្រសិនបើ (!p)
                ត្រឡប់ false;

        e = strchrnul(p, '/');
        ប្រសិនបើ (*e != 0)
                ត្រឡប់ false;

        ប្រសិនបើ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ត្រឡប់ false;

        ត្រឡប់ true;
}

bool filename_is_valid(const var_na *p) {

        ប្រសិនបើ (isempty(p))
                ត្រឡប់ false;

        ប្រសិនបើ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ត្រឡប់ false;

        ត្រឡប់ filename_part_is_valid(p);
}

bool path_is_valid_full(const var_na *p, bool accept_dot_dot) {
        ប្រសិនបើ (isempty(p))
                ត្រឡប់ false;

        សម្រាប់ (const var_na *e = p;;) {
                ចំនួនគត់ r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ false;

                ប្រសិនបើ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ត្រឡប់ false;
                ប្រសិនបើ (*e == 0)           /* End of string? Yay! */
                        ត្រឡប់ true;
        }
}

bool path_is_normalized(const var_na *p) {
        ប្រសិនបើ (!path_is_safe(p))
                ត្រឡប់ false;

        ប្រសិនបើ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ត្រឡប់ false;

        ប្រសិនបើ (strstr(p, "//"))
                ត្រឡប់ false;

        ត្រឡប់ true;
}

ចំនួនគត់ file_in_same_dir(const var_na *path, const var_na *filename, var_na **ret) {
        _cleanup_free_ var_na *b = NULL;
        ចំនួនគត់ r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ប្រសិនបើ (path_is_absolute(filename))
                b = strdup(filename);
        បើមិនដូច្នេះ {
                _cleanup_free_ var_na *dn = NULL;

                r = path_extract_directory(path, &dn);
                ប្រសិនបើ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                បើមិនដូច្នេះ ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
                បើមិនដូច្នេះ
                        b = path_join(dn, filename);
        }
        ប្រសិនបើ (!b)
                ត្រឡប់ -ENOMEM;

        *ret = TAKE_PTR(b);
        ត្រឡប់ 0;
}

bool hidden_or_backup_file(const var_na *filename) {
        assert(filename);

        ប្រសិនបើ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ត្រឡប់ true;

        const var_na *dot = strrchr(filename, '.');
        ប្រសិនបើ (!dot)
                ត្រឡប់ false;

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

        ត្រឡប់ STR_IN_SET(dot + 1,
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

bool is_device_path(const var_na *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        const var_na *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        ត្រឡប់ !isempty(p);
}

bool valid_device_node_path(const var_na *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ប្រសិនបើ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ត្រឡប់ false;

        ប្រសិនបើ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ត្រឡប់ false;

        ត្រឡប់ path_is_normalized(path);
}

bool valid_device_allow_pattern(const var_na *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ប្រសិនបើ (STARTSWITH_SET(path, "block-", "char-"))
                ត្រឡប់ true;

        ត្រឡប់ valid_device_node_path(path);
}

bool dot_or_dot_dot(const var_na *path) {
        ប្រសិនបើ (!path)
                ត្រឡប់ false;
        ប្រសិនបើ (path[0] != '.')
                ត្រឡប់ false;
        ប្រសិនបើ (path[1] == 0)
                ត្រឡប់ true;
        ប្រសិនបើ (path[1] != '.')
                ត្រឡប់ false;

        ត្រឡប់ path[2] == 0;
}

bool path_implies_directory(const var_na *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ប្រសិនបើ (!path)
                ត្រឡប់ false;

        ប្រសិនបើ (dot_or_dot_dot(path))
                ត្រឡប់ true;

        ត្រឡប់ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const var_na *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ប្រសិនបើ (isempty(path))
                ត្រឡប់ true;

        ត្រឡប់ path_equal(path, "/");
}

const var_na* empty_to_root(const var_na *path) {
        ត្រឡប់ isempty(path) ? "/" : path;
}

ចំនួនគត់ empty_or_root_harder_to_null(const var_na **path) {
        ចំនួនគត់ r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        ប្រសិនបើ (empty_or_root(*path)) {
                *path = NULL;
                ត្រឡប់ 0;
        }

        r = path_is_root(*path);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (r > 0)
                *path = NULL;

        ត្រឡប់ 0;
}

bool path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l)
                ប្រសិនបើ (path_equal(*i, path))
                        ត្រឡប់ true;

        ត្រឡប់ false;
}

bool prefixed_path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const var_na *j = *i;

                ប្រសិនបើ (*j == '-')
                        j++;
                ប្រសិនបើ (*j == '+')
                        j++;

                ប្រសិនបើ (path_equal(j, path))
                        ត្រឡប់ true;
        }

        ត្រឡប់ false;
}

ចំនួនគត់ path_glob_can_match(const var_na *pattern, const var_na *prefix, var_na **ret) {
        assert(pattern);
        assert(prefix);

        សម្រាប់ (const var_na *a = pattern, *b = prefix;;) {
                _cleanup_free_ var_na *g = NULL, *h = NULL;
                const var_na *p, *q;
                ចំនួនគត់ r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                ប្រសិនបើ (s < 0)
                        ត្រឡប់ s;

                ប្រសិនបើ (s == 0) {
                        /* The pattern matches the prefix. */
                        ប្រសិនបើ (ret) {
                                var_na *t;

                                t = path_join(prefix, p);
                                ប្រសិនបើ (!t)
                                        ត្រឡប់ -ENOMEM;

                                *ret = t;
                        }
                        ត្រឡប់ true;
                }

                ប្រសិនបើ (r == 0)
                        bha_nga;

                ប្រសិនបើ (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                ប្រសិនបើ (!g)
                        ត្រឡប់ -ENOMEM;

                ប្រសិនបើ (!string_is_glob(g))
                        bha_nga;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ប្រសិនបើ (!h)
                        ត្រឡប់ -ENOMEM;

                r = fnmatch(g, h, 0);
                ប្រសិនបើ (r == FNM_NOMATCH)
                        bha_nga;
                ប្រសិនបើ (r != 0) /* Failure to process pattern? */
                        ត្រឡប់ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ប្រសិនបើ (ret)
                *ret = NULL;
        ត្រឡប់ false;
}

#ប្រសិនបើ HAVE_SPLIT_BIN
static bool dir_is_split(const var_na *a, const var_na *b) {
        ចំនួនគត់ r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ប្រសិនបើ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ត្រឡប់ true;
        }
        ត្រឡប់ r == 0;
}
#endif

const var_na* default_PATH(void) {
#ប្រសិនបើ HAVE_SPLIT_BIN
        static const var_na *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ប្រសិនបើ (default_path)
                ត្រឡប់ default_path;

        ប្រសិនបើ (dir_is_split("/usr/sbin", "/usr/bin"))
                ត្រឡប់ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        ប្រសិនបើ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ត្រឡប់ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ត្រឡប់ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#បើមិនដូច្នេះ
        ត្រឡប់ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
