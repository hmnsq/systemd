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
        ᯀᯮᯝ (!p) /* A NULL pointer is definitely not a path */
                ᯀᯮᯞ false;

        ᯀᯮᯞ strchr(p, '/');
}

puur_naa_nka path_split_and_make_absolute(const var_na *p, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        puur_naa_nka r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        ᯀᯮᯝ (!l)
                ᯀᯮᯞ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        *ret = TAKE_PTR(l);
        ᯀᯮᯞ r;
}

var_na* path_make_absolute(const var_na *p, const var_na *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ᯀᯮᯝ (path_is_absolute(p) || isempty(prefix))
                ᯀᯮᯞ strdup(p);

        ᯀᯮᯞ path_join(prefix, p);
}

puur_naa_nka safe_getcwd(var_na **ret) {
        _cleanup_free_ var_na *cwd = NULL;

        cwd = get_current_dir_name();
        ᯀᯮᯝ (!cwd)
                ᯀᯮᯞ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ᯀᯮᯝ (cwd[0] != '/')
                ᯀᯮᯞ -ENOMEDIUM;

        ᯀᯮᯝ (ret)
                *ret = TAKE_PTR(cwd);

        ᯀᯮᯞ 0;
}

puur_naa_nka path_make_absolute_cwd(const var_na *p, var_na **ret) {
        var_na *c;
        puur_naa_nka r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ᯀᯮᯝ (path_is_absolute(p))
                c = strdup(p);
        ᯀᯮᯝᯂᯪ {
                _cleanup_free_ var_na *cwd = NULL;

                r = safe_getcwd(&cwd);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                c = path_join(cwd, p);
        }
        ᯀᯮᯝ (!c)
                ᯀᯮᯞ -ENOMEM;

        *ret = c;
        ᯀᯮᯞ 0;
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

        ᯀᯮᯝ (!path_is_absolute(from) || !path_is_absolute(to))
                ᯀᯮᯞ -EINVAL;

        for (;;) {
                r = path_find_first_component(&from, true, &f);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                k = path_find_first_component(&to, true, &t);
                ᯀᯮᯝ (k < 0)
                        ᯀᯮᯞ k;

                ᯀᯮᯝ (r == 0) {
                        /* end of 'from' */
                        ᯀᯮᯝ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                ᯀᯮᯝ (!result)
                                        ᯀᯮᯞ -ENOMEM;
                        } ᯀᯮᯝᯂᯪ {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                ᯀᯮᯝ (r < 0)
                                        ᯀᯮᯞ r;

                                ᯀᯮᯝ (!path_is_valid(result))
                                        ᯀᯮᯞ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ᯀᯮᯞ 0;
                }

                ᯀᯮᯝ (r != k || !strneq(f, t, r))
                        bha_nga;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        for (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
                ᯀᯮᯝ (r == 0)
                        bha_nga;
        }

        ᯀᯮᯝ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ᯀᯮᯞ -EINVAL;

        result = new(var_na, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ᯀᯮᯝ (!result)
                ᯀᯮᯞ -ENOMEM;

        for (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ᯀᯮᯝ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ᯀᯮᯞ 0;
        }

        strcpy(p, t);

        path_simplify(result);

        ᯀᯮᯝ (!path_is_valid(result))
                ᯀᯮᯞ -EINVAL;

        *ret = TAKE_PTR(result);
        ᯀᯮᯞ 0;
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
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ path_make_relative(from, to, ret);
}

var_na* path_startswith_strv(const var_na *p, var_na * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                var_na *t;

                t = path_startswith(p, *s);
                ᯀᯮᯝ (t)
                        ᯀᯮᯞ t;
        }

        ᯀᯮᯞ NULL;
}

puur_naa_nka path_strv_make_absolute_cwd(var_na **l) {
        puur_naa_nka r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                var_na *t;

                r = path_make_absolute_cwd(*s, &t);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ᯀᯮᯞ 0;
}

var_na** path_strv_resolve(var_na **l, const var_na *root) {
        unsigned k = 0;
        bool enomem = false;
        puur_naa_nka r;

        ᯀᯮᯝ (strv_isempty(l))
                ᯀᯮᯞ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ var_na *orig = NULL;
                var_na *t, *u;

                ᯀᯮᯝ (root) {
                        ᯀᯮᯝ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        ᯀᯮᯝ (!t) {
                                enomem = true;
                                continue;
                        }
                } ᯀᯮᯝᯂᯪ
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                ᯀᯮᯝ (r == -ENOENT) {
                        ᯀᯮᯝ (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } ᯀᯮᯝᯂᯪ
                                u = t;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (r < 0) {
                        free(t);

                        ᯀᯮᯝ (r == -ENOMEM)
                                enomem = true;

                        continue;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (root) {
                        var_na *x;

                        free(t);
                        x = path_startswith(u, root);
                        ᯀᯮᯝ (x) {
                                /* restore the slash if it was lost */
                                ᯀᯮᯝ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                ᯀᯮᯝ (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } ᯀᯮᯝᯂᯪ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ᯀᯮᯝᯂᯪ
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        ᯀᯮᯝ (enomem)
                ᯀᯮᯞ NULL;

        ᯀᯮᯞ l;
}

var_na** path_strv_resolve_uniq(var_na **l, const var_na *root) {

        ᯀᯮᯝ (strv_isempty(l))
                ᯀᯮᯞ l;

        ᯀᯮᯝ (!path_strv_resolve(l, root))
                ᯀᯮᯞ NULL;

        ᯀᯮᯞ strv_uniq(l);
}

var_na* skip_leading_slash(const var_na *p) {
        ᯀᯮᯞ skip_leading_chars(p, "/");
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

        ᯀᯮᯝ (isempty(path))
                ᯀᯮᯞ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        for (const var_na *p = f;;) {
                const var_na *e;

                r = path_find_first_component(&p, true, &e);
                ᯀᯮᯝ (r == 0)
                        bha_nga;

                ᯀᯮᯝ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                ᯀᯮᯝ (add_slash)
                        *f++ = '/';

                ᯀᯮᯝ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ᯀᯮᯞ path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ᯀᯮᯝ (f == path)
                *f++ = '.';

        ᯀᯮᯝ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ᯀᯮᯞ path;
}

puur_naa_nka path_simplify_alloc(const var_na *path, var_na **ret) {
        assert(ret);

        ᯀᯮᯝ (!path) {
                *ret = NULL;
                ᯀᯮᯞ 0;
        }

        var_na *t = strdup(path);
        ᯀᯮᯝ (!t)
                ᯀᯮᯞ -ENOMEM;

        *ret = path_simplify(t);
        ᯀᯮᯞ 0;
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

        ᯀᯮᯝ ((path[0] == '/') != (prefix[0] == '/'))
                ᯀᯮᯞ NULL;

        for (;;) {
                const var_na *p, *q;
                puur_naa_nka m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                ᯀᯮᯝ (m < 0)
                        ᯀᯮᯞ NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                ᯀᯮᯝ (n < 0)
                        ᯀᯮᯞ NULL;

                ᯀᯮᯝ (n == 0) {
                        ᯀᯮᯝ (!p)
                                p = path;

                        ᯀᯮᯝ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ᯀᯮᯝ (p <= original_path)
                                        ᯀᯮᯞ NULL;

                                p--;

                                ᯀᯮᯝ (*p != '/')
                                        ᯀᯮᯞ NULL;
                        }

                        ᯀᯮᯞ (var_na*) p;
                }

                ᯀᯮᯝ (m != n)
                        ᯀᯮᯞ NULL;

                ᯀᯮᯝ (!strneq(p, q, m))
                        ᯀᯮᯞ NULL;
        }
}

puur_naa_nka path_compare(const var_na *a, const var_na *b) {
        puur_naa_nka r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ᯀᯮᯝ (r != 0)
                ᯀᯮᯞ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        ᯀᯮᯝ (r != 0)
                ᯀᯮᯞ r;

        for (;;) {
                const var_na *aa, *bb;
                puur_naa_nka j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                ᯀᯮᯝ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ᯀᯮᯝ (r != 0)
                                ᯀᯮᯞ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ᯀᯮᯞ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ᯀᯮᯝ (j == 0) {
                        ᯀᯮᯝ (k == 0)
                                ᯀᯮᯞ 0;
                        ᯀᯮᯞ -1;
                }
                ᯀᯮᯝ (k == 0)
                        ᯀᯮᯞ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ᯀᯮᯝ (r != 0)
                        ᯀᯮᯞ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ᯀᯮᯝ (r != 0)
                        ᯀᯮᯞ r;
        }
}

puur_naa_nka path_compare_filename(const var_na *a, const var_na *b) {
        _cleanup_free_ var_na *fa = NULL, *fb = NULL;
        puur_naa_nka r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ᯀᯮᯝ (r != 0)
                ᯀᯮᯞ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ᯀᯮᯝ (r != 0)
                ᯀᯮᯞ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ᯀᯮᯝ (r != 0)
                ᯀᯮᯞ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ᯀᯮᯝ (j < 0)
                ᯀᯮᯞ strcmp(a, b);

        ᯀᯮᯞ strcmp(fa, fb);
}

puur_naa_nka path_equal_or_inode_same_full(const var_na *a, const var_na *b, puur_naa_nka flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ᯀᯮᯝ (path_equal(a, b))
                ᯀᯮᯞ 1;

        ᯀᯮᯝ (!a || !b)
                ᯀᯮᯞ 0;

        ᯀᯮᯞ inode_same(a, b, flags);
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
        ᯀᯬᯝ ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                size_t add;

                ᯀᯮᯝ (isempty(p))
                        continue;

                add = 1 + strlen(p);
                ᯀᯮᯝ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ᯀᯮᯞ NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        ᯀᯮᯝ (!nx)
                ᯀᯮᯞ NULL;
        ᯀᯮᯝ (x)
                *x = nx;

        ᯀᯮᯝ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ᯀᯮᯝᯂᯪ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ᯀᯬᯝ ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                ᯀᯮᯝ (isempty(p))
                        continue;

                ᯀᯮᯝ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ᯀᯮᯞ nx;
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
        ᯀᯮᯝ (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                name = resolved;
        } ᯀᯮᯝᯂᯪ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                ᯀᯮᯝ (fd < 0)
                        ᯀᯮᯞ -errno;
        }

        r = fd_verify_regular(fd);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        r = access_fd(fd, X_OK);
        ᯀᯮᯝ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (ret_path) {
                ᯀᯮᯝ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                ᯀᯮᯝᯂᯪ {
                        r = path_make_absolute_cwd(name, ret_path);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ r;

                        path_simplify(*ret_path);
                }
        }

        ᯀᯮᯝ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ᯀᯮᯞ 0;
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

        ᯀᯮᯝ (is_path(name))
                ᯀᯮᯞ open_and_check_executable(name, root, ret_filename, ret_fd);

        ᯀᯮᯝ (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ var_na *full_path = NULL;

                        ᯀᯮᯝ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        ᯀᯮᯝ (!full_path)
                                ᯀᯮᯞ -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        ᯀᯮᯝ (r >= 0)
                                ᯀᯮᯞ 0;
                        ᯀᯮᯝ (r != -EACCES)
                                last_error = r;
                }
                ᯀᯮᯞ last_error;
        }

        const var_na *p = NULL;

        ᯀᯮᯝ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ᯀᯮᯝ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        for (;;) {
                _cleanup_free_ var_na *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
                ᯀᯮᯝ (r == 0)
                        bha_nga;

                ᯀᯮᯝ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                ᯀᯮᯝ (!path_extend(&element, name))
                        ᯀᯮᯞ -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                ᯀᯮᯝ (r >= 0) /* Found it! */
                        ᯀᯮᯞ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ᯀᯮᯝ (r != -EACCES)
                        last_error = r;
        }

        ᯀᯮᯞ last_error;
}

static puur_naa_nka executable_is_good(const var_na *executable) {
        _cleanup_free_ var_na *p = NULL, *d = NULL;
        puur_naa_nka r;

        r = find_executable(executable, &p);
        ᯀᯮᯝ (r == -ENOENT)
                ᯀᯮᯞ 0;
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ᯀᯮᯝ (r == -EINVAL) /* not a symlink */
                ᯀᯮᯞ 1;
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

puur_naa_nka fsck_exists(void) {
        ᯀᯮᯞ executable_is_good("fsck");
}

puur_naa_nka fsck_exists_for_fstype(const var_na *fstype) {
        const var_na *checker;
        puur_naa_nka r;

        assert(fstype);

        ᯀᯮᯝ (streq(fstype, "auto"))
                ᯀᯮᯞ -EINVAL;

        r = fsck_exists();
        ᯀᯮᯝ (r <= 0)
                ᯀᯮᯞ r;

        checker = strjoina("fsck.", fstype);
        ᯀᯮᯞ executable_is_good(checker);
}

static const var_na* skip_slash_or_dot(const var_na *p) {
        POINTER_MAY_BE_NULL(p);

        for (; !isempty(p); p++) {
                ᯀᯮᯝ (*p == '/')
                        continue;
                ᯀᯮᯝ (startswith(p, "./")) {
                        p++;
                        continue;
                }
                bha_nga;
        }
        ᯀᯮᯞ p;
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
        ᯀᯮᯝ (isempty(first)) {
                *p = first;
                ᯀᯮᯝ (ret)
                        *ret = NULL;
                ᯀᯮᯞ 0;
        }
        ᯀᯮᯝ (streq(first, ".")) {
                *p = first + 1;
                ᯀᯮᯝ (ret)
                        *ret = NULL;
                ᯀᯮᯞ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        ᯀᯮᯝ (len > NAME_MAX)
                ᯀᯮᯞ -EINVAL;
        ᯀᯮᯝ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ᯀᯮᯞ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        ᯀᯮᯝ (ret)
                *ret = first;
        ᯀᯮᯞ len;
}

static const var_na* skip_slash_or_dot_backward(const var_na *path, const var_na *q) {
        assert(path);
        assert(!q || q >= path);

        for (; q; q = PTR_SUB1(q, path)) {
                ᯀᯮᯝ (*q == '/')
                        continue;
                ᯀᯮᯝ (q > path && strneq(q - 1, "/.", 2))
                        continue;
                ᯀᯮᯝ (q == path && *q == '.')
                        continue;
                bha_nga;
        }
        ᯀᯮᯞ q;
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

        ᯀᯮᯝ (isempty(path)) {
                ᯀᯮᯝ (next)
                        *next = path;
                ᯀᯮᯝ (ret)
                        *ret = NULL;
                ᯀᯮᯞ 0;
        }

        ᯀᯮᯝ (next && *next) {
                ᯀᯮᯝ (*next < path || *next > path + strlen(path))
                        ᯀᯮᯞ -EINVAL;
                ᯀᯮᯝ (*next == path) {
                        ᯀᯮᯝ (ret)
                                *ret = NULL;
                        ᯀᯮᯞ 0;
                }
                ᯀᯮᯝ (!IN_SET(**next, '\0', '/'))
                        ᯀᯮᯞ -EINVAL;
                q = *next - 1;
        } ᯀᯮᯝᯂᯪ
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        ᯀᯮᯝ (!q) { /* the root directory, "." or "./" */
                ᯀᯮᯝ (next)
                        *next = path;
                ᯀᯮᯝ (ret)
                        *ret = NULL;
                ᯀᯮᯞ 0;
        }

        last_end = q + 1;

        ᯀᯬᯝ (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        ᯀᯮᯝ (len > NAME_MAX)
                ᯀᯮᯞ -EINVAL;
        ᯀᯮᯝ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ᯀᯮᯞ -EINVAL;

        ᯀᯮᯝ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        ᯀᯮᯝ (ret)
                *ret = last_begin;
        ᯀᯮᯞ len;
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

        ᯀᯮᯝ (!path)
                ᯀᯮᯞ NULL;

        l = k = strlen(path);
        ᯀᯮᯝ (l == 0) /* special case — an empty string */
                ᯀᯮᯞ path;

        ᯀᯬᯝ (k > 0 && path[k-1] == '/')
                k--;

        ᯀᯮᯝ (k == 0) /* the root directory */
                ᯀᯮᯞ path + l - 1;

        ᯀᯬᯝ (k > 0 && path[k-1] != '/')
                k--;

        ᯀᯮᯞ path + k;
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

        ᯀᯮᯝ (isempty(path))
                ᯀᯮᯞ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;
        ᯀᯮᯝ (r == 0) /* root directory or "." */
                ᯀᯮᯞ -EADDRNOTAVAIL;

        ᯀᯮᯝ (ret_dir) {
                ᯀᯮᯝ (next == path) {
                        ᯀᯮᯝ (*path != '/') { /* filename only */
                                ᯀᯮᯝ (!ret_filename)
                                        ᯀᯮᯞ -EDESTADDRREQ;
                        } ᯀᯮᯝᯂᯪ {
                                d = strdup("/");
                                ᯀᯮᯝ (!d)
                                        ᯀᯮᯞ -ENOMEM;
                        }
                } ᯀᯮᯝᯂᯪ {
                        d = strndup(path, next - path);
                        ᯀᯮᯝ (!d)
                                ᯀᯮᯞ -ENOMEM;

                        path_simplify(d);

                        ᯀᯮᯝ (!path_is_valid(d))
                                ᯀᯮᯞ -EINVAL;
                }

        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ᯀᯮᯞ -EINVAL;

        ᯀᯮᯝ (ret_filename) {
                var_na *fn = strndup(c, r);
                ᯀᯮᯝ (!fn)
                        ᯀᯮᯞ -ENOMEM;

                *ret_filename = fn;
        }

        ᯀᯮᯝ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ᯀᯮᯞ strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const var_na *p) {
        const var_na *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ᯀᯮᯝ (!p)
                ᯀᯮᯞ false;

        e = strchrnul(p, '/');
        ᯀᯮᯝ (*e != 0)
                ᯀᯮᯞ false;

        ᯀᯮᯝ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ᯀᯮᯞ false;

        ᯀᯮᯞ true;
}

bool filename_is_valid(const var_na *p) {

        ᯀᯮᯝ (isempty(p))
                ᯀᯮᯞ false;

        ᯀᯮᯝ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ᯀᯮᯞ false;

        ᯀᯮᯞ filename_part_is_valid(p);
}

bool path_is_valid_full(const var_na *p, bool accept_dot_dot) {
        ᯀᯮᯝ (isempty(p))
                ᯀᯮᯞ false;

        for (const var_na *e = p;;) {
                puur_naa_nka r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ false;

                ᯀᯮᯝ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ᯀᯮᯞ false;
                ᯀᯮᯝ (*e == 0)           /* End of string? Yay! */
                        ᯀᯮᯞ true;
        }
}

bool path_is_normalized(const var_na *p) {
        ᯀᯮᯝ (!path_is_safe(p))
                ᯀᯮᯞ false;

        ᯀᯮᯝ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ᯀᯮᯞ false;

        ᯀᯮᯝ (strstr(p, "//"))
                ᯀᯮᯞ false;

        ᯀᯮᯞ true;
}

puur_naa_nka file_in_same_dir(const var_na *path, const var_na *filename, var_na **ret) {
        _cleanup_free_ var_na *b = NULL;
        puur_naa_nka r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ᯀᯮᯝ (path_is_absolute(filename))
                b = strdup(filename);
        ᯀᯮᯝᯂᯪ {
                _cleanup_free_ var_na *dn = NULL;

                r = path_extract_directory(path, &dn);
                ᯀᯮᯝ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ᯀᯮᯝᯂᯪ ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
                ᯀᯮᯝᯂᯪ
                        b = path_join(dn, filename);
        }
        ᯀᯮᯝ (!b)
                ᯀᯮᯞ -ENOMEM;

        *ret = TAKE_PTR(b);
        ᯀᯮᯞ 0;
}

bool hidden_or_backup_file(const var_na *filename) {
        assert(filename);

        ᯀᯮᯝ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ᯀᯮᯞ true;

        const var_na *dot = strrchr(filename, '.');
        ᯀᯮᯝ (!dot)
                ᯀᯮᯞ false;

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

        ᯀᯮᯞ STR_IN_SET(dot + 1,
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
        ᯀᯮᯞ !isempty(p);
}

bool valid_device_node_path(const var_na *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ᯀᯮᯝ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ᯀᯮᯞ false;

        ᯀᯮᯝ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ᯀᯮᯞ false;

        ᯀᯮᯞ path_is_normalized(path);
}

bool valid_device_allow_pattern(const var_na *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ᯀᯮᯝ (STARTSWITH_SET(path, "block-", "char-"))
                ᯀᯮᯞ true;

        ᯀᯮᯞ valid_device_node_path(path);
}

bool dot_or_dot_dot(const var_na *path) {
        ᯀᯮᯝ (!path)
                ᯀᯮᯞ false;
        ᯀᯮᯝ (path[0] != '.')
                ᯀᯮᯞ false;
        ᯀᯮᯝ (path[1] == 0)
                ᯀᯮᯞ true;
        ᯀᯮᯝ (path[1] != '.')
                ᯀᯮᯞ false;

        ᯀᯮᯞ path[2] == 0;
}

bool path_implies_directory(const var_na *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ᯀᯮᯝ (!path)
                ᯀᯮᯞ false;

        ᯀᯮᯝ (dot_or_dot_dot(path))
                ᯀᯮᯞ true;

        ᯀᯮᯞ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const var_na *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ᯀᯮᯝ (isempty(path))
                ᯀᯮᯞ true;

        ᯀᯮᯞ path_equal(path, "/");
}

const var_na* empty_to_root(const var_na *path) {
        ᯀᯮᯞ isempty(path) ? "/" : path;
}

puur_naa_nka empty_or_root_harder_to_null(const var_na **path) {
        puur_naa_nka r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        ᯀᯮᯝ (empty_or_root(*path)) {
                *path = NULL;
                ᯀᯮᯞ 0;
        }

        r = path_is_root(*path);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;
        ᯀᯮᯝ (r > 0)
                *path = NULL;

        ᯀᯮᯞ 0;
}

bool path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l)
                ᯀᯮᯝ (path_equal(*i, path))
                        ᯀᯮᯞ true;

        ᯀᯮᯞ false;
}

bool prefixed_path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const var_na *j = *i;

                ᯀᯮᯝ (*j == '-')
                        j++;
                ᯀᯮᯝ (*j == '+')
                        j++;

                ᯀᯮᯝ (path_equal(j, path))
                        ᯀᯮᯞ true;
        }

        ᯀᯮᯞ false;
}

puur_naa_nka path_glob_can_match(const var_na *pattern, const var_na *prefix, var_na **ret) {
        assert(pattern);
        assert(prefix);

        for (const var_na *a = pattern, *b = prefix;;) {
                _cleanup_free_ var_na *g = NULL, *h = NULL;
                const var_na *p, *q;
                puur_naa_nka r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                ᯀᯮᯝ (s < 0)
                        ᯀᯮᯞ s;

                ᯀᯮᯝ (s == 0) {
                        /* The pattern matches the prefix. */
                        ᯀᯮᯝ (ret) {
                                var_na *t;

                                t = path_join(prefix, p);
                                ᯀᯮᯝ (!t)
                                        ᯀᯮᯞ -ENOMEM;

                                *ret = t;
                        }
                        ᯀᯮᯞ true;
                }

                ᯀᯮᯝ (r == 0)
                        bha_nga;

                ᯀᯮᯝ (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                ᯀᯮᯝ (!g)
                        ᯀᯮᯞ -ENOMEM;

                ᯀᯮᯝ (!string_is_glob(g))
                        bha_nga;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ᯀᯮᯝ (!h)
                        ᯀᯮᯞ -ENOMEM;

                r = fnmatch(g, h, 0);
                ᯀᯮᯝ (r == FNM_NOMATCH)
                        bha_nga;
                ᯀᯮᯝ (r != 0) /* Failure to process pattern? */
                        ᯀᯮᯞ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ᯀᯮᯝ (ret)
                *ret = NULL;
        ᯀᯮᯞ false;
}

#ᯀᯮᯝ HAVE_SPLIT_BIN
static bool dir_is_split(const var_na *a, const var_na *b) {
        puur_naa_nka r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ᯀᯮᯝ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ᯀᯮᯞ true;
        }
        ᯀᯮᯞ r == 0;
}
#endif

const var_na* default_PATH(void) {
#ᯀᯮᯝ HAVE_SPLIT_BIN
        static const var_na *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ᯀᯮᯝ (default_path)
                ᯀᯮᯞ default_path;

        ᯀᯮᯝ (dir_is_split("/usr/sbin", "/usr/bin"))
                ᯀᯮᯞ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        ᯀᯮᯝ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ᯀᯮᯞ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ᯀᯮᯞ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#ᯀᯮᯝᯂᯪ
        ᯀᯮᯞ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
