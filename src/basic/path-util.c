/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fnmatch.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "chase.h"
#شامل "errno-util.h"
#شامل "extract-word.h"
#شامل "fd-util.h"
#شامل "fs-util.h"
#شامل "glob-util.h"
#شامل "log.h"
#شامل "path-util.h"
#شامل "stat-util.h"
#شامل "string-util.h"
#شامل "strv.h"

bool is_راستہ(ثابت حرف *p) {
        اگر (!p) /* A NULL pointer is definitely not a path */
                واپس false;

        واپس strchr(p, '/');
}

صحیح راستہ_بانٹو_and_make_absolute(ثابت حرف *p, حرف ***ret) {
        _cleanup_strv_free_ حرف **l = تہی;
        صحیح r;

        أكد(p);
        أكد(ret);

        l = strv_بانٹو(p, ":");
        اگر (!l)
                واپس -ENOMEM;

        r = راستہ_strv_make_absolute_cwd(l);
        اگر (r < 0)
                واپس r;

        *ret = TAKE_PTR(l);
        واپس r;
}

حرف* راستہ_make_absolute(ثابت حرف *p, ثابت حرف *سابقہ) {
        أكد(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        اگر (راستہ_is_absolute(p) || isempty(سابقہ))
                واپس strdup(p);

        واپس راستہ_join(سابقہ, p);
}

صحیح محفوظ_getcwd(حرف **ret) {
        _cleanup_free_ حرف *cwd = تہی;

        cwd = get_current_dir_name();
        اگر (!cwd)
                واپس negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        اگر (cwd[0] != '/')
                واپس -ENOMEDIUM;

        اگر (ret)
                *ret = TAKE_PTR(cwd);

        واپس 0;
}

صحیح راستہ_make_absolute_cwd(ثابت حرف *p, حرف **ret) {
        حرف *c;
        صحیح r;

        أكد(p);
        أكد(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        اگر (راستہ_is_absolute(p))
                c = strdup(p);
        ورنہ {
                _cleanup_free_ حرف *cwd = تہی;

                r = محفوظ_getcwd(&cwd);
                اگر (r < 0)
                        واپس r;

                c = راستہ_join(cwd, p);
        }
        اگر (!c)
                واپس -ENOMEM;

        *ret = c;
        واپس 0;
}

صحیح راستہ_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret) {
        _cleanup_free_ حرف *نتیجہ = تہی;
        غیرعلامتی n_parents;
        ثابت حرف *f, *t;
        صحیح r, k;
        حرف *p;

        أكد(from);
        أكد(to);
        أكد(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        اگر (!راستہ_is_absolute(from) || !راستہ_is_absolute(to))
                واپس -EINVAL;

        برائے (;;) {
                r = راستہ_ڈھونڈو_پہلا_component(&from, true, &f);
                اگر (r < 0)
                        واپس r;

                k = راستہ_ڈھونڈو_پہلا_component(&to, true, &t);
                اگر (k < 0)
                        واپس k;

                اگر (r == 0) {
                        /* end of 'from' */
                        اگر (k == 0) {
                                /* from and to are equivalent. */
                                نتیجہ = strdup(".");
                                اگر (!نتیجہ)
                                        واپس -ENOMEM;
                        } ورنہ {
                                /* 'to' is inside of 'from'. */
                                r = راستہ_simplify_alloc(t, &نتیجہ);
                                اگر (r < 0)
                                        واپس r;

                                اگر (!راستہ_is_valid(نتیجہ))
                                        واپس -EINVAL;
                        }

                        *ret = TAKE_PTR(نتیجہ);
                        واپس 0;
                }

                اگر (r != k || !strneq(f, t, r))
                        توڑو;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        برائے (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = راستہ_ڈھونڈو_پہلا_component(&from, false, &f);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;
        }

        اگر (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                واپس -EINVAL;

        نتیجہ = new(حرف, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        اگر (!نتیجہ)
                واپس -ENOMEM;

        برائے (p = نتیجہ; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        اگر (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(نتیجہ);
                واپس 0;
        }

        strcpy(p, t);

        راستہ_simplify(نتیجہ);

        اگر (!راستہ_is_valid(نتیجہ))
                واپس -EINVAL;

        *ret = TAKE_PTR(نتیجہ);
        واپس 0;
}

صحیح راستہ_make_relative_والد(ثابت حرف *from_بچہ, ثابت حرف *to, حرف **ret) {
        _cleanup_free_ حرف *from = تہی;
        صحیح r;

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

        r = راستہ_نکالو_directory(from_بچہ, &from);
        اگر (r < 0)
                واپس r;

        واپس راستہ_make_relative(from, to, ret);
}

حرف* راستہ_startswith_strv(ثابت حرف *p, حرف * ثابت *strv) {
        أكد(p);

        STRV_FOREACH(s, strv) {
                حرف *t;

                t = راستہ_startswith(p, *s);
                اگر (t)
                        واپس t;
        }

        واپس تہی;
}

صحیح راستہ_strv_make_absolute_cwd(حرف **l) {
        صحیح r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                حرف *t;

                r = راستہ_make_absolute_cwd(*s, &t);
                اگر (r < 0)
                        واپس r;

                راستہ_simplify(t);
                free_and_replace(*s, t);
        }

        واپس 0;
}

حرف** راستہ_strv_resolve(حرف **l, ثابت حرف *جڑ) {
        غیرعلامتی k = 0;
        bool enomem = false;
        صحیح r;

        اگر (strv_isempty(l))
                واپس l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ حرف *orig = تہی;
                حرف *t, *u;

                اگر (جڑ) {
                        اگر (!راستہ_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                آزاد(*s);
                                جاری;
                        }

                        orig = *s;
                        t = راستہ_join(جڑ, orig);
                        اگر (!t) {
                                enomem = true;
                                جاری;
                        }
                } ورنہ
                        t = *s;

                r = chase(t, جڑ, 0, &u, تہی);
                اگر (r == -ENOENT) {
                        اگر (جڑ) {
                                u = TAKE_PTR(orig);
                                آزاد(t);
                        } ورنہ
                                u = t;
                } ورنہ اگر (r < 0) {
                        آزاد(t);

                        اگر (r == -ENOMEM)
                                enomem = true;

                        جاری;
                } ورنہ اگر (جڑ) {
                        حرف *x;

                        آزاد(t);
                        x = راستہ_startswith(u, جڑ);
                        اگر (x) {
                                /* restore the slash if it was lost */
                                اگر (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                آزاد(u);
                                اگر (!t) {
                                        enomem = true;
                                        جاری;
                                }
                                u = t;
                        } ورنہ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ورنہ
                        آزاد(t);

                l[k++] = u;
        }

        l[k] = تہی;

        اگر (enomem)
                واپس تہی;

        واپس l;
}

حرف** راستہ_strv_resolve_uniq(حرف **l, ثابت حرف *جڑ) {

        اگر (strv_isempty(l))
                واپس l;

        اگر (!راستہ_strv_resolve(l, جڑ))
                واپس تہی;

        واپس strv_uniq(l);
}

حرف* skip_leading_slash(ثابت حرف *p) {
        واپس skip_leading_chars(p, "/");
}

حرف* راستہ_simplify_بھرا(حرف *راستہ, راستہ_Simplify_Flags flags) {
        bool جوڑو_slash = false, keep_trailing_slash, absolute, beginning = true;
        حرف *f = راستہ;
        صحیح r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        اگر (isempty(راستہ))
                واپس راستہ;

        keep_trailing_slash = FLAGS_سیٹ(flags, راستہ_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(راستہ, "/");

        absolute = راستہ_is_absolute(راستہ);
        f += absolute;  /* Keep leading /, if present. */

        برائے (ثابت حرف *p = f;;) {
                ثابت حرف *e;

                r = راستہ_ڈھونڈو_پہلا_component(&p, true, &e);
                اگر (r == 0)
                        توڑو;

                اگر (r > 0 && absolute && beginning && راستہ_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        جاری;

                beginning = false;

                اگر (جوڑو_slash)
                        *f++ = '/';

                اگر (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, لڑی_لمبائی(p) + 1);
                        واپس راستہ;
                }

                memmove(f, e, r);
                f += r;

                جوڑو_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        اگر (f == راستہ)
                *f++ = '.';

        اگر (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        واپس راستہ;
}

صحیح راستہ_simplify_alloc(ثابت حرف *راستہ, حرف **ret) {
        أكد(ret);

        اگر (!راستہ) {
                *ret = تہی;
                واپس 0;
        }

        حرف *t = strdup(راستہ);
        اگر (!t)
                واپس -ENOMEM;

        *ret = راستہ_simplify(t);
        واپس 0;
}

حرف* راستہ_startswith_بھرا(ثابت حرف *original_راستہ, ثابت حرف *سابقہ, راستہ_آغاز_With_Flags flags) {
        أكد(original_راستہ);
        أكد(سابقہ);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ثابت حرف *راستہ = original_راستہ;

        اگر ((راستہ[0] == '/') != (سابقہ[0] == '/'))
                واپس تہی;

        برائے (;;) {
                ثابت حرف *p, *q;
                صحیح m, n;

                m = راستہ_ڈھونڈو_پہلا_component(&راستہ, !FLAGS_سیٹ(flags, راستہ_STARTSWITH_REFUSE_DOT_DOT), &p);
                اگر (m < 0)
                        واپس تہی;

                n = راستہ_ڈھونڈو_پہلا_component(&سابقہ, !FLAGS_سیٹ(flags, راستہ_STARTSWITH_REFUSE_DOT_DOT), &q);
                اگر (n < 0)
                        واپس تہی;

                اگر (n == 0) {
                        اگر (!p)
                                p = راستہ;

                        اگر (FLAGS_سیٹ(flags, راستہ_STARTSWITH_RETURN_LEADING_SLASH)) {

                                اگر (p <= original_راستہ)
                                        واپس تہی;

                                p--;

                                اگر (*p != '/')
                                        واپس تہی;
                        }

                        واپس (حرف*) p;
                }

                اگر (m != n)
                        واپس تہی;

                اگر (!strneq(p, q, m))
                        واپس تہی;
        }
}

صحیح راستہ_compare(ثابت حرف *a, ثابت حرف *b) {
        صحیح r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        اگر (r != 0)
                واپس r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(راستہ_is_absolute(a), راستہ_is_absolute(b));
        اگر (r != 0)
                واپس r;

        برائے (;;) {
                ثابت حرف *aa, *bb;
                صحیح j, k;

                j = راستہ_ڈھونڈو_پہلا_component(&a, true, &aa);
                k = راستہ_ڈھونڈو_پہلا_component(&b, true, &bb);

                اگر (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        اگر (r != 0)
                                واپس r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        واپس strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                اگر (j == 0) {
                        اگر (k == 0)
                                واپس 0;
                        واپس -1;
                }
                اگر (k == 0)
                        واپس 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                اگر (r != 0)
                        واپس r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                اگر (r != 0)
                        واپس r;
        }
}

صحیح راستہ_compare_filename(ثابت حرف *a, ثابت حرف *b) {
        _cleanup_free_ حرف *fa = تہی, *fb = تہی;
        صحیح r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        اگر (r != 0)
                واپس r;

        j = راستہ_نکالو_filename(a, &fa);
        k = راستہ_نکالو_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        اگر (r != 0)
                واپس r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        اگر (r != 0)
                واپس r;

        /* fallback to use strcmp() if both paths are invalid. */
        اگر (j < 0)
                واپس strcmp(a, b);

        واپس strcmp(fa, fb);
}

صحیح راستہ_equal_or_inode_same_بھرا(ثابت حرف *a, ثابت حرف *b, صحیح flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        اگر (راستہ_equal(a, b))
                واپس 1;

        اگر (!a || !b)
                واپس 0;

        واپس inode_same(a, b, flags);
}

حرف* راستہ_extend_اندرونی(حرف **x, ...) {
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
        جب ((p = va_arg(ap, حرف*)) != اشارہ_گر_MAX) {
                size_t جوڑو;

                اگر (isempty(p))
                        جاری;

                جوڑو = 1 + لڑی_لمبائی(p);
                اگر (sz > SIZE_MAX - جوڑو) { /* overflow check */
                        va_end(ap);
                        واپس تہی;
                }

                sz += جوڑو;
        }
        va_end(ap);

        nx = realloc(x ? *x : تہی, GREEDY_ALLOC_باری_اوپر(sz+1));
        اگر (!nx)
                واپس تہی;
        اگر (x)
                *x = nx;

        اگر (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ورنہ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        جب ((p = va_arg(ap, حرف*)) != اشارہ_گر_MAX) {
                اگر (isempty(p))
                        جاری;

                اگر (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        واپس nx;
}

صحیح open_and_check_executable(ثابت حرف *name, ثابت حرف *جڑ, حرف **ret_راستہ, صحیح *ret_fd) {
        _cleanup_close_ صحیح fd = -EBADF;
        _cleanup_free_ حرف *resolved = تہی;
        صحیح r;

        أكد(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        اگر (جڑ) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, جڑ, CHASE_سابقہ_جڑ, &resolved, &fd);
                اگر (r < 0)
                        واپس r;

                name = resolved;
        } ورنہ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_راستہ|O_CLOEXEC);
                اگر (fd < 0)
                        واپس -errno;
        }

        r = fd_verify_regular(fd);
        اگر (r < 0)
                واپس r;

        r = access_fd(fd, X_OK);
        اگر (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ) {
                اگر (resolved)
                        *ret_راستہ = TAKE_PTR(resolved);
                ورنہ {
                        r = راستہ_make_absolute_cwd(name, ret_راستہ);
                        اگر (r < 0)
                                واپس r;

                        راستہ_simplify(*ret_راستہ);
                }
        }

        اگر (ret_fd)
                *ret_fd = TAKE_FD(fd);

        واپس 0;
}

صحیح ڈھونڈو_executable_بھرا(
                ثابت حرف *name,
                ثابت حرف *جڑ,
                حرف * ثابت *exec_تلاش_راستہ,
                bool use_راستہ_envvar,
                حرف **ret_filename,
                صحیح *ret_fd) {

        صحیح آخری_error = -ENOENT, r = 0;

        أكد(name);

        اگر (is_راستہ(name))
                واپس open_and_check_executable(name, جڑ, ret_filename, ret_fd);

        اگر (exec_تلاش_راستہ) {
                STRV_FOREACH(element, exec_تلاش_راستہ) {
                        _cleanup_free_ حرف *بھرا_راستہ = تہی;

                        اگر (!راستہ_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                جاری;
                        }

                        بھرا_راستہ = راستہ_join(*element, name);
                        اگر (!بھرا_راستہ)
                                واپس -ENOMEM;

                        r = open_and_check_executable(بھرا_راستہ, جڑ, ret_filename, ret_fd);
                        اگر (r >= 0)
                                واپس 0;
                        اگر (r != -EACCES)
                                آخری_error = r;
                }
                واپس آخری_error;
        }

        ثابت حرف *p = تہی;

        اگر (use_راستہ_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        اگر (!p)
                p = default_راستہ();

        /* Resolve a single-component name to a full path */
        برائے (;;) {
                _cleanup_free_ حرف *element = تہی;

                r = نکالو_پہلا_لفظ(&p, &element, ":", نکالو_RELAX|نکالو_DONT_COALESCE_SEPARATORS);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                اگر (!راستہ_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        جاری;
                }

                اگر (!راستہ_extend(&element, name))
                        واپس -ENOMEM;

                r = open_and_check_executable(element, جڑ, ret_filename, ret_fd);
                اگر (r >= 0) /* Found it! */
                        واپس 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                اگر (r != -EACCES)
                        آخری_error = r;
        }

        واپس آخری_error;
}

ساکن صحیح executable_is_good(ثابت حرف *executable) {
        _cleanup_free_ حرف *p = تہی, *d = تہی;
        صحیح r;

        r = ڈھونڈو_executable(executable, &p);
        اگر (r == -ENOENT)
                واپس 0;
        اگر (r < 0)
                واپس r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        اگر (r == -EINVAL) /* not a symlink */
                واپس 1;
        اگر (r < 0)
                واپس r;

        واپس !راستہ_اندر_سیٹ(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

صحیح fsck_exists(خالی) {
        واپس executable_is_good("fsck");
}

صحیح fsck_exists_for_fstype(ثابت حرف *fstype) {
        ثابت حرف *checker;
        صحیح r;

        أكد(fstype);

        اگر (streq(fstype, "auto"))
                واپس -EINVAL;

        r = fsck_exists();
        اگر (r <= 0)
                واپس r;

        checker = strjoina("fsck.", fstype);
        واپس executable_is_good(checker);
}

ساکن ثابت حرف* skip_slash_or_dot(ثابت حرف *p) {
        اشارہ_گر_MAY_BE_NULL(p);

        برائے (; !isempty(p); p++) {
                اگر (*p == '/')
                        جاری;
                اگر (startswith(p, "./")) {
                        p++;
                        جاری;
                }
                توڑو;
        }
        واپس p;
}

صحیح راستہ_ڈھونڈو_پہلا_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret) {
        ثابت حرف *q, *پہلا, *انتہا_پہلا, *اگلا_2;
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
        اگر (isempty(پہلا)) {
                *p = پہلا;
                اگر (ret)
                        *ret = تہی;
                واپس 0;
        }
        اگر (streq(پہلا, ".")) {
                *p = پہلا + 1;
                اگر (ret)
                        *ret = تہی;
                واپس 0;
        }

        انتہا_پہلا = strchrnul(پہلا, '/');
        len = انتہا_پہلا - پہلا;

        اگر (len > NAME_MAX)
                واپس -EINVAL;
        اگر (!accept_dot_dot && len == 2 && پہلا[0] == '.' && پہلا[1] == '.')
                واپس -EINVAL;

        اگلا_2 = skip_slash_or_dot(انتہا_پہلا);

        *p = اگلا_2 + streq(اگلا_2, ".");
        اگر (ret)
                *ret = پہلا;
        واپس len;
}

ساکن ثابت حرف* skip_slash_or_dot_backward(ثابت حرف *راستہ, ثابت حرف *q) {
        أكد(راستہ);
        أكد(!q || q >= راستہ);

        برائے (; q; q = PTR_SUB1(q, راستہ)) {
                اگر (*q == '/')
                        جاری;
                اگر (q > راستہ && strneq(q - 1, "/.", 2))
                        جاری;
                اگر (q == راستہ && *q == '.')
                        جاری;
                توڑو;
        }
        واپس q;
}

صحیح راستہ_ڈھونڈو_آخری_component(ثابت حرف *راستہ, bool accept_dot_dot, ثابت حرف **اگلا_2, ثابت حرف **ret) {
        ثابت حرف *q, *آخری_انتہا, *آخری_begin;
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

        اگر (isempty(راستہ)) {
                اگر (اگلا_2)
                        *اگلا_2 = راستہ;
                اگر (ret)
                        *ret = تہی;
                واپس 0;
        }

        اگر (اگلا_2 && *اگلا_2) {
                اگر (*اگلا_2 < راستہ || *اگلا_2 > راستہ + لڑی_لمبائی(راستہ))
                        واپس -EINVAL;
                اگر (*اگلا_2 == راستہ) {
                        اگر (ret)
                                *ret = تہی;
                        واپس 0;
                }
                اگر (!اندر_سیٹ(**اگلا_2, '\0', '/'))
                        واپس -EINVAL;
                q = *اگلا_2 - 1;
        } ورنہ
                q = راستہ + لڑی_لمبائی(راستہ) - 1;

        q = skip_slash_or_dot_backward(راستہ, q);
        اگر (!q) { /* the root directory, "." or "./" */
                اگر (اگلا_2)
                        *اگلا_2 = راستہ;
                اگر (ret)
                        *ret = تہی;
                واپس 0;
        }

        آخری_انتہا = q + 1;

        جب (q && *q != '/')
                q = PTR_SUB1(q, راستہ);

        آخری_begin = q ? q + 1 : راستہ;
        len = آخری_انتہا - آخری_begin;

        اگر (len > NAME_MAX)
                واپس -EINVAL;
        اگر (!accept_dot_dot && len == 2 && strneq(آخری_begin, "..", 2))
                واپس -EINVAL;

        اگر (اگلا_2) {
                q = skip_slash_or_dot_backward(راستہ, q);
                *اگلا_2 = q ? q + 1 : راستہ;
        }

        اگر (ret)
                *ret = آخری_begin;
        واپس len;
}

ثابت حرف* آخری_راستہ_component(ثابت حرف *راستہ) {

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

        غیرعلامتی l, k;

        اگر (!راستہ)
                واپس تہی;

        l = k = لڑی_لمبائی(راستہ);
        اگر (l == 0) /* special case — an empty string */
                واپس راستہ;

        جب (k > 0 && راستہ[k-1] == '/')
                k--;

        اگر (k == 0) /* the root directory */
                واپس راستہ + l - 1;

        جب (k > 0 && راستہ[k-1] != '/')
                k--;

        واپس راستہ + k;
}

صحیح راستہ_بانٹو_سابقہ_filename(ثابت حرف *راستہ, حرف **ret_dir, حرف **ret_filename) {
        _cleanup_free_ حرف *d = تہی;
        ثابت حرف *c, *اگلا_2 = تہی;
        صحیح r;

        اشارہ_گر_MAY_BE_NULL(راستہ);

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

        اگر (isempty(راستہ))
                واپس -EINVAL;

        r = راستہ_ڈھونڈو_آخری_component(راستہ, /* accept_dot_dot = */ false, &اگلا_2, &c);
        اگر (r < 0)
                واپس r;
        اگر (r == 0) /* root directory or "." */
                واپس -EADDRNOTAVAIL;

        اگر (ret_dir) {
                اگر (اگلا_2 == راستہ) {
                        اگر (*راستہ != '/') { /* filename only */
                                اگر (!ret_filename)
                                        واپس -EDESTADDRREQ;
                        } ورنہ {
                                d = strdup("/");
                                اگر (!d)
                                        واپس -ENOMEM;
                        }
                } ورنہ {
                        d = strndup(راستہ, اگلا_2 - راستہ);
                        اگر (!d)
                                واپس -ENOMEM;

                        راستہ_simplify(d);

                        اگر (!راستہ_is_valid(d))
                                واپس -EINVAL;
                }

        } ورنہ اگر (!راستہ_is_valid(راستہ))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                واپس -EINVAL;

        اگر (ret_filename) {
                حرف *fn = strndup(c, r);
                اگر (!fn)
                        واپس -ENOMEM;

                *ret_filename = fn;
        }

        اگر (ret_dir)
                *ret_dir = TAKE_PTR(d);

        واپس لڑی_لمبائی(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ثابت حرف *p) {
        ثابت حرف *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        اگر (!p)
                واپس false;

        e = strchrnul(p, '/');
        اگر (*e != 0)
                واپس false;

        اگر (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                واپس false;

        واپس true;
}

bool filename_is_valid(ثابت حرف *p) {

        اگر (isempty(p))
                واپس false;

        اگر (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                واپس false;

        واپس filename_part_is_valid(p);
}

bool راستہ_is_valid_بھرا(ثابت حرف *p, bool accept_dot_dot) {
        اگر (isempty(p))
                واپس false;

        برائے (ثابت حرف *e = p;;) {
                صحیح r;

                r = راستہ_ڈھونڈو_پہلا_component(&e, accept_dot_dot, تہی);
                اگر (r < 0)
                        واپس false;

                اگر (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        واپس false;
                اگر (*e == 0)           /* End of string? Yay! */
                        واپس true;
        }
}

bool راستہ_is_normalized(ثابت حرف *p) {
        اگر (!راستہ_is_محفوظ(p))
                واپس false;

        اگر (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || لڑی_تلاش(p, "/./"))
                واپس false;

        اگر (لڑی_تلاش(p, "//"))
                واپس false;

        واپس true;
}

صحیح file_اندر_same_dir(ثابت حرف *راستہ, ثابت حرف *filename, حرف **ret) {
        _cleanup_free_ حرف *b = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(filename);
        أكد(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        اگر (راستہ_is_absolute(filename))
                b = strdup(filename);
        ورنہ {
                _cleanup_free_ حرف *dn = تہی;

                r = راستہ_نکالو_directory(راستہ, &dn);
                اگر (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ورنہ اگر (r < 0)
                        واپس r;
                ورنہ
                        b = راستہ_join(dn, filename);
        }
        اگر (!b)
                واپس -ENOMEM;

        *ret = TAKE_PTR(b);
        واپس 0;
}

bool hidden_or_backup_file(ثابت حرف *filename) {
        أكد(filename);

        اگر (filename[0] == '.' ||
            STR_اندر_سیٹ(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                واپس true;

        ثابت حرف *dot = strrchr(filename, '.');
        اگر (!dot)
                واپس false;

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

        واپس STR_اندر_سیٹ(dot + 1,
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

bool is_device_راستہ(ثابت حرف *راستہ) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ثابت حرف *p = راستہ_STARTSWITH_سیٹ(ASSERT_PTR(راستہ), "/dev/", "/sys/");
        واپس !isempty(p);
}

bool valid_device_گرہ_راستہ(ثابت حرف *راستہ) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        اگر (!راستہ_STARTSWITH_سیٹ(راستہ, "/dev/", "/run/systemd/inaccessible/"))
                واپس false;

        اگر (endswith(راستہ, "/")) /* can't be a device node if it ends in a slash */
                واپس false;

        واپس راستہ_is_normalized(راستہ);
}

bool valid_device_allow_نمونہ(ثابت حرف *راستہ) {
        أكد(راستہ);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        اگر (STARTSWITH_سیٹ(راستہ, "block-", "char-"))
                واپس true;

        واپس valid_device_گرہ_راستہ(راستہ);
}

bool dot_or_dot_dot(ثابت حرف *راستہ) {
        اگر (!راستہ)
                واپس false;
        اگر (راستہ[0] != '.')
                واپس false;
        اگر (راستہ[1] == 0)
                واپس true;
        اگر (راستہ[1] != '.')
                واپس false;

        واپس راستہ[2] == 0;
}

bool راستہ_مستلزم_directory(ثابت حرف *راستہ) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        اگر (!راستہ)
                واپس false;

        اگر (dot_or_dot_dot(راستہ))
                واپس true;

        واپس ENDSWITH_سیٹ(راستہ, "/", "/.", "/..");
}

bool خالی_خانے_or_جڑ(ثابت حرف *راستہ) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        اگر (isempty(راستہ))
                واپس true;

        واپس راستہ_equal(راستہ, "/");
}

ثابت حرف* خالی_خانے_مقصد_جڑ(ثابت حرف *راستہ) {
        واپس isempty(راستہ) ? "/" : راستہ;
}

صحیح خالی_خانے_or_جڑ_harder_مقصد_null(ثابت حرف **راستہ) {
        صحیح r;

        أكد(راستہ);

        /* This nullifies the input path when the path is empty or points to "/". */

        اگر (خالی_خانے_or_جڑ(*راستہ)) {
                *راستہ = تہی;
                واپس 0;
        }

        r = راستہ_is_جڑ(*راستہ);
        اگر (r < 0)
                واپس r;
        اگر (r > 0)
                *راستہ = تہی;

        واپس 0;
}

bool راستہ_strv_شامل_ہے(حرف * ثابت *l, ثابت حرف *راستہ) {
        أكد(راستہ);

        STRV_FOREACH(i, l)
                اگر (راستہ_equal(*i, راستہ))
                        واپس true;

        واپس false;
}

bool prefixed_راستہ_strv_شامل_ہے(حرف * ثابت *l, ثابت حرف *راستہ) {
        أكد(راستہ);

        STRV_FOREACH(i, l) {
                ثابت حرف *j = *i;

                اگر (*j == '-')
                        j++;
                اگر (*j == '+')
                        j++;

                اگر (راستہ_equal(j, راستہ))
                        واپس true;
        }

        واپس false;
}

صحیح راستہ_glob_can_مطابقت(ثابت حرف *نمونہ, ثابت حرف *سابقہ, حرف **ret) {
        أكد(نمونہ);
        أكد(سابقہ);

        برائے (ثابت حرف *a = نمونہ, *b = سابقہ;;) {
                _cleanup_free_ حرف *g = تہی, *h = تہی;
                ثابت حرف *p, *q;
                صحیح r, s;

                r = راستہ_ڈھونڈو_پہلا_component(&a, /* accept_dot_dot= */ false, &p);
                اگر (r < 0)
                        واپس r;

                s = راستہ_ڈھونڈو_پہلا_component(&b, /* accept_dot_dot= */ false, &q);
                اگر (s < 0)
                        واپس s;

                اگر (s == 0) {
                        /* The pattern matches the prefix. */
                        اگر (ret) {
                                حرف *t;

                                t = راستہ_join(سابقہ, p);
                                اگر (!t)
                                        واپس -ENOMEM;

                                *ret = t;
                        }
                        واپس true;
                }

                اگر (r == 0)
                        توڑو;

                اگر (r == s && strneq(p, q, r))
                        جاری; /* common component. Check next. */

                g = strndup(p, r);
                اگر (!g)
                        واپس -ENOMEM;

                اگر (!string_is_glob(g))
                        توڑو;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                اگر (!h)
                        واپس -ENOMEM;

                r = fnmatch(g, h, 0);
                اگر (r == FNM_NOMATCH)
                        توڑو;
                اگر (r != 0) /* Failure to process pattern? */
                        واپس -EINVAL;
        }

        /* The pattern does not match the prefix. */
        اگر (ret)
                *ret = تہی;
        واپس false;
}

#اگر HAVE_بانٹو_BIN
ساکن bool dir_is_بانٹو(ثابت حرف *a, ثابت حرف *b) {
        صحیح r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        اگر (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                واپس true;
        }
        واپس r == 0;
}
#اگر_ختم

ثابت حرف* default_راستہ(خالی) {
#اگر HAVE_بانٹو_BIN
        ساکن ثابت حرف *default_راستہ_2 = تہی;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        اگر (default_راستہ_2)
                واپس default_راستہ_2;

        اگر (dir_is_بانٹو("/usr/sbin", "/usr/bin"))
                واپس (default_راستہ_2 = DEFAULT_راستہ_WITH_بھرا_SBIN);  /* a */
        اگر (dir_is_بانٹو("/usr/local/sbin", "/usr/local/bin"))
                واپس (default_راستہ_2 = DEFAULT_راستہ_WITH_LOCAL_SBIN); /* b */
        واپس (default_راستہ_2 = DEFAULT_راستہ_WITHOUT_SBIN);            /* c */
#ورنہ
        واپس DEFAULT_راستہ_WITHOUT_SBIN;
#اگر_ختم
}
