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
        ຖ້າ (!p) /* A NULL pointer is definitely not a path */
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ strchr(p, '/');
}

ຈຳນວນເຕັມ path_split_and_make_absolute(const var_na *p, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        ຈຳນວນເຕັມ r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        ຖ້າ (!l)
                ສົ່ງຄືນ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        *ret = TAKE_PTR(l);
        ສົ່ງຄືນ r;
}

var_na* path_make_absolute(const var_na *p, const var_na *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ຖ້າ (path_is_absolute(p) || isempty(prefix))
                ສົ່ງຄືນ strdup(p);

        ສົ່ງຄືນ path_join(prefix, p);
}

ຈຳນວນເຕັມ safe_getcwd(var_na **ret) {
        _cleanup_free_ var_na *cwd = NULL;

        cwd = get_current_dir_name();
        ຖ້າ (!cwd)
                ສົ່ງຄືນ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ຖ້າ (cwd[0] != '/')
                ສົ່ງຄືນ -ENOMEDIUM;

        ຖ້າ (ret)
                *ret = TAKE_PTR(cwd);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ path_make_absolute_cwd(const var_na *p, var_na **ret) {
        var_na *c;
        ຈຳນວນເຕັມ r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ຖ້າ (path_is_absolute(p))
                c = strdup(p);
        ບໍ່ດັ່ງນັ້ນ {
                _cleanup_free_ var_na *cwd = NULL;

                r = safe_getcwd(&cwd);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                c = path_join(cwd, p);
        }
        ຖ້າ (!c)
                ສົ່ງຄືນ -ENOMEM;

        *ret = c;
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ path_make_relative(const var_na *from, const var_na *to, var_na **ret) {
        _cleanup_free_ var_na *result = NULL;
        unsigned n_parents;
        const var_na *f, *t;
        ຈຳນວນເຕັມ r, k;
        var_na *p;

        assert(from);
        assert(to);
        assert(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        ຖ້າ (!path_is_absolute(from) || !path_is_absolute(to))
                ສົ່ງຄືນ -EINVAL;

        ສຳລັບ (;;) {
                r = path_find_first_component(&from, true, &f);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                k = path_find_first_component(&to, true, &t);
                ຖ້າ (k < 0)
                        ສົ່ງຄືນ k;

                ຖ້າ (r == 0) {
                        /* end of 'from' */
                        ຖ້າ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                ຖ້າ (!result)
                                        ສົ່ງຄືນ -ENOMEM;
                        } ບໍ່ດັ່ງນັ້ນ {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                ຖ້າ (r < 0)
                                        ສົ່ງຄືນ r;

                                ຖ້າ (!path_is_valid(result))
                                        ສົ່ງຄືນ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ສົ່ງຄືນ 0;
                }

                ຖ້າ (r != k || !strneq(f, t, r))
                        bha_nga;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        ສຳລັບ (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ຖ້າ (r == 0)
                        bha_nga;
        }

        ຖ້າ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ສົ່ງຄືນ -EINVAL;

        result = new(var_na, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ຖ້າ (!result)
                ສົ່ງຄືນ -ENOMEM;

        ສຳລັບ (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ຖ້າ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ສົ່ງຄືນ 0;
        }

        strcpy(p, t);

        path_simplify(result);

        ຖ້າ (!path_is_valid(result))
                ສົ່ງຄືນ -EINVAL;

        *ret = TAKE_PTR(result);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ path_make_relative_parent(const var_na *from_child, const var_na *to, var_na **ret) {
        _cleanup_free_ var_na *from = NULL;
        ຈຳນວນເຕັມ r;

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
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ path_make_relative(from, to, ret);
}

var_na* path_startswith_strv(const var_na *p, var_na * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                var_na *t;

                t = path_startswith(p, *s);
                ຖ້າ (t)
                        ສົ່ງຄືນ t;
        }

        ສົ່ງຄືນ NULL;
}

ຈຳນວນເຕັມ path_strv_make_absolute_cwd(var_na **l) {
        ຈຳນວນເຕັມ r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                var_na *t;

                r = path_make_absolute_cwd(*s, &t);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ສົ່ງຄືນ 0;
}

var_na** path_strv_resolve(var_na **l, const var_na *root) {
        unsigned k = 0;
        bool enomem = false;
        ຈຳນວນເຕັມ r;

        ຖ້າ (strv_isempty(l))
                ສົ່ງຄືນ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ var_na *orig = NULL;
                var_na *t, *u;

                ຖ້າ (root) {
                        ຖ້າ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        ຖ້າ (!t) {
                                enomem = true;
                                continue;
                        }
                } ບໍ່ດັ່ງນັ້ນ
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                ຖ້າ (r == -ENOENT) {
                        ຖ້າ (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } ບໍ່ດັ່ງນັ້ນ
                                u = t;
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ (r < 0) {
                        free(t);

                        ຖ້າ (r == -ENOMEM)
                                enomem = true;

                        continue;
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ (root) {
                        var_na *x;

                        free(t);
                        x = path_startswith(u, root);
                        ຖ້າ (x) {
                                /* restore the slash if it was lost */
                                ຖ້າ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                ຖ້າ (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } ບໍ່ດັ່ງນັ້ນ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ບໍ່ດັ່ງນັ້ນ
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        ຖ້າ (enomem)
                ສົ່ງຄືນ NULL;

        ສົ່ງຄືນ l;
}

var_na** path_strv_resolve_uniq(var_na **l, const var_na *root) {

        ຖ້າ (strv_isempty(l))
                ສົ່ງຄືນ l;

        ຖ້າ (!path_strv_resolve(l, root))
                ສົ່ງຄືນ NULL;

        ສົ່ງຄືນ strv_uniq(l);
}

var_na* skip_leading_slash(const var_na *p) {
        ສົ່ງຄືນ skip_leading_chars(p, "/");
}

var_na* path_simplify_full(var_na *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        var_na *f = path;
        ຈຳນວນເຕັມ r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        ຖ້າ (isempty(path))
                ສົ່ງຄືນ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        ສຳລັບ (const var_na *p = f;;) {
                const var_na *e;

                r = path_find_first_component(&p, true, &e);
                ຖ້າ (r == 0)
                        bha_nga;

                ຖ້າ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                ຖ້າ (add_slash)
                        *f++ = '/';

                ຖ້າ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ສົ່ງຄືນ path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ຖ້າ (f == path)
                *f++ = '.';

        ຖ້າ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ສົ່ງຄືນ path;
}

ຈຳນວນເຕັມ path_simplify_alloc(const var_na *path, var_na **ret) {
        assert(ret);

        ຖ້າ (!path) {
                *ret = NULL;
                ສົ່ງຄືນ 0;
        }

        var_na *t = strdup(path);
        ຖ້າ (!t)
                ສົ່ງຄືນ -ENOMEM;

        *ret = path_simplify(t);
        ສົ່ງຄືນ 0;
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

        ຖ້າ ((path[0] == '/') != (prefix[0] == '/'))
                ສົ່ງຄືນ NULL;

        ສຳລັບ (;;) {
                const var_na *p, *q;
                ຈຳນວນເຕັມ m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                ຖ້າ (m < 0)
                        ສົ່ງຄືນ NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                ຖ້າ (n < 0)
                        ສົ່ງຄືນ NULL;

                ຖ້າ (n == 0) {
                        ຖ້າ (!p)
                                p = path;

                        ຖ້າ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ຖ້າ (p <= original_path)
                                        ສົ່ງຄືນ NULL;

                                p--;

                                ຖ້າ (*p != '/')
                                        ສົ່ງຄືນ NULL;
                        }

                        ສົ່ງຄືນ (var_na*) p;
                }

                ຖ້າ (m != n)
                        ສົ່ງຄືນ NULL;

                ຖ້າ (!strneq(p, q, m))
                        ສົ່ງຄືນ NULL;
        }
}

ຈຳນວນເຕັມ path_compare(const var_na *a, const var_na *b) {
        ຈຳນວນເຕັມ r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ຖ້າ (r != 0)
                ສົ່ງຄືນ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        ຖ້າ (r != 0)
                ສົ່ງຄືນ r;

        ສຳລັບ (;;) {
                const var_na *aa, *bb;
                ຈຳນວນເຕັມ j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                ຖ້າ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ຖ້າ (r != 0)
                                ສົ່ງຄືນ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ສົ່ງຄືນ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ຖ້າ (j == 0) {
                        ຖ້າ (k == 0)
                                ສົ່ງຄືນ 0;
                        ສົ່ງຄືນ -1;
                }
                ຖ້າ (k == 0)
                        ສົ່ງຄືນ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ຖ້າ (r != 0)
                        ສົ່ງຄືນ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ຖ້າ (r != 0)
                        ສົ່ງຄືນ r;
        }
}

ຈຳນວນເຕັມ path_compare_filename(const var_na *a, const var_na *b) {
        _cleanup_free_ var_na *fa = NULL, *fb = NULL;
        ຈຳນວນເຕັມ r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ຖ້າ (r != 0)
                ສົ່ງຄືນ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ຖ້າ (r != 0)
                ສົ່ງຄືນ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ຖ້າ (r != 0)
                ສົ່ງຄືນ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ຖ້າ (j < 0)
                ສົ່ງຄືນ strcmp(a, b);

        ສົ່ງຄືນ strcmp(fa, fb);
}

ຈຳນວນເຕັມ path_equal_or_inode_same_full(const var_na *a, const var_na *b, ຈຳນວນເຕັມ flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ຖ້າ (path_equal(a, b))
                ສົ່ງຄືນ 1;

        ຖ້າ (!a || !b)
                ສົ່ງຄືນ 0;

        ສົ່ງຄືນ inode_same(a, b, flags);
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
        ໃນຂະນະທີ່ ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                size_t add;

                ຖ້າ (isempty(p))
                        continue;

                add = 1 + strlen(p);
                ຖ້າ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ສົ່ງຄືນ NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        ຖ້າ (!nx)
                ສົ່ງຄືນ NULL;
        ຖ້າ (x)
                *x = nx;

        ຖ້າ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ບໍ່ດັ່ງນັ້ນ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ໃນຂະນະທີ່ ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                ຖ້າ (isempty(p))
                        continue;

                ຖ້າ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ສົ່ງຄືນ nx;
}

ຈຳນວນເຕັມ open_and_check_executable(const var_na *name, const var_na *root, var_na **ret_path, ຈຳນວນເຕັມ *ret_fd) {
        _cleanup_close_ ຈຳນວນເຕັມ fd = -EBADF;
        _cleanup_free_ var_na *resolved = NULL;
        ຈຳນວນເຕັມ r;

        assert(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        ຖ້າ (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                name = resolved;
        } ບໍ່ດັ່ງນັ້ນ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                ຖ້າ (fd < 0)
                        ສົ່ງຄືນ -errno;
        }

        r = fd_verify_regular(fd);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        r = access_fd(fd, X_OK);
        ຖ້າ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (ret_path) {
                ຖ້າ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                ບໍ່ດັ່ງນັ້ນ {
                        r = path_make_absolute_cwd(name, ret_path);
                        ຖ້າ (r < 0)
                                ສົ່ງຄືນ r;

                        path_simplify(*ret_path);
                }
        }

        ຖ້າ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ find_executable_full(
                const var_na *name,
                const var_na *root,
                var_na * const *exec_search_path,
                bool use_path_envvar,
                var_na **ret_filename,
                ຈຳນວນເຕັມ *ret_fd) {

        ຈຳນວນເຕັມ last_error = -ENOENT, r = 0;

        assert(name);

        ຖ້າ (is_path(name))
                ສົ່ງຄືນ open_and_check_executable(name, root, ret_filename, ret_fd);

        ຖ້າ (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ var_na *full_path = NULL;

                        ຖ້າ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        ຖ້າ (!full_path)
                                ສົ່ງຄືນ -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        ຖ້າ (r >= 0)
                                ສົ່ງຄືນ 0;
                        ຖ້າ (r != -EACCES)
                                last_error = r;
                }
                ສົ່ງຄືນ last_error;
        }

        const var_na *p = NULL;

        ຖ້າ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ຖ້າ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        ສຳລັບ (;;) {
                _cleanup_free_ var_na *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ຖ້າ (r == 0)
                        bha_nga;

                ຖ້າ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                ຖ້າ (!path_extend(&element, name))
                        ສົ່ງຄືນ -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                ຖ້າ (r >= 0) /* Found it! */
                        ສົ່ງຄືນ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ຖ້າ (r != -EACCES)
                        last_error = r;
        }

        ສົ່ງຄືນ last_error;
}

static ຈຳນວນເຕັມ executable_is_good(const var_na *executable) {
        _cleanup_free_ var_na *p = NULL, *d = NULL;
        ຈຳນວນເຕັມ r;

        r = find_executable(executable, &p);
        ຖ້າ (r == -ENOENT)
                ສົ່ງຄືນ 0;
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ຖ້າ (r == -EINVAL) /* not a symlink */
                ສົ່ງຄືນ 1;
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

ຈຳນວນເຕັມ fsck_exists(void) {
        ສົ່ງຄືນ executable_is_good("fsck");
}

ຈຳນວນເຕັມ fsck_exists_for_fstype(const var_na *fstype) {
        const var_na *checker;
        ຈຳນວນເຕັມ r;

        assert(fstype);

        ຖ້າ (streq(fstype, "auto"))
                ສົ່ງຄືນ -EINVAL;

        r = fsck_exists();
        ຖ້າ (r <= 0)
                ສົ່ງຄືນ r;

        checker = strjoina("fsck.", fstype);
        ສົ່ງຄືນ executable_is_good(checker);
}

static const var_na* skip_slash_or_dot(const var_na *p) {
        POINTER_MAY_BE_NULL(p);

        ສຳລັບ (; !isempty(p); p++) {
                ຖ້າ (*p == '/')
                        continue;
                ຖ້າ (startswith(p, "./")) {
                        p++;
                        continue;
                }
                bha_nga;
        }
        ສົ່ງຄືນ p;
}

ຈຳນວນເຕັມ path_find_first_component(const var_na **p, bool accept_dot_dot, const var_na **ret) {
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
        ຖ້າ (isempty(first)) {
                *p = first;
                ຖ້າ (ret)
                        *ret = NULL;
                ສົ່ງຄືນ 0;
        }
        ຖ້າ (streq(first, ".")) {
                *p = first + 1;
                ຖ້າ (ret)
                        *ret = NULL;
                ສົ່ງຄືນ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        ຖ້າ (len > NAME_MAX)
                ສົ່ງຄືນ -EINVAL;
        ຖ້າ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ສົ່ງຄືນ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        ຖ້າ (ret)
                *ret = first;
        ສົ່ງຄືນ len;
}

static const var_na* skip_slash_or_dot_backward(const var_na *path, const var_na *q) {
        assert(path);
        assert(!q || q >= path);

        ສຳລັບ (; q; q = PTR_SUB1(q, path)) {
                ຖ້າ (*q == '/')
                        continue;
                ຖ້າ (q > path && strneq(q - 1, "/.", 2))
                        continue;
                ຖ້າ (q == path && *q == '.')
                        continue;
                bha_nga;
        }
        ສົ່ງຄືນ q;
}

ຈຳນວນເຕັມ path_find_last_component(const var_na *path, bool accept_dot_dot, const var_na **next, const var_na **ret) {
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

        ຖ້າ (isempty(path)) {
                ຖ້າ (next)
                        *next = path;
                ຖ້າ (ret)
                        *ret = NULL;
                ສົ່ງຄືນ 0;
        }

        ຖ້າ (next && *next) {
                ຖ້າ (*next < path || *next > path + strlen(path))
                        ສົ່ງຄືນ -EINVAL;
                ຖ້າ (*next == path) {
                        ຖ້າ (ret)
                                *ret = NULL;
                        ສົ່ງຄືນ 0;
                }
                ຖ້າ (!IN_SET(**next, '\0', '/'))
                        ສົ່ງຄືນ -EINVAL;
                q = *next - 1;
        } ບໍ່ດັ່ງນັ້ນ
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        ຖ້າ (!q) { /* the root directory, "." or "./" */
                ຖ້າ (next)
                        *next = path;
                ຖ້າ (ret)
                        *ret = NULL;
                ສົ່ງຄືນ 0;
        }

        last_end = q + 1;

        ໃນຂະນະທີ່ (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        ຖ້າ (len > NAME_MAX)
                ສົ່ງຄືນ -EINVAL;
        ຖ້າ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        ຖ້າ (ret)
                *ret = last_begin;
        ສົ່ງຄືນ len;
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

        ຖ້າ (!path)
                ສົ່ງຄືນ NULL;

        l = k = strlen(path);
        ຖ້າ (l == 0) /* special case — an empty string */
                ສົ່ງຄືນ path;

        ໃນຂະນະທີ່ (k > 0 && path[k-1] == '/')
                k--;

        ຖ້າ (k == 0) /* the root directory */
                ສົ່ງຄືນ path + l - 1;

        ໃນຂະນະທີ່ (k > 0 && path[k-1] != '/')
                k--;

        ສົ່ງຄືນ path + k;
}

ຈຳນວນເຕັມ path_split_prefix_filename(const var_na *path, var_na **ret_dir, var_na **ret_filename) {
        _cleanup_free_ var_na *d = NULL;
        const var_na *c, *next = NULL;
        ຈຳນວນເຕັມ r;

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

        ຖ້າ (isempty(path))
                ສົ່ງຄືນ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (r == 0) /* root directory or "." */
                ສົ່ງຄືນ -EADDRNOTAVAIL;

        ຖ້າ (ret_dir) {
                ຖ້າ (next == path) {
                        ຖ້າ (*path != '/') { /* filename only */
                                ຖ້າ (!ret_filename)
                                        ສົ່ງຄືນ -EDESTADDRREQ;
                        } ບໍ່ດັ່ງນັ້ນ {
                                d = strdup("/");
                                ຖ້າ (!d)
                                        ສົ່ງຄືນ -ENOMEM;
                        }
                } ບໍ່ດັ່ງນັ້ນ {
                        d = strndup(path, next - path);
                        ຖ້າ (!d)
                                ສົ່ງຄືນ -ENOMEM;

                        path_simplify(d);

                        ຖ້າ (!path_is_valid(d))
                                ສົ່ງຄືນ -EINVAL;
                }

        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (ret_filename) {
                var_na *fn = strndup(c, r);
                ຖ້າ (!fn)
                        ສົ່ງຄືນ -ENOMEM;

                *ret_filename = fn;
        }

        ຖ້າ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ສົ່ງຄືນ strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const var_na *p) {
        const var_na *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ຖ້າ (!p)
                ສົ່ງຄືນ false;

        e = strchrnul(p, '/');
        ຖ້າ (*e != 0)
                ສົ່ງຄືນ false;

        ຖ້າ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ true;
}

bool filename_is_valid(const var_na *p) {

        ຖ້າ (isempty(p))
                ສົ່ງຄືນ false;

        ຖ້າ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ filename_part_is_valid(p);
}

bool path_is_valid_full(const var_na *p, bool accept_dot_dot) {
        ຖ້າ (isempty(p))
                ສົ່ງຄືນ false;

        ສຳລັບ (const var_na *e = p;;) {
                ຈຳນວນເຕັມ r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ false;

                ຖ້າ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ສົ່ງຄືນ false;
                ຖ້າ (*e == 0)           /* End of string? Yay! */
                        ສົ່ງຄືນ true;
        }
}

bool path_is_normalized(const var_na *p) {
        ຖ້າ (!path_is_safe(p))
                ສົ່ງຄືນ false;

        ຖ້າ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ສົ່ງຄືນ false;

        ຖ້າ (strstr(p, "//"))
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ true;
}

ຈຳນວນເຕັມ file_in_same_dir(const var_na *path, const var_na *filename, var_na **ret) {
        _cleanup_free_ var_na *b = NULL;
        ຈຳນວນເຕັມ r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ຖ້າ (path_is_absolute(filename))
                b = strdup(filename);
        ບໍ່ດັ່ງນັ້ນ {
                _cleanup_free_ var_na *dn = NULL;

                r = path_extract_directory(path, &dn);
                ຖ້າ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ບໍ່ດັ່ງນັ້ນ ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ບໍ່ດັ່ງນັ້ນ
                        b = path_join(dn, filename);
        }
        ຖ້າ (!b)
                ສົ່ງຄືນ -ENOMEM;

        *ret = TAKE_PTR(b);
        ສົ່ງຄືນ 0;
}

bool hidden_or_backup_file(const var_na *filename) {
        assert(filename);

        ຖ້າ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ສົ່ງຄືນ true;

        const var_na *dot = strrchr(filename, '.');
        ຖ້າ (!dot)
                ສົ່ງຄືນ false;

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

        ສົ່ງຄືນ STR_IN_SET(dot + 1,
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
        ສົ່ງຄືນ !isempty(p);
}

bool valid_device_node_path(const var_na *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ຖ້າ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ສົ່ງຄືນ false;

        ຖ້າ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ path_is_normalized(path);
}

bool valid_device_allow_pattern(const var_na *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ຖ້າ (STARTSWITH_SET(path, "block-", "char-"))
                ສົ່ງຄືນ true;

        ສົ່ງຄືນ valid_device_node_path(path);
}

bool dot_or_dot_dot(const var_na *path) {
        ຖ້າ (!path)
                ສົ່ງຄືນ false;
        ຖ້າ (path[0] != '.')
                ສົ່ງຄືນ false;
        ຖ້າ (path[1] == 0)
                ສົ່ງຄືນ true;
        ຖ້າ (path[1] != '.')
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ path[2] == 0;
}

bool path_implies_directory(const var_na *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ຖ້າ (!path)
                ສົ່ງຄືນ false;

        ຖ້າ (dot_or_dot_dot(path))
                ສົ່ງຄືນ true;

        ສົ່ງຄືນ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const var_na *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ຖ້າ (isempty(path))
                ສົ່ງຄືນ true;

        ສົ່ງຄືນ path_equal(path, "/");
}

const var_na* empty_to_root(const var_na *path) {
        ສົ່ງຄືນ isempty(path) ? "/" : path;
}

ຈຳນວນເຕັມ empty_or_root_harder_to_null(const var_na **path) {
        ຈຳນວນເຕັມ r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        ຖ້າ (empty_or_root(*path)) {
                *path = NULL;
                ສົ່ງຄືນ 0;
        }

        r = path_is_root(*path);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (r > 0)
                *path = NULL;

        ສົ່ງຄືນ 0;
}

bool path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l)
                ຖ້າ (path_equal(*i, path))
                        ສົ່ງຄືນ true;

        ສົ່ງຄືນ false;
}

bool prefixed_path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const var_na *j = *i;

                ຖ້າ (*j == '-')
                        j++;
                ຖ້າ (*j == '+')
                        j++;

                ຖ້າ (path_equal(j, path))
                        ສົ່ງຄືນ true;
        }

        ສົ່ງຄືນ false;
}

ຈຳນວນເຕັມ path_glob_can_match(const var_na *pattern, const var_na *prefix, var_na **ret) {
        assert(pattern);
        assert(prefix);

        ສຳລັບ (const var_na *a = pattern, *b = prefix;;) {
                _cleanup_free_ var_na *g = NULL, *h = NULL;
                const var_na *p, *q;
                ຈຳນວນເຕັມ r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                ຖ້າ (s < 0)
                        ສົ່ງຄືນ s;

                ຖ້າ (s == 0) {
                        /* The pattern matches the prefix. */
                        ຖ້າ (ret) {
                                var_na *t;

                                t = path_join(prefix, p);
                                ຖ້າ (!t)
                                        ສົ່ງຄືນ -ENOMEM;

                                *ret = t;
                        }
                        ສົ່ງຄືນ true;
                }

                ຖ້າ (r == 0)
                        bha_nga;

                ຖ້າ (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                ຖ້າ (!g)
                        ສົ່ງຄືນ -ENOMEM;

                ຖ້າ (!string_is_glob(g))
                        bha_nga;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ຖ້າ (!h)
                        ສົ່ງຄືນ -ENOMEM;

                r = fnmatch(g, h, 0);
                ຖ້າ (r == FNM_NOMATCH)
                        bha_nga;
                ຖ້າ (r != 0) /* Failure to process pattern? */
                        ສົ່ງຄືນ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ຖ້າ (ret)
                *ret = NULL;
        ສົ່ງຄືນ false;
}

#ຖ້າ HAVE_SPLIT_BIN
static bool dir_is_split(const var_na *a, const var_na *b) {
        ຈຳນວນເຕັມ r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ຖ້າ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ສົ່ງຄືນ true;
        }
        ສົ່ງຄືນ r == 0;
}
#endif

const var_na* default_PATH(void) {
#ຖ້າ HAVE_SPLIT_BIN
        static const var_na *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ຖ້າ (default_path)
                ສົ່ງຄືນ default_path;

        ຖ້າ (dir_is_split("/usr/sbin", "/usr/bin"))
                ສົ່ງຄືນ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        ຖ້າ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ສົ່ງຄືນ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ສົ່ງຄືນ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#ບໍ່ດັ່ງນັ້ນ
        ສົ່ງຄືນ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
