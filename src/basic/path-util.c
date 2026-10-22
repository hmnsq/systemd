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
        ꦪꦼꦤ꧀ (!p) /* A NULL pointer is definitely not a path */
                ꦮꦭꦶ false;

        ꦮꦭꦶ strchr(p, '/');
}

puur_naa_nka path_split_and_make_absolute(const var_na *p, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        puur_naa_nka r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        ꦪꦼꦤ꧀ (!l)
                ꦮꦭꦶ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        *ret = TAKE_PTR(l);
        ꦮꦭꦶ r;
}

var_na* path_make_absolute(const var_na *p, const var_na *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ꦪꦼꦤ꧀ (path_is_absolute(p) || isempty(prefix))
                ꦮꦭꦶ strdup(p);

        ꦮꦭꦶ path_join(prefix, p);
}

puur_naa_nka safe_getcwd(var_na **ret) {
        _cleanup_free_ var_na *cwd = NULL;

        cwd = get_current_dir_name();
        ꦪꦼꦤ꧀ (!cwd)
                ꦮꦭꦶ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ꦪꦼꦤ꧀ (cwd[0] != '/')
                ꦮꦭꦶ -ENOMEDIUM;

        ꦪꦼꦤ꧀ (ret)
                *ret = TAKE_PTR(cwd);

        ꦮꦭꦶ 0;
}

puur_naa_nka path_make_absolute_cwd(const var_na *p, var_na **ret) {
        var_na *c;
        puur_naa_nka r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ꦪꦼꦤ꧀ (path_is_absolute(p))
                c = strdup(p);
        ꦭꦶꦪ {
                _cleanup_free_ var_na *cwd = NULL;

                r = safe_getcwd(&cwd);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                c = path_join(cwd, p);
        }
        ꦪꦼꦤ꧀ (!c)
                ꦮꦭꦶ -ENOMEM;

        *ret = c;
        ꦮꦭꦶ 0;
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

        ꦪꦼꦤ꧀ (!path_is_absolute(from) || !path_is_absolute(to))
                ꦮꦭꦶ -EINVAL;

        ꦏꦁꦒꦺꦴ (;;) {
                r = path_find_first_component(&from, true, &f);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                k = path_find_first_component(&to, true, &t);
                ꦪꦼꦤ꧀ (k < 0)
                        ꦮꦭꦶ k;

                ꦪꦼꦤ꧀ (r == 0) {
                        /* end of 'from' */
                        ꦪꦼꦤ꧀ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                ꦪꦼꦤ꧀ (!result)
                                        ꦮꦭꦶ -ENOMEM;
                        } ꦭꦶꦪ {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                ꦪꦼꦤ꧀ (r < 0)
                                        ꦮꦭꦶ r;

                                ꦪꦼꦤ꧀ (!path_is_valid(result))
                                        ꦮꦭꦶ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ꦮꦭꦶ 0;
                }

                ꦪꦼꦤ꧀ (r != k || !strneq(f, t, r))
                        bha_nga;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        ꦏꦁꦒꦺꦴ (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦪꦼꦤ꧀ (r == 0)
                        bha_nga;
        }

        ꦪꦼꦤ꧀ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ꦮꦭꦶ -EINVAL;

        result = new(var_na, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ꦪꦼꦤ꧀ (!result)
                ꦮꦭꦶ -ENOMEM;

        ꦏꦁꦒꦺꦴ (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ꦪꦼꦤ꧀ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ꦮꦭꦶ 0;
        }

        strcpy(p, t);

        path_simplify(result);

        ꦪꦼꦤ꧀ (!path_is_valid(result))
                ꦮꦭꦶ -EINVAL;

        *ret = TAKE_PTR(result);
        ꦮꦭꦶ 0;
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
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ path_make_relative(from, to, ret);
}

var_na* path_startswith_strv(const var_na *p, var_na * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                var_na *t;

                t = path_startswith(p, *s);
                ꦪꦼꦤ꧀ (t)
                        ꦮꦭꦶ t;
        }

        ꦮꦭꦶ NULL;
}

puur_naa_nka path_strv_make_absolute_cwd(var_na **l) {
        puur_naa_nka r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                var_na *t;

                r = path_make_absolute_cwd(*s, &t);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ꦮꦭꦶ 0;
}

var_na** path_strv_resolve(var_na **l, const var_na *root) {
        unsigned k = 0;
        bool enomem = false;
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (strv_isempty(l))
                ꦮꦭꦶ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ var_na *orig = NULL;
                var_na *t, *u;

                ꦪꦼꦤ꧀ (root) {
                        ꦪꦼꦤ꧀ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        ꦪꦼꦤ꧀ (!t) {
                                enomem = true;
                                continue;
                        }
                } ꦭꦶꦪ
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                ꦪꦼꦤ꧀ (r == -ENOENT) {
                        ꦪꦼꦤ꧀ (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } ꦭꦶꦪ
                                u = t;
                } ꦭꦶꦪ ꦪꦼꦤ꧀ (r < 0) {
                        free(t);

                        ꦪꦼꦤ꧀ (r == -ENOMEM)
                                enomem = true;

                        continue;
                } ꦭꦶꦪ ꦪꦼꦤ꧀ (root) {
                        var_na *x;

                        free(t);
                        x = path_startswith(u, root);
                        ꦪꦼꦤ꧀ (x) {
                                /* restore the slash if it was lost */
                                ꦪꦼꦤ꧀ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                ꦪꦼꦤ꧀ (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } ꦭꦶꦪ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ꦭꦶꦪ
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        ꦪꦼꦤ꧀ (enomem)
                ꦮꦭꦶ NULL;

        ꦮꦭꦶ l;
}

var_na** path_strv_resolve_uniq(var_na **l, const var_na *root) {

        ꦪꦼꦤ꧀ (strv_isempty(l))
                ꦮꦭꦶ l;

        ꦪꦼꦤ꧀ (!path_strv_resolve(l, root))
                ꦮꦭꦶ NULL;

        ꦮꦭꦶ strv_uniq(l);
}

var_na* skip_leading_slash(const var_na *p) {
        ꦮꦭꦶ skip_leading_chars(p, "/");
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

        ꦪꦼꦤ꧀ (isempty(path))
                ꦮꦭꦶ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        ꦏꦁꦒꦺꦴ (const var_na *p = f;;) {
                const var_na *e;

                r = path_find_first_component(&p, true, &e);
                ꦪꦼꦤ꧀ (r == 0)
                        bha_nga;

                ꦪꦼꦤ꧀ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                ꦪꦼꦤ꧀ (add_slash)
                        *f++ = '/';

                ꦪꦼꦤ꧀ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ꦮꦭꦶ path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ꦪꦼꦤ꧀ (f == path)
                *f++ = '.';

        ꦪꦼꦤ꧀ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ꦮꦭꦶ path;
}

puur_naa_nka path_simplify_alloc(const var_na *path, var_na **ret) {
        assert(ret);

        ꦪꦼꦤ꧀ (!path) {
                *ret = NULL;
                ꦮꦭꦶ 0;
        }

        var_na *t = strdup(path);
        ꦪꦼꦤ꧀ (!t)
                ꦮꦭꦶ -ENOMEM;

        *ret = path_simplify(t);
        ꦮꦭꦶ 0;
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

        ꦪꦼꦤ꧀ ((path[0] == '/') != (prefix[0] == '/'))
                ꦮꦭꦶ NULL;

        ꦏꦁꦒꦺꦴ (;;) {
                const var_na *p, *q;
                puur_naa_nka m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                ꦪꦼꦤ꧀ (m < 0)
                        ꦮꦭꦶ NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                ꦪꦼꦤ꧀ (n < 0)
                        ꦮꦭꦶ NULL;

                ꦪꦼꦤ꧀ (n == 0) {
                        ꦪꦼꦤ꧀ (!p)
                                p = path;

                        ꦪꦼꦤ꧀ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ꦪꦼꦤ꧀ (p <= original_path)
                                        ꦮꦭꦶ NULL;

                                p--;

                                ꦪꦼꦤ꧀ (*p != '/')
                                        ꦮꦭꦶ NULL;
                        }

                        ꦮꦭꦶ (var_na*) p;
                }

                ꦪꦼꦤ꧀ (m != n)
                        ꦮꦭꦶ NULL;

                ꦪꦼꦤ꧀ (!strneq(p, q, m))
                        ꦮꦭꦶ NULL;
        }
}

puur_naa_nka path_compare(const var_na *a, const var_na *b) {
        puur_naa_nka r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ r;

        ꦏꦁꦒꦺꦴ (;;) {
                const var_na *aa, *bb;
                puur_naa_nka j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                ꦪꦼꦤ꧀ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ꦪꦼꦤ꧀ (r != 0)
                                ꦮꦭꦶ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ꦮꦭꦶ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ꦪꦼꦤ꧀ (j == 0) {
                        ꦪꦼꦤ꧀ (k == 0)
                                ꦮꦭꦶ 0;
                        ꦮꦭꦶ -1;
                }
                ꦪꦼꦤ꧀ (k == 0)
                        ꦮꦭꦶ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ꦪꦼꦤ꧀ (r != 0)
                        ꦮꦭꦶ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ꦪꦼꦤ꧀ (r != 0)
                        ꦮꦭꦶ r;
        }
}

puur_naa_nka path_compare_filename(const var_na *a, const var_na *b) {
        _cleanup_free_ var_na *fa = NULL, *fb = NULL;
        puur_naa_nka r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ꦪꦼꦤ꧀ (j < 0)
                ꦮꦭꦶ strcmp(a, b);

        ꦮꦭꦶ strcmp(fa, fb);
}

puur_naa_nka path_equal_or_inode_same_full(const var_na *a, const var_na *b, puur_naa_nka flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ꦪꦼꦤ꧀ (path_equal(a, b))
                ꦮꦭꦶ 1;

        ꦪꦼꦤ꧀ (!a || !b)
                ꦮꦭꦶ 0;

        ꦮꦭꦶ inode_same(a, b, flags);
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
        ꦱꦼꦢꦼꦭ ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                size_t add;

                ꦪꦼꦤ꧀ (isempty(p))
                        continue;

                add = 1 + strlen(p);
                ꦪꦼꦤ꧀ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ꦮꦭꦶ NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        ꦪꦼꦤ꧀ (!nx)
                ꦮꦭꦶ NULL;
        ꦪꦼꦤ꧀ (x)
                *x = nx;

        ꦪꦼꦤ꧀ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ꦭꦶꦪ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ꦱꦼꦢꦼꦭ ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                ꦪꦼꦤ꧀ (isempty(p))
                        continue;

                ꦪꦼꦤ꧀ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ꦮꦭꦶ nx;
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
        ꦪꦼꦤ꧀ (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                name = resolved;
        } ꦭꦶꦪ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                ꦪꦼꦤ꧀ (fd < 0)
                        ꦮꦭꦶ -errno;
        }

        r = fd_verify_regular(fd);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = access_fd(fd, X_OK);
        ꦪꦼꦤ꧀ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret_path) {
                ꦪꦼꦤ꧀ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                ꦭꦶꦪ {
                        r = path_make_absolute_cwd(name, ret_path);
                        ꦪꦼꦤ꧀ (r < 0)
                                ꦮꦭꦶ r;

                        path_simplify(*ret_path);
                }
        }

        ꦪꦼꦤ꧀ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ꦮꦭꦶ 0;
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

        ꦪꦼꦤ꧀ (is_path(name))
                ꦮꦭꦶ open_and_check_executable(name, root, ret_filename, ret_fd);

        ꦪꦼꦤ꧀ (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ var_na *full_path = NULL;

                        ꦪꦼꦤ꧀ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        ꦪꦼꦤ꧀ (!full_path)
                                ꦮꦭꦶ -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        ꦪꦼꦤ꧀ (r >= 0)
                                ꦮꦭꦶ 0;
                        ꦪꦼꦤ꧀ (r != -EACCES)
                                last_error = r;
                }
                ꦮꦭꦶ last_error;
        }

        const var_na *p = NULL;

        ꦪꦼꦤ꧀ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ꦪꦼꦤ꧀ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        ꦏꦁꦒꦺꦴ (;;) {
                _cleanup_free_ var_na *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦪꦼꦤ꧀ (r == 0)
                        bha_nga;

                ꦪꦼꦤ꧀ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                ꦪꦼꦤ꧀ (!path_extend(&element, name))
                        ꦮꦭꦶ -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                ꦪꦼꦤ꧀ (r >= 0) /* Found it! */
                        ꦮꦭꦶ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ꦪꦼꦤ꧀ (r != -EACCES)
                        last_error = r;
        }

        ꦮꦭꦶ last_error;
}

static puur_naa_nka executable_is_good(const var_na *executable) {
        _cleanup_free_ var_na *p = NULL, *d = NULL;
        puur_naa_nka r;

        r = find_executable(executable, &p);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ 0;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ꦪꦼꦤ꧀ (r == -EINVAL) /* not a symlink */
                ꦮꦭꦶ 1;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

puur_naa_nka fsck_exists(void) {
        ꦮꦭꦶ executable_is_good("fsck");
}

puur_naa_nka fsck_exists_for_fstype(const var_na *fstype) {
        const var_na *checker;
        puur_naa_nka r;

        assert(fstype);

        ꦪꦼꦤ꧀ (streq(fstype, "auto"))
                ꦮꦭꦶ -EINVAL;

        r = fsck_exists();
        ꦪꦼꦤ꧀ (r <= 0)
                ꦮꦭꦶ r;

        checker = strjoina("fsck.", fstype);
        ꦮꦭꦶ executable_is_good(checker);
}

static const var_na* skip_slash_or_dot(const var_na *p) {
        POINTER_MAY_BE_NULL(p);

        ꦏꦁꦒꦺꦴ (; !isempty(p); p++) {
                ꦪꦼꦤ꧀ (*p == '/')
                        continue;
                ꦪꦼꦤ꧀ (startswith(p, "./")) {
                        p++;
                        continue;
                }
                bha_nga;
        }
        ꦮꦭꦶ p;
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
        ꦪꦼꦤ꧀ (isempty(first)) {
                *p = first;
                ꦪꦼꦤ꧀ (ret)
                        *ret = NULL;
                ꦮꦭꦶ 0;
        }
        ꦪꦼꦤ꧀ (streq(first, ".")) {
                *p = first + 1;
                ꦪꦼꦤ꧀ (ret)
                        *ret = NULL;
                ꦮꦭꦶ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        ꦪꦼꦤ꧀ (len > NAME_MAX)
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ꦮꦭꦶ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        ꦪꦼꦤ꧀ (ret)
                *ret = first;
        ꦮꦭꦶ len;
}

static const var_na* skip_slash_or_dot_backward(const var_na *path, const var_na *q) {
        assert(path);
        assert(!q || q >= path);

        ꦏꦁꦒꦺꦴ (; q; q = PTR_SUB1(q, path)) {
                ꦪꦼꦤ꧀ (*q == '/')
                        continue;
                ꦪꦼꦤ꧀ (q > path && strneq(q - 1, "/.", 2))
                        continue;
                ꦪꦼꦤ꧀ (q == path && *q == '.')
                        continue;
                bha_nga;
        }
        ꦮꦭꦶ q;
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

        ꦪꦼꦤ꧀ (isempty(path)) {
                ꦪꦼꦤ꧀ (next)
                        *next = path;
                ꦪꦼꦤ꧀ (ret)
                        *ret = NULL;
                ꦮꦭꦶ 0;
        }

        ꦪꦼꦤ꧀ (next && *next) {
                ꦪꦼꦤ꧀ (*next < path || *next > path + strlen(path))
                        ꦮꦭꦶ -EINVAL;
                ꦪꦼꦤ꧀ (*next == path) {
                        ꦪꦼꦤ꧀ (ret)
                                *ret = NULL;
                        ꦮꦭꦶ 0;
                }
                ꦪꦼꦤ꧀ (!IN_SET(**next, '\0', '/'))
                        ꦮꦭꦶ -EINVAL;
                q = *next - 1;
        } ꦭꦶꦪ
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        ꦪꦼꦤ꧀ (!q) { /* the root directory, "." or "./" */
                ꦪꦼꦤ꧀ (next)
                        *next = path;
                ꦪꦼꦤ꧀ (ret)
                        *ret = NULL;
                ꦮꦭꦶ 0;
        }

        last_end = q + 1;

        ꦱꦼꦢꦼꦭ (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        ꦪꦼꦤ꧀ (len > NAME_MAX)
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        ꦪꦼꦤ꧀ (ret)
                *ret = last_begin;
        ꦮꦭꦶ len;
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

        ꦪꦼꦤ꧀ (!path)
                ꦮꦭꦶ NULL;

        l = k = strlen(path);
        ꦪꦼꦤ꧀ (l == 0) /* special case — an empty string */
                ꦮꦭꦶ path;

        ꦱꦼꦢꦼꦭ (k > 0 && path[k-1] == '/')
                k--;

        ꦪꦼꦤ꧀ (k == 0) /* the root directory */
                ꦮꦭꦶ path + l - 1;

        ꦱꦼꦢꦼꦭ (k > 0 && path[k-1] != '/')
                k--;

        ꦮꦭꦶ path + k;
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

        ꦪꦼꦤ꧀ (isempty(path))
                ꦮꦭꦶ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (r == 0) /* root directory or "." */
                ꦮꦭꦶ -EADDRNOTAVAIL;

        ꦪꦼꦤ꧀ (ret_dir) {
                ꦪꦼꦤ꧀ (next == path) {
                        ꦪꦼꦤ꧀ (*path != '/') { /* filename only */
                                ꦪꦼꦤ꧀ (!ret_filename)
                                        ꦮꦭꦶ -EDESTADDRREQ;
                        } ꦭꦶꦪ {
                                d = strdup("/");
                                ꦪꦼꦤ꧀ (!d)
                                        ꦮꦭꦶ -ENOMEM;
                        }
                } ꦭꦶꦪ {
                        d = strndup(path, next - path);
                        ꦪꦼꦤ꧀ (!d)
                                ꦮꦭꦶ -ENOMEM;

                        path_simplify(d);

                        ꦪꦼꦤ꧀ (!path_is_valid(d))
                                ꦮꦭꦶ -EINVAL;
                }

        } ꦭꦶꦪ ꦪꦼꦤ꧀ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (ret_filename) {
                var_na *fn = strndup(c, r);
                ꦪꦼꦤ꧀ (!fn)
                        ꦮꦭꦶ -ENOMEM;

                *ret_filename = fn;
        }

        ꦪꦼꦤ꧀ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ꦮꦭꦶ strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const var_na *p) {
        const var_na *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ꦪꦼꦤ꧀ (!p)
                ꦮꦭꦶ false;

        e = strchrnul(p, '/');
        ꦪꦼꦤ꧀ (*e != 0)
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ꦮꦭꦶ false;

        ꦮꦭꦶ true;
}

bool filename_is_valid(const var_na *p) {

        ꦪꦼꦤ꧀ (isempty(p))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ꦮꦭꦶ false;

        ꦮꦭꦶ filename_part_is_valid(p);
}

bool path_is_valid_full(const var_na *p, bool accept_dot_dot) {
        ꦪꦼꦤ꧀ (isempty(p))
                ꦮꦭꦶ false;

        ꦏꦁꦒꦺꦴ (const var_na *e = p;;) {
                puur_naa_nka r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ false;

                ꦪꦼꦤ꧀ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ꦮꦭꦶ false;
                ꦪꦼꦤ꧀ (*e == 0)           /* End of string? Yay! */
                        ꦮꦭꦶ true;
        }
}

bool path_is_normalized(const var_na *p) {
        ꦪꦼꦤ꧀ (!path_is_safe(p))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (strstr(p, "//"))
                ꦮꦭꦶ false;

        ꦮꦭꦶ true;
}

puur_naa_nka file_in_same_dir(const var_na *path, const var_na *filename, var_na **ret) {
        _cleanup_free_ var_na *b = NULL;
        puur_naa_nka r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ꦪꦼꦤ꧀ (path_is_absolute(filename))
                b = strdup(filename);
        ꦭꦶꦪ {
                _cleanup_free_ var_na *dn = NULL;

                r = path_extract_directory(path, &dn);
                ꦪꦼꦤ꧀ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ꦭꦶꦪ ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦭꦶꦪ
                        b = path_join(dn, filename);
        }
        ꦪꦼꦤ꧀ (!b)
                ꦮꦭꦶ -ENOMEM;

        *ret = TAKE_PTR(b);
        ꦮꦭꦶ 0;
}

bool hidden_or_backup_file(const var_na *filename) {
        assert(filename);

        ꦪꦼꦤ꧀ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ꦮꦭꦶ true;

        const var_na *dot = strrchr(filename, '.');
        ꦪꦼꦤ꧀ (!dot)
                ꦮꦭꦶ false;

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

        ꦮꦭꦶ STR_IN_SET(dot + 1,
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
        ꦮꦭꦶ !isempty(p);
}

bool valid_device_node_path(const var_na *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ꦪꦼꦤ꧀ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ꦮꦭꦶ false;

        ꦮꦭꦶ path_is_normalized(path);
}

bool valid_device_allow_pattern(const var_na *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ꦪꦼꦤ꧀ (STARTSWITH_SET(path, "block-", "char-"))
                ꦮꦭꦶ true;

        ꦮꦭꦶ valid_device_node_path(path);
}

bool dot_or_dot_dot(const var_na *path) {
        ꦪꦼꦤ꧀ (!path)
                ꦮꦭꦶ false;
        ꦪꦼꦤ꧀ (path[0] != '.')
                ꦮꦭꦶ false;
        ꦪꦼꦤ꧀ (path[1] == 0)
                ꦮꦭꦶ true;
        ꦪꦼꦤ꧀ (path[1] != '.')
                ꦮꦭꦶ false;

        ꦮꦭꦶ path[2] == 0;
}

bool path_implies_directory(const var_na *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ꦪꦼꦤ꧀ (!path)
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (dot_or_dot_dot(path))
                ꦮꦭꦶ true;

        ꦮꦭꦶ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const var_na *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ꦪꦼꦤ꧀ (isempty(path))
                ꦮꦭꦶ true;

        ꦮꦭꦶ path_equal(path, "/");
}

const var_na* empty_to_root(const var_na *path) {
        ꦮꦭꦶ isempty(path) ? "/" : path;
}

puur_naa_nka empty_or_root_harder_to_null(const var_na **path) {
        puur_naa_nka r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        ꦪꦼꦤ꧀ (empty_or_root(*path)) {
                *path = NULL;
                ꦮꦭꦶ 0;
        }

        r = path_is_root(*path);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (r > 0)
                *path = NULL;

        ꦮꦭꦶ 0;
}

bool path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l)
                ꦪꦼꦤ꧀ (path_equal(*i, path))
                        ꦮꦭꦶ true;

        ꦮꦭꦶ false;
}

bool prefixed_path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const var_na *j = *i;

                ꦪꦼꦤ꧀ (*j == '-')
                        j++;
                ꦪꦼꦤ꧀ (*j == '+')
                        j++;

                ꦪꦼꦤ꧀ (path_equal(j, path))
                        ꦮꦭꦶ true;
        }

        ꦮꦭꦶ false;
}

puur_naa_nka path_glob_can_match(const var_na *pattern, const var_na *prefix, var_na **ret) {
        assert(pattern);
        assert(prefix);

        ꦏꦁꦒꦺꦴ (const var_na *a = pattern, *b = prefix;;) {
                _cleanup_free_ var_na *g = NULL, *h = NULL;
                const var_na *p, *q;
                puur_naa_nka r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                ꦪꦼꦤ꧀ (s < 0)
                        ꦮꦭꦶ s;

                ꦪꦼꦤ꧀ (s == 0) {
                        /* The pattern matches the prefix. */
                        ꦪꦼꦤ꧀ (ret) {
                                var_na *t;

                                t = path_join(prefix, p);
                                ꦪꦼꦤ꧀ (!t)
                                        ꦮꦭꦶ -ENOMEM;

                                *ret = t;
                        }
                        ꦮꦭꦶ true;
                }

                ꦪꦼꦤ꧀ (r == 0)
                        bha_nga;

                ꦪꦼꦤ꧀ (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                ꦪꦼꦤ꧀ (!g)
                        ꦮꦭꦶ -ENOMEM;

                ꦪꦼꦤ꧀ (!string_is_glob(g))
                        bha_nga;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ꦪꦼꦤ꧀ (!h)
                        ꦮꦭꦶ -ENOMEM;

                r = fnmatch(g, h, 0);
                ꦪꦼꦤ꧀ (r == FNM_NOMATCH)
                        bha_nga;
                ꦪꦼꦤ꧀ (r != 0) /* Failure to process pattern? */
                        ꦮꦭꦶ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ꦪꦼꦤ꧀ (ret)
                *ret = NULL;
        ꦮꦭꦶ false;
}

#ꦪꦼꦤ꧀ HAVE_SPLIT_BIN
static bool dir_is_split(const var_na *a, const var_na *b) {
        puur_naa_nka r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ꦪꦼꦤ꧀ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ꦮꦭꦶ true;
        }
        ꦮꦭꦶ r == 0;
}
#endif

const var_na* default_PATH(void) {
#ꦪꦼꦤ꧀ HAVE_SPLIT_BIN
        static const var_na *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ꦪꦼꦤ꧀ (default_path)
                ꦮꦭꦶ default_path;

        ꦪꦼꦤ꧀ (dir_is_split("/usr/sbin", "/usr/bin"))
                ꦮꦭꦶ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        ꦪꦼꦤ꧀ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ꦮꦭꦶ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ꦮꦭꦶ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#ꦭꦶꦪ
        ꦮꦭꦶ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
