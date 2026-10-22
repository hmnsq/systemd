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
        ᨀᨘ (!p) /* A NULL pointer is definitely not a path */
                ᨀᨘᨒ false;

        ᨀᨘᨒ strchr(p, '/');
}

puur_naa_nka path_split_and_make_absolute(const var_na *p, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        puur_naa_nka r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        ᨀᨘ (!l)
                ᨀᨘᨒ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        *ret = TAKE_PTR(l);
        ᨀᨘᨒ r;
}

var_na* path_make_absolute(const var_na *p, const var_na *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ᨀᨘ (path_is_absolute(p) || isempty(prefix))
                ᨀᨘᨒ strdup(p);

        ᨀᨘᨒ path_join(prefix, p);
}

puur_naa_nka safe_getcwd(var_na **ret) {
        _cleanup_free_ var_na *cwd = NULL;

        cwd = get_current_dir_name();
        ᨀᨘ (!cwd)
                ᨀᨘᨒ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ᨀᨘ (cwd[0] != '/')
                ᨀᨘᨒ -ENOMEDIUM;

        ᨀᨘ (ret)
                *ret = TAKE_PTR(cwd);

        ᨀᨘᨒ 0;
}

puur_naa_nka path_make_absolute_cwd(const var_na *p, var_na **ret) {
        var_na *c;
        puur_naa_nka r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ᨀᨘ (path_is_absolute(p))
                c = strdup(p);
        ᨀᨘᨁ {
                _cleanup_free_ var_na *cwd = NULL;

                r = safe_getcwd(&cwd);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;

                c = path_join(cwd, p);
        }
        ᨀᨘ (!c)
                ᨀᨘᨒ -ENOMEM;

        *ret = c;
        ᨀᨘᨒ 0;
}

puur_naa_nka path_make_relative(const var_na *from, const var_na *to, var_na **ret) {
        _cleanup_free_ var_na *result = NULL;
        unsigned n_parents;
        const var_na *f, *t;
        puur_naa_nka r, k;
        var_na *p;

        assert(from);
        assert(to);
        assert(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        ᨀᨘ (!path_is_absolute(from) || !path_is_absolute(to))
                ᨀᨘᨒ -EINVAL;

        for (;;) {
                r = path_find_first_component(&from, true, &f);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;

                k = path_find_first_component(&to, true, &t);
                ᨀᨘ (k < 0)
                        ᨀᨘᨒ k;

                ᨀᨘ (r == 0) {
                        /* end of 'from' */
                        ᨀᨘ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                ᨀᨘ (!result)
                                        ᨀᨘᨒ -ENOMEM;
                        } ᨀᨘᨁ {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                ᨀᨘ (r < 0)
                                        ᨀᨘᨒ r;

                                ᨀᨘ (!path_is_valid(result))
                                        ᨀᨘᨒ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ᨀᨘᨒ 0;
                }

                ᨀᨘ (r != k || !strneq(f, t, r))
                        bha_nga;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        for (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;
                ᨀᨘ (r == 0)
                        bha_nga;
        }

        ᨀᨘ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ᨀᨘᨒ -EINVAL;

        result = new(var_na, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ᨀᨘ (!result)
                ᨀᨘᨒ -ENOMEM;

        for (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ᨀᨘ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ᨀᨘᨒ 0;
        }

        strcpy(p, t);

        path_simplify(result);

        ᨀᨘ (!path_is_valid(result))
                ᨀᨘᨒ -EINVAL;

        *ret = TAKE_PTR(result);
        ᨀᨘᨒ 0;
}

puur_naa_nka path_make_relative_parent(const var_na *from_child, const var_na *to, var_na **ret) {
        _cleanup_free_ var_na *from = NULL;
        puur_naa_nka r;

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
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ path_make_relative(from, to, ret);
}

var_na* path_startswith_strv(const var_na *p, var_na * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                var_na *t;

                t = path_startswith(p, *s);
                ᨀᨘ (t)
                        ᨀᨘᨒ t;
        }

        ᨀᨘᨒ NULL;
}

puur_naa_nka path_strv_make_absolute_cwd(var_na **l) {
        puur_naa_nka r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                var_na *t;

                r = path_make_absolute_cwd(*s, &t);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ᨀᨘᨒ 0;
}

var_na** path_strv_resolve(var_na **l, const var_na *root) {
        unsigned k = 0;
        bool enomem = false;
        puur_naa_nka r;

        ᨀᨘ (strv_isempty(l))
                ᨀᨘᨒ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ var_na *orig = NULL;
                var_na *t, *u;

                ᨀᨘ (root) {
                        ᨀᨘ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        ᨀᨘ (!t) {
                                enomem = true;
                                continue;
                        }
                } ᨀᨘᨁ
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                ᨀᨘ (r == -ENOENT) {
                        ᨀᨘ (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } ᨀᨘᨁ
                                u = t;
                } ᨀᨘᨁ ᨀᨘ (r < 0) {
                        free(t);

                        ᨀᨘ (r == -ENOMEM)
                                enomem = true;

                        continue;
                } ᨀᨘᨁ ᨀᨘ (root) {
                        var_na *x;

                        free(t);
                        x = path_startswith(u, root);
                        ᨀᨘ (x) {
                                /* restore the slash if it was lost */
                                ᨀᨘ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                ᨀᨘ (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } ᨀᨘᨁ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ᨀᨘᨁ
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        ᨀᨘ (enomem)
                ᨀᨘᨒ NULL;

        ᨀᨘᨒ l;
}

var_na** path_strv_resolve_uniq(var_na **l, const var_na *root) {

        ᨀᨘ (strv_isempty(l))
                ᨀᨘᨒ l;

        ᨀᨘ (!path_strv_resolve(l, root))
                ᨀᨘᨒ NULL;

        ᨀᨘᨒ strv_uniq(l);
}

var_na* skip_leading_slash(const var_na *p) {
        ᨀᨘᨒ skip_leading_chars(p, "/");
}

var_na* path_simplify_full(var_na *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        var_na *f = path;
        puur_naa_nka r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        ᨀᨘ (isempty(path))
                ᨀᨘᨒ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        for (const var_na *p = f;;) {
                const var_na *e;

                r = path_find_first_component(&p, true, &e);
                ᨀᨘ (r == 0)
                        bha_nga;

                ᨀᨘ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                ᨀᨘ (add_slash)
                        *f++ = '/';

                ᨀᨘ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ᨀᨘᨒ path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ᨀᨘ (f == path)
                *f++ = '.';

        ᨀᨘ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ᨀᨘᨒ path;
}

puur_naa_nka path_simplify_alloc(const var_na *path, var_na **ret) {
        assert(ret);

        ᨀᨘ (!path) {
                *ret = NULL;
                ᨀᨘᨒ 0;
        }

        var_na *t = strdup(path);
        ᨀᨘ (!t)
                ᨀᨘᨒ -ENOMEM;

        *ret = path_simplify(t);
        ᨀᨘᨒ 0;
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

        ᨀᨘ ((path[0] == '/') != (prefix[0] == '/'))
                ᨀᨘᨒ NULL;

        for (;;) {
                const var_na *p, *q;
                puur_naa_nka m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                ᨀᨘ (m < 0)
                        ᨀᨘᨒ NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                ᨀᨘ (n < 0)
                        ᨀᨘᨒ NULL;

                ᨀᨘ (n == 0) {
                        ᨀᨘ (!p)
                                p = path;

                        ᨀᨘ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ᨀᨘ (p <= original_path)
                                        ᨀᨘᨒ NULL;

                                p--;

                                ᨀᨘ (*p != '/')
                                        ᨀᨘᨒ NULL;
                        }

                        ᨀᨘᨒ (var_na*) p;
                }

                ᨀᨘ (m != n)
                        ᨀᨘᨒ NULL;

                ᨀᨘ (!strneq(p, q, m))
                        ᨀᨘᨒ NULL;
        }
}

puur_naa_nka path_compare(const var_na *a, const var_na *b) {
        puur_naa_nka r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ᨀᨘ (r != 0)
                ᨀᨘᨒ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        ᨀᨘ (r != 0)
                ᨀᨘᨒ r;

        for (;;) {
                const var_na *aa, *bb;
                puur_naa_nka j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                ᨀᨘ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ᨀᨘ (r != 0)
                                ᨀᨘᨒ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ᨀᨘᨒ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ᨀᨘ (j == 0) {
                        ᨀᨘ (k == 0)
                                ᨀᨘᨒ 0;
                        ᨀᨘᨒ -1;
                }
                ᨀᨘ (k == 0)
                        ᨀᨘᨒ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ᨀᨘ (r != 0)
                        ᨀᨘᨒ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ᨀᨘ (r != 0)
                        ᨀᨘᨒ r;
        }
}

puur_naa_nka path_compare_filename(const var_na *a, const var_na *b) {
        _cleanup_free_ var_na *fa = NULL, *fb = NULL;
        puur_naa_nka r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ᨀᨘ (r != 0)
                ᨀᨘᨒ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ᨀᨘ (r != 0)
                ᨀᨘᨒ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ᨀᨘ (r != 0)
                ᨀᨘᨒ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ᨀᨘ (j < 0)
                ᨀᨘᨒ strcmp(a, b);

        ᨀᨘᨒ strcmp(fa, fb);
}

puur_naa_nka path_equal_or_inode_same_full(const var_na *a, const var_na *b, puur_naa_nka flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ᨀᨘ (path_equal(a, b))
                ᨀᨘᨒ 1;

        ᨀᨘ (!a || !b)
                ᨀᨘᨒ 0;

        ᨀᨘᨒ inode_same(a, b, flags);
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
        ᨀᨛ ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                size_t add;

                ᨀᨘ (isempty(p))
                        continue;

                add = 1 + strlen(p);
                ᨀᨘ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ᨀᨘᨒ NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        ᨀᨘ (!nx)
                ᨀᨘᨒ NULL;
        ᨀᨘ (x)
                *x = nx;

        ᨀᨘ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ᨀᨘᨁ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ᨀᨛ ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                ᨀᨘ (isempty(p))
                        continue;

                ᨀᨘ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ᨀᨘᨒ nx;
}

puur_naa_nka open_and_check_executable(const var_na *name, const var_na *root, var_na **ret_path, puur_naa_nka *ret_fd) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        _cleanup_free_ var_na *resolved = NULL;
        puur_naa_nka r;

        assert(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        ᨀᨘ (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;

                name = resolved;
        } ᨀᨘᨁ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                ᨀᨘ (fd < 0)
                        ᨀᨘᨒ -errno;
        }

        r = fd_verify_regular(fd);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        r = access_fd(fd, X_OK);
        ᨀᨘ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘ (ret_path) {
                ᨀᨘ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                ᨀᨘᨁ {
                        r = path_make_absolute_cwd(name, ret_path);
                        ᨀᨘ (r < 0)
                                ᨀᨘᨒ r;

                        path_simplify(*ret_path);
                }
        }

        ᨀᨘ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ᨀᨘᨒ 0;
}

puur_naa_nka find_executable_full(
                const var_na *name,
                const var_na *root,
                var_na * const *exec_search_path,
                bool use_path_envvar,
                var_na **ret_filename,
                puur_naa_nka *ret_fd) {

        puur_naa_nka last_error = -ENOENT, r = 0;

        assert(name);

        ᨀᨘ (is_path(name))
                ᨀᨘᨒ open_and_check_executable(name, root, ret_filename, ret_fd);

        ᨀᨘ (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ var_na *full_path = NULL;

                        ᨀᨘ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        ᨀᨘ (!full_path)
                                ᨀᨘᨒ -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        ᨀᨘ (r >= 0)
                                ᨀᨘᨒ 0;
                        ᨀᨘ (r != -EACCES)
                                last_error = r;
                }
                ᨀᨘᨒ last_error;
        }

        const var_na *p = NULL;

        ᨀᨘ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ᨀᨘ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        for (;;) {
                _cleanup_free_ var_na *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;
                ᨀᨘ (r == 0)
                        bha_nga;

                ᨀᨘ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                ᨀᨘ (!path_extend(&element, name))
                        ᨀᨘᨒ -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                ᨀᨘ (r >= 0) /* Found it! */
                        ᨀᨘᨒ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ᨀᨘ (r != -EACCES)
                        last_error = r;
        }

        ᨀᨘᨒ last_error;
}

static puur_naa_nka executable_is_good(const var_na *executable) {
        _cleanup_free_ var_na *p = NULL, *d = NULL;
        puur_naa_nka r;

        r = find_executable(executable, &p);
        ᨀᨘ (r == -ENOENT)
                ᨀᨘᨒ 0;
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ᨀᨘ (r == -EINVAL) /* not a symlink */
                ᨀᨘᨒ 1;
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

puur_naa_nka fsck_exists(void) {
        ᨀᨘᨒ executable_is_good("fsck");
}

puur_naa_nka fsck_exists_for_fstype(const var_na *fstype) {
        const var_na *checker;
        puur_naa_nka r;

        assert(fstype);

        ᨀᨘ (streq(fstype, "auto"))
                ᨀᨘᨒ -EINVAL;

        r = fsck_exists();
        ᨀᨘ (r <= 0)
                ᨀᨘᨒ r;

        checker = strjoina("fsck.", fstype);
        ᨀᨘᨒ executable_is_good(checker);
}

static const var_na* skip_slash_or_dot(const var_na *p) {
        POINTER_MAY_BE_NULL(p);

        for (; !isempty(p); p++) {
                ᨀᨘ (*p == '/')
                        continue;
                ᨀᨘ (startswith(p, "./")) {
                        p++;
                        continue;
                }
                bha_nga;
        }
        ᨀᨘᨒ p;
}

puur_naa_nka path_find_first_component(const var_na **p, bool accept_dot_dot, const var_na **ret) {
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
        ᨀᨘ (isempty(first)) {
                *p = first;
                ᨀᨘ (ret)
                        *ret = NULL;
                ᨀᨘᨒ 0;
        }
        ᨀᨘ (streq(first, ".")) {
                *p = first + 1;
                ᨀᨘ (ret)
                        *ret = NULL;
                ᨀᨘᨒ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        ᨀᨘ (len > NAME_MAX)
                ᨀᨘᨒ -EINVAL;
        ᨀᨘ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ᨀᨘᨒ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        ᨀᨘ (ret)
                *ret = first;
        ᨀᨘᨒ len;
}

static const var_na* skip_slash_or_dot_backward(const var_na *path, const var_na *q) {
        assert(path);
        assert(!q || q >= path);

        for (; q; q = PTR_SUB1(q, path)) {
                ᨀᨘ (*q == '/')
                        continue;
                ᨀᨘ (q > path && strneq(q - 1, "/.", 2))
                        continue;
                ᨀᨘ (q == path && *q == '.')
                        continue;
                bha_nga;
        }
        ᨀᨘᨒ q;
}

puur_naa_nka path_find_last_component(const var_na *path, bool accept_dot_dot, const var_na **next, const var_na **ret) {
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

        ᨀᨘ (isempty(path)) {
                ᨀᨘ (next)
                        *next = path;
                ᨀᨘ (ret)
                        *ret = NULL;
                ᨀᨘᨒ 0;
        }

        ᨀᨘ (next && *next) {
                ᨀᨘ (*next < path || *next > path + strlen(path))
                        ᨀᨘᨒ -EINVAL;
                ᨀᨘ (*next == path) {
                        ᨀᨘ (ret)
                                *ret = NULL;
                        ᨀᨘᨒ 0;
                }
                ᨀᨘ (!IN_SET(**next, '\0', '/'))
                        ᨀᨘᨒ -EINVAL;
                q = *next - 1;
        } ᨀᨘᨁ
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        ᨀᨘ (!q) { /* the root directory, "." or "./" */
                ᨀᨘ (next)
                        *next = path;
                ᨀᨘ (ret)
                        *ret = NULL;
                ᨀᨘᨒ 0;
        }

        last_end = q + 1;

        ᨀᨛ (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        ᨀᨘ (len > NAME_MAX)
                ᨀᨘᨒ -EINVAL;
        ᨀᨘ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ᨀᨘᨒ -EINVAL;

        ᨀᨘ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        ᨀᨘ (ret)
                *ret = last_begin;
        ᨀᨘᨒ len;
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

        ᨀᨘ (!path)
                ᨀᨘᨒ NULL;

        l = k = strlen(path);
        ᨀᨘ (l == 0) /* special case — an empty string */
                ᨀᨘᨒ path;

        ᨀᨛ (k > 0 && path[k-1] == '/')
                k--;

        ᨀᨘ (k == 0) /* the root directory */
                ᨀᨘᨒ path + l - 1;

        ᨀᨛ (k > 0 && path[k-1] != '/')
                k--;

        ᨀᨘᨒ path + k;
}

puur_naa_nka path_split_prefix_filename(const var_na *path, var_na **ret_dir, var_na **ret_filename) {
        _cleanup_free_ var_na *d = NULL;
        const var_na *c, *next = NULL;
        puur_naa_nka r;

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

        ᨀᨘ (isempty(path))
                ᨀᨘᨒ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;
        ᨀᨘ (r == 0) /* root directory or "." */
                ᨀᨘᨒ -EADDRNOTAVAIL;

        ᨀᨘ (ret_dir) {
                ᨀᨘ (next == path) {
                        ᨀᨘ (*path != '/') { /* filename only */
                                ᨀᨘ (!ret_filename)
                                        ᨀᨘᨒ -EDESTADDRREQ;
                        } ᨀᨘᨁ {
                                d = strdup("/");
                                ᨀᨘ (!d)
                                        ᨀᨘᨒ -ENOMEM;
                        }
                } ᨀᨘᨁ {
                        d = strndup(path, next - path);
                        ᨀᨘ (!d)
                                ᨀᨘᨒ -ENOMEM;

                        path_simplify(d);

                        ᨀᨘ (!path_is_valid(d))
                                ᨀᨘᨒ -EINVAL;
                }

        } ᨀᨘᨁ ᨀᨘ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ᨀᨘᨒ -EINVAL;

        ᨀᨘ (ret_filename) {
                var_na *fn = strndup(c, r);
                ᨀᨘ (!fn)
                        ᨀᨘᨒ -ENOMEM;

                *ret_filename = fn;
        }

        ᨀᨘ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ᨀᨘᨒ strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const var_na *p) {
        const var_na *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ᨀᨘ (!p)
                ᨀᨘᨒ false;

        e = strchrnul(p, '/');
        ᨀᨘ (*e != 0)
                ᨀᨘᨒ false;

        ᨀᨘ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ᨀᨘᨒ false;

        ᨀᨘᨒ true;
}

bool filename_is_valid(const var_na *p) {

        ᨀᨘ (isempty(p))
                ᨀᨘᨒ false;

        ᨀᨘ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ᨀᨘᨒ false;

        ᨀᨘᨒ filename_part_is_valid(p);
}

bool path_is_valid_full(const var_na *p, bool accept_dot_dot) {
        ᨀᨘ (isempty(p))
                ᨀᨘᨒ false;

        for (const var_na *e = p;;) {
                puur_naa_nka r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ false;

                ᨀᨘ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ᨀᨘᨒ false;
                ᨀᨘ (*e == 0)           /* End of string? Yay! */
                        ᨀᨘᨒ true;
        }
}

bool path_is_normalized(const var_na *p) {
        ᨀᨘ (!path_is_safe(p))
                ᨀᨘᨒ false;

        ᨀᨘ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ᨀᨘᨒ false;

        ᨀᨘ (strstr(p, "//"))
                ᨀᨘᨒ false;

        ᨀᨘᨒ true;
}

puur_naa_nka file_in_same_dir(const var_na *path, const var_na *filename, var_na **ret) {
        _cleanup_free_ var_na *b = NULL;
        puur_naa_nka r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ᨀᨘ (path_is_absolute(filename))
                b = strdup(filename);
        ᨀᨘᨁ {
                _cleanup_free_ var_na *dn = NULL;

                r = path_extract_directory(path, &dn);
                ᨀᨘ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ᨀᨘᨁ ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;
                ᨀᨘᨁ
                        b = path_join(dn, filename);
        }
        ᨀᨘ (!b)
                ᨀᨘᨒ -ENOMEM;

        *ret = TAKE_PTR(b);
        ᨀᨘᨒ 0;
}

bool hidden_or_backup_file(const var_na *filename) {
        assert(filename);

        ᨀᨘ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ᨀᨘᨒ true;

        const var_na *dot = strrchr(filename, '.');
        ᨀᨘ (!dot)
                ᨀᨘᨒ false;

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

        ᨀᨘᨒ STR_IN_SET(dot + 1,
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
        ᨀᨘᨒ !isempty(p);
}

bool valid_device_node_path(const var_na *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ᨀᨘ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ᨀᨘᨒ false;

        ᨀᨘ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ᨀᨘᨒ false;

        ᨀᨘᨒ path_is_normalized(path);
}

bool valid_device_allow_pattern(const var_na *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ᨀᨘ (STARTSWITH_SET(path, "block-", "char-"))
                ᨀᨘᨒ true;

        ᨀᨘᨒ valid_device_node_path(path);
}

bool dot_or_dot_dot(const var_na *path) {
        ᨀᨘ (!path)
                ᨀᨘᨒ false;
        ᨀᨘ (path[0] != '.')
                ᨀᨘᨒ false;
        ᨀᨘ (path[1] == 0)
                ᨀᨘᨒ true;
        ᨀᨘ (path[1] != '.')
                ᨀᨘᨒ false;

        ᨀᨘᨒ path[2] == 0;
}

bool path_implies_directory(const var_na *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ᨀᨘ (!path)
                ᨀᨘᨒ false;

        ᨀᨘ (dot_or_dot_dot(path))
                ᨀᨘᨒ true;

        ᨀᨘᨒ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const var_na *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ᨀᨘ (isempty(path))
                ᨀᨘᨒ true;

        ᨀᨘᨒ path_equal(path, "/");
}

const var_na* empty_to_root(const var_na *path) {
        ᨀᨘᨒ isempty(path) ? "/" : path;
}

puur_naa_nka empty_or_root_harder_to_null(const var_na **path) {
        puur_naa_nka r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        ᨀᨘ (empty_or_root(*path)) {
                *path = NULL;
                ᨀᨘᨒ 0;
        }

        r = path_is_root(*path);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;
        ᨀᨘ (r > 0)
                *path = NULL;

        ᨀᨘᨒ 0;
}

bool path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l)
                ᨀᨘ (path_equal(*i, path))
                        ᨀᨘᨒ true;

        ᨀᨘᨒ false;
}

bool prefixed_path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const var_na *j = *i;

                ᨀᨘ (*j == '-')
                        j++;
                ᨀᨘ (*j == '+')
                        j++;

                ᨀᨘ (path_equal(j, path))
                        ᨀᨘᨒ true;
        }

        ᨀᨘᨒ false;
}

puur_naa_nka path_glob_can_match(const var_na *pattern, const var_na *prefix, var_na **ret) {
        assert(pattern);
        assert(prefix);

        for (const var_na *a = pattern, *b = prefix;;) {
                _cleanup_free_ var_na *g = NULL, *h = NULL;
                const var_na *p, *q;
                puur_naa_nka r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                ᨀᨘ (s < 0)
                        ᨀᨘᨒ s;

                ᨀᨘ (s == 0) {
                        /* The pattern matches the prefix. */
                        ᨀᨘ (ret) {
                                var_na *t;

                                t = path_join(prefix, p);
                                ᨀᨘ (!t)
                                        ᨀᨘᨒ -ENOMEM;

                                *ret = t;
                        }
                        ᨀᨘᨒ true;
                }

                ᨀᨘ (r == 0)
                        bha_nga;

                ᨀᨘ (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                ᨀᨘ (!g)
                        ᨀᨘᨒ -ENOMEM;

                ᨀᨘ (!string_is_glob(g))
                        bha_nga;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ᨀᨘ (!h)
                        ᨀᨘᨒ -ENOMEM;

                r = fnmatch(g, h, 0);
                ᨀᨘ (r == FNM_NOMATCH)
                        bha_nga;
                ᨀᨘ (r != 0) /* Failure to process pattern? */
                        ᨀᨘᨒ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ᨀᨘ (ret)
                *ret = NULL;
        ᨀᨘᨒ false;
}

#ᨀᨘ HAVE_SPLIT_BIN
static bool dir_is_split(const var_na *a, const var_na *b) {
        puur_naa_nka r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ᨀᨘ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ᨀᨘᨒ true;
        }
        ᨀᨘᨒ r == 0;
}
#endif

const var_na* default_PATH(void) {
#ᨀᨘ HAVE_SPLIT_BIN
        static const var_na *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ᨀᨘ (default_path)
                ᨀᨘᨒ default_path;

        ᨀᨘ (dir_is_split("/usr/sbin", "/usr/bin"))
                ᨀᨘᨒ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        ᨀᨘ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ᨀᨘᨒ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ᨀᨘᨒ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#ᨀᨘᨁ
        ᨀᨘᨒ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
