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
        إذا (!p) /* A NULL pointer is definitely not a path */
                أرجع false;

        أرجع strchr(p, '/');
}

صحيح path_split_and_make_absolute(ثابت حرف *p, حرف ***ret) {
        _cleanup_strv_free_ حرف **l = عدم;
        صحيح r;

        أكد(p);
        أكد(ret);

        l = strv_split(p, ":");
        إذا (!l)
                أرجع -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(l);
        أرجع r;
}

حرف* path_make_absolute(ثابت حرف *p, ثابت حرف *prefix) {
        أكد(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        إذا (path_is_absolute(p) || isempty(prefix))
                أرجع strdup(p);

        أرجع path_join(prefix, p);
}

صحيح safe_getcwd(حرف **ret) {
        _cleanup_free_ حرف *cwd = عدم;

        cwd = get_current_dir_name();
        إذا (!cwd)
                أرجع negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        إذا (cwd[0] != '/')
                أرجع -ENOMEDIUM;

        إذا (ret)
                *ret = TAKE_PTR(cwd);

        أرجع 0;
}

صحيح path_make_absolute_cwd(ثابت حرف *p, حرف **ret) {
        حرف *c;
        صحيح r;

        أكد(p);
        أكد(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        إذا (path_is_absolute(p))
                c = strdup(p);
        وإلا {
                _cleanup_free_ حرف *cwd = عدم;

                r = safe_getcwd(&cwd);
                إذا (r < 0)
                        أرجع r;

                c = path_join(cwd, p);
        }
        إذا (!c)
                أرجع -ENOMEM;

        *ret = c;
        أرجع 0;
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

        إذا (!path_is_absolute(from) || !path_is_absolute(to))
                أرجع -EINVAL;

        لكل (;;) {
                r = path_find_first_component(&from, true, &f);
                إذا (r < 0)
                        أرجع r;

                k = path_find_first_component(&to, true, &t);
                إذا (k < 0)
                        أرجع k;

                إذا (r == 0) {
                        /* end of 'from' */
                        إذا (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                إذا (!result)
                                        أرجع -ENOMEM;
                        } وإلا {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (!path_is_valid(result))
                                        أرجع -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        أرجع 0;
                }

                إذا (r != k || !strneq(f, t, r))
                        اكسر;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        لكل (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;
        }

        إذا (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                أرجع -EINVAL;

        result = new(حرف, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        إذا (!result)
                أرجع -ENOMEM;

        لكل (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        إذا (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                أرجع 0;
        }

        strcpy(p, t);

        path_simplify(result);

        إذا (!path_is_valid(result))
                أرجع -EINVAL;

        *ret = TAKE_PTR(result);
        أرجع 0;
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
        إذا (r < 0)
                أرجع r;

        أرجع path_make_relative(from, to, ret);
}

حرف* path_startswith_strv(ثابت حرف *p, حرف * ثابت *strv) {
        أكد(p);

        STRV_FOREACH(s, strv) {
                حرف *t;

                t = path_startswith(p, *s);
                إذا (t)
                        أرجع t;
        }

        أرجع عدم;
}

صحيح path_strv_make_absolute_cwd(حرف **l) {
        صحيح r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                حرف *t;

                r = path_make_absolute_cwd(*s, &t);
                إذا (r < 0)
                        أرجع r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        أرجع 0;
}

حرف** path_strv_resolve(حرف **l, ثابت حرف *جڑ) {
        unsigned k = 0;
        bool enomem = false;
        صحيح r;

        إذا (strv_isempty(l))
                أرجع l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ حرف *orig = عدم;
                حرف *t, *u;

                إذا (جڑ) {
                        إذا (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                حرر(*s);
                                استمر;
                        }

                        orig = *s;
                        t = path_join(جڑ, orig);
                        إذا (!t) {
                                enomem = true;
                                استمر;
                        }
                } وإلا
                        t = *s;

                r = chase(t, جڑ, 0, &u, عدم);
                إذا (r == -ENOENT) {
                        إذا (جڑ) {
                                u = TAKE_PTR(orig);
                                حرر(t);
                        } وإلا
                                u = t;
                } وإلا إذا (r < 0) {
                        حرر(t);

                        إذا (r == -ENOMEM)
                                enomem = true;

                        استمر;
                } وإلا إذا (جڑ) {
                        حرف *x;

                        حرر(t);
                        x = path_startswith(u, جڑ);
                        إذا (x) {
                                /* restore the slash if it was lost */
                                إذا (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                حرر(u);
                                إذا (!t) {
                                        enomem = true;
                                        استمر;
                                }
                                u = t;
                        } وإلا {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } وإلا
                        حرر(t);

                l[k++] = u;
        }

        l[k] = عدم;

        إذا (enomem)
                أرجع عدم;

        أرجع l;
}

حرف** path_strv_resolve_uniq(حرف **l, ثابت حرف *جڑ) {

        إذا (strv_isempty(l))
                أرجع l;

        إذا (!path_strv_resolve(l, جڑ))
                أرجع عدم;

        أرجع strv_uniq(l);
}

حرف* skip_leading_slash(ثابت حرف *p) {
        أرجع skip_leading_chars(p, "/");
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

        إذا (isempty(path))
                أرجع path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        لكل (ثابت حرف *p = f;;) {
                ثابت حرف *e;

                r = path_find_first_component(&p, true, &e);
                إذا (r == 0)
                        اكسر;

                إذا (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        استمر;

                beginning = false;

                إذا (add_slash)
                        *f++ = '/';

                إذا (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        أرجع path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        إذا (f == path)
                *f++ = '.';

        إذا (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        أرجع path;
}

صحيح path_simplify_alloc(ثابت حرف *path, حرف **ret) {
        أكد(ret);

        إذا (!path) {
                *ret = عدم;
                أرجع 0;
        }

        حرف *t = strdup(path);
        إذا (!t)
                أرجع -ENOMEM;

        *ret = path_simplify(t);
        أرجع 0;
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

        إذا ((path[0] == '/') != (prefix[0] == '/'))
                أرجع عدم;

        لكل (;;) {
                ثابت حرف *p, *q;
                صحيح m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                إذا (m < 0)
                        أرجع عدم;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                إذا (n < 0)
                        أرجع عدم;

                إذا (n == 0) {
                        إذا (!p)
                                p = path;

                        إذا (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                إذا (p <= original_path)
                                        أرجع عدم;

                                p--;

                                إذا (*p != '/')
                                        أرجع عدم;
                        }

                        أرجع (حرف*) p;
                }

                إذا (m != n)
                        أرجع عدم;

                إذا (!strneq(p, q, m))
                        أرجع عدم;
        }
}

صحيح path_compare(ثابت حرف *a, ثابت حرف *b) {
        صحيح r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        إذا (r != 0)
                أرجع r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        إذا (r != 0)
                أرجع r;

        لكل (;;) {
                ثابت حرف *aa, *bb;
                صحيح j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                إذا (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        إذا (r != 0)
                                أرجع r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        أرجع strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                إذا (j == 0) {
                        إذا (k == 0)
                                أرجع 0;
                        أرجع -1;
                }
                إذا (k == 0)
                        أرجع 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                إذا (r != 0)
                        أرجع r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                إذا (r != 0)
                        أرجع r;
        }
}

صحيح path_compare_filename(ثابت حرف *a, ثابت حرف *b) {
        _cleanup_free_ حرف *fa = عدم, *fb = عدم;
        صحيح r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        إذا (r != 0)
                أرجع r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        إذا (r != 0)
                أرجع r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        إذا (r != 0)
                أرجع r;

        /* fallback to use strcmp() if both paths are invalid. */
        إذا (j < 0)
                أرجع strcmp(a, b);

        أرجع strcmp(fa, fb);
}

صحيح path_equal_or_inode_same_full(ثابت حرف *a, ثابت حرف *b, صحيح flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        إذا (path_equal(a, b))
                أرجع 1;

        إذا (!a || !b)
                أرجع 0;

        أرجع inode_same(a, b, flags);
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
        طالما ((p = va_arg(ap, حرف*)) != POINTER_MAX) {
                size_t add;

                إذا (isempty(p))
                        استمر;

                add = 1 + strlen(p);
                إذا (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        أرجع عدم;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : عدم, GREEDY_ALLOC_ROUND_UP(sz+1));
        إذا (!nx)
                أرجع عدم;
        إذا (x)
                *x = nx;

        إذا (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        وإلا {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        طالما ((p = va_arg(ap, حرف*)) != POINTER_MAX) {
                إذا (isempty(p))
                        استمر;

                إذا (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        أرجع nx;
}

صحيح open_and_check_executable(ثابت حرف *name, ثابت حرف *جڑ, حرف **ret_path, صحيح *ret_fd) {
        _cleanup_close_ صحيح fd = -EBADF;
        _cleanup_free_ حرف *resolved = عدم;
        صحيح r;

        أكد(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        إذا (جڑ) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, جڑ, CHASE_PREFIX_جڑ, &resolved, &fd);
                إذا (r < 0)
                        أرجع r;

                name = resolved;
        } وإلا {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                إذا (fd < 0)
                        أرجع -errno;
        }

        r = fd_verify_regular(fd);
        إذا (r < 0)
                أرجع r;

        r = access_fd(fd, X_OK);
        إذا (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        إذا (r < 0)
                أرجع r;

        إذا (ret_path) {
                إذا (resolved)
                        *ret_path = TAKE_PTR(resolved);
                وإلا {
                        r = path_make_absolute_cwd(name, ret_path);
                        إذا (r < 0)
                                أرجع r;

                        path_simplify(*ret_path);
                }
        }

        إذا (ret_fd)
                *ret_fd = TAKE_FD(fd);

        أرجع 0;
}

صحيح find_executable_full(
                ثابت حرف *name,
                ثابت حرف *جڑ,
                حرف * ثابت *exec_تلاش_path,
                bool use_path_envvar,
                حرف **ret_filename,
                صحيح *ret_fd) {

        صحيح last_error = -ENOENT, r = 0;

        أكد(name);

        إذا (is_path(name))
                أرجع open_and_check_executable(name, جڑ, ret_filename, ret_fd);

        إذا (exec_تلاش_path) {
                STRV_FOREACH(element, exec_تلاش_path) {
                        _cleanup_free_ حرف *full_path = عدم;

                        إذا (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                استمر;
                        }

                        full_path = path_join(*element, name);
                        إذا (!full_path)
                                أرجع -ENOMEM;

                        r = open_and_check_executable(full_path, جڑ, ret_filename, ret_fd);
                        إذا (r >= 0)
                                أرجع 0;
                        إذا (r != -EACCES)
                                last_error = r;
                }
                أرجع last_error;
        }

        ثابت حرف *p = عدم;

        إذا (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        إذا (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        لكل (;;) {
                _cleanup_free_ حرف *element = عدم;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                إذا (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        استمر;
                }

                إذا (!path_extend(&element, name))
                        أرجع -ENOMEM;

                r = open_and_check_executable(element, جڑ, ret_filename, ret_fd);
                إذا (r >= 0) /* Found it! */
                        أرجع 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                إذا (r != -EACCES)
                        last_error = r;
        }

        أرجع last_error;
}

ساكن صحيح executable_is_good(ثابت حرف *executable) {
        _cleanup_free_ حرف *p = عدم, *d = عدم;
        صحيح r;

        r = find_executable(executable, &p);
        إذا (r == -ENOENT)
                أرجع 0;
        إذا (r < 0)
                أرجع r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        إذا (r == -EINVAL) /* not a symlink */
                أرجع 1;
        إذا (r < 0)
                أرجع r;

        أرجع !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

صحيح fsck_exists(فراغ) {
        أرجع executable_is_good("fsck");
}

صحيح fsck_exists_for_fstype(ثابت حرف *fstype) {
        ثابت حرف *checker;
        صحيح r;

        أكد(fstype);

        إذا (streq(fstype, "auto"))
                أرجع -EINVAL;

        r = fsck_exists();
        إذا (r <= 0)
                أرجع r;

        checker = strjoina("fsck.", fstype);
        أرجع executable_is_good(checker);
}

ساكن ثابت حرف* skip_slash_or_dot(ثابت حرف *p) {
        POINTER_MAY_BE_NULL(p);

        لكل (; !isempty(p); p++) {
                إذا (*p == '/')
                        استمر;
                إذا (startswith(p, "./")) {
                        p++;
                        استمر;
                }
                اكسر;
        }
        أرجع p;
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
        إذا (isempty(first)) {
                *p = first;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }
        إذا (streq(first, ".")) {
                *p = first + 1;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        إذا (len > NAME_MAX)
                أرجع -EINVAL;
        إذا (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                أرجع -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        إذا (ret)
                *ret = first;
        أرجع len;
}

ساكن ثابت حرف* skip_slash_or_dot_backward(ثابت حرف *path, ثابت حرف *q) {
        أكد(path);
        أكد(!q || q >= path);

        لكل (; q; q = PTR_SUB1(q, path)) {
                إذا (*q == '/')
                        استمر;
                إذا (q > path && strneq(q - 1, "/.", 2))
                        استمر;
                إذا (q == path && *q == '.')
                        استمر;
                اكسر;
        }
        أرجع q;
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

        إذا (isempty(path)) {
                إذا (next)
                        *next = path;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }

        إذا (next && *next) {
                إذا (*next < path || *next > path + strlen(path))
                        أرجع -EINVAL;
                إذا (*next == path) {
                        إذا (ret)
                                *ret = عدم;
                        أرجع 0;
                }
                إذا (!IN_SET(**next, '\0', '/'))
                        أرجع -EINVAL;
                q = *next - 1;
        } وإلا
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        إذا (!q) { /* the root directory, "." or "./" */
                إذا (next)
                        *next = path;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }

        last_end = q + 1;

        طالما (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        إذا (len > NAME_MAX)
                أرجع -EINVAL;
        إذا (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                أرجع -EINVAL;

        إذا (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        إذا (ret)
                *ret = last_begin;
        أرجع len;
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

        إذا (!path)
                أرجع عدم;

        l = k = strlen(path);
        إذا (l == 0) /* special case — an empty string */
                أرجع path;

        طالما (k > 0 && path[k-1] == '/')
                k--;

        إذا (k == 0) /* the root directory */
                أرجع path + l - 1;

        طالما (k > 0 && path[k-1] != '/')
                k--;

        أرجع path + k;
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

        إذا (isempty(path))
                أرجع -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) /* root directory or "." */
                أرجع -EADDRNOTAVAIL;

        إذا (ret_dir) {
                إذا (next == path) {
                        إذا (*path != '/') { /* filename only */
                                إذا (!ret_filename)
                                        أرجع -EDESTADDRREQ;
                        } وإلا {
                                d = strdup("/");
                                إذا (!d)
                                        أرجع -ENOMEM;
                        }
                } وإلا {
                        d = strndup(path, next - path);
                        إذا (!d)
                                أرجع -ENOMEM;

                        path_simplify(d);

                        إذا (!path_is_valid(d))
                                أرجع -EINVAL;
                }

        } وإلا إذا (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                أرجع -EINVAL;

        إذا (ret_filename) {
                حرف *fn = strndup(c, r);
                إذا (!fn)
                        أرجع -ENOMEM;

                *ret_filename = fn;
        }

        إذا (ret_dir)
                *ret_dir = TAKE_PTR(d);

        أرجع strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ثابت حرف *p) {
        ثابت حرف *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        إذا (!p)
                أرجع false;

        e = strchrnul(p, '/');
        إذا (*e != 0)
                أرجع false;

        إذا (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                أرجع false;

        أرجع true;
}

bool filename_is_valid(ثابت حرف *p) {

        إذا (isempty(p))
                أرجع false;

        إذا (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                أرجع false;

        أرجع filename_part_is_valid(p);
}

bool path_is_valid_full(ثابت حرف *p, bool accept_dot_dot) {
        إذا (isempty(p))
                أرجع false;

        لكل (ثابت حرف *e = p;;) {
                صحيح r;

                r = path_find_first_component(&e, accept_dot_dot, عدم);
                إذا (r < 0)
                        أرجع false;

                إذا (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        أرجع false;
                إذا (*e == 0)           /* End of string? Yay! */
                        أرجع true;
        }
}

bool path_is_normalized(ثابت حرف *p) {
        إذا (!path_is_safe(p))
                أرجع false;

        إذا (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                أرجع false;

        إذا (strstr(p, "//"))
                أرجع false;

        أرجع true;
}

صحيح file_in_same_dir(ثابت حرف *path, ثابت حرف *filename, حرف **ret) {
        _cleanup_free_ حرف *b = عدم;
        صحيح r;

        أكد(path);
        أكد(filename);
        أكد(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        إذا (path_is_absolute(filename))
                b = strdup(filename);
        وإلا {
                _cleanup_free_ حرف *dn = عدم;

                r = path_extract_directory(path, &dn);
                إذا (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                وإلا إذا (r < 0)
                        أرجع r;
                وإلا
                        b = path_join(dn, filename);
        }
        إذا (!b)
                أرجع -ENOMEM;

        *ret = TAKE_PTR(b);
        أرجع 0;
}

bool hidden_or_backup_file(ثابت حرف *filename) {
        أكد(filename);

        إذا (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                أرجع true;

        ثابت حرف *dot = strrchr(filename, '.');
        إذا (!dot)
                أرجع false;

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

        أرجع STR_IN_SET(dot + 1,
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
        أرجع !isempty(p);
}

bool valid_device_گرہ_path(ثابت حرف *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        إذا (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                أرجع false;

        إذا (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                أرجع false;

        أرجع path_is_normalized(path);
}

bool valid_device_allow_pattern(ثابت حرف *path) {
        أكد(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        إذا (STARTSWITH_SET(path, "block-", "char-"))
                أرجع true;

        أرجع valid_device_گرہ_path(path);
}

bool dot_or_dot_dot(ثابت حرف *path) {
        إذا (!path)
                أرجع false;
        إذا (path[0] != '.')
                أرجع false;
        إذا (path[1] == 0)
                أرجع true;
        إذا (path[1] != '.')
                أرجع false;

        أرجع path[2] == 0;
}

bool path_implies_directory(ثابت حرف *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        إذا (!path)
                أرجع false;

        إذا (dot_or_dot_dot(path))
                أرجع true;

        أرجع ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_جڑ(ثابت حرف *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        إذا (isempty(path))
                أرجع true;

        أرجع path_equal(path, "/");
}

ثابت حرف* empty_to_جڑ(ثابت حرف *path) {
        أرجع isempty(path) ? "/" : path;
}

صحيح empty_or_جڑ_harder_to_null(ثابت حرف **path) {
        صحيح r;

        أكد(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        إذا (empty_or_جڑ(*path)) {
                *path = عدم;
                أرجع 0;
        }

        r = path_is_جڑ(*path);
        إذا (r < 0)
                أرجع r;
        إذا (r > 0)
                *path = عدم;

        أرجع 0;
}

bool path_strv_contains(حرف * ثابت *l, ثابت حرف *path) {
        أكد(path);

        STRV_FOREACH(i, l)
                إذا (path_equal(*i, path))
                        أرجع true;

        أرجع false;
}

bool prefixed_path_strv_contains(حرف * ثابت *l, ثابت حرف *path) {
        أكد(path);

        STRV_FOREACH(i, l) {
                ثابت حرف *j = *i;

                إذا (*j == '-')
                        j++;
                إذا (*j == '+')
                        j++;

                إذا (path_equal(j, path))
                        أرجع true;
        }

        أرجع false;
}

صحيح path_glob_can_match(ثابت حرف *pattern, ثابت حرف *prefix, حرف **ret) {
        أكد(pattern);
        أكد(prefix);

        لكل (ثابت حرف *a = pattern, *b = prefix;;) {
                _cleanup_free_ حرف *g = عدم, *h = عدم;
                ثابت حرف *p, *q;
                صحيح r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                إذا (r < 0)
                        أرجع r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                إذا (s < 0)
                        أرجع s;

                إذا (s == 0) {
                        /* The pattern matches the prefix. */
                        إذا (ret) {
                                حرف *t;

                                t = path_join(prefix, p);
                                إذا (!t)
                                        أرجع -ENOMEM;

                                *ret = t;
                        }
                        أرجع true;
                }

                إذا (r == 0)
                        اكسر;

                إذا (r == s && strneq(p, q, r))
                        استمر; /* common component. Check next. */

                g = strndup(p, r);
                إذا (!g)
                        أرجع -ENOMEM;

                إذا (!string_is_glob(g))
                        اكسر;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                إذا (!h)
                        أرجع -ENOMEM;

                r = fnmatch(g, h, 0);
                إذا (r == FNM_NOMATCH)
                        اكسر;
                إذا (r != 0) /* Failure to process pattern? */
                        أرجع -EINVAL;
        }

        /* The pattern does not match the prefix. */
        إذا (ret)
                *ret = عدم;
        أرجع false;
}

#إذا HAVE_SPLIT_BIN
ساكن bool dir_is_split(ثابت حرف *a, ثابت حرف *b) {
        صحيح r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        إذا (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                أرجع true;
        }
        أرجع r == 0;
}
#endif

ثابت حرف* default_PATH(فراغ) {
#إذا HAVE_SPLIT_BIN
        ساكن ثابت حرف *default_path = عدم;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        إذا (default_path)
                أرجع default_path;

        إذا (dir_is_split("/usr/sbin", "/usr/bin"))
                أرجع (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        إذا (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                أرجع (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        أرجع (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#وإلا
        أرجع DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
