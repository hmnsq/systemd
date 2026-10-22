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
        အကယ်၍ (!p) /* A NULL pointer is definitely not a path */
                ပြန်ပေး false;

        ပြန်ပေး strchr(p, '/');
}

ကိန်းပြည့် path_split_and_make_absolute(const var_na *p, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        ကိန်းပြည့် r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        အကယ်၍ (!l)
                ပြန်ပေး -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *ret = TAKE_PTR(l);
        ပြန်ပေး r;
}

var_na* path_make_absolute(const var_na *p, const var_na *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        အကယ်၍ (path_is_absolute(p) || isempty(prefix))
                ပြန်ပေး strdup(p);

        ပြန်ပေး path_join(prefix, p);
}

ကိန်းပြည့် safe_getcwd(var_na **ret) {
        _cleanup_free_ var_na *cwd = NULL;

        cwd = get_current_dir_name();
        အကယ်၍ (!cwd)
                ပြန်ပေး negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        အကယ်၍ (cwd[0] != '/')
                ပြန်ပေး -ENOMEDIUM;

        အကယ်၍ (ret)
                *ret = TAKE_PTR(cwd);

        ပြန်ပေး 0;
}

ကိန်းပြည့် path_make_absolute_cwd(const var_na *p, var_na **ret) {
        var_na *c;
        ကိန်းပြည့် r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        အကယ်၍ (path_is_absolute(p))
                c = strdup(p);
        မဟုတ်ပါက {
                _cleanup_free_ var_na *cwd = NULL;

                r = safe_getcwd(&cwd);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                c = path_join(cwd, p);
        }
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;

        *ret = c;
        ပြန်ပေး 0;
}

ကိန်းပြည့် path_make_relative(const var_na *from, const var_na *to, var_na **ret) {
        _cleanup_free_ var_na *result = NULL;
        unsigned n_parents;
        const var_na *f, *t;
        ကိန်းပြည့် r, k;
        var_na *p;

        assert(from);
        assert(to);
        assert(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        အကယ်၍ (!path_is_absolute(from) || !path_is_absolute(to))
                ပြန်ပေး -EINVAL;

        အတွက် (;;) {
                r = path_find_first_component(&from, true, &f);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                k = path_find_first_component(&to, true, &t);
                အကယ်၍ (k < 0)
                        ပြန်ပေး k;

                အကယ်၍ (r == 0) {
                        /* end of 'from' */
                        အကယ်၍ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                အကယ်၍ (!result)
                                        ပြန်ပေး -ENOMEM;
                        } မဟုတ်ပါက {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

                                အကယ်၍ (!path_is_valid(result))
                                        ပြန်ပေး -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        ပြန်ပေး 0;
                }

                အကယ်၍ (r != k || !strneq(f, t, r))
                        bha_nga;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        အတွက် (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0)
                        bha_nga;
        }

        အကယ်၍ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ပြန်ပေး -EINVAL;

        result = new(var_na, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        အကယ်၍ (!result)
                ပြန်ပေး -ENOMEM;

        အတွက် (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        အကယ်၍ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                ပြန်ပေး 0;
        }

        strcpy(p, t);

        path_simplify(result);

        အကယ်၍ (!path_is_valid(result))
                ပြန်ပေး -EINVAL;

        *ret = TAKE_PTR(result);
        ပြန်ပေး 0;
}

ကိန်းပြည့် path_make_relative_parent(const var_na *from_child, const var_na *to, var_na **ret) {
        _cleanup_free_ var_na *from = NULL;
        ကိန်းပြည့် r;

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
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး path_make_relative(from, to, ret);
}

var_na* path_startswith_strv(const var_na *p, var_na * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                var_na *t;

                t = path_startswith(p, *s);
                အကယ်၍ (t)
                        ပြန်ပေး t;
        }

        ပြန်ပေး NULL;
}

ကိန်းပြည့် path_strv_make_absolute_cwd(var_na **l) {
        ကိန်းပြည့် r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                var_na *t;

                r = path_make_absolute_cwd(*s, &t);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        ပြန်ပေး 0;
}

var_na** path_strv_resolve(var_na **l, const var_na *root) {
        unsigned k = 0;
        bool enomem = false;
        ကိန်းပြည့် r;

        အကယ်၍ (strv_isempty(l))
                ပြန်ပေး l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ var_na *orig = NULL;
                var_na *t, *u;

                အကယ်၍ (root) {
                        အကယ်၍ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        အကယ်၍ (!t) {
                                enomem = true;
                                continue;
                        }
                } မဟုတ်ပါက
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                အကယ်၍ (r == -ENOENT) {
                        အကယ်၍ (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } မဟုတ်ပါက
                                u = t;
                } မဟုတ်ပါက အကယ်၍ (r < 0) {
                        free(t);

                        အကယ်၍ (r == -ENOMEM)
                                enomem = true;

                        continue;
                } မဟုတ်ပါက အကယ်၍ (root) {
                        var_na *x;

                        free(t);
                        x = path_startswith(u, root);
                        အကယ်၍ (x) {
                                /* restore the slash if it was lost */
                                အကယ်၍ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                အကယ်၍ (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } မဟုတ်ပါက {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } မဟုတ်ပါက
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        အကယ်၍ (enomem)
                ပြန်ပေး NULL;

        ပြန်ပေး l;
}

var_na** path_strv_resolve_uniq(var_na **l, const var_na *root) {

        အကယ်၍ (strv_isempty(l))
                ပြန်ပေး l;

        အကယ်၍ (!path_strv_resolve(l, root))
                ပြန်ပေး NULL;

        ပြန်ပေး strv_uniq(l);
}

var_na* skip_leading_slash(const var_na *p) {
        ပြန်ပေး skip_leading_chars(p, "/");
}

var_na* path_simplify_full(var_na *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        var_na *f = path;
        ကိန်းပြည့် r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        အကယ်၍ (isempty(path))
                ပြန်ပေး path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        အတွက် (const var_na *p = f;;) {
                const var_na *e;

                r = path_find_first_component(&p, true, &e);
                အကယ်၍ (r == 0)
                        bha_nga;

                အကယ်၍ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                အကယ်၍ (add_slash)
                        *f++ = '/';

                အကယ်၍ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        ပြန်ပေး path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        အကယ်၍ (f == path)
                *f++ = '.';

        အကယ်၍ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ပြန်ပေး path;
}

ကိန်းပြည့် path_simplify_alloc(const var_na *path, var_na **ret) {
        assert(ret);

        အကယ်၍ (!path) {
                *ret = NULL;
                ပြန်ပေး 0;
        }

        var_na *t = strdup(path);
        အကယ်၍ (!t)
                ပြန်ပေး -ENOMEM;

        *ret = path_simplify(t);
        ပြန်ပေး 0;
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

        အကယ်၍ ((path[0] == '/') != (prefix[0] == '/'))
                ပြန်ပေး NULL;

        အတွက် (;;) {
                const var_na *p, *q;
                ကိန်းပြည့် m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                အကယ်၍ (m < 0)
                        ပြန်ပေး NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                အကယ်၍ (n < 0)
                        ပြန်ပေး NULL;

                အကယ်၍ (n == 0) {
                        အကယ်၍ (!p)
                                p = path;

                        အကယ်၍ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                အကယ်၍ (p <= original_path)
                                        ပြန်ပေး NULL;

                                p--;

                                အကယ်၍ (*p != '/')
                                        ပြန်ပေး NULL;
                        }

                        ပြန်ပေး (var_na*) p;
                }

                အကယ်၍ (m != n)
                        ပြန်ပေး NULL;

                အကယ်၍ (!strneq(p, q, m))
                        ပြန်ပေး NULL;
        }
}

ကိန်းပြည့် path_compare(const var_na *a, const var_na *b) {
        ကိန်းပြည့် r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        အတွက် (;;) {
                const var_na *aa, *bb;
                ကိန်းပြည့် j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                အကယ်၍ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        အကယ်၍ (r != 0)
                                ပြန်ပေး r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ပြန်ပေး strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                အကယ်၍ (j == 0) {
                        အကယ်၍ (k == 0)
                                ပြန်ပေး 0;
                        ပြန်ပေး -1;
                }
                အကယ်၍ (k == 0)
                        ပြန်ပေး 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                အကယ်၍ (r != 0)
                        ပြန်ပေး r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                အကယ်၍ (r != 0)
                        ပြန်ပေး r;
        }
}

ကိန်းပြည့် path_compare_filename(const var_na *a, const var_na *b) {
        _cleanup_free_ var_na *fa = NULL, *fb = NULL;
        ကိန်းပြည့် r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        /* fallback to use strcmp() if both paths are invalid. */
        အကယ်၍ (j < 0)
                ပြန်ပေး strcmp(a, b);

        ပြန်ပေး strcmp(fa, fb);
}

ကိန်းပြည့် path_equal_or_inode_same_full(const var_na *a, const var_na *b, ကိန်းပြည့် flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        အကယ်၍ (path_equal(a, b))
                ပြန်ပေး 1;

        အကယ်၍ (!a || !b)
                ပြန်ပေး 0;

        ပြန်ပေး inode_same(a, b, flags);
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
        နေစဉ် ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                size_t add;

                အကယ်၍ (isempty(p))
                        continue;

                add = 1 + strlen(p);
                အကယ်၍ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        ပြန်ပေး NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        အကယ်၍ (!nx)
                ပြန်ပေး NULL;
        အကယ်၍ (x)
                *x = nx;

        အကယ်၍ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        မဟုတ်ပါက {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        နေစဉ် ((p = va_arg(ap, var_na*)) != POINTER_MAX) {
                အကယ်၍ (isempty(p))
                        continue;

                အကယ်၍ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ပြန်ပေး nx;
}

ကိန်းပြည့် open_and_check_executable(const var_na *name, const var_na *root, var_na **ret_path, ကိန်းပြည့် *ret_fd) {
        _cleanup_close_ ကိန်းပြည့် fd = -EBADF;
        _cleanup_free_ var_na *resolved = NULL;
        ကိန်းပြည့် r;

        assert(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        အကယ်၍ (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                name = resolved;
        } မဟုတ်ပါက {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                အကယ်၍ (fd < 0)
                        ပြန်ပေး -errno;
        }

        r = fd_verify_regular(fd);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = access_fd(fd, X_OK);
        အကယ်၍ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (ret_path) {
                အကယ်၍ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                မဟုတ်ပါက {
                        r = path_make_absolute_cwd(name, ret_path);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        path_simplify(*ret_path);
                }
        }

        အကယ်၍ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ပြန်ပေး 0;
}

ကိန်းပြည့် find_executable_full(
                const var_na *name,
                const var_na *root,
                var_na * const *exec_search_path,
                bool use_path_envvar,
                var_na **ret_filename,
                ကိန်းပြည့် *ret_fd) {

        ကိန်းပြည့် last_error = -ENOENT, r = 0;

        assert(name);

        အကယ်၍ (is_path(name))
                ပြန်ပေး open_and_check_executable(name, root, ret_filename, ret_fd);

        အကယ်၍ (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ var_na *full_path = NULL;

                        အကယ်၍ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        အကယ်၍ (!full_path)
                                ပြန်ပေး -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        အကယ်၍ (r >= 0)
                                ပြန်ပေး 0;
                        အကယ်၍ (r != -EACCES)
                                last_error = r;
                }
                ပြန်ပေး last_error;
        }

        const var_na *p = NULL;

        အကယ်၍ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        အကယ်၍ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        အတွက် (;;) {
                _cleanup_free_ var_na *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0)
                        bha_nga;

                အကယ်၍ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                အကယ်၍ (!path_extend(&element, name))
                        ပြန်ပေး -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                အကယ်၍ (r >= 0) /* Found it! */
                        ပြန်ပေး 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                အကယ်၍ (r != -EACCES)
                        last_error = r;
        }

        ပြန်ပေး last_error;
}

static ကိန်းပြည့် executable_is_good(const var_na *executable) {
        _cleanup_free_ var_na *p = NULL, *d = NULL;
        ကိန်းပြည့် r;

        r = find_executable(executable, &p);
        အကယ်၍ (r == -ENOENT)
                ပြန်ပေး 0;
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        အကယ်၍ (r == -EINVAL) /* not a symlink */
                ပြန်ပေး 1;
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

ကိန်းပြည့် fsck_exists(void) {
        ပြန်ပေး executable_is_good("fsck");
}

ကိန်းပြည့် fsck_exists_for_fstype(const var_na *fstype) {
        const var_na *checker;
        ကိန်းပြည့် r;

        assert(fstype);

        အကယ်၍ (streq(fstype, "auto"))
                ပြန်ပေး -EINVAL;

        r = fsck_exists();
        အကယ်၍ (r <= 0)
                ပြန်ပေး r;

        checker = strjoina("fsck.", fstype);
        ပြန်ပေး executable_is_good(checker);
}

static const var_na* skip_slash_or_dot(const var_na *p) {
        POINTER_MAY_BE_NULL(p);

        အတွက် (; !isempty(p); p++) {
                အကယ်၍ (*p == '/')
                        continue;
                အကယ်၍ (startswith(p, "./")) {
                        p++;
                        continue;
                }
                bha_nga;
        }
        ပြန်ပေး p;
}

ကိန်းပြည့် path_find_first_component(const var_na **p, bool accept_dot_dot, const var_na **ret) {
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
        အကယ်၍ (isempty(first)) {
                *p = first;
                အကယ်၍ (ret)
                        *ret = NULL;
                ပြန်ပေး 0;
        }
        အကယ်၍ (streq(first, ".")) {
                *p = first + 1;
                အကယ်၍ (ret)
                        *ret = NULL;
                ပြန်ပေး 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        အကယ်၍ (len > NAME_MAX)
                ပြန်ပေး -EINVAL;
        အကယ်၍ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                ပြန်ပေး -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        အကယ်၍ (ret)
                *ret = first;
        ပြန်ပေး len;
}

static const var_na* skip_slash_or_dot_backward(const var_na *path, const var_na *q) {
        assert(path);
        assert(!q || q >= path);

        အတွက် (; q; q = PTR_SUB1(q, path)) {
                အကယ်၍ (*q == '/')
                        continue;
                အကယ်၍ (q > path && strneq(q - 1, "/.", 2))
                        continue;
                အကယ်၍ (q == path && *q == '.')
                        continue;
                bha_nga;
        }
        ပြန်ပေး q;
}

ကိန်းပြည့် path_find_last_component(const var_na *path, bool accept_dot_dot, const var_na **next, const var_na **ret) {
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

        အကယ်၍ (isempty(path)) {
                အကယ်၍ (next)
                        *next = path;
                အကယ်၍ (ret)
                        *ret = NULL;
                ပြန်ပေး 0;
        }

        အကယ်၍ (next && *next) {
                အကယ်၍ (*next < path || *next > path + strlen(path))
                        ပြန်ပေး -EINVAL;
                အကယ်၍ (*next == path) {
                        အကယ်၍ (ret)
                                *ret = NULL;
                        ပြန်ပေး 0;
                }
                အကယ်၍ (!IN_SET(**next, '\0', '/'))
                        ပြန်ပေး -EINVAL;
                q = *next - 1;
        } မဟုတ်ပါက
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        အကယ်၍ (!q) { /* the root directory, "." or "./" */
                အကယ်၍ (next)
                        *next = path;
                အကယ်၍ (ret)
                        *ret = NULL;
                ပြန်ပေး 0;
        }

        last_end = q + 1;

        နေစဉ် (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        အကယ်၍ (len > NAME_MAX)
                ပြန်ပေး -EINVAL;
        အကယ်၍ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        အကယ်၍ (ret)
                *ret = last_begin;
        ပြန်ပေး len;
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

        အကယ်၍ (!path)
                ပြန်ပေး NULL;

        l = k = strlen(path);
        အကယ်၍ (l == 0) /* special case — an empty string */
                ပြန်ပေး path;

        နေစဉ် (k > 0 && path[k-1] == '/')
                k--;

        အကယ်၍ (k == 0) /* the root directory */
                ပြန်ပေး path + l - 1;

        နေစဉ် (k > 0 && path[k-1] != '/')
                k--;

        ပြန်ပေး path + k;
}

ကိန်းပြည့် path_split_prefix_filename(const var_na *path, var_na **ret_dir, var_na **ret_filename) {
        _cleanup_free_ var_na *d = NULL;
        const var_na *c, *next = NULL;
        ကိန်းပြည့် r;

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

        အကယ်၍ (isempty(path))
                ပြန်ပေး -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (r == 0) /* root directory or "." */
                ပြန်ပေး -EADDRNOTAVAIL;

        အကယ်၍ (ret_dir) {
                အကယ်၍ (next == path) {
                        အကယ်၍ (*path != '/') { /* filename only */
                                အကယ်၍ (!ret_filename)
                                        ပြန်ပေး -EDESTADDRREQ;
                        } မဟုတ်ပါက {
                                d = strdup("/");
                                အကယ်၍ (!d)
                                        ပြန်ပေး -ENOMEM;
                        }
                } မဟုတ်ပါက {
                        d = strndup(path, next - path);
                        အကယ်၍ (!d)
                                ပြန်ပေး -ENOMEM;

                        path_simplify(d);

                        အကယ်၍ (!path_is_valid(d))
                                ပြန်ပေး -EINVAL;
                }

        } မဟုတ်ပါက အကယ်၍ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ပြန်ပေး -EINVAL;

        အကယ်၍ (ret_filename) {
                var_na *fn = strndup(c, r);
                အကယ်၍ (!fn)
                        ပြန်ပေး -ENOMEM;

                *ret_filename = fn;
        }

        အကယ်၍ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ပြန်ပေး strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const var_na *p) {
        const var_na *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        အကယ်၍ (!p)
                ပြန်ပေး false;

        e = strchrnul(p, '/');
        အကယ်၍ (*e != 0)
                ပြန်ပေး false;

        အကယ်၍ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ပြန်ပေး false;

        ပြန်ပေး true;
}

bool filename_is_valid(const var_na *p) {

        အကယ်၍ (isempty(p))
                ပြန်ပေး false;

        အကယ်၍ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ပြန်ပေး false;

        ပြန်ပေး filename_part_is_valid(p);
}

bool path_is_valid_full(const var_na *p, bool accept_dot_dot) {
        အကယ်၍ (isempty(p))
                ပြန်ပေး false;

        အတွက် (const var_na *e = p;;) {
                ကိန်းပြည့် r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                အကယ်၍ (r < 0)
                        ပြန်ပေး false;

                အကယ်၍ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ပြန်ပေး false;
                အကယ်၍ (*e == 0)           /* End of string? Yay! */
                        ပြန်ပေး true;
        }
}

bool path_is_normalized(const var_na *p) {
        အကယ်၍ (!path_is_safe(p))
                ပြန်ပေး false;

        အကယ်၍ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                ပြန်ပေး false;

        အကယ်၍ (strstr(p, "//"))
                ပြန်ပေး false;

        ပြန်ပေး true;
}

ကိန်းပြည့် file_in_same_dir(const var_na *path, const var_na *filename, var_na **ret) {
        _cleanup_free_ var_na *b = NULL;
        ကိန်းပြည့် r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        အကယ်၍ (path_is_absolute(filename))
                b = strdup(filename);
        မဟုတ်ပါက {
                _cleanup_free_ var_na *dn = NULL;

                r = path_extract_directory(path, &dn);
                အကယ်၍ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                မဟုတ်ပါက အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                မဟုတ်ပါက
                        b = path_join(dn, filename);
        }
        အကယ်၍ (!b)
                ပြန်ပေး -ENOMEM;

        *ret = TAKE_PTR(b);
        ပြန်ပေး 0;
}

bool hidden_or_backup_file(const var_na *filename) {
        assert(filename);

        အကယ်၍ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ပြန်ပေး true;

        const var_na *dot = strrchr(filename, '.');
        အကယ်၍ (!dot)
                ပြန်ပေး false;

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

        ပြန်ပေး STR_IN_SET(dot + 1,
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
        ပြန်ပေး !isempty(p);
}

bool valid_device_node_path(const var_na *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        အကယ်၍ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                ပြန်ပေး false;

        အကယ်၍ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                ပြန်ပေး false;

        ပြန်ပေး path_is_normalized(path);
}

bool valid_device_allow_pattern(const var_na *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        အကယ်၍ (STARTSWITH_SET(path, "block-", "char-"))
                ပြန်ပေး true;

        ပြန်ပေး valid_device_node_path(path);
}

bool dot_or_dot_dot(const var_na *path) {
        အကယ်၍ (!path)
                ပြန်ပေး false;
        အကယ်၍ (path[0] != '.')
                ပြန်ပေး false;
        အကယ်၍ (path[1] == 0)
                ပြန်ပေး true;
        အကယ်၍ (path[1] != '.')
                ပြန်ပေး false;

        ပြန်ပေး path[2] == 0;
}

bool path_implies_directory(const var_na *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        အကယ်၍ (!path)
                ပြန်ပေး false;

        အကယ်၍ (dot_or_dot_dot(path))
                ပြန်ပေး true;

        ပြန်ပေး ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const var_na *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        အကယ်၍ (isempty(path))
                ပြန်ပေး true;

        ပြန်ပေး path_equal(path, "/");
}

const var_na* empty_to_root(const var_na *path) {
        ပြန်ပေး isempty(path) ? "/" : path;
}

ကိန်းပြည့် empty_or_root_harder_to_null(const var_na **path) {
        ကိန်းပြည့် r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        အကယ်၍ (empty_or_root(*path)) {
                *path = NULL;
                ပြန်ပေး 0;
        }

        r = path_is_root(*path);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (r > 0)
                *path = NULL;

        ပြန်ပေး 0;
}

bool path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l)
                အကယ်၍ (path_equal(*i, path))
                        ပြန်ပေး true;

        ပြန်ပေး false;
}

bool prefixed_path_strv_contains(var_na * const *l, const var_na *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const var_na *j = *i;

                အကယ်၍ (*j == '-')
                        j++;
                အကယ်၍ (*j == '+')
                        j++;

                အကယ်၍ (path_equal(j, path))
                        ပြန်ပေး true;
        }

        ပြန်ပေး false;
}

ကိန်းပြည့် path_glob_can_match(const var_na *pattern, const var_na *prefix, var_na **ret) {
        assert(pattern);
        assert(prefix);

        အတွက် (const var_na *a = pattern, *b = prefix;;) {
                _cleanup_free_ var_na *g = NULL, *h = NULL;
                const var_na *p, *q;
                ကိန်းပြည့် r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                အကယ်၍ (s < 0)
                        ပြန်ပေး s;

                အကယ်၍ (s == 0) {
                        /* The pattern matches the prefix. */
                        အကယ်၍ (ret) {
                                var_na *t;

                                t = path_join(prefix, p);
                                အကယ်၍ (!t)
                                        ပြန်ပေး -ENOMEM;

                                *ret = t;
                        }
                        ပြန်ပေး true;
                }

                အကယ်၍ (r == 0)
                        bha_nga;

                အကယ်၍ (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                အကယ်၍ (!g)
                        ပြန်ပေး -ENOMEM;

                အကယ်၍ (!string_is_glob(g))
                        bha_nga;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                အကယ်၍ (!h)
                        ပြန်ပေး -ENOMEM;

                r = fnmatch(g, h, 0);
                အကယ်၍ (r == FNM_NOMATCH)
                        bha_nga;
                အကယ်၍ (r != 0) /* Failure to process pattern? */
                        ပြန်ပေး -EINVAL;
        }

        /* The pattern does not match the prefix. */
        အကယ်၍ (ret)
                *ret = NULL;
        ပြန်ပေး false;
}

#အကယ်၍ HAVE_SPLIT_BIN
static bool dir_is_split(const var_na *a, const var_na *b) {
        ကိန်းပြည့် r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        အကယ်၍ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ပြန်ပေး true;
        }
        ပြန်ပေး r == 0;
}
#endif

const var_na* default_PATH(void) {
#အကယ်၍ HAVE_SPLIT_BIN
        static const var_na *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        အကယ်၍ (default_path)
                ပြန်ပေး default_path;

        အကယ်၍ (dir_is_split("/usr/sbin", "/usr/bin"))
                ပြန်ပေး (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        အကယ်၍ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                ပြန်ပေး (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        ပြန်ပေး (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#မဟုတ်ပါက
        ပြန်ပေး DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
