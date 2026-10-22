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

bool is_لار(ثابت حرف *p) {
        إذا (!p) /* A NULL pointer is definitely not a path */
                أرجع false;

        أرجع strchr(p, '/');
}

صحيح لار_وېشل_and_make_absolute(ثابت حرف *p, حرف ***ret) {
        _cleanup_strv_free_ حرف **l = عدم;
        صحيح r;

        أكد(p);
        أكد(ret);

        l = strv_وېشل(p, ":");
        إذا (!l)
                أرجع -ENOMEM;

        r = لار_strv_make_absolute_cwd(l);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(l);
        أرجع r;
}

حرف* لار_make_absolute(ثابت حرف *p, ثابت حرف *مختاړی) {
        أكد(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        إذا (لار_is_absolute(p) || isempty(مختاړی))
                أرجع strdup(p);

        أرجع لار_join(مختاړی, p);
}

صحيح خوندي_getcwd(حرف **ret) {
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

صحيح لار_make_absolute_cwd(ثابت حرف *p, حرف **ret) {
        حرف *c;
        صحيح r;

        أكد(p);
        أكد(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        إذا (لار_is_absolute(p))
                c = strdup(p);
        وإلا {
                _cleanup_free_ حرف *cwd = عدم;

                r = خوندي_getcwd(&cwd);
                إذا (r < 0)
                        أرجع r;

                c = لار_join(cwd, p);
        }
        إذا (!c)
                أرجع -ENOMEM;

        *ret = c;
        أرجع 0;
}

صحيح لار_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret) {
        _cleanup_free_ حرف *پایله = عدم;
        بینښه n_parents;
        ثابت حرف *f, *t;
        صحيح r, k;
        حرف *p;

        أكد(from);
        أكد(to);
        أكد(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        إذا (!لار_is_absolute(from) || !لار_is_absolute(to))
                أرجع -EINVAL;

        لكل (;;) {
                r = لار_پیدا_کړه_لومړی_component(&from, true, &f);
                إذا (r < 0)
                        أرجع r;

                k = لار_پیدا_کړه_لومړی_component(&to, true, &t);
                إذا (k < 0)
                        أرجع k;

                إذا (r == 0) {
                        /* end of 'from' */
                        إذا (k == 0) {
                                /* from and to are equivalent. */
                                پایله = strdup(".");
                                إذا (!پایله)
                                        أرجع -ENOMEM;
                        } وإلا {
                                /* 'to' is inside of 'from'. */
                                r = لار_simplify_alloc(t, &پایله);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (!لار_is_valid(پایله))
                                        أرجع -EINVAL;
                        }

                        *ret = TAKE_PTR(پایله);
                        أرجع 0;
                }

                إذا (r != k || !strneq(f, t, r))
                        اكسر;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        لكل (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = لار_پیدا_کړه_لومړی_component(&from, false, &f);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;
        }

        إذا (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                أرجع -EINVAL;

        پایله = new(حرف, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        إذا (!پایله)
                أرجع -ENOMEM;

        لكل (p = پایله; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        إذا (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(پایله);
                أرجع 0;
        }

        strcpy(p, t);

        لار_simplify(پایله);

        إذا (!لار_is_valid(پایله))
                أرجع -EINVAL;

        *ret = TAKE_PTR(پایله);
        أرجع 0;
}

صحيح لار_make_relative_پلار(ثابت حرف *from_ماشوم, ثابت حرف *to, حرف **ret) {
        _cleanup_free_ حرف *from = عدم;
        صحيح r;

        أكد(from_ماشوم);
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

        r = لار_ایستل_directory(from_ماشوم, &from);
        إذا (r < 0)
                أرجع r;

        أرجع لار_make_relative(from, to, ret);
}

حرف* لار_startswith_strv(ثابت حرف *p, حرف * ثابت *strv) {
        أكد(p);

        STRV_FOREACH(s, strv) {
                حرف *t;

                t = لار_startswith(p, *s);
                إذا (t)
                        أرجع t;
        }

        أرجع عدم;
}

صحيح لار_strv_make_absolute_cwd(حرف **l) {
        صحيح r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                حرف *t;

                r = لار_make_absolute_cwd(*s, &t);
                إذا (r < 0)
                        أرجع r;

                لار_simplify(t);
                free_and_replace(*s, t);
        }

        أرجع 0;
}

حرف** لار_strv_resolve(حرف **l, ثابت حرف *ریښه) {
        بینښه k = 0;
        bool enomem = false;
        صحيح r;

        إذا (strv_isempty(l))
                أرجع l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ حرف *orig = عدم;
                حرف *t, *u;

                إذا (ریښه) {
                        إذا (!لار_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                حرر(*s);
                                استمر;
                        }

                        orig = *s;
                        t = لار_join(ریښه, orig);
                        إذا (!t) {
                                enomem = true;
                                استمر;
                        }
                } وإلا
                        t = *s;

                r = chase(t, ریښه, 0, &u, عدم);
                إذا (r == -ENOENT) {
                        إذا (ریښه) {
                                u = TAKE_PTR(orig);
                                حرر(t);
                        } وإلا
                                u = t;
                } وإلا إذا (r < 0) {
                        حرر(t);

                        إذا (r == -ENOMEM)
                                enomem = true;

                        استمر;
                } وإلا إذا (ریښه) {
                        حرف *x;

                        حرر(t);
                        x = لار_startswith(u, ریښه);
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

حرف** لار_strv_resolve_uniq(حرف **l, ثابت حرف *ریښه) {

        إذا (strv_isempty(l))
                أرجع l;

        إذا (!لار_strv_resolve(l, ریښه))
                أرجع عدم;

        أرجع strv_uniq(l);
}

حرف* skip_leading_slash(ثابت حرف *p) {
        أرجع skip_leading_chars(p, "/");
}

حرف* لار_simplify_ډک(حرف *لار, لار_Simplify_Flags flags) {
        bool جمع_کړه_slash = false, keep_trailing_slash, absolute, beginning = true;
        حرف *f = لار;
        صحيح r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        إذا (isempty(لار))
                أرجع لار;

        keep_trailing_slash = FLAGS_سېټ(flags, لار_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(لار, "/");

        absolute = لار_is_absolute(لار);
        f += absolute;  /* Keep leading /, if present. */

        لكل (ثابت حرف *p = f;;) {
                ثابت حرف *e;

                r = لار_پیدا_کړه_لومړی_component(&p, true, &e);
                إذا (r == 0)
                        اكسر;

                إذا (r > 0 && absolute && beginning && لار_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        استمر;

                beginning = false;

                إذا (جمع_کړه_slash)
                        *f++ = '/';

                إذا (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, مزي_اوږدوالی(p) + 1);
                        أرجع لار;
                }

                memmove(f, e, r);
                f += r;

                جمع_کړه_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        إذا (f == لار)
                *f++ = '.';

        إذا (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        أرجع لار;
}

صحيح لار_simplify_alloc(ثابت حرف *لار, حرف **ret) {
        أكد(ret);

        إذا (!لار) {
                *ret = عدم;
                أرجع 0;
        }

        حرف *t = strdup(لار);
        إذا (!t)
                أرجع -ENOMEM;

        *ret = لار_simplify(t);
        أرجع 0;
}

حرف* لار_startswith_ډک(ثابت حرف *original_لار, ثابت حرف *مختاړی, لار_پیل_With_Flags flags) {
        أكد(original_لار);
        أكد(مختاړی);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ثابت حرف *لار = original_لار;

        إذا ((لار[0] == '/') != (مختاړی[0] == '/'))
                أرجع عدم;

        لكل (;;) {
                ثابت حرف *p, *q;
                صحيح m, n;

                m = لار_پیدا_کړه_لومړی_component(&لار, !FLAGS_سېټ(flags, لار_STARTSWITH_REFUSE_DOT_DOT), &p);
                إذا (m < 0)
                        أرجع عدم;

                n = لار_پیدا_کړه_لومړی_component(&مختاړی, !FLAGS_سېټ(flags, لار_STARTSWITH_REFUSE_DOT_DOT), &q);
                إذا (n < 0)
                        أرجع عدم;

                إذا (n == 0) {
                        إذا (!p)
                                p = لار;

                        إذا (FLAGS_سېټ(flags, لار_STARTSWITH_RETURN_LEADING_SLASH)) {

                                إذا (p <= original_لار)
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

صحيح لار_compare(ثابت حرف *a, ثابت حرف *b) {
        صحيح r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        إذا (r != 0)
                أرجع r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(لار_is_absolute(a), لار_is_absolute(b));
        إذا (r != 0)
                أرجع r;

        لكل (;;) {
                ثابت حرف *aa, *bb;
                صحيح j, k;

                j = لار_پیدا_کړه_لومړی_component(&a, true, &aa);
                k = لار_پیدا_کړه_لومړی_component(&b, true, &bb);

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

صحيح لار_compare_filename(ثابت حرف *a, ثابت حرف *b) {
        _cleanup_free_ حرف *fa = عدم, *fb = عدم;
        صحيح r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        إذا (r != 0)
                أرجع r;

        j = لار_ایستل_filename(a, &fa);
        k = لار_ایستل_filename(b, &fb);

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

صحيح لار_equal_or_inode_same_ډک(ثابت حرف *a, ثابت حرف *b, صحيح flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        إذا (لار_equal(a, b))
                أرجع 1;

        إذا (!a || !b)
                أرجع 0;

        أرجع inode_same(a, b, flags);
}

حرف* لار_extend_دننی(حرف **x, ...) {
        size_t sz, old_sz;
        حرف *q, *nx;
        ثابت حرف *p;
        va_list ap;
        bool slash;

        اشاره_ګر_MAY_BE_NULL(x);

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
        طالما ((p = va_arg(ap, حرف*)) != اشاره_ګر_MAX) {
                size_t جمع_کړه;

                إذا (isempty(p))
                        استمر;

                جمع_کړه = 1 + مزي_اوږدوالی(p);
                إذا (sz > SIZE_MAX - جمع_کړه) { /* overflow check */
                        va_end(ap);
                        أرجع عدم;
                }

                sz += جمع_کړه;
        }
        va_end(ap);

        nx = realloc(x ? *x : عدم, GREEDY_ALLOC_پړاو_پورته(sz+1));
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
        طالما ((p = va_arg(ap, حرف*)) != اشاره_ګر_MAX) {
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

صحيح open_and_check_executable(ثابت حرف *name, ثابت حرف *ریښه, حرف **ret_لار, صحيح *ret_fd) {
        _cleanup_close_ صحيح fd = -EBADF;
        _cleanup_free_ حرف *resolved = عدم;
        صحيح r;

        أكد(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        إذا (ریښه) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, ریښه, CHASE_مختاړی_ریښه, &resolved, &fd);
                إذا (r < 0)
                        أرجع r;

                name = resolved;
        } وإلا {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_لار|O_CLOEXEC);
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

        إذا (ret_لار) {
                إذا (resolved)
                        *ret_لار = TAKE_PTR(resolved);
                وإلا {
                        r = لار_make_absolute_cwd(name, ret_لار);
                        إذا (r < 0)
                                أرجع r;

                        لار_simplify(*ret_لار);
                }
        }

        إذا (ret_fd)
                *ret_fd = TAKE_FD(fd);

        أرجع 0;
}

صحيح پیدا_کړه_executable_ډک(
                ثابت حرف *name,
                ثابت حرف *ریښه,
                حرف * ثابت *exec_پلټنه_لار,
                bool use_لار_envvar,
                حرف **ret_filename,
                صحيح *ret_fd) {

        صحيح وروستی_error = -ENOENT, r = 0;

        أكد(name);

        إذا (is_لار(name))
                أرجع open_and_check_executable(name, ریښه, ret_filename, ret_fd);

        إذا (exec_پلټنه_لار) {
                STRV_FOREACH(element, exec_پلټنه_لار) {
                        _cleanup_free_ حرف *ډک_لار = عدم;

                        إذا (!لار_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                استمر;
                        }

                        ډک_لار = لار_join(*element, name);
                        إذا (!ډک_لار)
                                أرجع -ENOMEM;

                        r = open_and_check_executable(ډک_لار, ریښه, ret_filename, ret_fd);
                        إذا (r >= 0)
                                أرجع 0;
                        إذا (r != -EACCES)
                                وروستی_error = r;
                }
                أرجع وروستی_error;
        }

        ثابت حرف *p = عدم;

        إذا (use_لار_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        إذا (!p)
                p = default_لار();

        /* Resolve a single-component name to a full path */
        لكل (;;) {
                _cleanup_free_ حرف *element = عدم;

                r = ایستل_لومړی_کلمه(&p, &element, ":", ایستل_RELAX|ایستل_DONT_COALESCE_SEPARATORS);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                إذا (!لار_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        استمر;
                }

                إذا (!لار_extend(&element, name))
                        أرجع -ENOMEM;

                r = open_and_check_executable(element, ریښه, ret_filename, ret_fd);
                إذا (r >= 0) /* Found it! */
                        أرجع 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                إذا (r != -EACCES)
                        وروستی_error = r;
        }

        أرجع وروستی_error;
}

ساكن صحيح executable_is_good(ثابت حرف *executable) {
        _cleanup_free_ حرف *p = عدم, *d = عدم;
        صحيح r;

        r = پیدا_کړه_executable(executable, &p);
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

        أرجع !لار_دننه_سېټ(d, "true",
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
        اشاره_ګر_MAY_BE_NULL(p);

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

صحيح لار_پیدا_کړه_لومړی_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret) {
        ثابت حرف *q, *لومړی, *پای_ټکی_لومړی, *بل_2;
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

        لومړی = skip_slash_or_dot(q);
        إذا (isempty(لومړی)) {
                *p = لومړی;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }
        إذا (streq(لومړی, ".")) {
                *p = لومړی + 1;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }

        پای_ټکی_لومړی = strchrnul(لومړی, '/');
        len = پای_ټکی_لومړی - لومړی;

        إذا (len > NAME_MAX)
                أرجع -EINVAL;
        إذا (!accept_dot_dot && len == 2 && لومړی[0] == '.' && لومړی[1] == '.')
                أرجع -EINVAL;

        بل_2 = skip_slash_or_dot(پای_ټکی_لومړی);

        *p = بل_2 + streq(بل_2, ".");
        إذا (ret)
                *ret = لومړی;
        أرجع len;
}

ساكن ثابت حرف* skip_slash_or_dot_backward(ثابت حرف *لار, ثابت حرف *q) {
        أكد(لار);
        أكد(!q || q >= لار);

        لكل (; q; q = PTR_SUB1(q, لار)) {
                إذا (*q == '/')
                        استمر;
                إذا (q > لار && strneq(q - 1, "/.", 2))
                        استمر;
                إذا (q == لار && *q == '.')
                        استمر;
                اكسر;
        }
        أرجع q;
}

صحيح لار_پیدا_کړه_وروستی_component(ثابت حرف *لار, bool accept_dot_dot, ثابت حرف **بل_2, ثابت حرف **ret) {
        ثابت حرف *q, *وروستی_پای_ټکی, *وروستی_begin;
        size_t len;

        اشاره_ګر_MAY_BE_NULL(بل_2);
        اشاره_ګر_MAY_BE_NULL(ret);

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

        إذا (isempty(لار)) {
                إذا (بل_2)
                        *بل_2 = لار;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }

        إذا (بل_2 && *بل_2) {
                إذا (*بل_2 < لار || *بل_2 > لار + مزي_اوږدوالی(لار))
                        أرجع -EINVAL;
                إذا (*بل_2 == لار) {
                        إذا (ret)
                                *ret = عدم;
                        أرجع 0;
                }
                إذا (!دننه_سېټ(**بل_2, '\0', '/'))
                        أرجع -EINVAL;
                q = *بل_2 - 1;
        } وإلا
                q = لار + مزي_اوږدوالی(لار) - 1;

        q = skip_slash_or_dot_backward(لار, q);
        إذا (!q) { /* the root directory, "." or "./" */
                إذا (بل_2)
                        *بل_2 = لار;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }

        وروستی_پای_ټکی = q + 1;

        طالما (q && *q != '/')
                q = PTR_SUB1(q, لار);

        وروستی_begin = q ? q + 1 : لار;
        len = وروستی_پای_ټکی - وروستی_begin;

        إذا (len > NAME_MAX)
                أرجع -EINVAL;
        إذا (!accept_dot_dot && len == 2 && strneq(وروستی_begin, "..", 2))
                أرجع -EINVAL;

        إذا (بل_2) {
                q = skip_slash_or_dot_backward(لار, q);
                *بل_2 = q ? q + 1 : لار;
        }

        إذا (ret)
                *ret = وروستی_begin;
        أرجع len;
}

ثابت حرف* وروستی_لار_component(ثابت حرف *لار) {

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

        بینښه l, k;

        إذا (!لار)
                أرجع عدم;

        l = k = مزي_اوږدوالی(لار);
        إذا (l == 0) /* special case — an empty string */
                أرجع لار;

        طالما (k > 0 && لار[k-1] == '/')
                k--;

        إذا (k == 0) /* the root directory */
                أرجع لار + l - 1;

        طالما (k > 0 && لار[k-1] != '/')
                k--;

        أرجع لار + k;
}

صحيح لار_وېشل_مختاړی_filename(ثابت حرف *لار, حرف **ret_dir, حرف **ret_filename) {
        _cleanup_free_ حرف *d = عدم;
        ثابت حرف *c, *بل_2 = عدم;
        صحيح r;

        اشاره_ګر_MAY_BE_NULL(لار);

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

        إذا (isempty(لار))
                أرجع -EINVAL;

        r = لار_پیدا_کړه_وروستی_component(لار, /* accept_dot_dot = */ false, &بل_2, &c);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) /* root directory or "." */
                أرجع -EADDRNOTAVAIL;

        إذا (ret_dir) {
                إذا (بل_2 == لار) {
                        إذا (*لار != '/') { /* filename only */
                                إذا (!ret_filename)
                                        أرجع -EDESTADDRREQ;
                        } وإلا {
                                d = strdup("/");
                                إذا (!d)
                                        أرجع -ENOMEM;
                        }
                } وإلا {
                        d = strndup(لار, بل_2 - لار);
                        إذا (!d)
                                أرجع -ENOMEM;

                        لار_simplify(d);

                        إذا (!لار_is_valid(d))
                                أرجع -EINVAL;
                }

        } وإلا إذا (!لار_is_valid(لار))
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

        أرجع مزي_اوږدوالی(c) > (size_t) r ? O_DIRECTORY : 0;
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

bool لار_is_valid_ډک(ثابت حرف *p, bool accept_dot_dot) {
        إذا (isempty(p))
                أرجع false;

        لكل (ثابت حرف *e = p;;) {
                صحيح r;

                r = لار_پیدا_کړه_لومړی_component(&e, accept_dot_dot, عدم);
                إذا (r < 0)
                        أرجع false;

                إذا (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        أرجع false;
                إذا (*e == 0)           /* End of string? Yay! */
                        أرجع true;
        }
}

bool لار_is_normalized(ثابت حرف *p) {
        إذا (!لار_is_خوندي(p))
                أرجع false;

        إذا (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || مزي_پلټنه(p, "/./"))
                أرجع false;

        إذا (مزي_پلټنه(p, "//"))
                أرجع false;

        أرجع true;
}

صحيح file_دننه_same_dir(ثابت حرف *لار, ثابت حرف *filename, حرف **ret) {
        _cleanup_free_ حرف *b = عدم;
        صحيح r;

        أكد(لار);
        أكد(filename);
        أكد(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        إذا (لار_is_absolute(filename))
                b = strdup(filename);
        وإلا {
                _cleanup_free_ حرف *dn = عدم;

                r = لار_ایستل_directory(لار, &dn);
                إذا (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                وإلا إذا (r < 0)
                        أرجع r;
                وإلا
                        b = لار_join(dn, filename);
        }
        إذا (!b)
                أرجع -ENOMEM;

        *ret = TAKE_PTR(b);
        أرجع 0;
}

bool hidden_or_backup_file(ثابت حرف *filename) {
        أكد(filename);

        إذا (filename[0] == '.' ||
            STR_دننه_سېټ(filename,
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

        أرجع STR_دننه_سېټ(dot + 1,
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

bool is_device_لار(ثابت حرف *لار) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ثابت حرف *p = لار_STARTSWITH_سېټ(ASSERT_PTR(لار), "/dev/", "/sys/");
        أرجع !isempty(p);
}

bool valid_device_غوټه_لار(ثابت حرف *لار) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        إذا (!لار_STARTSWITH_سېټ(لار, "/dev/", "/run/systemd/inaccessible/"))
                أرجع false;

        إذا (endswith(لار, "/")) /* can't be a device node if it ends in a slash */
                أرجع false;

        أرجع لار_is_normalized(لار);
}

bool valid_device_allow_نمونه(ثابت حرف *لار) {
        أكد(لار);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        إذا (STARTSWITH_سېټ(لار, "block-", "char-"))
                أرجع true;

        أرجع valid_device_غوټه_لار(لار);
}

bool dot_or_dot_dot(ثابت حرف *لار) {
        إذا (!لار)
                أرجع false;
        إذا (لار[0] != '.')
                أرجع false;
        إذا (لار[1] == 0)
                أرجع true;
        إذا (لار[1] != '.')
                أرجع false;

        أرجع لار[2] == 0;
}

bool لار_پایله_ورکوي_directory(ثابت حرف *لار) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        إذا (!لار)
                أرجع false;

        إذا (dot_or_dot_dot(لار))
                أرجع true;

        أرجع ENDSWITH_سېټ(لار, "/", "/.", "/..");
}

bool تشه_or_ریښه(ثابت حرف *لار) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        إذا (isempty(لار))
                أرجع true;

        أرجع لار_equal(لار, "/");
}

ثابت حرف* تشه_منزل_ریښه(ثابت حرف *لار) {
        أرجع isempty(لار) ? "/" : لار;
}

صحيح تشه_or_ریښه_harder_منزل_null(ثابت حرف **لار) {
        صحيح r;

        أكد(لار);

        /* This nullifies the input path when the path is empty or points to "/". */

        إذا (تشه_or_ریښه(*لار)) {
                *لار = عدم;
                أرجع 0;
        }

        r = لار_is_ریښه(*لار);
        إذا (r < 0)
                أرجع r;
        إذا (r > 0)
                *لار = عدم;

        أرجع 0;
}

bool لار_strv_لري(حرف * ثابت *l, ثابت حرف *لار) {
        أكد(لار);

        STRV_FOREACH(i, l)
                إذا (لار_equal(*i, لار))
                        أرجع true;

        أرجع false;
}

bool prefixed_لار_strv_لري(حرف * ثابت *l, ثابت حرف *لار) {
        أكد(لار);

        STRV_FOREACH(i, l) {
                ثابت حرف *j = *i;

                إذا (*j == '-')
                        j++;
                إذا (*j == '+')
                        j++;

                إذا (لار_equal(j, لار))
                        أرجع true;
        }

        أرجع false;
}

صحيح لار_glob_can_سمون(ثابت حرف *نمونه, ثابت حرف *مختاړی, حرف **ret) {
        أكد(نمونه);
        أكد(مختاړی);

        لكل (ثابت حرف *a = نمونه, *b = مختاړی;;) {
                _cleanup_free_ حرف *g = عدم, *h = عدم;
                ثابت حرف *p, *q;
                صحيح r, s;

                r = لار_پیدا_کړه_لومړی_component(&a, /* accept_dot_dot= */ false, &p);
                إذا (r < 0)
                        أرجع r;

                s = لار_پیدا_کړه_لومړی_component(&b, /* accept_dot_dot= */ false, &q);
                إذا (s < 0)
                        أرجع s;

                إذا (s == 0) {
                        /* The pattern matches the prefix. */
                        إذا (ret) {
                                حرف *t;

                                t = لار_join(مختاړی, p);
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

#إذا HAVE_وېشل_BIN
ساكن bool dir_is_وېشل(ثابت حرف *a, ثابت حرف *b) {
        صحيح r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        إذا (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                أرجع true;
        }
        أرجع r == 0;
}
#د_که_پای

ثابت حرف* default_لار(فراغ) {
#إذا HAVE_وېشل_BIN
        ساكن ثابت حرف *default_لار_2 = عدم;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        إذا (default_لار_2)
                أرجع default_لار_2;

        إذا (dir_is_وېشل("/usr/sbin", "/usr/bin"))
                أرجع (default_لار_2 = DEFAULT_لار_WITH_ډک_SBIN);  /* a */
        إذا (dir_is_وېشل("/usr/local/sbin", "/usr/local/bin"))
                أرجع (default_لار_2 = DEFAULT_لار_WITH_LOCAL_SBIN); /* b */
        أرجع (default_لار_2 = DEFAULT_لار_WITHOUT_SBIN);            /* c */
#وإلا
        أرجع DEFAULT_لار_WITHOUT_SBIN;
#د_که_پای
}
