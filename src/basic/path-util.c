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

bool is_رستہ(ثابت حرف *p) {
        إذا (!p) /* A NULL pointer is definitely not a path */
                أرجع false;

        أرجع strchr(p, '/');
}

صحيح رستہ_ونڈو_and_make_absolute(ثابت حرف *p, حرف ***ret) {
        _cleanup_strv_free_ حرف **l = عدم;
        صحيح r;

        أكد(p);
        أكد(ret);

        l = strv_ونڈو(p, ":");
        إذا (!l)
                أرجع -ENOMEM;

        r = رستہ_strv_make_absolute_cwd(l);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(l);
        أرجع r;
}

حرف* رستہ_make_absolute(ثابت حرف *p, ثابت حرف *اگیتر) {
        أكد(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        إذا (رستہ_is_absolute(p) || isempty(اگیتر))
                أرجع strdup(p);

        أرجع رستہ_join(اگیتر, p);
}

صحيح محفوظ_getcwd(حرف **ret) {
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

صحيح رستہ_make_absolute_cwd(ثابت حرف *p, حرف **ret) {
        حرف *c;
        صحيح r;

        أكد(p);
        أكد(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        إذا (رستہ_is_absolute(p))
                c = strdup(p);
        وإلا {
                _cleanup_free_ حرف *cwd = عدم;

                r = محفوظ_getcwd(&cwd);
                إذا (r < 0)
                        أرجع r;

                c = رستہ_join(cwd, p);
        }
        إذا (!c)
                أرجع -ENOMEM;

        *ret = c;
        أرجع 0;
}

صحيح رستہ_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret) {
        _cleanup_free_ حرف *نتیجہ = عدم;
        بنانشانی n_parents;
        ثابت حرف *f, *t;
        صحيح r, k;
        حرف *p;

        أكد(from);
        أكد(to);
        أكد(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        إذا (!رستہ_is_absolute(from) || !رستہ_is_absolute(to))
                أرجع -EINVAL;

        لكل (;;) {
                r = رستہ_لبھو_پہلا_component(&from, true, &f);
                إذا (r < 0)
                        أرجع r;

                k = رستہ_لبھو_پہلا_component(&to, true, &t);
                إذا (k < 0)
                        أرجع k;

                إذا (r == 0) {
                        /* end of 'from' */
                        إذا (k == 0) {
                                /* from and to are equivalent. */
                                نتیجہ = strdup(".");
                                إذا (!نتیجہ)
                                        أرجع -ENOMEM;
                        } وإلا {
                                /* 'to' is inside of 'from'. */
                                r = رستہ_simplify_alloc(t, &نتیجہ);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (!رستہ_is_valid(نتیجہ))
                                        أرجع -EINVAL;
                        }

                        *ret = TAKE_PTR(نتیجہ);
                        أرجع 0;
                }

                إذا (r != k || !strneq(f, t, r))
                        اكسر;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        لكل (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = رستہ_لبھو_پہلا_component(&from, false, &f);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;
        }

        إذا (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                أرجع -EINVAL;

        نتیجہ = new(حرف, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        إذا (!نتیجہ)
                أرجع -ENOMEM;

        لكل (p = نتیجہ; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        إذا (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(نتیجہ);
                أرجع 0;
        }

        strcpy(p, t);

        رستہ_simplify(نتیجہ);

        إذا (!رستہ_is_valid(نتیجہ))
                أرجع -EINVAL;

        *ret = TAKE_PTR(نتیجہ);
        أرجع 0;
}

صحيح رستہ_make_relative_ماپا(ثابت حرف *from_بچہ, ثابت حرف *to, حرف **ret) {
        _cleanup_free_ حرف *from = عدم;
        صحيح r;

        أكد(from_بچہ);
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

        r = رستہ_کڈھو_directory(from_بچہ, &from);
        إذا (r < 0)
                أرجع r;

        أرجع رستہ_make_relative(from, to, ret);
}

حرف* رستہ_startswith_strv(ثابت حرف *p, حرف * ثابت *strv) {
        أكد(p);

        STRV_FOREACH(s, strv) {
                حرف *t;

                t = رستہ_startswith(p, *s);
                إذا (t)
                        أرجع t;
        }

        أرجع عدم;
}

صحيح رستہ_strv_make_absolute_cwd(حرف **l) {
        صحيح r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                حرف *t;

                r = رستہ_make_absolute_cwd(*s, &t);
                إذا (r < 0)
                        أرجع r;

                رستہ_simplify(t);
                free_and_replace(*s, t);
        }

        أرجع 0;
}

حرف** رستہ_strv_resolve(حرف **l, ثابت حرف *جڑ) {
        بنانشانی k = 0;
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
                        إذا (!رستہ_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                حرر(*s);
                                استمر;
                        }

                        orig = *s;
                        t = رستہ_join(جڑ, orig);
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
                        x = رستہ_startswith(u, جڑ);
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

حرف** رستہ_strv_resolve_uniq(حرف **l, ثابت حرف *جڑ) {

        إذا (strv_isempty(l))
                أرجع l;

        إذا (!رستہ_strv_resolve(l, جڑ))
                أرجع عدم;

        أرجع strv_uniq(l);
}

حرف* skip_leading_slash(ثابت حرف *p) {
        أرجع skip_leading_chars(p, "/");
}

حرف* رستہ_simplify_بھریا(حرف *رستہ, رستہ_Simplify_Flags flags) {
        bool جوڑو_slash = false, keep_trailing_slash, absolute, beginning = true;
        حرف *f = رستہ;
        صحيح r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        إذا (isempty(رستہ))
                أرجع رستہ;

        keep_trailing_slash = FLAGS_سیٹ(flags, رستہ_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(رستہ, "/");

        absolute = رستہ_is_absolute(رستہ);
        f += absolute;  /* Keep leading /, if present. */

        لكل (ثابت حرف *p = f;;) {
                ثابت حرف *e;

                r = رستہ_لبھو_پہلا_component(&p, true, &e);
                إذا (r == 0)
                        اكسر;

                إذا (r > 0 && absolute && beginning && رستہ_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        استمر;

                beginning = false;

                إذا (جوڑو_slash)
                        *f++ = '/';

                إذا (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, لڑی_لمبائی(p) + 1);
                        أرجع رستہ;
                }

                memmove(f, e, r);
                f += r;

                جوڑو_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        إذا (f == رستہ)
                *f++ = '.';

        إذا (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        أرجع رستہ;
}

صحيح رستہ_simplify_alloc(ثابت حرف *رستہ, حرف **ret) {
        أكد(ret);

        إذا (!رستہ) {
                *ret = عدم;
                أرجع 0;
        }

        حرف *t = strdup(رستہ);
        إذا (!t)
                أرجع -ENOMEM;

        *ret = رستہ_simplify(t);
        أرجع 0;
}

حرف* رستہ_startswith_بھریا(ثابت حرف *original_رستہ, ثابت حرف *اگیتر, رستہ_شروع_With_Flags flags) {
        أكد(original_رستہ);
        أكد(اگیتر);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ثابت حرف *رستہ = original_رستہ;

        إذا ((رستہ[0] == '/') != (اگیتر[0] == '/'))
                أرجع عدم;

        لكل (;;) {
                ثابت حرف *p, *q;
                صحيح m, n;

                m = رستہ_لبھو_پہلا_component(&رستہ, !FLAGS_سیٹ(flags, رستہ_STARTSWITH_REFUSE_DOT_DOT), &p);
                إذا (m < 0)
                        أرجع عدم;

                n = رستہ_لبھو_پہلا_component(&اگیتر, !FLAGS_سیٹ(flags, رستہ_STARTSWITH_REFUSE_DOT_DOT), &q);
                إذا (n < 0)
                        أرجع عدم;

                إذا (n == 0) {
                        إذا (!p)
                                p = رستہ;

                        إذا (FLAGS_سیٹ(flags, رستہ_STARTSWITH_RETURN_LEADING_SLASH)) {

                                إذا (p <= original_رستہ)
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

صحيح رستہ_compare(ثابت حرف *a, ثابت حرف *b) {
        صحيح r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        إذا (r != 0)
                أرجع r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(رستہ_is_absolute(a), رستہ_is_absolute(b));
        إذا (r != 0)
                أرجع r;

        لكل (;;) {
                ثابت حرف *aa, *bb;
                صحيح j, k;

                j = رستہ_لبھو_پہلا_component(&a, true, &aa);
                k = رستہ_لبھو_پہلا_component(&b, true, &bb);

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

صحيح رستہ_compare_filename(ثابت حرف *a, ثابت حرف *b) {
        _cleanup_free_ حرف *fa = عدم, *fb = عدم;
        صحيح r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        إذا (r != 0)
                أرجع r;

        j = رستہ_کڈھو_filename(a, &fa);
        k = رستہ_کڈھو_filename(b, &fb);

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

صحيح رستہ_equal_or_inode_same_بھریا(ثابت حرف *a, ثابت حرف *b, صحيح flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        إذا (رستہ_equal(a, b))
                أرجع 1;

        إذا (!a || !b)
                أرجع 0;

        أرجع inode_same(a, b, flags);
}

حرف* رستہ_extend_اندرونی(حرف **x, ...) {
        size_t sz, old_sz;
        حرف *q, *nx;
        ثابت حرف *p;
        va_list ap;
        bool slash;

        اشارہ_گر_MAY_BE_NULL(x);

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
        طالما ((p = va_arg(ap, حرف*)) != اشارہ_گر_MAX) {
                size_t جوڑو;

                إذا (isempty(p))
                        استمر;

                جوڑو = 1 + لڑی_لمبائی(p);
                إذا (sz > SIZE_MAX - جوڑو) { /* overflow check */
                        va_end(ap);
                        أرجع عدم;
                }

                sz += جوڑو;
        }
        va_end(ap);

        nx = realloc(x ? *x : عدم, GREEDY_ALLOC_گیڑا_اتے(sz+1));
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
        طالما ((p = va_arg(ap, حرف*)) != اشارہ_گر_MAX) {
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

صحيح open_and_check_executable(ثابت حرف *name, ثابت حرف *جڑ, حرف **ret_رستہ, صحيح *ret_fd) {
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
                r = chase(name, جڑ, CHASE_اگیتر_جڑ, &resolved, &fd);
                إذا (r < 0)
                        أرجع r;

                name = resolved;
        } وإلا {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_رستہ|O_CLOEXEC);
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

        إذا (ret_رستہ) {
                إذا (resolved)
                        *ret_رستہ = TAKE_PTR(resolved);
                وإلا {
                        r = رستہ_make_absolute_cwd(name, ret_رستہ);
                        إذا (r < 0)
                                أرجع r;

                        رستہ_simplify(*ret_رستہ);
                }
        }

        إذا (ret_fd)
                *ret_fd = TAKE_FD(fd);

        أرجع 0;
}

صحيح لبھو_executable_بھریا(
                ثابت حرف *name,
                ثابت حرف *جڑ,
                حرف * ثابت *exec_تلاش_رستہ,
                bool use_رستہ_envvar,
                حرف **ret_filename,
                صحيح *ret_fd) {

        صحيح آخری_error = -ENOENT, r = 0;

        أكد(name);

        إذا (is_رستہ(name))
                أرجع open_and_check_executable(name, جڑ, ret_filename, ret_fd);

        إذا (exec_تلاش_رستہ) {
                STRV_FOREACH(element, exec_تلاش_رستہ) {
                        _cleanup_free_ حرف *بھریا_رستہ = عدم;

                        إذا (!رستہ_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                استمر;
                        }

                        بھریا_رستہ = رستہ_join(*element, name);
                        إذا (!بھریا_رستہ)
                                أرجع -ENOMEM;

                        r = open_and_check_executable(بھریا_رستہ, جڑ, ret_filename, ret_fd);
                        إذا (r >= 0)
                                أرجع 0;
                        إذا (r != -EACCES)
                                آخری_error = r;
                }
                أرجع آخری_error;
        }

        ثابت حرف *p = عدم;

        إذا (use_رستہ_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        إذا (!p)
                p = default_رستہ();

        /* Resolve a single-component name to a full path */
        لكل (;;) {
                _cleanup_free_ حرف *element = عدم;

                r = کڈھو_پہلا_لفظ(&p, &element, ":", کڈھو_RELAX|کڈھو_DONT_COALESCE_SEPARATORS);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                إذا (!رستہ_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        استمر;
                }

                إذا (!رستہ_extend(&element, name))
                        أرجع -ENOMEM;

                r = open_and_check_executable(element, جڑ, ret_filename, ret_fd);
                إذا (r >= 0) /* Found it! */
                        أرجع 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                إذا (r != -EACCES)
                        آخری_error = r;
        }

        أرجع آخری_error;
}

ساكن صحيح executable_is_good(ثابت حرف *executable) {
        _cleanup_free_ حرف *p = عدم, *d = عدم;
        صحيح r;

        r = لبھو_executable(executable, &p);
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

        أرجع !رستہ_وچ_سیٹ(d, "true",
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
        اشارہ_گر_MAY_BE_NULL(p);

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

صحيح رستہ_لبھو_پہلا_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret) {
        ثابت حرف *q, *پہلا, *سرا_پہلا, *اگلا_2;
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

        پہلا = skip_slash_or_dot(q);
        إذا (isempty(پہلا)) {
                *p = پہلا;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }
        إذا (streq(پہلا, ".")) {
                *p = پہلا + 1;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }

        سرا_پہلا = strchrnul(پہلا, '/');
        len = سرا_پہلا - پہلا;

        إذا (len > NAME_MAX)
                أرجع -EINVAL;
        إذا (!accept_dot_dot && len == 2 && پہلا[0] == '.' && پہلا[1] == '.')
                أرجع -EINVAL;

        اگلا_2 = skip_slash_or_dot(سرا_پہلا);

        *p = اگلا_2 + streq(اگلا_2, ".");
        إذا (ret)
                *ret = پہلا;
        أرجع len;
}

ساكن ثابت حرف* skip_slash_or_dot_backward(ثابت حرف *رستہ, ثابت حرف *q) {
        أكد(رستہ);
        أكد(!q || q >= رستہ);

        لكل (; q; q = PTR_SUB1(q, رستہ)) {
                إذا (*q == '/')
                        استمر;
                إذا (q > رستہ && strneq(q - 1, "/.", 2))
                        استمر;
                إذا (q == رستہ && *q == '.')
                        استمر;
                اكسر;
        }
        أرجع q;
}

صحيح رستہ_لبھو_آخری_component(ثابت حرف *رستہ, bool accept_dot_dot, ثابت حرف **اگلا_2, ثابت حرف **ret) {
        ثابت حرف *q, *آخری_سرا, *آخری_begin;
        size_t len;

        اشارہ_گر_MAY_BE_NULL(اگلا_2);
        اشارہ_گر_MAY_BE_NULL(ret);

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

        إذا (isempty(رستہ)) {
                إذا (اگلا_2)
                        *اگلا_2 = رستہ;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }

        إذا (اگلا_2 && *اگلا_2) {
                إذا (*اگلا_2 < رستہ || *اگلا_2 > رستہ + لڑی_لمبائی(رستہ))
                        أرجع -EINVAL;
                إذا (*اگلا_2 == رستہ) {
                        إذا (ret)
                                *ret = عدم;
                        أرجع 0;
                }
                إذا (!وچ_سیٹ(**اگلا_2, '\0', '/'))
                        أرجع -EINVAL;
                q = *اگلا_2 - 1;
        } وإلا
                q = رستہ + لڑی_لمبائی(رستہ) - 1;

        q = skip_slash_or_dot_backward(رستہ, q);
        إذا (!q) { /* the root directory, "." or "./" */
                إذا (اگلا_2)
                        *اگلا_2 = رستہ;
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }

        آخری_سرا = q + 1;

        طالما (q && *q != '/')
                q = PTR_SUB1(q, رستہ);

        آخری_begin = q ? q + 1 : رستہ;
        len = آخری_سرا - آخری_begin;

        إذا (len > NAME_MAX)
                أرجع -EINVAL;
        إذا (!accept_dot_dot && len == 2 && strneq(آخری_begin, "..", 2))
                أرجع -EINVAL;

        إذا (اگلا_2) {
                q = skip_slash_or_dot_backward(رستہ, q);
                *اگلا_2 = q ? q + 1 : رستہ;
        }

        إذا (ret)
                *ret = آخری_begin;
        أرجع len;
}

ثابت حرف* آخری_رستہ_component(ثابت حرف *رستہ) {

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

        بنانشانی l, k;

        إذا (!رستہ)
                أرجع عدم;

        l = k = لڑی_لمبائی(رستہ);
        إذا (l == 0) /* special case — an empty string */
                أرجع رستہ;

        طالما (k > 0 && رستہ[k-1] == '/')
                k--;

        إذا (k == 0) /* the root directory */
                أرجع رستہ + l - 1;

        طالما (k > 0 && رستہ[k-1] != '/')
                k--;

        أرجع رستہ + k;
}

صحيح رستہ_ونڈو_اگیتر_filename(ثابت حرف *رستہ, حرف **ret_dir, حرف **ret_filename) {
        _cleanup_free_ حرف *d = عدم;
        ثابت حرف *c, *اگلا_2 = عدم;
        صحيح r;

        اشارہ_گر_MAY_BE_NULL(رستہ);

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

        إذا (isempty(رستہ))
                أرجع -EINVAL;

        r = رستہ_لبھو_آخری_component(رستہ, /* accept_dot_dot = */ false, &اگلا_2, &c);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) /* root directory or "." */
                أرجع -EADDRNOTAVAIL;

        إذا (ret_dir) {
                إذا (اگلا_2 == رستہ) {
                        إذا (*رستہ != '/') { /* filename only */
                                إذا (!ret_filename)
                                        أرجع -EDESTADDRREQ;
                        } وإلا {
                                d = strdup("/");
                                إذا (!d)
                                        أرجع -ENOMEM;
                        }
                } وإلا {
                        d = strndup(رستہ, اگلا_2 - رستہ);
                        إذا (!d)
                                أرجع -ENOMEM;

                        رستہ_simplify(d);

                        إذا (!رستہ_is_valid(d))
                                أرجع -EINVAL;
                }

        } وإلا إذا (!رستہ_is_valid(رستہ))
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

        أرجع لڑی_لمبائی(c) > (size_t) r ? O_DIRECTORY : 0;
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

bool رستہ_is_valid_بھریا(ثابت حرف *p, bool accept_dot_dot) {
        إذا (isempty(p))
                أرجع false;

        لكل (ثابت حرف *e = p;;) {
                صحيح r;

                r = رستہ_لبھو_پہلا_component(&e, accept_dot_dot, عدم);
                إذا (r < 0)
                        أرجع false;

                إذا (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        أرجع false;
                إذا (*e == 0)           /* End of string? Yay! */
                        أرجع true;
        }
}

bool رستہ_is_normalized(ثابت حرف *p) {
        إذا (!رستہ_is_محفوظ(p))
                أرجع false;

        إذا (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || لڑی_کھوج(p, "/./"))
                أرجع false;

        إذا (لڑی_کھوج(p, "//"))
                أرجع false;

        أرجع true;
}

صحيح file_وچ_same_dir(ثابت حرف *رستہ, ثابت حرف *filename, حرف **ret) {
        _cleanup_free_ حرف *b = عدم;
        صحيح r;

        أكد(رستہ);
        أكد(filename);
        أكد(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        إذا (رستہ_is_absolute(filename))
                b = strdup(filename);
        وإلا {
                _cleanup_free_ حرف *dn = عدم;

                r = رستہ_کڈھو_directory(رستہ, &dn);
                إذا (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                وإلا إذا (r < 0)
                        أرجع r;
                وإلا
                        b = رستہ_join(dn, filename);
        }
        إذا (!b)
                أرجع -ENOMEM;

        *ret = TAKE_PTR(b);
        أرجع 0;
}

bool hidden_or_backup_file(ثابت حرف *filename) {
        أكد(filename);

        إذا (filename[0] == '.' ||
            STR_وچ_سیٹ(filename,
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

        أرجع STR_وچ_سیٹ(dot + 1,
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

bool is_device_رستہ(ثابت حرف *رستہ) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ثابت حرف *p = رستہ_STARTSWITH_سیٹ(ASSERT_PTR(رستہ), "/dev/", "/sys/");
        أرجع !isempty(p);
}

bool valid_device_گرہ_رستہ(ثابت حرف *رستہ) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        إذا (!رستہ_STARTSWITH_سیٹ(رستہ, "/dev/", "/run/systemd/inaccessible/"))
                أرجع false;

        إذا (endswith(رستہ, "/")) /* can't be a device node if it ends in a slash */
                أرجع false;

        أرجع رستہ_is_normalized(رستہ);
}

bool valid_device_allow_نمونہ(ثابت حرف *رستہ) {
        أكد(رستہ);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        إذا (STARTSWITH_سیٹ(رستہ, "block-", "char-"))
                أرجع true;

        أرجع valid_device_گرہ_رستہ(رستہ);
}

bool dot_or_dot_dot(ثابت حرف *رستہ) {
        إذا (!رستہ)
                أرجع false;
        إذا (رستہ[0] != '.')
                أرجع false;
        إذا (رستہ[1] == 0)
                أرجع true;
        إذا (رستہ[1] != '.')
                أرجع false;

        أرجع رستہ[2] == 0;
}

bool رستہ_سٹا_دیندا_directory(ثابت حرف *رستہ) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        إذا (!رستہ)
                أرجع false;

        إذا (dot_or_dot_dot(رستہ))
                أرجع true;

        أرجع ENDSWITH_سیٹ(رستہ, "/", "/.", "/..");
}

bool خالی_تھاواں_or_جڑ(ثابت حرف *رستہ) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        إذا (isempty(رستہ))
                أرجع true;

        أرجع رستہ_equal(رستہ, "/");
}

ثابت حرف* خالی_تھاواں_منزل_جڑ(ثابت حرف *رستہ) {
        أرجع isempty(رستہ) ? "/" : رستہ;
}

صحيح خالی_تھاواں_or_جڑ_harder_منزل_null(ثابت حرف **رستہ) {
        صحيح r;

        أكد(رستہ);

        /* This nullifies the input path when the path is empty or points to "/". */

        إذا (خالی_تھاواں_or_جڑ(*رستہ)) {
                *رستہ = عدم;
                أرجع 0;
        }

        r = رستہ_is_جڑ(*رستہ);
        إذا (r < 0)
                أرجع r;
        إذا (r > 0)
                *رستہ = عدم;

        أرجع 0;
}

bool رستہ_strv_شامل_اے(حرف * ثابت *l, ثابت حرف *رستہ) {
        أكد(رستہ);

        STRV_FOREACH(i, l)
                إذا (رستہ_equal(*i, رستہ))
                        أرجع true;

        أرجع false;
}

bool prefixed_رستہ_strv_شامل_اے(حرف * ثابت *l, ثابت حرف *رستہ) {
        أكد(رستہ);

        STRV_FOREACH(i, l) {
                ثابت حرف *j = *i;

                إذا (*j == '-')
                        j++;
                إذا (*j == '+')
                        j++;

                إذا (رستہ_equal(j, رستہ))
                        أرجع true;
        }

        أرجع false;
}

صحيح رستہ_glob_can_میل(ثابت حرف *نمونہ, ثابت حرف *اگیتر, حرف **ret) {
        أكد(نمونہ);
        أكد(اگیتر);

        لكل (ثابت حرف *a = نمونہ, *b = اگیتر;;) {
                _cleanup_free_ حرف *g = عدم, *h = عدم;
                ثابت حرف *p, *q;
                صحيح r, s;

                r = رستہ_لبھو_پہلا_component(&a, /* accept_dot_dot= */ false, &p);
                إذا (r < 0)
                        أرجع r;

                s = رستہ_لبھو_پہلا_component(&b, /* accept_dot_dot= */ false, &q);
                إذا (s < 0)
                        أرجع s;

                إذا (s == 0) {
                        /* The pattern matches the prefix. */
                        إذا (ret) {
                                حرف *t;

                                t = رستہ_join(اگیتر, p);
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

#إذا HAVE_ونڈو_BIN
ساكن bool dir_is_ونڈو(ثابت حرف *a, ثابت حرف *b) {
        صحيح r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        إذا (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                أرجع true;
        }
        أرجع r == 0;
}
#جے_انت

ثابت حرف* default_رستہ(فراغ) {
#إذا HAVE_ونڈو_BIN
        ساكن ثابت حرف *default_رستہ_2 = عدم;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        إذا (default_رستہ_2)
                أرجع default_رستہ_2;

        إذا (dir_is_ونڈو("/usr/sbin", "/usr/bin"))
                أرجع (default_رستہ_2 = DEFAULT_رستہ_WITH_بھریا_SBIN);  /* a */
        إذا (dir_is_ونڈو("/usr/local/sbin", "/usr/local/bin"))
                أرجع (default_رستہ_2 = DEFAULT_رستہ_WITH_LOCAL_SBIN); /* b */
        أرجع (default_رستہ_2 = DEFAULT_رستہ_WITHOUT_SBIN);            /* c */
#وإلا
        أرجع DEFAULT_رستہ_WITHOUT_SBIN;
#جے_انت
}
