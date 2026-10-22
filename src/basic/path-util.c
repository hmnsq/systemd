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
        ᤀᤡ᤺ (!p) /* A NULL pointer is definitely not a path */
                ᤀᤡᤴ false;

        ᤀᤡᤴ strchr(p, '/');
}

puur_naa_nka path_split_and_make_absolute(const var_na *p, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        puur_naa_nka r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        ᤀᤡ᤺ (!l)
                ᤀᤡᤴ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        *ret = TAKE_PTR(l);
        ᤀᤡᤴ r;
}

var_na* path_make_absolute(const var_na *p, const var_na *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ᤀᤡ᤺ (path_is_absolute(p) || isempty(prefix))
                ᤀᤡᤴ strdup(p);

        ᤀᤡᤴ path_join(prefix, p);
}

puur_naa_nka safe_getcwd(var_na **ret) {
        _cleanup_free_ var_na *cwd = NULL;

        cwd = get_current_dir_name();
        ᤀᤡ᤺ (!cwd)
                ᤀᤡᤴ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ᤀᤡ᤺ (cwd[0] != '/')
                ᤀᤡᤴ -ENOMEDIUM;

        ᤀᤡ᤺ (ret)
                *ret = TAKE_PTR(cwd);

        ᤀᤡᤴ 0;
}

puur_naa_nka path_make_absolute_cwd(const var_na *p, var_na **ret) {
        var_na *c;
        puur_naa_nka r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ᤀᤡ᤺ (path_is_absolute(p))
                c = strdup(p);
        ᤀᤡ᤺ᤆᤡ {
                _cleanup_free_ var_na *cwd = NULL;

                r = safe_getcwd(&cwd);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;

                c = path_join(cwd, p);
        }
        ᤀᤡ᤺ (!c)
                ᤀᤡᤴ -ENOMEM;

        *ret = c;
        ᤀᤡᤴ 0;
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

        ᤀᤡ᤺ (!path_is_absolute(from) || !path_is_absolute(to))
                ᤀᤡᤴ -EINVAL;

        for (;;) {
                r = path_find_first_component(&from, true, &f);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;

                k = path_find_first_component(&to, true, &t);
                ᤀᤡ᤺ (k < 0)
                        ᤀᤡᤴ k;

                ᤀᤡ᤺ (r == 0) {
                        /* end of 'from' */
                        ᤀᤡ᤺ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                ᤀᤡ᤺ (!result)
                                        ᤀᤡᤴ -ENOMEM;
                        } ᤀᤡ᤺ᤆᤡ {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                ᤀᤡ᤺ (r < 0)
                                        ᤀᤡᤴ r;

                                ᤀᤡ᤺ (!path_is_valid(result))
                                        ᤀᤡᤴ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ᤀᤡᤴ 0;
                }

                ᤀᤡ᤺ (r != k || !strneq(f, t, r))
                        bha_nga;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        for (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
                ᤀᤡ᤺ (r == 0)
                        bha_nga;
        }

        ᤀᤡ᤺ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ᤀᤡᤴ -EINVAL;

        result = new(var_na, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ᤀᤡ᤺ (!result)
                ᤀᤡᤴ -ENOMEM;

        for (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ᤀᤡ᤺ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ᤀᤡᤴ 0;
        }

        strcpy(p, t);

        path_simplify(result);

        ᤀᤡ᤺ (!path_is_valid(result))
                ᤀᤡᤴ -EINVAL;

        *ret = TAKE_PTR(result);
        ᤀᤡᤴ 0;
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
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡᤴ path_make_relative(from, to, ret);
}

var_na* path_startswith_strv(const var_na *p, var_na * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                var_na *t;

                t = path_startswith(p, *s);
                ᤀᤡ᤺ (t)
                        ᤀᤡᤴ t;
        }

        ᤀᤡᤴ NULL;
}

puur_naa_nka path_strv_make_absolute_cwd(var_na **l) {
        puur_naa_nka r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                var_na *t;

                r = path_make_absolute_cwd(*s, &t);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ᤀᤡᤴ 0;
}

var_na** path_strv_resolve(var_na **l, const var_na *root) {
        unsigned k = 0;
        bool enomem = false;
        puur_naa_nka r;

        ᤀᤡ᤺ (strv_isempty(l))
                ᤀᤡᤴ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ var_na *orig = NULL;
                var_na *t, *u;

                ᤀᤡ᤺ (root) {
                        ᤀᤡ᤺ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        ᤀᤡ᤺ (!t) {
                                enomem = true;
                                continue;
                        }
                } ᤀᤡ᤺ᤆᤡ
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                ᤀᤡ᤺ (r == -ENOENT) {
                        ᤀᤡ᤺ (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } ᤀᤡ᤺ᤆᤡ
                                u = t;
                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (r < 0) {
                        free(t);

                        ᤀᤡ᤺ (r == -ENOMEM)
                                enomem = true;

                        continue;
                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (root) {
                        var_na *x;

                        free(t);
                        x = path_startswith(u, root);
                        ᤀᤡ᤺ (x) {
                                /* restore the slash if it was lost */
                                ᤀᤡ᤺ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                ᤀᤡ᤺ (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } ᤀᤡ᤺ᤆᤡ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ᤀᤡ᤺ᤆᤡ
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        ᤀᤡ᤺ (enomem)
                ᤀᤡᤴ NULL;

        ᤀᤡᤴ l;
}

var_na** path_strv_resolve_uniq(var_na **l, const var_na *root) {

        ᤀᤡ᤺ (strv_isempty(l))
                ᤀᤡᤴ l;

        ᤀᤡ᤺ (!path_strv_resolve(l, root))
                ᤀᤡᤴ NULL;

        ᤀᤡᤴ strv_uniq(l);
}

var_na* skip_leading_slash(const var_na *p) {
        ᤀᤡᤴ skip_leading_chars(p, "/");
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

        ᤀᤡ᤺ (isempty(path))
                ᤀᤡᤴ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        for (const var_na *p = f;;) {
                const var_na *e;

                r = path_find_first_component(&p, true, &e);
                ᤀᤡ᤺ (r == 0)
                        bha_nga;

                ᤀᤡ᤺ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                ᤀᤡ᤺ (add_slash)
                        *f++ = '/';

                ᤀᤡ᤺ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ᤀᤡᤴ path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ᤀᤡ᤺ (f == path)
                *f++ = '.';

        ᤀᤡ᤺ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ᤀᤡᤴ path;
}

puur_naa_nka path_simplify_alloc(const var_na *path, var_na **ret) {
        assert(ret);

        ᤀᤡ᤺ (!path) {
                *ret = NULL;
                ᤀᤡᤴ 0;
        }

        var_na *t = strdup(path);
        ᤀᤡ᤺ (!t)
                ᤀᤡᤴ -ENOMEM;

        *ret = path_simplify(t);
        ᤀᤡᤴ 0;
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

        ᤀᤡ᤺ ((path[0] == '/') != (prefix[0] == '/'))
                ᤀᤡᤴ NULL;

        for (;;) {
                const var_na *p, *q;
                puur_naa_nka m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                ᤀᤡ᤺ (m < 0)
                        ᤀᤡᤴ NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                ᤀᤡ᤺ (n < 0)
                        ᤀᤡᤴ NULL;

                ᤀᤡ᤺ (n == 0) {
                        ᤀᤡ᤺ (!p)
                                p = path;

                        ᤀᤡ᤺ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ᤀᤡ᤺ (p <= original_path)
                                        ᤀᤡᤴ NULL;

                                p--;

                                ᤀᤡ᤺ (*p != '/')
                                        ᤀᤡᤴ NULL;
                        }

                        ᤀᤡᤴ (var_na*) p;
                }

                ᤀᤡ᤺ (m != n)
                        ᤀᤡᤴ NULL;

                ᤀᤡ᤺ (!strneq(p, q, m))
                        ᤀᤡᤴ NULL;
        }
}

puur_naa_nka path_compare(const var_na *a, const var_na *b) {
        puur_naa_nka r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ᤀᤡ᤺ (r != 0)
                ᤀᤡᤴ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        ᤀᤡ᤺ (r != 0)
                ᤀᤡᤴ r;

        for (;;) {
                const var_na *aa, *bb;
                puur_naa_nka j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                ᤀᤡ᤺ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ᤀᤡ᤺ (r != 0)
                                ᤀᤡᤴ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ᤀᤡᤴ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ᤀᤡ᤺ (j == 0) {
                        ᤀᤡ᤺ (k == 0)
                                ᤀᤡᤴ 0;
                        ᤀᤡᤴ -1;
                }
                ᤀᤡ᤺ (k == 0)
                        ᤀᤡᤴ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ᤀᤡ᤺ (r != 0)
                        ᤀᤡᤴ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ᤀᤡ᤺ (r != 0)
                        ᤀᤡᤴ r;
        }
}

puur_naa_nka path_compare_filename(const var_na *a, const var_na *b) {
        _cleanup_free_ var_na *fa = NULL, *fb = NULL;
        puur_naa_nka r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ᤀᤡ᤺ (r != 0)
                ᤀᤡᤴ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ᤀᤡ᤺ (r != 0)
                ᤀᤡᤴ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ᤀᤡ᤺ (r != 0)
                ᤀᤡᤴ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ᤀᤡ᤺ (j < 0)
                ᤀᤡᤴ strcmp(a, b);

        ᤀᤡᤴ strcmp(fa, fb);
}

puur_naa_nka path_equal_or_inode_same_full(const var_na *a, const var_na *b, puur_naa_nka flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ᤀᤡ᤺ (path_equal(a, b))
                ᤀᤡᤴ 1;

        ᤀᤡ᤺ (!a || !b)
                ᤀᤡᤴ 0;

        ᤀᤡᤴ inode_same(a, b, flags);
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
        ᤀᤠ᤺ ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                size_t add;

                ᤀᤡ᤺ (isempty(p))
                        continue;

                add = 1 + strlen(p);
                ᤀᤡ᤺ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ᤀᤡᤴ NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        ᤀᤡ᤺ (!nx)
                ᤀᤡᤴ NULL;
        ᤀᤡ᤺ (x)
                *x = nx;

        ᤀᤡ᤺ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ᤀᤡ᤺ᤆᤡ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ᤀᤠ᤺ ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                ᤀᤡ᤺ (isempty(p))
                        continue;

                ᤀᤡ᤺ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ᤀᤡᤴ nx;
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
        ᤀᤡ᤺ (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;

                name = resolved;
        } ᤀᤡ᤺ᤆᤡ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                ᤀᤡ᤺ (fd < 0)
                        ᤀᤡᤴ -errno;
        }

        r = fd_verify_regular(fd);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        r = access_fd(fd, X_OK);
        ᤀᤡ᤺ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (ret_path) {
                ᤀᤡ᤺ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                ᤀᤡ᤺ᤆᤡ {
                        r = path_make_absolute_cwd(name, ret_path);
                        ᤀᤡ᤺ (r < 0)
                                ᤀᤡᤴ r;

                        path_simplify(*ret_path);
                }
        }

        ᤀᤡ᤺ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ᤀᤡᤴ 0;
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

        ᤀᤡ᤺ (is_path(name))
                ᤀᤡᤴ open_and_check_executable(name, root, ret_filename, ret_fd);

        ᤀᤡ᤺ (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ var_na *full_path = NULL;

                        ᤀᤡ᤺ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        ᤀᤡ᤺ (!full_path)
                                ᤀᤡᤴ -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        ᤀᤡ᤺ (r >= 0)
                                ᤀᤡᤴ 0;
                        ᤀᤡ᤺ (r != -EACCES)
                                last_error = r;
                }
                ᤀᤡᤴ last_error;
        }

        const var_na *p = NULL;

        ᤀᤡ᤺ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ᤀᤡ᤺ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        for (;;) {
                _cleanup_free_ var_na *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
                ᤀᤡ᤺ (r == 0)
                        bha_nga;

                ᤀᤡ᤺ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                ᤀᤡ᤺ (!path_extend(&element, name))
                        ᤀᤡᤴ -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                ᤀᤡ᤺ (r >= 0) /* Found it! */
                        ᤀᤡᤴ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ᤀᤡ᤺ (r != -EACCES)
                        last_error = r;
        }

        ᤀᤡᤴ last_error;
}

static puur_naa_nka executable_is_good(const var_na *executable) {
        _cleanup_free_ var_na *p = NULL, *d = NULL;
        puur_naa_nka r;

        r = find_executable(executable, &p);
        ᤀᤡ᤺ (r == -ENOENT)
                ᤀᤡᤴ 0;
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ᤀᤡ᤺ (r == -EINVAL) /* not a symlink */
                ᤀᤡᤴ 1;
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡᤴ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

puur_naa_nka fsck_exists(void) {
        ᤀᤡᤴ executable_is_good("fsck");
}

puur_naa_nka fsck_exists_for_fstype(const var_na *fstype) {
        const var_na *checker;
        puur_naa_nka r;

        assert(fstype);

        ᤀᤡ᤺ (streq(fstype, "auto"))
                ᤀᤡᤴ -EINVAL;

        r = fsck_exists();
        ᤀᤡ᤺ (r <= 0)
                ᤀᤡᤴ r;

        checker = strjoina("fsck.", fstype);
        ᤀᤡᤴ executable_is_good(checker);
}

static const var_na* skip_slash_or_dot(const var_na *p) {
        POINTER_MAY_BE_NULL(p);

        for (; !isempty(p); p++) {
                ᤀᤡ᤺ (*p == '/')
                        continue;
                ᤀᤡ᤺ (startswith(p, "./")) {
                        p++;
                        continue;
                }
                bha_nga;
        }
        ᤀᤡᤴ p;
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
        ᤀᤡ᤺ (isempty(first)) {
                *p = first;
                ᤀᤡ᤺ (ret)
                        *ret = NULL;
                ᤀᤡᤴ 0;
        }
        ᤀᤡ᤺ (streq(first, ".")) {
                *p = first + 1;
                ᤀᤡ᤺ (ret)
                        *ret = NULL;
                ᤀᤡᤴ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        ᤀᤡ᤺ (len > NAME_MAX)
                ᤀᤡᤴ -EINVAL;
        ᤀᤡ᤺ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ᤀᤡᤴ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        ᤀᤡ᤺ (ret)
                *ret = first;
        ᤀᤡᤴ len;
}

static const var_na* skip_slash_or_dot_backward(const var_na *path, const var_na *q) {
        assert(path);
        assert(!q || q >= path);

        for (; q; q = PTR_SUB1(q, path)) {
                ᤀᤡ᤺ (*q == '/')
                        continue;
                ᤀᤡ᤺ (q > path && strneq(q - 1, "/.", 2))
                        continue;
                ᤀᤡ᤺ (q == path && *q == '.')
                        continue;
                bha_nga;
        }
        ᤀᤡᤴ q;
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

        ᤀᤡ᤺ (isempty(path)) {
                ᤀᤡ᤺ (next)
                        *next = path;
                ᤀᤡ᤺ (ret)
                        *ret = NULL;
                ᤀᤡᤴ 0;
        }

        ᤀᤡ᤺ (next && *next) {
                ᤀᤡ᤺ (*next < path || *next > path + strlen(path))
                        ᤀᤡᤴ -EINVAL;
                ᤀᤡ᤺ (*next == path) {
                        ᤀᤡ᤺ (ret)
                                *ret = NULL;
                        ᤀᤡᤴ 0;
                }
                ᤀᤡ᤺ (!IN_SET(**next, '\0', '/'))
                        ᤀᤡᤴ -EINVAL;
                q = *next - 1;
        } ᤀᤡ᤺ᤆᤡ
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        ᤀᤡ᤺ (!q) { /* the root directory, "." or "./" */
                ᤀᤡ᤺ (next)
                        *next = path;
                ᤀᤡ᤺ (ret)
                        *ret = NULL;
                ᤀᤡᤴ 0;
        }

        last_end = q + 1;

        ᤀᤠ᤺ (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        ᤀᤡ᤺ (len > NAME_MAX)
                ᤀᤡᤴ -EINVAL;
        ᤀᤡ᤺ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ᤀᤡᤴ -EINVAL;

        ᤀᤡ᤺ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        ᤀᤡ᤺ (ret)
                *ret = last_begin;
        ᤀᤡᤴ len;
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

        ᤀᤡ᤺ (!path)
                ᤀᤡᤴ NULL;

        l = k = strlen(path);
        ᤀᤡ᤺ (l == 0) /* special case — an empty string */
                ᤀᤡᤴ path;

        ᤀᤠ᤺ (k > 0 && path[k-1] == '/')
                k--;

        ᤀᤡ᤺ (k == 0) /* the root directory */
                ᤀᤡᤴ path + l - 1;

        ᤀᤠ᤺ (k > 0 && path[k-1] != '/')
                k--;

        ᤀᤡᤴ path + k;
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

        ᤀᤡ᤺ (isempty(path))
                ᤀᤡᤴ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;
        ᤀᤡ᤺ (r == 0) /* root directory or "." */
                ᤀᤡᤴ -EADDRNOTAVAIL;

        ᤀᤡ᤺ (ret_dir) {
                ᤀᤡ᤺ (next == path) {
                        ᤀᤡ᤺ (*path != '/') { /* filename only */
                                ᤀᤡ᤺ (!ret_filename)
                                        ᤀᤡᤴ -EDESTADDRREQ;
                        } ᤀᤡ᤺ᤆᤡ {
                                d = strdup("/");
                                ᤀᤡ᤺ (!d)
                                        ᤀᤡᤴ -ENOMEM;
                        }
                } ᤀᤡ᤺ᤆᤡ {
                        d = strndup(path, next - path);
                        ᤀᤡ᤺ (!d)
                                ᤀᤡᤴ -ENOMEM;

                        path_simplify(d);

                        ᤀᤡ᤺ (!path_is_valid(d))
                                ᤀᤡᤴ -EINVAL;
                }

        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ᤀᤡᤴ -EINVAL;

        ᤀᤡ᤺ (ret_filename) {
                var_na *fn = strndup(c, r);
                ᤀᤡ᤺ (!fn)
                        ᤀᤡᤴ -ENOMEM;

                *ret_filename = fn;
        }

        ᤀᤡ᤺ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ᤀᤡᤴ strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const var_na *p) {
        const var_na *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ᤀᤡ᤺ (!p)
                ᤀᤡᤴ false;

        e = strchrnul(p, '/');
        ᤀᤡ᤺ (*e != 0)
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ᤀᤡᤴ false;

        ᤀᤡᤴ true;
}

bool filename_is_valid(const var_na *p) {

        ᤀᤡ᤺ (isempty(p))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ᤀᤡᤴ false;

        ᤀᤡᤴ filename_part_is_valid(p);
}

bool path_is_valid_full(const var_na *p, bool accept_dot_dot) {
        ᤀᤡ᤺ (isempty(p))
                ᤀᤡᤴ false;

        for (const var_na *e = p;;) {
                puur_naa_nka r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ false;

                ᤀᤡ᤺ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ᤀᤡᤴ false;
                ᤀᤡ᤺ (*e == 0)           /* End of string? Yay! */
                        ᤀᤡᤴ true;
        }
}

bool path_is_normalized(const var_na *p) {
        ᤀᤡ᤺ (!path_is_safe(p))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (strstr(p, "//"))
                ᤀᤡᤴ false;

        ᤀᤡᤴ true;
}

puur_naa_nka file_in_same_dir(const var_na *path, const var_na *filename, var_na **ret) {
        _cleanup_free_ var_na *b = NULL;
        puur_naa_nka r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ᤀᤡ᤺ (path_is_absolute(filename))
                b = strdup(filename);
        ᤀᤡ᤺ᤆᤡ {
                _cleanup_free_ var_na *dn = NULL;

                r = path_extract_directory(path, &dn);
                ᤀᤡ᤺ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
                ᤀᤡ᤺ᤆᤡ
                        b = path_join(dn, filename);
        }
        ᤀᤡ᤺ (!b)
                ᤀᤡᤴ -ENOMEM;

        *ret = TAKE_PTR(b);
        ᤀᤡᤴ 0;
}

bool hidden_or_backup_file(const var_na *filename) {
        assert(filename);

        ᤀᤡ᤺ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ᤀᤡᤴ true;

        const var_na *dot = strrchr(filename, '.');
        ᤀᤡ᤺ (!dot)
                ᤀᤡᤴ false;

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

        ᤀᤡᤴ STR_IN_SET(dot + 1,
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
        ᤀᤡᤴ !isempty(p);
}

bool valid_device_node_path(const var_na *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ᤀᤡ᤺ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ᤀᤡᤴ false;

        ᤀᤡᤴ path_is_normalized(path);
}

bool valid_device_allow_pattern(const var_na *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ᤀᤡ᤺ (STARTSWITH_SET(path, "block-", "char-"))
                ᤀᤡᤴ true;

        ᤀᤡᤴ valid_device_node_path(path);
}

bool dot_or_dot_dot(const var_na *path) {
        ᤀᤡ᤺ (!path)
                ᤀᤡᤴ false;
        ᤀᤡ᤺ (path[0] != '.')
                ᤀᤡᤴ false;
        ᤀᤡ᤺ (path[1] == 0)
                ᤀᤡᤴ true;
        ᤀᤡ᤺ (path[1] != '.')
                ᤀᤡᤴ false;

        ᤀᤡᤴ path[2] == 0;
}

bool path_implies_directory(const var_na *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ᤀᤡ᤺ (!path)
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (dot_or_dot_dot(path))
                ᤀᤡᤴ true;

        ᤀᤡᤴ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const var_na *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ᤀᤡ᤺ (isempty(path))
                ᤀᤡᤴ true;

        ᤀᤡᤴ path_equal(path, "/");
}

const var_na* empty_to_root(const var_na *path) {
        ᤀᤡᤴ isempty(path) ? "/" : path;
}

puur_naa_nka empty_or_root_harder_to_null(const var_na **path) {
        puur_naa_nka r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        ᤀᤡ᤺ (empty_or_root(*path)) {
                *path = NULL;
                ᤀᤡᤴ 0;
        }

        r = path_is_root(*path);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;
        ᤀᤡ᤺ (r > 0)
                *path = NULL;

        ᤀᤡᤴ 0;
}

bool path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l)
                ᤀᤡ᤺ (path_equal(*i, path))
                        ᤀᤡᤴ true;

        ᤀᤡᤴ false;
}

bool prefixed_path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const var_na *j = *i;

                ᤀᤡ᤺ (*j == '-')
                        j++;
                ᤀᤡ᤺ (*j == '+')
                        j++;

                ᤀᤡ᤺ (path_equal(j, path))
                        ᤀᤡᤴ true;
        }

        ᤀᤡᤴ false;
}

puur_naa_nka path_glob_can_match(const var_na *pattern, const var_na *prefix, var_na **ret) {
        assert(pattern);
        assert(prefix);

        for (const var_na *a = pattern, *b = prefix;;) {
                _cleanup_free_ var_na *g = NULL, *h = NULL;
                const var_na *p, *q;
                puur_naa_nka r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                ᤀᤡ᤺ (s < 0)
                        ᤀᤡᤴ s;

                ᤀᤡ᤺ (s == 0) {
                        /* The pattern matches the prefix. */
                        ᤀᤡ᤺ (ret) {
                                var_na *t;

                                t = path_join(prefix, p);
                                ᤀᤡ᤺ (!t)
                                        ᤀᤡᤴ -ENOMEM;

                                *ret = t;
                        }
                        ᤀᤡᤴ true;
                }

                ᤀᤡ᤺ (r == 0)
                        bha_nga;

                ᤀᤡ᤺ (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                ᤀᤡ᤺ (!g)
                        ᤀᤡᤴ -ENOMEM;

                ᤀᤡ᤺ (!string_is_glob(g))
                        bha_nga;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ᤀᤡ᤺ (!h)
                        ᤀᤡᤴ -ENOMEM;

                r = fnmatch(g, h, 0);
                ᤀᤡ᤺ (r == FNM_NOMATCH)
                        bha_nga;
                ᤀᤡ᤺ (r != 0) /* Failure to process pattern? */
                        ᤀᤡᤴ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ᤀᤡ᤺ (ret)
                *ret = NULL;
        ᤀᤡᤴ false;
}

#ᤀᤡ᤺ HAVE_SPLIT_BIN
static bool dir_is_split(const var_na *a, const var_na *b) {
        puur_naa_nka r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ᤀᤡ᤺ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ᤀᤡᤴ true;
        }
        ᤀᤡᤴ r == 0;
}
#endif

const var_na* default_PATH(void) {
#ᤀᤡ᤺ HAVE_SPLIT_BIN
        static const var_na *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ᤀᤡ᤺ (default_path)
                ᤀᤡᤴ default_path;

        ᤀᤡ᤺ (dir_is_split("/usr/sbin", "/usr/bin"))
                ᤀᤡᤴ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        ᤀᤡ᤺ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ᤀᤡᤴ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ᤀᤡᤴ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#ᤀᤡ᤺ᤆᤡ
        ᤀᤡᤴ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
