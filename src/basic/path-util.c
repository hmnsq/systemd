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
        གལ་ཏེ (!p) /* A NULL pointer is definitely not a path */
                ཕྱིར false;

        ཕྱིར strchr(p, '/');
}

ཧྲིལ path_split_and_make_absolute(const var_na *p, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        ཧྲིལ r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        གལ་ཏེ (!l)
                ཕྱིར -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        *ret = TAKE_PTR(l);
        ཕྱིར r;
}

var_na* path_make_absolute(const var_na *p, const var_na *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        གལ་ཏེ (path_is_absolute(p) || isempty(prefix))
                ཕྱིར strdup(p);

        ཕྱིར path_join(prefix, p);
}

ཧྲིལ safe_getcwd(var_na **ret) {
        _cleanup_free_ var_na *cwd = NULL;

        cwd = get_current_dir_name();
        གལ་ཏེ (!cwd)
                ཕྱིར negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        གལ་ཏེ (cwd[0] != '/')
                ཕྱིར -ENOMEDIUM;

        གལ་ཏེ (ret)
                *ret = TAKE_PTR(cwd);

        ཕྱིར 0;
}

ཧྲིལ path_make_absolute_cwd(const var_na *p, var_na **ret) {
        var_na *c;
        ཧྲིལ r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        གལ་ཏེ (path_is_absolute(p))
                c = strdup(p);
        དེ་མིན {
                _cleanup_free_ var_na *cwd = NULL;

                r = safe_getcwd(&cwd);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                c = path_join(cwd, p);
        }
        གལ་ཏེ (!c)
                ཕྱིར -ENOMEM;

        *ret = c;
        ཕྱིར 0;
}

ཧྲིལ path_make_relative(const var_na *from, const var_na *to, var_na **ret) {
        _cleanup_free_ var_na *result = NULL;
        unsigned n_parents;
        const var_na *f, *t;
        ཧྲིལ r, k;
        var_na *p;

        assert(from);
        assert(to);
        assert(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        གལ་ཏེ (!path_is_absolute(from) || !path_is_absolute(to))
                ཕྱིར -EINVAL;

        ཆེད་དུ (;;) {
                r = path_find_first_component(&from, true, &f);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                k = path_find_first_component(&to, true, &t);
                གལ་ཏེ (k < 0)
                        ཕྱིར k;

                གལ་ཏེ (r == 0) {
                        /* end of 'from' */
                        གལ་ཏེ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                གལ་ཏེ (!result)
                                        ཕྱིར -ENOMEM;
                        } དེ་མིན {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                གལ་ཏེ (r < 0)
                                        ཕྱིར r;

                                གལ་ཏེ (!path_is_valid(result))
                                        ཕྱིར -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ཕྱིར 0;
                }

                གལ་ཏེ (r != k || !strneq(f, t, r))
                        bha_nga;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        ཆེད་དུ (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                གལ་ཏེ (r == 0)
                        bha_nga;
        }

        གལ་ཏེ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ཕྱིར -EINVAL;

        result = new(var_na, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        གལ་ཏེ (!result)
                ཕྱིར -ENOMEM;

        ཆེད་དུ (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        གལ་ཏེ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ཕྱིར 0;
        }

        strcpy(p, t);

        path_simplify(result);

        གལ་ཏེ (!path_is_valid(result))
                ཕྱིར -EINVAL;

        *ret = TAKE_PTR(result);
        ཕྱིར 0;
}

ཧྲིལ path_make_relative_parent(const var_na *from_child, const var_na *to, var_na **ret) {
        _cleanup_free_ var_na *from = NULL;
        ཧྲིལ r;

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
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར path_make_relative(from, to, ret);
}

var_na* path_startswith_strv(const var_na *p, var_na * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                var_na *t;

                t = path_startswith(p, *s);
                གལ་ཏེ (t)
                        ཕྱིར t;
        }

        ཕྱིར NULL;
}

ཧྲིལ path_strv_make_absolute_cwd(var_na **l) {
        ཧྲིལ r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                var_na *t;

                r = path_make_absolute_cwd(*s, &t);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ཕྱིར 0;
}

var_na** path_strv_resolve(var_na **l, const var_na *root) {
        unsigned k = 0;
        bool enomem = false;
        ཧྲིལ r;

        གལ་ཏེ (strv_isempty(l))
                ཕྱིར l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ var_na *orig = NULL;
                var_na *t, *u;

                གལ་ཏེ (root) {
                        གལ་ཏེ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        གལ་ཏེ (!t) {
                                enomem = true;
                                continue;
                        }
                } དེ་མིན
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                གལ་ཏེ (r == -ENOENT) {
                        གལ་ཏེ (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } དེ་མིན
                                u = t;
                } དེ་མིན གལ་ཏེ (r < 0) {
                        free(t);

                        གལ་ཏེ (r == -ENOMEM)
                                enomem = true;

                        continue;
                } དེ་མིན གལ་ཏེ (root) {
                        var_na *x;

                        free(t);
                        x = path_startswith(u, root);
                        གལ་ཏེ (x) {
                                /* restore the slash if it was lost */
                                གལ་ཏེ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                གལ་ཏེ (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } དེ་མིན {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } དེ་མིན
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        གལ་ཏེ (enomem)
                ཕྱིར NULL;

        ཕྱིར l;
}

var_na** path_strv_resolve_uniq(var_na **l, const var_na *root) {

        གལ་ཏེ (strv_isempty(l))
                ཕྱིར l;

        གལ་ཏེ (!path_strv_resolve(l, root))
                ཕྱིར NULL;

        ཕྱིར strv_uniq(l);
}

var_na* skip_leading_slash(const var_na *p) {
        ཕྱིར skip_leading_chars(p, "/");
}

var_na* path_simplify_full(var_na *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        var_na *f = path;
        ཧྲིལ r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        གལ་ཏེ (isempty(path))
                ཕྱིར path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        ཆེད་དུ (const var_na *p = f;;) {
                const var_na *e;

                r = path_find_first_component(&p, true, &e);
                གལ་ཏེ (r == 0)
                        bha_nga;

                གལ་ཏེ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                གལ་ཏེ (add_slash)
                        *f++ = '/';

                གལ་ཏེ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ཕྱིར path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        གལ་ཏེ (f == path)
                *f++ = '.';

        གལ་ཏེ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ཕྱིར path;
}

ཧྲིལ path_simplify_alloc(const var_na *path, var_na **ret) {
        assert(ret);

        གལ་ཏེ (!path) {
                *ret = NULL;
                ཕྱིར 0;
        }

        var_na *t = strdup(path);
        གལ་ཏེ (!t)
                ཕྱིར -ENOMEM;

        *ret = path_simplify(t);
        ཕྱིར 0;
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

        གལ་ཏེ ((path[0] == '/') != (prefix[0] == '/'))
                ཕྱིར NULL;

        ཆེད་དུ (;;) {
                const var_na *p, *q;
                ཧྲིལ m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                གལ་ཏེ (m < 0)
                        ཕྱིར NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                གལ་ཏེ (n < 0)
                        ཕྱིར NULL;

                གལ་ཏེ (n == 0) {
                        གལ་ཏེ (!p)
                                p = path;

                        གལ་ཏེ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                གལ་ཏེ (p <= original_path)
                                        ཕྱིར NULL;

                                p--;

                                གལ་ཏེ (*p != '/')
                                        ཕྱིར NULL;
                        }

                        ཕྱིར (var_na*) p;
                }

                གལ་ཏེ (m != n)
                        ཕྱིར NULL;

                གལ་ཏེ (!strneq(p, q, m))
                        ཕྱིར NULL;
        }
}

ཧྲིལ path_compare(const var_na *a, const var_na *b) {
        ཧྲིལ r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        གལ་ཏེ (r != 0)
                ཕྱིར r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        གལ་ཏེ (r != 0)
                ཕྱིར r;

        ཆེད་དུ (;;) {
                const var_na *aa, *bb;
                ཧྲིལ j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                གལ་ཏེ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        གལ་ཏེ (r != 0)
                                ཕྱིར r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ཕྱིར strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                གལ་ཏེ (j == 0) {
                        གལ་ཏེ (k == 0)
                                ཕྱིར 0;
                        ཕྱིར -1;
                }
                གལ་ཏེ (k == 0)
                        ཕྱིར 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                གལ་ཏེ (r != 0)
                        ཕྱིར r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                གལ་ཏེ (r != 0)
                        ཕྱིར r;
        }
}

ཧྲིལ path_compare_filename(const var_na *a, const var_na *b) {
        _cleanup_free_ var_na *fa = NULL, *fb = NULL;
        ཧྲིལ r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        གལ་ཏེ (r != 0)
                ཕྱིར r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        གལ་ཏེ (r != 0)
                ཕྱིར r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        གལ་ཏེ (r != 0)
                ཕྱིར r;

        /* fallback to use strcmp() if both paths are invalid. */
        གལ་ཏེ (j < 0)
                ཕྱིར strcmp(a, b);

        ཕྱིར strcmp(fa, fb);
}

ཧྲིལ path_equal_or_inode_same_full(const var_na *a, const var_na *b, ཧྲིལ flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        གལ་ཏེ (path_equal(a, b))
                ཕྱིར 1;

        གལ་ཏེ (!a || !b)
                ཕྱིར 0;

        ཕྱིར inode_same(a, b, flags);
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
        སྐབས ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                size_t add;

                གལ་ཏེ (isempty(p))
                        continue;

                add = 1 + strlen(p);
                གལ་ཏེ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ཕྱིར NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        གལ་ཏེ (!nx)
                ཕྱིར NULL;
        གལ་ཏེ (x)
                *x = nx;

        གལ་ཏེ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        དེ་མིན {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        སྐབས ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                གལ་ཏེ (isempty(p))
                        continue;

                གལ་ཏེ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ཕྱིར nx;
}

ཧྲིལ open_and_check_executable(const var_na *name, const var_na *root, var_na **ret_path, ཧྲིལ *ret_fd) {
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        _cleanup_free_ var_na *resolved = NULL;
        ཧྲིལ r;

        assert(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        གལ་ཏེ (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                name = resolved;
        } དེ་མིན {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                གལ་ཏེ (fd < 0)
                        ཕྱིར -errno;
        }

        r = fd_verify_regular(fd);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = access_fd(fd, X_OK);
        གལ་ཏེ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ret_path) {
                གལ་ཏེ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                དེ་མིན {
                        r = path_make_absolute_cwd(name, ret_path);
                        གལ་ཏེ (r < 0)
                                ཕྱིར r;

                        path_simplify(*ret_path);
                }
        }

        གལ་ཏེ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ཕྱིར 0;
}

ཧྲིལ find_executable_full(
                const var_na *name,
                const var_na *root,
                var_na * const *exec_search_path,
                bool use_path_envvar,
                var_na **ret_filename,
                ཧྲིལ *ret_fd) {

        ཧྲིལ last_error = -ENOENT, r = 0;

        assert(name);

        གལ་ཏེ (is_path(name))
                ཕྱིར open_and_check_executable(name, root, ret_filename, ret_fd);

        གལ་ཏེ (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ var_na *full_path = NULL;

                        གལ་ཏེ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        གལ་ཏེ (!full_path)
                                ཕྱིར -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        གལ་ཏེ (r >= 0)
                                ཕྱིར 0;
                        གལ་ཏེ (r != -EACCES)
                                last_error = r;
                }
                ཕྱིར last_error;
        }

        const var_na *p = NULL;

        གལ་ཏེ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        གལ་ཏེ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        ཆེད་དུ (;;) {
                _cleanup_free_ var_na *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                གལ་ཏེ (r == 0)
                        bha_nga;

                གལ་ཏེ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                གལ་ཏེ (!path_extend(&element, name))
                        ཕྱིར -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                གལ་ཏེ (r >= 0) /* Found it! */
                        ཕྱིར 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                གལ་ཏེ (r != -EACCES)
                        last_error = r;
        }

        ཕྱིར last_error;
}

static ཧྲིལ executable_is_good(const var_na *executable) {
        _cleanup_free_ var_na *p = NULL, *d = NULL;
        ཧྲིལ r;

        r = find_executable(executable, &p);
        གལ་ཏེ (r == -ENOENT)
                ཕྱིར 0;
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        གལ་ཏེ (r == -EINVAL) /* not a symlink */
                ཕྱིར 1;
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

ཧྲིལ fsck_exists(void) {
        ཕྱིར executable_is_good("fsck");
}

ཧྲིལ fsck_exists_for_fstype(const var_na *fstype) {
        const var_na *checker;
        ཧྲིལ r;

        assert(fstype);

        གལ་ཏེ (streq(fstype, "auto"))
                ཕྱིར -EINVAL;

        r = fsck_exists();
        གལ་ཏེ (r <= 0)
                ཕྱིར r;

        checker = strjoina("fsck.", fstype);
        ཕྱིར executable_is_good(checker);
}

static const var_na* skip_slash_or_dot(const var_na *p) {
        POINTER_MAY_BE_NULL(p);

        ཆེད་དུ (; !isempty(p); p++) {
                གལ་ཏེ (*p == '/')
                        continue;
                གལ་ཏེ (startswith(p, "./")) {
                        p++;
                        continue;
                }
                bha_nga;
        }
        ཕྱིར p;
}

ཧྲིལ path_find_first_component(const var_na **p, bool accept_dot_dot, const var_na **ret) {
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
        གལ་ཏེ (isempty(first)) {
                *p = first;
                གལ་ཏེ (ret)
                        *ret = NULL;
                ཕྱིར 0;
        }
        གལ་ཏེ (streq(first, ".")) {
                *p = first + 1;
                གལ་ཏེ (ret)
                        *ret = NULL;
                ཕྱིར 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        གལ་ཏེ (len > NAME_MAX)
                ཕྱིར -EINVAL;
        གལ་ཏེ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ཕྱིར -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        གལ་ཏེ (ret)
                *ret = first;
        ཕྱིར len;
}

static const var_na* skip_slash_or_dot_backward(const var_na *path, const var_na *q) {
        assert(path);
        assert(!q || q >= path);

        ཆེད་དུ (; q; q = PTR_SUB1(q, path)) {
                གལ་ཏེ (*q == '/')
                        continue;
                གལ་ཏེ (q > path && strneq(q - 1, "/.", 2))
                        continue;
                གལ་ཏེ (q == path && *q == '.')
                        continue;
                bha_nga;
        }
        ཕྱིར q;
}

ཧྲིལ path_find_last_component(const var_na *path, bool accept_dot_dot, const var_na **next, const var_na **ret) {
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

        གལ་ཏེ (isempty(path)) {
                གལ་ཏེ (next)
                        *next = path;
                གལ་ཏེ (ret)
                        *ret = NULL;
                ཕྱིར 0;
        }

        གལ་ཏེ (next && *next) {
                གལ་ཏེ (*next < path || *next > path + strlen(path))
                        ཕྱིར -EINVAL;
                གལ་ཏེ (*next == path) {
                        གལ་ཏེ (ret)
                                *ret = NULL;
                        ཕྱིར 0;
                }
                གལ་ཏེ (!IN_SET(**next, '\0', '/'))
                        ཕྱིར -EINVAL;
                q = *next - 1;
        } དེ་མིན
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        གལ་ཏེ (!q) { /* the root directory, "." or "./" */
                གལ་ཏེ (next)
                        *next = path;
                གལ་ཏེ (ret)
                        *ret = NULL;
                ཕྱིར 0;
        }

        last_end = q + 1;

        སྐབས (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        གལ་ཏེ (len > NAME_MAX)
                ཕྱིར -EINVAL;
        གལ་ཏེ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ཕྱིར -EINVAL;

        གལ་ཏེ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        གལ་ཏེ (ret)
                *ret = last_begin;
        ཕྱིར len;
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

        གལ་ཏེ (!path)
                ཕྱིར NULL;

        l = k = strlen(path);
        གལ་ཏེ (l == 0) /* special case — an empty string */
                ཕྱིར path;

        སྐབས (k > 0 && path[k-1] == '/')
                k--;

        གལ་ཏེ (k == 0) /* the root directory */
                ཕྱིར path + l - 1;

        སྐབས (k > 0 && path[k-1] != '/')
                k--;

        ཕྱིར path + k;
}

ཧྲིལ path_split_prefix_filename(const var_na *path, var_na **ret_dir, var_na **ret_filename) {
        _cleanup_free_ var_na *d = NULL;
        const var_na *c, *next = NULL;
        ཧྲིལ r;

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

        གལ་ཏེ (isempty(path))
                ཕྱིར -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        གལ་ཏེ (r < 0)
                ཕྱིར r;
        གལ་ཏེ (r == 0) /* root directory or "." */
                ཕྱིར -EADDRNOTAVAIL;

        གལ་ཏེ (ret_dir) {
                གལ་ཏེ (next == path) {
                        གལ་ཏེ (*path != '/') { /* filename only */
                                གལ་ཏེ (!ret_filename)
                                        ཕྱིར -EDESTADDRREQ;
                        } དེ་མིན {
                                d = strdup("/");
                                གལ་ཏེ (!d)
                                        ཕྱིར -ENOMEM;
                        }
                } དེ་མིན {
                        d = strndup(path, next - path);
                        གལ་ཏེ (!d)
                                ཕྱིར -ENOMEM;

                        path_simplify(d);

                        གལ་ཏེ (!path_is_valid(d))
                                ཕྱིར -EINVAL;
                }

        } དེ་མིན གལ་ཏེ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ཕྱིར -EINVAL;

        གལ་ཏེ (ret_filename) {
                var_na *fn = strndup(c, r);
                གལ་ཏེ (!fn)
                        ཕྱིར -ENOMEM;

                *ret_filename = fn;
        }

        གལ་ཏེ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ཕྱིར strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const var_na *p) {
        const var_na *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        གལ་ཏེ (!p)
                ཕྱིར false;

        e = strchrnul(p, '/');
        གལ་ཏེ (*e != 0)
                ཕྱིར false;

        གལ་ཏེ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ཕྱིར false;

        ཕྱིར true;
}

bool filename_is_valid(const var_na *p) {

        གལ་ཏེ (isempty(p))
                ཕྱིར false;

        གལ་ཏེ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ཕྱིར false;

        ཕྱིར filename_part_is_valid(p);
}

bool path_is_valid_full(const var_na *p, bool accept_dot_dot) {
        གལ་ཏེ (isempty(p))
                ཕྱིར false;

        ཆེད་དུ (const var_na *e = p;;) {
                ཧྲིལ r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                གལ་ཏེ (r < 0)
                        ཕྱིར false;

                གལ་ཏེ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ཕྱིར false;
                གལ་ཏེ (*e == 0)           /* End of string? Yay! */
                        ཕྱིར true;
        }
}

bool path_is_normalized(const var_na *p) {
        གལ་ཏེ (!path_is_safe(p))
                ཕྱིར false;

        གལ་ཏེ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ཕྱིར false;

        གལ་ཏེ (strstr(p, "//"))
                ཕྱིར false;

        ཕྱིར true;
}

ཧྲིལ file_in_same_dir(const var_na *path, const var_na *filename, var_na **ret) {
        _cleanup_free_ var_na *b = NULL;
        ཧྲིལ r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        གལ་ཏེ (path_is_absolute(filename))
                b = strdup(filename);
        དེ་མིན {
                _cleanup_free_ var_na *dn = NULL;

                r = path_extract_directory(path, &dn);
                གལ་ཏེ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                དེ་མིན གལ་ཏེ (r < 0)
                        ཕྱིར r;
                དེ་མིན
                        b = path_join(dn, filename);
        }
        གལ་ཏེ (!b)
                ཕྱིར -ENOMEM;

        *ret = TAKE_PTR(b);
        ཕྱིར 0;
}

bool hidden_or_backup_file(const var_na *filename) {
        assert(filename);

        གལ་ཏེ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ཕྱིར true;

        const var_na *dot = strrchr(filename, '.');
        གལ་ཏེ (!dot)
                ཕྱིར false;

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

        ཕྱིར STR_IN_SET(dot + 1,
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
        ཕྱིར !isempty(p);
}

bool valid_device_node_path(const var_na *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        གལ་ཏེ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ཕྱིར false;

        གལ་ཏེ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ཕྱིར false;

        ཕྱིར path_is_normalized(path);
}

bool valid_device_allow_pattern(const var_na *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        གལ་ཏེ (STARTSWITH_SET(path, "block-", "char-"))
                ཕྱིར true;

        ཕྱིར valid_device_node_path(path);
}

bool dot_or_dot_dot(const var_na *path) {
        གལ་ཏེ (!path)
                ཕྱིར false;
        གལ་ཏེ (path[0] != '.')
                ཕྱིར false;
        གལ་ཏེ (path[1] == 0)
                ཕྱིར true;
        གལ་ཏེ (path[1] != '.')
                ཕྱིར false;

        ཕྱིར path[2] == 0;
}

bool path_implies_directory(const var_na *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        གལ་ཏེ (!path)
                ཕྱིར false;

        གལ་ཏེ (dot_or_dot_dot(path))
                ཕྱིར true;

        ཕྱིར ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const var_na *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        གལ་ཏེ (isempty(path))
                ཕྱིར true;

        ཕྱིར path_equal(path, "/");
}

const var_na* empty_to_root(const var_na *path) {
        ཕྱིར isempty(path) ? "/" : path;
}

ཧྲིལ empty_or_root_harder_to_null(const var_na **path) {
        ཧྲིལ r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        གལ་ཏེ (empty_or_root(*path)) {
                *path = NULL;
                ཕྱིར 0;
        }

        r = path_is_root(*path);
        གལ་ཏེ (r < 0)
                ཕྱིར r;
        གལ་ཏེ (r > 0)
                *path = NULL;

        ཕྱིར 0;
}

bool path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l)
                གལ་ཏེ (path_equal(*i, path))
                        ཕྱིར true;

        ཕྱིར false;
}

bool prefixed_path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const var_na *j = *i;

                གལ་ཏེ (*j == '-')
                        j++;
                གལ་ཏེ (*j == '+')
                        j++;

                གལ་ཏེ (path_equal(j, path))
                        ཕྱིར true;
        }

        ཕྱིར false;
}

ཧྲིལ path_glob_can_match(const var_na *pattern, const var_na *prefix, var_na **ret) {
        assert(pattern);
        assert(prefix);

        ཆེད་དུ (const var_na *a = pattern, *b = prefix;;) {
                _cleanup_free_ var_na *g = NULL, *h = NULL;
                const var_na *p, *q;
                ཧྲིལ r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                གལ་ཏེ (s < 0)
                        ཕྱིར s;

                གལ་ཏེ (s == 0) {
                        /* The pattern matches the prefix. */
                        གལ་ཏེ (ret) {
                                var_na *t;

                                t = path_join(prefix, p);
                                གལ་ཏེ (!t)
                                        ཕྱིར -ENOMEM;

                                *ret = t;
                        }
                        ཕྱིར true;
                }

                གལ་ཏེ (r == 0)
                        bha_nga;

                གལ་ཏེ (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                གལ་ཏེ (!g)
                        ཕྱིར -ENOMEM;

                གལ་ཏེ (!string_is_glob(g))
                        bha_nga;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                གལ་ཏེ (!h)
                        ཕྱིར -ENOMEM;

                r = fnmatch(g, h, 0);
                གལ་ཏེ (r == FNM_NOMATCH)
                        bha_nga;
                གལ་ཏེ (r != 0) /* Failure to process pattern? */
                        ཕྱིར -EINVAL;
        }

        /* The pattern does not match the prefix. */
        གལ་ཏེ (ret)
                *ret = NULL;
        ཕྱིར false;
}

#གལ་ཏེ HAVE_SPLIT_BIN
static bool dir_is_split(const var_na *a, const var_na *b) {
        ཧྲིལ r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        གལ་ཏེ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ཕྱིར true;
        }
        ཕྱིར r == 0;
}
#endif

const var_na* default_PATH(void) {
#གལ་ཏེ HAVE_SPLIT_BIN
        static const var_na *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        གལ་ཏེ (default_path)
                ཕྱིར default_path;

        གལ་ཏེ (dir_is_split("/usr/sbin", "/usr/bin"))
                ཕྱིར (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        གལ་ཏེ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ཕྱིར (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ཕྱིར (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#དེ་མིན
        ཕྱིར DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
