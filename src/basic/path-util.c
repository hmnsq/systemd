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

bool is_path(ثابت نویسه *p) {
        اگر (!p) /* A NULL pointer is definitely not a path */
                بازگشت false;

        بازگشت strchr(p, '/');
}

صحیح path_split_and_make_absolute(ثابت نویسه *p, نویسه ***ret) {
        _cleanup_strv_free_ نویسه **l = پوچ;
        صحیح r;

        أكد(p);
        أكد(ret);

        l = strv_split(p, ":");
        اگر (!l)
                بازگشت -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        اگر (r < 0)
                بازگشت r;

        *ret = TAKE_PTR(l);
        بازگشت r;
}

نویسه* path_make_absolute(ثابت نویسه *p, ثابت نویسه *prefix) {
        أكد(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        اگر (path_is_absolute(p) || isempty(prefix))
                بازگشت strdup(p);

        بازگشت path_join(prefix, p);
}

صحیح safe_getcwd(نویسه **ret) {
        _cleanup_free_ نویسه *cwd = پوچ;

        cwd = get_current_dir_name();
        اگر (!cwd)
                بازگشت negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        اگر (cwd[0] != '/')
                بازگشت -ENOMEDIUM;

        اگر (ret)
                *ret = TAKE_PTR(cwd);

        بازگشت 0;
}

صحیح path_make_absolute_cwd(ثابت نویسه *p, نویسه **ret) {
        نویسه *c;
        صحیح r;

        أكد(p);
        أكد(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        اگر (path_is_absolute(p))
                c = strdup(p);
        وگرنه {
                _cleanup_free_ نویسه *cwd = پوچ;

                r = safe_getcwd(&cwd);
                اگر (r < 0)
                        بازگشت r;

                c = path_join(cwd, p);
        }
        اگر (!c)
                بازگشت -ENOMEM;

        *ret = c;
        بازگشت 0;
}

صحیح path_make_relative(ثابت نویسه *from, ثابت نویسه *to, نویسه **ret) {
        _cleanup_free_ نویسه *result = پوچ;
        unsigned n_parents;
        ثابت نویسه *f, *t;
        صحیح r, k;
        نویسه *p;

        أكد(from);
        أكد(to);
        أكد(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        اگر (!path_is_absolute(from) || !path_is_absolute(to))
                بازگشت -EINVAL;

        برای (;;) {
                r = path_find_first_component(&from, true, &f);
                اگر (r < 0)
                        بازگشت r;

                k = path_find_first_component(&to, true, &t);
                اگر (k < 0)
                        بازگشت k;

                اگر (r == 0) {
                        /* end of 'from' */
                        اگر (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                اگر (!result)
                                        بازگشت -ENOMEM;
                        } وگرنه {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                اگر (r < 0)
                                        بازگشت r;

                                اگر (!path_is_valid(result))
                                        بازگشت -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        بازگشت 0;
                }

                اگر (r != k || !strneq(f, t, r))
                        بشکن;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        برای (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;
        }

        اگر (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                بازگشت -EINVAL;

        result = new(نویسه, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        اگر (!result)
                بازگشت -ENOMEM;

        برای (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        اگر (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                بازگشت 0;
        }

        strcpy(p, t);

        path_simplify(result);

        اگر (!path_is_valid(result))
                بازگشت -EINVAL;

        *ret = TAKE_PTR(result);
        بازگشت 0;
}

صحیح path_make_relative_parent(ثابت نویسه *from_child, ثابت نویسه *to, نویسه **ret) {
        _cleanup_free_ نویسه *from = پوچ;
        صحیح r;

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
        اگر (r < 0)
                بازگشت r;

        بازگشت path_make_relative(from, to, ret);
}

نویسه* path_startswith_strv(ثابت نویسه *p, نویسه * ثابت *strv) {
        أكد(p);

        STRV_FOREACH(s, strv) {
                نویسه *t;

                t = path_startswith(p, *s);
                اگر (t)
                        بازگشت t;
        }

        بازگشت پوچ;
}

صحیح path_strv_make_absolute_cwd(نویسه **l) {
        صحیح r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                نویسه *t;

                r = path_make_absolute_cwd(*s, &t);
                اگر (r < 0)
                        بازگشت r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        بازگشت 0;
}

نویسه** path_strv_resolve(نویسه **l, ثابت نویسه *ریشه) {
        unsigned k = 0;
        bool enomem = false;
        صحیح r;

        اگر (strv_isempty(l))
                بازگشت l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ نویسه *orig = پوچ;
                نویسه *t, *u;

                اگر (ریشه) {
                        اگر (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                آزاد(*s);
                                ادامه;
                        }

                        orig = *s;
                        t = path_join(ریشه, orig);
                        اگر (!t) {
                                enomem = true;
                                ادامه;
                        }
                } وگرنه
                        t = *s;

                r = chase(t, ریشه, 0, &u, پوچ);
                اگر (r == -ENOENT) {
                        اگر (ریشه) {
                                u = TAKE_PTR(orig);
                                آزاد(t);
                        } وگرنه
                                u = t;
                } وگرنه اگر (r < 0) {
                        آزاد(t);

                        اگر (r == -ENOMEM)
                                enomem = true;

                        ادامه;
                } وگرنه اگر (ریشه) {
                        نویسه *x;

                        آزاد(t);
                        x = path_startswith(u, ریشه);
                        اگر (x) {
                                /* restore the slash if it was lost */
                                اگر (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                آزاد(u);
                                اگر (!t) {
                                        enomem = true;
                                        ادامه;
                                }
                                u = t;
                        } وگرنه {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } وگرنه
                        آزاد(t);

                l[k++] = u;
        }

        l[k] = پوچ;

        اگر (enomem)
                بازگشت پوچ;

        بازگشت l;
}

نویسه** path_strv_resolve_uniq(نویسه **l, ثابت نویسه *ریشه) {

        اگر (strv_isempty(l))
                بازگشت l;

        اگر (!path_strv_resolve(l, ریشه))
                بازگشت پوچ;

        بازگشت strv_uniq(l);
}

نویسه* skip_leading_slash(ثابت نویسه *p) {
        بازگشت skip_leading_chars(p, "/");
}

نویسه* path_simplify_full(نویسه *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        نویسه *f = path;
        صحیح r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        اگر (isempty(path))
                بازگشت path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        برای (ثابت نویسه *p = f;;) {
                ثابت نویسه *e;

                r = path_find_first_component(&p, true, &e);
                اگر (r == 0)
                        بشکن;

                اگر (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        ادامه;

                beginning = false;

                اگر (add_slash)
                        *f++ = '/';

                اگر (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        بازگشت path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        اگر (f == path)
                *f++ = '.';

        اگر (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        بازگشت path;
}

صحیح path_simplify_alloc(ثابت نویسه *path, نویسه **ret) {
        أكد(ret);

        اگر (!path) {
                *ret = پوچ;
                بازگشت 0;
        }

        نویسه *t = strdup(path);
        اگر (!t)
                بازگشت -ENOMEM;

        *ret = path_simplify(t);
        بازگشت 0;
}

نویسه* path_startswith_full(ثابت نویسه *original_path, ثابت نویسه *prefix, PathStartWithFlags flags) {
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

        ثابت نویسه *path = original_path;

        اگر ((path[0] == '/') != (prefix[0] == '/'))
                بازگشت پوچ;

        برای (;;) {
                ثابت نویسه *p, *q;
                صحیح m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                اگر (m < 0)
                        بازگشت پوچ;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                اگر (n < 0)
                        بازگشت پوچ;

                اگر (n == 0) {
                        اگر (!p)
                                p = path;

                        اگر (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                اگر (p <= original_path)
                                        بازگشت پوچ;

                                p--;

                                اگر (*p != '/')
                                        بازگشت پوچ;
                        }

                        بازگشت (نویسه*) p;
                }

                اگر (m != n)
                        بازگشت پوچ;

                اگر (!strneq(p, q, m))
                        بازگشت پوچ;
        }
}

صحیح path_compare(ثابت نویسه *a, ثابت نویسه *b) {
        صحیح r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        اگر (r != 0)
                بازگشت r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        اگر (r != 0)
                بازگشت r;

        برای (;;) {
                ثابت نویسه *aa, *bb;
                صحیح j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                اگر (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        اگر (r != 0)
                                بازگشت r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        بازگشت strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                اگر (j == 0) {
                        اگر (k == 0)
                                بازگشت 0;
                        بازگشت -1;
                }
                اگر (k == 0)
                        بازگشت 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                اگر (r != 0)
                        بازگشت r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                اگر (r != 0)
                        بازگشت r;
        }
}

صحیح path_compare_filename(ثابت نویسه *a, ثابت نویسه *b) {
        _cleanup_free_ نویسه *fa = پوچ, *fb = پوچ;
        صحیح r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        اگر (r != 0)
                بازگشت r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        اگر (r != 0)
                بازگشت r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        اگر (r != 0)
                بازگشت r;

        /* fallback to use strcmp() if both paths are invalid. */
        اگر (j < 0)
                بازگشت strcmp(a, b);

        بازگشت strcmp(fa, fb);
}

صحیح path_equal_or_inode_same_full(ثابت نویسه *a, ثابت نویسه *b, صحیح flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        اگر (path_equal(a, b))
                بازگشت 1;

        اگر (!a || !b)
                بازگشت 0;

        بازگشت inode_same(a, b, flags);
}

نویسه* path_extend_internal(نویسه **x, ...) {
        size_t sz, old_sz;
        نویسه *q, *nx;
        ثابت نویسه *p;
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
        وقتی ((p = va_arg(ap, نویسه*)) != POINTER_MAX) {
                size_t add;

                اگر (isempty(p))
                        ادامه;

                add = 1 + strlen(p);
                اگر (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        بازگشت پوچ;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : پوچ, GREEDY_ALLOC_ROUND_UP(sz+1));
        اگر (!nx)
                بازگشت پوچ;
        اگر (x)
                *x = nx;

        اگر (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        وگرنه {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        وقتی ((p = va_arg(ap, نویسه*)) != POINTER_MAX) {
                اگر (isempty(p))
                        ادامه;

                اگر (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        بازگشت nx;
}

صحیح open_and_check_executable(ثابت نویسه *name, ثابت نویسه *ریشه, نویسه **ret_path, صحیح *ret_fd) {
        _cleanup_close_ صحیح fd = -EBADF;
        _cleanup_free_ نویسه *resolved = پوچ;
        صحیح r;

        أكد(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        اگر (ریشه) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, ریشه, CHASE_PREFIX_ریشه, &resolved, &fd);
                اگر (r < 0)
                        بازگشت r;

                name = resolved;
        } وگرنه {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                اگر (fd < 0)
                        بازگشت -errno;
        }

        r = fd_verify_regular(fd);
        اگر (r < 0)
                بازگشت r;

        r = access_fd(fd, X_OK);
        اگر (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path) {
                اگر (resolved)
                        *ret_path = TAKE_PTR(resolved);
                وگرنه {
                        r = path_make_absolute_cwd(name, ret_path);
                        اگر (r < 0)
                                بازگشت r;

                        path_simplify(*ret_path);
                }
        }

        اگر (ret_fd)
                *ret_fd = TAKE_FD(fd);

        بازگشت 0;
}

صحیح find_executable_full(
                ثابت نویسه *name,
                ثابت نویسه *ریشه,
                نویسه * ثابت *exec_جستجو_path,
                bool use_path_envvar,
                نویسه **ret_filename,
                صحیح *ret_fd) {

        صحیح last_error = -ENOENT, r = 0;

        أكد(name);

        اگر (is_path(name))
                بازگشت open_and_check_executable(name, ریشه, ret_filename, ret_fd);

        اگر (exec_جستجو_path) {
                STRV_FOREACH(element, exec_جستجو_path) {
                        _cleanup_free_ نویسه *full_path = پوچ;

                        اگر (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                ادامه;
                        }

                        full_path = path_join(*element, name);
                        اگر (!full_path)
                                بازگشت -ENOMEM;

                        r = open_and_check_executable(full_path, ریشه, ret_filename, ret_fd);
                        اگر (r >= 0)
                                بازگشت 0;
                        اگر (r != -EACCES)
                                last_error = r;
                }
                بازگشت last_error;
        }

        ثابت نویسه *p = پوچ;

        اگر (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        اگر (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        برای (;;) {
                _cleanup_free_ نویسه *element = پوچ;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                اگر (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        ادامه;
                }

                اگر (!path_extend(&element, name))
                        بازگشت -ENOMEM;

                r = open_and_check_executable(element, ریشه, ret_filename, ret_fd);
                اگر (r >= 0) /* Found it! */
                        بازگشت 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                اگر (r != -EACCES)
                        last_error = r;
        }

        بازگشت last_error;
}

ایستا صحیح executable_is_good(ثابت نویسه *executable) {
        _cleanup_free_ نویسه *p = پوچ, *d = پوچ;
        صحیح r;

        r = find_executable(executable, &p);
        اگر (r == -ENOENT)
                بازگشت 0;
        اگر (r < 0)
                بازگشت r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        اگر (r == -EINVAL) /* not a symlink */
                بازگشت 1;
        اگر (r < 0)
                بازگشت r;

        بازگشت !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

صحیح fsck_exists(تهی) {
        بازگشت executable_is_good("fsck");
}

صحیح fsck_exists_for_fstype(ثابت نویسه *fstype) {
        ثابت نویسه *checker;
        صحیح r;

        أكد(fstype);

        اگر (streq(fstype, "auto"))
                بازگشت -EINVAL;

        r = fsck_exists();
        اگر (r <= 0)
                بازگشت r;

        checker = strjoina("fsck.", fstype);
        بازگشت executable_is_good(checker);
}

ایستا ثابت نویسه* skip_slash_or_dot(ثابت نویسه *p) {
        POINTER_MAY_BE_NULL(p);

        برای (; !isempty(p); p++) {
                اگر (*p == '/')
                        ادامه;
                اگر (startswith(p, "./")) {
                        p++;
                        ادامه;
                }
                بشکن;
        }
        بازگشت p;
}

صحیح path_find_first_component(ثابت نویسه **p, bool accept_dot_dot, ثابت نویسه **ret) {
        ثابت نویسه *q, *first, *end_first, *next;
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
        اگر (isempty(first)) {
                *p = first;
                اگر (ret)
                        *ret = پوچ;
                بازگشت 0;
        }
        اگر (streq(first, ".")) {
                *p = first + 1;
                اگر (ret)
                        *ret = پوچ;
                بازگشت 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        اگر (len > NAME_MAX)
                بازگشت -EINVAL;
        اگر (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                بازگشت -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        اگر (ret)
                *ret = first;
        بازگشت len;
}

ایستا ثابت نویسه* skip_slash_or_dot_backward(ثابت نویسه *path, ثابت نویسه *q) {
        أكد(path);
        أكد(!q || q >= path);

        برای (; q; q = PTR_SUB1(q, path)) {
                اگر (*q == '/')
                        ادامه;
                اگر (q > path && strneq(q - 1, "/.", 2))
                        ادامه;
                اگر (q == path && *q == '.')
                        ادامه;
                بشکن;
        }
        بازگشت q;
}

صحیح path_find_last_component(ثابت نویسه *path, bool accept_dot_dot, ثابت نویسه **next, ثابت نویسه **ret) {
        ثابت نویسه *q, *last_end, *last_begin;
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

        اگر (isempty(path)) {
                اگر (next)
                        *next = path;
                اگر (ret)
                        *ret = پوچ;
                بازگشت 0;
        }

        اگر (next && *next) {
                اگر (*next < path || *next > path + strlen(path))
                        بازگشت -EINVAL;
                اگر (*next == path) {
                        اگر (ret)
                                *ret = پوچ;
                        بازگشت 0;
                }
                اگر (!IN_SET(**next, '\0', '/'))
                        بازگشت -EINVAL;
                q = *next - 1;
        } وگرنه
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        اگر (!q) { /* the root directory, "." or "./" */
                اگر (next)
                        *next = path;
                اگر (ret)
                        *ret = پوچ;
                بازگشت 0;
        }

        last_end = q + 1;

        وقتی (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        اگر (len > NAME_MAX)
                بازگشت -EINVAL;
        اگر (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                بازگشت -EINVAL;

        اگر (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        اگر (ret)
                *ret = last_begin;
        بازگشت len;
}

ثابت نویسه* last_path_component(ثابت نویسه *path) {

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

        اگر (!path)
                بازگشت پوچ;

        l = k = strlen(path);
        اگر (l == 0) /* special case — an empty string */
                بازگشت path;

        وقتی (k > 0 && path[k-1] == '/')
                k--;

        اگر (k == 0) /* the root directory */
                بازگشت path + l - 1;

        وقتی (k > 0 && path[k-1] != '/')
                k--;

        بازگشت path + k;
}

صحیح path_split_prefix_filename(ثابت نویسه *path, نویسه **ret_dir, نویسه **ret_filename) {
        _cleanup_free_ نویسه *d = پوچ;
        ثابت نویسه *c, *next = پوچ;
        صحیح r;

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

        اگر (isempty(path))
                بازگشت -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0) /* root directory or "." */
                بازگشت -EADDRNOTAVAIL;

        اگر (ret_dir) {
                اگر (next == path) {
                        اگر (*path != '/') { /* filename only */
                                اگر (!ret_filename)
                                        بازگشت -EDESTADDRREQ;
                        } وگرنه {
                                d = strdup("/");
                                اگر (!d)
                                        بازگشت -ENOMEM;
                        }
                } وگرنه {
                        d = strndup(path, next - path);
                        اگر (!d)
                                بازگشت -ENOMEM;

                        path_simplify(d);

                        اگر (!path_is_valid(d))
                                بازگشت -EINVAL;
                }

        } وگرنه اگر (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                بازگشت -EINVAL;

        اگر (ret_filename) {
                نویسه *fn = strndup(c, r);
                اگر (!fn)
                        بازگشت -ENOMEM;

                *ret_filename = fn;
        }

        اگر (ret_dir)
                *ret_dir = TAKE_PTR(d);

        بازگشت strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ثابت نویسه *p) {
        ثابت نویسه *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        اگر (!p)
                بازگشت false;

        e = strchrnul(p, '/');
        اگر (*e != 0)
                بازگشت false;

        اگر (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                بازگشت false;

        بازگشت true;
}

bool filename_is_valid(ثابت نویسه *p) {

        اگر (isempty(p))
                بازگشت false;

        اگر (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                بازگشت false;

        بازگشت filename_part_is_valid(p);
}

bool path_is_valid_full(ثابت نویسه *p, bool accept_dot_dot) {
        اگر (isempty(p))
                بازگشت false;

        برای (ثابت نویسه *e = p;;) {
                صحیح r;

                r = path_find_first_component(&e, accept_dot_dot, پوچ);
                اگر (r < 0)
                        بازگشت false;

                اگر (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        بازگشت false;
                اگر (*e == 0)           /* End of string? Yay! */
                        بازگشت true;
        }
}

bool path_is_normalized(ثابت نویسه *p) {
        اگر (!path_is_safe(p))
                بازگشت false;

        اگر (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                بازگشت false;

        اگر (strstr(p, "//"))
                بازگشت false;

        بازگشت true;
}

صحیح file_in_same_dir(ثابت نویسه *path, ثابت نویسه *filename, نویسه **ret) {
        _cleanup_free_ نویسه *b = پوچ;
        صحیح r;

        أكد(path);
        أكد(filename);
        أكد(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        اگر (path_is_absolute(filename))
                b = strdup(filename);
        وگرنه {
                _cleanup_free_ نویسه *dn = پوچ;

                r = path_extract_directory(path, &dn);
                اگر (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                وگرنه اگر (r < 0)
                        بازگشت r;
                وگرنه
                        b = path_join(dn, filename);
        }
        اگر (!b)
                بازگشت -ENOMEM;

        *ret = TAKE_PTR(b);
        بازگشت 0;
}

bool hidden_or_backup_file(ثابت نویسه *filename) {
        أكد(filename);

        اگر (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                بازگشت true;

        ثابت نویسه *dot = strrchr(filename, '.');
        اگر (!dot)
                بازگشت false;

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

        بازگشت STR_IN_SET(dot + 1,
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

bool is_device_path(ثابت نویسه *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ثابت نویسه *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        بازگشت !isempty(p);
}

bool valid_device_گره_path(ثابت نویسه *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        اگر (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                بازگشت false;

        اگر (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                بازگشت false;

        بازگشت path_is_normalized(path);
}

bool valid_device_allow_pattern(ثابت نویسه *path) {
        أكد(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        اگر (STARTSWITH_SET(path, "block-", "char-"))
                بازگشت true;

        بازگشت valid_device_گره_path(path);
}

bool dot_or_dot_dot(ثابت نویسه *path) {
        اگر (!path)
                بازگشت false;
        اگر (path[0] != '.')
                بازگشت false;
        اگر (path[1] == 0)
                بازگشت true;
        اگر (path[1] != '.')
                بازگشت false;

        بازگشت path[2] == 0;
}

bool path_implies_directory(ثابت نویسه *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        اگر (!path)
                بازگشت false;

        اگر (dot_or_dot_dot(path))
                بازگشت true;

        بازگشت ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_ریشه(ثابت نویسه *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        اگر (isempty(path))
                بازگشت true;

        بازگشت path_equal(path, "/");
}

ثابت نویسه* empty_to_ریشه(ثابت نویسه *path) {
        بازگشت isempty(path) ? "/" : path;
}

صحیح empty_or_ریشه_harder_to_null(ثابت نویسه **path) {
        صحیح r;

        أكد(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        اگر (empty_or_ریشه(*path)) {
                *path = پوچ;
                بازگشت 0;
        }

        r = path_is_ریشه(*path);
        اگر (r < 0)
                بازگشت r;
        اگر (r > 0)
                *path = پوچ;

        بازگشت 0;
}

bool path_strv_contains(نویسه * ثابت *l, ثابت نویسه *path) {
        أكد(path);

        STRV_FOREACH(i, l)
                اگر (path_equal(*i, path))
                        بازگشت true;

        بازگشت false;
}

bool prefixed_path_strv_contains(نویسه * ثابت *l, ثابت نویسه *path) {
        أكد(path);

        STRV_FOREACH(i, l) {
                ثابت نویسه *j = *i;

                اگر (*j == '-')
                        j++;
                اگر (*j == '+')
                        j++;

                اگر (path_equal(j, path))
                        بازگشت true;
        }

        بازگشت false;
}

صحیح path_glob_can_match(ثابت نویسه *pattern, ثابت نویسه *prefix, نویسه **ret) {
        أكد(pattern);
        أكد(prefix);

        برای (ثابت نویسه *a = pattern, *b = prefix;;) {
                _cleanup_free_ نویسه *g = پوچ, *h = پوچ;
                ثابت نویسه *p, *q;
                صحیح r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                اگر (r < 0)
                        بازگشت r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                اگر (s < 0)
                        بازگشت s;

                اگر (s == 0) {
                        /* The pattern matches the prefix. */
                        اگر (ret) {
                                نویسه *t;

                                t = path_join(prefix, p);
                                اگر (!t)
                                        بازگشت -ENOMEM;

                                *ret = t;
                        }
                        بازگشت true;
                }

                اگر (r == 0)
                        بشکن;

                اگر (r == s && strneq(p, q, r))
                        ادامه; /* common component. Check next. */

                g = strndup(p, r);
                اگر (!g)
                        بازگشت -ENOMEM;

                اگر (!string_is_glob(g))
                        بشکن;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                اگر (!h)
                        بازگشت -ENOMEM;

                r = fnmatch(g, h, 0);
                اگر (r == FNM_NOMATCH)
                        بشکن;
                اگر (r != 0) /* Failure to process pattern? */
                        بازگشت -EINVAL;
        }

        /* The pattern does not match the prefix. */
        اگر (ret)
                *ret = پوچ;
        بازگشت false;
}

#اگر HAVE_SPLIT_BIN
ایستا bool dir_is_split(ثابت نویسه *a, ثابت نویسه *b) {
        صحیح r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        اگر (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                بازگشت true;
        }
        بازگشت r == 0;
}
#endif

ثابت نویسه* default_PATH(تهی) {
#اگر HAVE_SPLIT_BIN
        ایستا ثابت نویسه *default_path = پوچ;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        اگر (default_path)
                بازگشت default_path;

        اگر (dir_is_split("/usr/sbin", "/usr/bin"))
                بازگشت (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        اگر (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                بازگشت (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        بازگشت (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#وگرنه
        بازگشت DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
